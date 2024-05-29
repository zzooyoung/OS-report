#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>


void printCount(int start_num){
    for(int i = start_num;i<start_num+125;i++){
        printf("%d\n", i*7);
    }
}

int main(void){
    int start = 1;
    int count = 0;
    int max_num = 1000;
    int stage = max_num/8;

    clock_t start_clock = clock();

    int status;
    
    pid_t pid;
    pid_t pid1;
    pid_t pid2;
    if(pid=fork()==0){
        if(pid1=fork()==0){
            if(pid2=fork()==0){
                printCount(start);
                exit(0);
            }
            else{
                printCount(start+stage*1);
                exit(0);
            }
        }
        else{
            if(pid2=fork()==0){
                printCount(start+stage*2);
                exit(0);
            }
            else{
                printCount(start+stage*3);
                exit(0);
            }
        }
    }
    else{
        if(pid1=fork()==0){
            if(pid2=fork()==0){
                printCount(start+stage*4);
                exit(0);
            }
            else{
                printCount(start+stage*5);
                exit(0);
            }
        }
        else{
            if(pid2=fork()==0){
                printCount(start+stage*6);
                exit(0);
            }
            else{
                printCount(start+stage*7);
            }
        }
    }


    for(int i=0;i<8;i++){
        wait(NULL);
    }

    clock_t end_clock = clock();
    printf("소요 시간: %lf\n", (double)(end_clock-start_clock)/ CLOCKS_PER_SEC);

    return 0;
}