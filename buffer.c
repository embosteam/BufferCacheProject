#define _GNU_SOURCE
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
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
struct MemoryBufferManager* memory_buffer_manager=NULL;
struct ThreadPool* tp = NULL;
int disk_fd;

int os_read(int block_nr, char *user_buffer)
{
	int ret;

	// implement BUFFERED_READ

	struct hashmap* memory_buffers = memory_buffer_manager->memory_buffers;
	struct MemoryBufferController* memory_buffer_controller = memory_buffer_manager->controller;
	struct MemoryBuffer* memory_buffer = memory_buffer_controller->getMemoryBufferMap(memory_buffer_manager,block_nr);
	const long long default_block_size_byte = memory_buffer_manager->explicit_block_size;
	if(memory_buffer!=NULL){
		printf("[os_read] memory buffer is not NULL (at %d)\n",block_nr);
		memcpy(user_buffer, memory_buffer->buffer, memory_buffer->header.block_size_byte);
		return (int)memory_buffer->header.block_size_byte;
	}
	else{
		printf("[os_read] memory buffer is NULL(at %d)\n",block_nr);
		int disk_fd2 = dup(disk_fd);
		if(disk_fd2==-1){
			disk_fd2 =disk_fd;
		}
		ret = lseek(disk_fd2, block_nr * default_block_size_byte, SEEK_SET); 
		if (ret < 0)
			return ret;

		ret = read(disk_fd2, disk_buffer, default_block_size_byte);
		if (ret < 0)
			return ret;
		memory_buffer = createNewMemoryBuffer(default_block_size_byte,block_nr);
		memcpy(memory_buffer->buffer,user_buffer,default_block_size_byte);
		memory_buffer_controller->putMemoryBufferAt(memory_buffer_manager,block_nr,memory_buffer);
		memcpy(user_buffer, disk_buffer, default_block_size_byte);
		
	}

	return ret;
}

int os_write(int block_nr, char *user_buffer)
{
	int ret = 0;
	const long long default_block_size_byte = memory_buffer_manager->explicit_block_size;
	struct hashmap* memory_buffers = memory_buffer_manager->memory_buffers;
	struct MemoryBufferController* memory_buffer_controller = memory_buffer_manager->controller;

	struct MemoryBuffer* buffer = createNewMemoryBuffer(default_block_size_byte,block_nr);
	memcpy(buffer->buffer,user_buffer,default_block_size_byte);
	printf("[os_write] membuffer: %x , blknm: %d\n",buffer, buffer->header.block_number);
	tp->addQueue(tp,disk_fd,buffer);
	memory_buffer_controller->putMemoryBufferAt(memory_buffer_manager,block_nr,buffer);
	/*
	// implement BUFFERED_WRITE

	ret = lseek(disk_fd, block_nr * BLOCK_SIZE, SEEK_SET); 
	if (ret < 0)
		return ret;

	ret = write(disk_fd, user_buffer, BLOCK_SIZE);
	if (ret < 0)
		return ret;
	*/
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
	int n = 1024;
	memory_buffer_manager  = createNewMemoryBufferManager(n,(int)(0.3f*(float)n),(long long)BLOCK_SIZE*1024);
	const long long default_block_size_byte = memory_buffer_manager->explicit_block_size;
	printf("before tp\n");
	tp = newThreadPool(5);
	printf("after tp\n");
	disk_buffer =  aligned_alloc(default_block_size_byte, default_block_size_byte);
	if (disk_buffer == NULL){
		return -errno;
	}

	printf("disk_buffer: %p\n", disk_buffer);
	#ifdef __APPLE__
		disk_fd = open("diskfile", O_RDWR);
	#else
		disk_fd = open("diskfile", O_RDWR|O_DIRECT);
	#endif
	#ifdef __APPLE__
		fcntl(disk_fd,F_SETFL,F_NOCACHE);
	#endif
	if (disk_fd < 0)
		return disk_fd;
	
}

int main (int argc, char *argv[])
{
	char *buffer;
	int ret;

	init();
	printf("disk_fd:%d\n",disk_fd);
	if(memory_buffer_manager!=NULL){
		printf("n=%d,mn=%d, blksize=%d, total=%d\n",
		memory_buffer_manager->total_number_of_memory_buffers,
		memory_buffer_manager->manageable_number_of_memory_buffers,
		memory_buffer_manager->explicit_block_size,
		memory_buffer_manager->explicit_block_size*memory_buffer_manager->total_number_of_memory_buffers
		//((memory_buffer_manager->readMemoryBuffer(1234)))
		);
	}
	else{
		printf("memory buffer wrapper is NULL\n");
	}
	buffer = (char*)malloc(memory_buffer_manager->explicit_block_size*sizeof(char)); //aligned_alloc(BLOCK_SIZE,BLOCK_SIZE);
	ret = lib_read(0, buffer);
	printf("nread: %d\n", ret);
	srand(time(NULL));
	
	printf("buffer content: \n");
	for(int i=0;i<100;i++){
		printf("%d ",buffer[i]);
	}
	printf("\n");
	uint8_t md5_result[16];
	md5String(buffer,md5_result);
	printf("md5 result: ");
	for(int i=0;i<16;i++){
		printf("%02x",md5_result[i]);
	}
	printf("\n");

    //ret = lib_read(0, buffer);
	//printf("nread: %d\n", ret);
	for(int i=0;i<BLOCK_SIZE;i++){
			buffer[i] = (rand())%256; //(i)%256;
	}
	for(int i=0;i<30;i++){
    	ret = lib_write(i, buffer);
	}
	printf("nwrite: %d\n", ret);

	
	ret = lib_read(0, buffer);
	printf("buffer content after read: \n");
	for(int i=0;i<100;i++){
		printf("%d ",buffer[i]);
	}
	printf("\n");
	
	
	bufferedread_exporttest();
	delayedwrite_exporttest();
	replacementpolicy_exporttest();
	usleep(4000*1000);
	tp->releaseInternalResource(tp);
	//close(disk_fd);
    return 0;
}
