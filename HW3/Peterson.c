#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

int var = 0;
int flag[2] = {0, 0};  // flag 1, 2번을 내림
int turn = 0; 
// 공유변수 turn 은 두 쓰레드 T1, T2가 동시에 임계 영역으로 들어가려고 충돌하는 것 방지 


void *func1(void *arg) {
    for (int i=0; i<50; i++) {
        flag[0] = TRUE;
        turn = 1;
        while (flag[1] == TRUE && turn == 1) {
            // 바쁜 대기 
        }
        ++var;
        printf("[%d번째]thread1: %d X 3 = %d\n", i+1, var, var*3);
        flag[0] = FALSE;
    }
    pthread_exit(NULL);  // 1번 thread 종료
}
void *func2(void *arg) {  // 2번 thread 실행 함수 
    for(int i=0; i<50; i++) {
        flag[1] = TRUE;
        turn = 0;
        while (flag[0] == TRUE && turn == 0) {
            // 바쁜 대기
        }
        ++var;
        printf("[%d번째]thread2: %d X 3 = %d\n", i+1, var, var*3);
        flag[1] = FALSE;
    }
    pthread_exit(NULL);  // 2번 thread 종료 
}

int main() {
    pthread_t tid1, tid2;

    if(pthread_create(&tid1, NULL, func1, NULL) != 0){
        fprintf(stderr, "thread create error\n");
        exit(1);
    }

    if(pthread_create(&tid2, NULL, func2, NULL) != 0) {
        fprintf(stderr, "thread create error\n");
        exit(1);
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    return 0;
}
