#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define BLOCK_SIZE 4096
int main(){
    int n;
    char* filename = "diskfile\0";
    printf("n값을 지정해주세요: ");
    scanf("%d",&n);
    if(n<1){
        perror("유효하지 않은 n값 입니다\n");
        return 1;
    }
    printf("생성할 파일의 정보 입니다.\n");
    printf("\tn=%d\n",n);
    printf("\tblock size=%dKB\n",BLOCK_SIZE);
    long long blk_size = BLOCK_SIZE*1024;
    printf("\t파일크기: %lldKB\n",(blk_size/1024)*n);
    char bs_char[256] = {0,};
    char count_char[512] = {0,};
    char file_path_char[1024] = {0,};

    sprintf(bs_char,"bs=%d",(int)BLOCK_SIZE);
    sprintf(count_char,"count=%lld",(long long)n*1024);
    sprintf(file_path_char,"of=%s",filename);
    
    char* argv[] = {"dd","if=/dev/zero",file_path_char,(char*)bs_char,(char*)count_char,NULL};
    execvp(argv[0],argv);
        
    return 0;
}