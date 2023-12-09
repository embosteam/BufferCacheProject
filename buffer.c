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

	ret = lseek(disk_fd, block_nr * BLOCK_SIZE, SEEK_SET); 
	if (ret < 0)
		return ret;

	ret = read(disk_fd, disk_buffer, BLOCK_SIZE);
	if (ret < 0)
		return ret;

	memcpy(user_buffer, disk_buffer, BLOCK_SIZE);

	return ret;
}

int os_write(int block_nr, char *user_buffer)
{
	int ret = 0;
	struct MemoryBuffer* buffer = createNewMemoryBuffer(BLOCK_SIZE,block_nr);
	memcpy(buffer->buffer,user_buffer,buffer->header.block_size_byte);
	tp->addQueue(tp,disk_fd,buffer);
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
	tp = newThreadPool(5);
	disk_buffer = aligned_alloc(BLOCK_SIZE, BLOCK_SIZE);
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
	buffer = (char*)malloc(1024*BLOCK_SIZE*sizeof(char)); //aligned_alloc(BLOCK_SIZE,BLOCK_SIZE);
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
    ret = lib_write(0, buffer);
	printf("nwrite: %d\n", ret);

	

	printf("buffer content after read: \n");
	for(int i=0;i<100;i++){
		printf("%d ",buffer[i]);
	}
	printf("\n");
	
	
	bufferedread_exporttest();
	delayedwrite_exporttest();
	replacementpolicy_exporttest();
	//usleep(1000*1000);
	tp->releaseInternalResource(tp);
	close(disk_fd);
    return 0;
}
