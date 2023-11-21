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
struct MemoryBufferWrapper* memory_buffer_wrapper=NULL;
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
	int ret;

	// implement BUFFERED_WRITE

	ret = lseek(disk_fd, block_nr * BLOCK_SIZE, SEEK_SET); 
	if (ret < 0)
		return ret;

	ret = write(disk_fd, user_buffer, BLOCK_SIZE);
	if (ret < 0)
		return ret;

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
	
	//printf("start setting memory buffer wrapper\n");
	memory_buffer_wrapper =(struct MemoryBufferWrapper*) malloc(sizeof(struct MemoryBufferWrapper));
	int n = 128;
	memory_buffer_wrapper->number_of_memory_buffers = n;
	memory_buffer_wrapper->explicit_block_size = BLOCK_SIZE;
	memory_buffer_wrapper->memory_buffers = (char**)calloc(n,sizeof(char*));
	for(int i=0;i<n;i++){
		memory_buffer_wrapper->memory_buffers[i] = (char*)malloc(sizeof(char)*BLOCK_SIZE);
	}
	memory_buffer_wrapper->readMemoryBuffer = &readMemBuffer;

	disk_buffer = aligned_alloc(BLOCK_SIZE, BLOCK_SIZE);
	if (disk_buffer == NULL)
		return -errno;

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
	if(memory_buffer_wrapper!=NULL){
		printf("n=%d, blksize=%d, total=%d\n, read=%d\n",
		memory_buffer_wrapper->number_of_memory_buffers,
		memory_buffer_wrapper->explicit_block_size,
		memory_buffer_wrapper->explicit_block_size*memory_buffer_wrapper->number_of_memory_buffers,
		((memory_buffer_wrapper->readMemoryBuffer(1234)))
		);
	}
	else{
		printf("memory buffer wrapper is NULL\n");
	}
	buffer = malloc(BLOCK_SIZE);

    ret = lib_read(0, buffer);
	printf("nread: %d\n", ret);

    ret = lib_write(0, buffer);
	printf("nwrite: %d\n", ret);
	bufferedread_exporttest();
	delayedwrite_exporttest();
	replacementpolicy_exporttest();
    return 0;
}
