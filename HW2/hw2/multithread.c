#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

#define PROCESS1 1
#define PROCESS2 2
#define PROCESS3 3
#define PROCESS4 4

int proc_num;
int proc_num_cal;

void *func1(void *arg) { // 1번 thread 실행 함수
    int count = 1;
    while (count <= 25) {
        printf("Process%d thread1 : %d X %d : %d\n", proc_num, count, proc_num_cal, count*proc_num_cal);
        count++;
    }
    pthread_exit(NULL); // 1번 thread 종료
}
void *func2(void *arg) { // 2번 thread 실행 함수
    int count = 26;
    while (count <= 50) {
        printf("Process%d thread2 : %d X %d : %d\n", proc_num, count, proc_num_cal, count*proc_num_cal);
        count++;
    }
    pthread_exit(NULL); // 2번 thread 종료
}
void *func3(void *arg) { // 3번 thread 실행 함수
    int count = 51;
    while (count <= 75) {
        printf("Process%d thread3 : %d X %d : %d\n", proc_num, count, proc_num_cal, count*proc_num_cal);
        count++;
    }
    pthread_exit(NULL); // 3번 thread 종료
}
void *func4(void *arg) { // 4번 thread 실행 함수
    int count = 76;
    while (count <= 100) {
        printf("Process%d thread4 : %d X %d : %d\n", proc_num, count, proc_num_cal, count*proc_num_cal);
        count++;
    }
    pthread_exit(NULL); // 4번 thread 종료
}

int main(void) {

    clock_t start_clock = clock();

    pid_t pid1, pid2, pid3, pid4;
    pthread_t tid1, tid2, tid3, tid4;
    int status;

    if((pid1=fork()) == 0){ // 0이면 자식프로세스, 아니면 부모
        if((pid2=fork()) == 0){
            proc_num = PROCESS4;
        }
        else {
            proc_num = PROCESS3;
        }
    }
    else {
        if((pid2=fork()) == 0) {
            proc_num = PROCESS2;
        }
        else {
            proc_num = PROCESS1;
        }
    }

    proc_num_cal = 3;
    for (int i=1;i<proc_num;i++){
        proc_num_cal += 2;
    }

    if(pthread_create(&tid1, NULL, func1, NULL) != 0){
        fprintf(stderr, "thread1 create error\n");
        exit(1);
    }
    if(pthread_create(&tid2, NULL, func2, NULL) != 0){
        fprintf(stderr, "thread2 create error\n");
        exit(1);
    }
    if(pthread_create(&tid3, NULL, func3, NULL) != 0){
        fprintf(stderr, "thread3 create error\n");
        exit(1);
    }
    if(pthread_create(&tid4, NULL, func4, NULL) != 0){
        fprintf(stderr, "thread4 create error\n");
        exit(1);
    }

    

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);

    if (proc_num == PROCESS1) {
        while ((wait(&status)) > 0); // 부모 프로세스가 모든 자식 프로세스의 종료를 기다림
    }
    else {
        exit(0);
    }

    clock_t end_clock = clock();
    printf("소요 시간: %lf\n", (double)(end_clock-start_clock)/ CLOCKS_PER_SEC);

    return 0;
}

