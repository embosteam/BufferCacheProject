#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "config.h"
#include "shared/common_shared.h"
#include "buffered_read/export_api.h"
#include "delayed_write/export_api.h"
#include "replacement_policy/export_api.h"

#define BLOCK_SIZE 4096

/**
 * 임시함수
*/
int readMemBuffer(int block_nr){
	return block_nr;
}

char *disk_buffer;
char* tmp_buffer;
struct MemoryBufferManager* memory_buffer_manager=NULL;
struct ThreadPool* tp = NULL;
double pure_hashmap_access_time = 0.0;
double hashhit_access_time = 0;
double hashmiss_access_time = 0;
unsigned long long hit_count =0;
int disk_fd;
int disk_fd_tmp;
int disk_fd3;


int os_read(int block_nr, char *user_buffer)
{
	int ret;

	// implement BUFFERED_READ

	struct hashmap* memory_buffers = memory_buffer_manager->memory_buffers;
	struct MemoryBufferController* memory_buffer_controller = memory_buffer_manager->controller;
	if(CONFIG_DEBUG_MODE){
		printf("[os_read] start getMemoryBUfferMap\n");
	}
	unsigned long long start_hash_time = getCurrentTime();
	struct MemoryBuffer* memory_buffer = memory_buffer_controller->getMemoryBufferMap(memory_buffer_manager,block_nr);
	unsigned long long finish_hash_time = getCurrentTime();
	pure_hashmap_access_time += (double)(finish_hash_time-start_hash_time)/(CONFIG_RANDOM_ACCESS_SEQUENCE_ELEMENT_COUNT+2);
	if(CONFIG_DEBUG_MODE){
		printf("[os_read] finish getMemoryBUfferMap\n");
	}
	const long long default_block_size_byte = memory_buffer_manager->explicit_block_size;
	if(memory_buffer!=NULL){
		hit_count++;
		if(CONFIG_DEBUG_MODE){
			printf("[os_read] memory buffer is not NULL (at %d)\n",block_nr);
		}
		memcpy(user_buffer, memory_buffer->buffer, memory_buffer->header.block_size_byte);
		finish_hash_time = getCurrentTime();
		hashhit_access_time += (double)(finish_hash_time-start_hash_time)/(CONFIG_RANDOM_ACCESS_SEQUENCE_ELEMENT_COUNT+2);
		struct ReplacementPolicyManager* replacement_manager = memory_buffer_controller->replacement_policy_manager;
        replacement_manager->updateAccessInfo(replacement_manager,memory_buffer);
                
		return (int)memory_buffer->header.block_size_byte;
	}
	else{
		if(CONFIG_DEBUG_MODE){
			printf("[os_read] memory buffer is NULL(at %d)\n",block_nr);
		}
		if(USE_MMAP_RW){
			memcpy(user_buffer,disk_buffer+block_nr*default_block_size_byte,default_block_size_byte);
		}else{
			int disk_fd2 = disk_fd;
			pread(disk_fd2,disk_buffer,default_block_size_byte,block_nr * default_block_size_byte);
			memcpy(user_buffer,disk_buffer,default_block_size_byte);
		}

		finish_hash_time = getCurrentTime();
		hashmiss_access_time += (double)(finish_hash_time-start_hash_time)/(CONFIG_RANDOM_ACCESS_SEQUENCE_ELEMENT_COUNT+2);
	
		memory_buffer = createNewMemoryBuffer(default_block_size_byte,block_nr);
		memcpy(memory_buffer->buffer,user_buffer,default_block_size_byte);
		memory_buffer->header.isValid=1;
		if(memory_buffer_controller->shouldFindEvictableBuffer(memory_buffer_controller,memory_buffer_manager)>0){
			
			struct MemoryBuffer* evictable_buffer = memory_buffer_controller->findEvictableBuffer(memory_buffer_controller,memory_buffer_manager);
			if(CONFIG_DEBUG_MODE){
				printf("[os_read] finish find Evictable Buffer\n");
				if(evictable_buffer!=NULL){
					printf("[find evitable buffer on write] found evitable buffer: (blk_number: %d)\n",evictable_buffer->header.block_number);
				}
				else{
					printf("[find evitable buffer on write] not find evitable buffer\n");
				}
			}
			memory_buffer_controller->deleteMemoryBuffer(memory_buffer_controller,memory_buffer_manager,evictable_buffer);
		}
		memory_buffer_controller->putMemoryBufferAt(memory_buffer_manager,block_nr,memory_buffer);
	}

	return ret;
}

int os_write(int block_nr, char *user_buffer)
{
	int ret = 0;
	const long long default_block_size_byte = memory_buffer_manager->explicit_block_size;
	struct hashmap* memory_buffers = memory_buffer_manager->memory_buffers;
	struct MemoryBufferController* memory_buffer_controller = memory_buffer_manager->controller;

	struct MemoryBuffer* buffer = memory_buffer_controller->getMemoryBufferMap(memory_buffer_manager,block_nr);
	if(CONFIG_DEBUG_MODE){
		printf("[try load] from hashmap ok (%d,%d)\n",buffer!=NULL,buffer!=NULL&&buffer->buffer!=NULL);
	}
	if(buffer==NULL){
		
		buffer = createNewMemoryBuffer(default_block_size_byte,block_nr);
		memcpy(buffer->buffer,user_buffer,default_block_size_byte);
		buffer->header.isValid=1;
		buffer->header.isDirty=1;
		buffer->header.isBufferBeingWrittenNow = 0;
		if(memory_buffer_controller->shouldFindEvictableBuffer(memory_buffer_controller,memory_buffer_manager)>0){
			struct MemoryBuffer* evictable_buffer = memory_buffer_controller->findEvictableBuffer(memory_buffer_controller,memory_buffer_manager);
			if(CONFIG_DEBUG_MODE){
				if(evictable_buffer!=NULL){
					printf("[find evitable buffer on write] found evitable buffer: (blk_number: %d)\n",evictable_buffer->header.block_number);
				}
				else{
					printf("[find evitable buffer on write] not find evitable buffer\n");
				}
			}
			memory_buffer_controller->deleteMemoryBuffer(memory_buffer_controller,memory_buffer_manager,evictable_buffer);
		}
		memory_buffer_controller->putMemoryBufferAt(memory_buffer_manager,block_nr,buffer);
	}
	else{
		struct ReplacementPolicyManager* replacement_manager = memory_buffer_controller->replacement_policy_manager;
        replacement_manager->updateAccessInfo(replacement_manager,buffer);

		if(buffer->header.isValid==0){
			
			sem_init(&buffer->header.write_lock,0,1);
		}

		sem_wait(&buffer->header.write_lock);
		if(buffer->buffer==NULL){
			buffer->buffer = (char*)malloc(sizeof(char)*buffer->header.block_size_byte);
			
		}
		buffer->header.isBufferBeingWrittenNow = 1;
		memcpy(buffer->buffer,user_buffer,default_block_size_byte);
		buffer->header.isValid=1;
		buffer->header.isDirty = 1;
		buffer->header.isBufferBeingWrittenNow = 0;
		sem_post(&buffer->header.write_lock);
	}

	if(CONFIG_DEBUG_MODE){
	printf("[os_write] membuffer: %x , blknm: %d\n",buffer, buffer->header.block_number);
	}
	
	if(!USE_MMAP_RW){
	 tp->addQueue(tp,disk_fd,buffer,0);
	}
	else{
	 tp->addQueue2(tp,disk_buffer,buffer,0);
	}
	return ret;
}

int lib_read(int block_nr, char *user_buffer)
{
	int ret;
	ret = os_read(block_nr, user_buffer);
	
	return ret;
}

int lib_write(int block_nr, char *user_buffer)
{
	int ret;
	ret = os_write(block_nr, user_buffer);
	
	return ret;
}

int init()
{
	hit_count = 0;
	memory_buffer_manager  = createNewMemoryBufferManager(
		CONFIG_TOTAL_N,CONFIG_MANANAGEABLE_N,
		(long long)BLOCK_SIZE,
		CONFIG_REPLACEMENT_POLICY_TYPE);
	if(CONFIG_DEBUG_MODE){
		printf("manageable_n: %d\n",memory_buffer_manager->manageable_number_of_memory_buffers);
	}
	const long long default_block_size_byte = memory_buffer_manager->explicit_block_size;
	tp = newThreadPool(CONFIG_THREAD_POOL_NUMBER_OF_INTERNAL_THREAD,CONFIG_THREAD_POOL_NUMBER_OF_PACKET_SIZE_FOR_MEMORY_BUFFER);//20);
	memory_buffer_manager->tp = tp;
	

	printf("disk_buffer: %p\n", disk_buffer);
	#ifdef __APPLE__
		disk_fd = open("diskfile", O_RDWR);
	#else
		disk_fd = open("diskfile", O_RDWR|O_DIRECT);
	#endif
	#ifdef __APPLE__
		fcntl(disk_fd,F_SETFL,F_NOCACHE);
	#endif
	#ifdef __APPLE__
		disk_fd_tmp = open("tmpfile", O_RDWR);
	#else
		disk_fd_tmp = open("tmpfile", O_RDWR|O_DIRECT);
	#endif
	#ifdef __APPLE__
		fcntl(disk_fd_tmp,F_SETFL,F_NOCACHE);
	#endif
	if(USE_MMAP_RW){
	disk_buffer = mmap(NULL,CONFIG_TOTAL_N*BLOCK_SIZE,PROT_READ|PROT_WRITE,MAP_SHARED,dup(disk_fd),0);  //aligned_alloc(default_block_size_byte, default_block_size_byte);
	msync(disk_buffer,CONFIG_TOTAL_N*BLOCK_SIZE,MS_SYNC);
	}
	else{
		disk_buffer =aligned_alloc(default_block_size_byte, default_block_size_byte);
	}
	tmp_buffer =aligned_alloc(default_block_size_byte, default_block_size_byte);
	
	if (disk_buffer == NULL){
		perror("mmap error:");
		exit(1);
		return -errno;
	}
	
	if (disk_fd < 0)
		return disk_fd;
	disk_fd3 = dup(disk_fd);
}
struct RandomAccessSequence{
	int random_distribution_type;
	int element_count;
	int min_element_value;
	int max_element_value;
	int* sequence;
};
struct RandomAccessSequence* createRandomSequessAccess(int random_distribution_type,int element_count,int max_element_value){
	struct RandomAccessSequence* sequence= (struct RandomAccessSequence*)malloc(sizeof(struct RandomAccessSequence));
	memset(sequence,0,sizeof(struct RandomAccessSequence));
	sequence->random_distribution_type = random_distribution_type;
	sequence->element_count = element_count;
	sequence->max_element_value = 0;
	sequence->max_element_value = max_element_value-1;
	sequence->sequence = (int*)malloc(sizeof(int)*element_count);
	srand(time(NULL));
	for(int i=0;i<element_count;i++){
		int block_number = getRandomValue(random_distribution_type)%(max_element_value-1);//(rand())%30;
		sequence->sequence[i] = block_number;
	}
	return sequence;
}

int main (int argc, char *argv[])
{
	char *buffer;
	int ret;

	init();
	char* tmp_buffer = (char*)malloc(BLOCK_SIZE*1024);
	unsigned long long disk_fd2_start_time = getCurrentTime();
	write(disk_fd_tmp,tmp_buffer,BLOCK_SIZE*1024);
	fsync(disk_fd_tmp);
	unsigned long long disk_fd2_end_time = getCurrentTime();
	
	const unsigned long long disk_fd2_write_time = disk_fd2_end_time-disk_fd2_start_time;

	disk_fd2_start_time = getCurrentTime();
	read(disk_fd_tmp,tmp_buffer,BLOCK_SIZE*1024);
	disk_fd2_end_time = getCurrentTime();
	const unsigned long long disk_fd2_read_time = disk_fd2_end_time-disk_fd2_start_time;

	close(disk_fd_tmp);


	memory_buffer_manager->disk_fd = disk_fd;
	memory_buffer_manager->disk_buffer = disk_buffer;
	buffer = (char*)malloc(memory_buffer_manager->explicit_block_size*sizeof(char)); //aligned_alloc(BLOCK_SIZE,BLOCK_SIZE);
	ret = lib_read(0, buffer);
	printf("nread: %d\n", ret);
	srand(time(NULL));
	
	printf("buffer content: \n");
	for(int i=0;i<100;i++){
		printf("%d ",buffer[i]);
	}
	printf("\n");
	
	for(int i=0;i<BLOCK_SIZE;i++){
			buffer[i] = (rand())%256; //(i)%256;
	}
	
	srand(time(NULL));
	double average_write_on_cache_time_ns = 0.0;
	for(int i=0;i<CONFIG_RANDOM_WRITE_COUNT;i++){
		int block_number = (rand())%(CONFIG_TOTAL_N-1);
		unsigned long long start_time = getCurrentTime();
    	ret = lib_write(block_number, buffer);
		unsigned long long end_time = getCurrentTime();
		average_write_on_cache_time_ns+=(double)(end_time-start_time)/((double)(CONFIG_RANDOM_WRITE_COUNT));
	}
	printf("nwrite: %d\n", ret);
	usleep(3000*1000);
	struct RandomAccessSequence* random_access_sequence = createRandomSequessAccess(CONFIG_RANDOM_DISTIRUBUTION_TYPE,
																					CONFIG_RANDOM_ACCESS_SEQUENCE_ELEMENT_COUNT,
																					CONFIG_RANDOM_ACCESS_SEQUENCE_MAX_VALUE);

	double average_read_time_ns = 0.0;

	for(int i=0;i<random_access_sequence->element_count;i++){
		int block_number = random_access_sequence->sequence[i];
		unsigned long long start_time = getCurrentTime();
		ret = lib_read(block_number, buffer);
		unsigned long long end_time = getCurrentTime();
		average_read_time_ns+=(double)(end_time-start_time)/((double)(random_access_sequence->element_count));
	}

	ret = lib_read(0, buffer);
	
	printf("buffer content after read: \n");
	for(int i=0;i<100;i++){
		printf("%d ",buffer[i]);
	}
	printf("\n");
	
	usleep(5000*1000);
	printf("[buffer.c] 5sec wait finished");
	tp->releaseInternalResource(tp);
	usleep(1000*1000);
	if(USE_MMAP_RW!=0){
		munmap(disk_buffer,CONFIG_TOTAL_N*BLOCK_SIZE);
	}
	printf("[4MB버퍼]순수하게 쓰는데 걸린 시간: %llu\n",disk_fd2_write_time);
	printf("[4MB버퍼]순수하게 읽는데 걸린 시간: %llu\n",disk_fd2_read_time);
	printf("[readhash hit] access avg time: %.8f\n",hashhit_access_time);
	printf("[readhash miss] access avg time: %.8f\n",hashmiss_access_time);
	
	printf("hit count: %llu\n",hit_count);
	printf("total hit ratio: %.3f\n",(double)hit_count/(random_access_sequence->element_count));
	printf("average total read time: %.8f\n",average_read_time_ns);
	printf(" hash func & update time: %.8f\n",pure_hashmap_access_time);
	printf("\tpure hash access time: %.8f\n",memory_buffer_manager->pure_hashmap_get_time);
	printf("cache에 쓰는 총 평균 시간: %.8f\n",average_write_on_cache_time_ns);
	printf("\t순수하게 hashmap에 쓰는 평균 시간: %.8f\n",memory_buffer_manager->pure_hashmap_set_time);
	//close(disk_fd);
    return 0;
}
