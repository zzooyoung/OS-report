#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define idle 0
#define wantIn 11
#define inCS 22



int var = 0;
int flag[4] = {0, 0, 0, 0};  // flag 1, 2번을 내림
int turn = 0; 
// 공유변수 turn 은 두 쓰레드 T1, T2가 동시에 임계 영역으로 들어가려고 충돌하는 것 방지 


void *func1(void *arg) {           // 1번 thread 실행 함수 
    for (int i=0; i<25; i++) {
        int j;
        do {
        /* 임계 지역 진입시도 1단계 */
        flag[0] = wantIn;
        while (turn != 0) {
            if (flag[turn] == 0) {
                turn = 0;
            }
        }

        /* 임계 지역 진입시도 2단계 */
        flag[0] = inCS;
        j = 0;
        while ((j<4) && (j==0||flag[j]!=inCS)){
            j = j+1;
        }
        } while(j < 4);
        ++var;
        printf("[%d번째]thread1: %d X 3 = %d\n", i+1, var, var*3);

        flag[0] = idle;
    }
    pthread_exit(NULL);  // 1번 thread 종료
}
void *func2(void *arg) {  // 2번 thread 실행 함수 
    for (int i=0; i<25; i++) {
        int j;
        do {
            /* 임계 지역 진입시도 1단계 */
        flag[1] = wantIn;
        while (turn != 1) {
            if (flag[turn] == idle) {
                turn = 1;
            }
        }

        /* 임계 지역 진입시도 2단계 */
        flag[1] = inCS;
        j = 0;
        while ((j<4) && (j==1||flag[j]!=inCS)){
            j = j+1;
        }
        } while(j < 4);
        ++var;
        printf("[%d번째]thread2: %d X 3 = %d\n", i+1, var, var*3);

        flag[1] = idle;
    }
    pthread_exit(NULL);  // 2번 thread 종료 
}
void *func3(void *arg) {  // 3번 thread 실행 함수 
    for (int i=0; i<25; i++) {
        int j;
        do {
            /* 임계 지역 진입시도 1단계 */
        flag[2] = wantIn;
        while (turn != 0) {
            if (flag[turn] == idle) {
                turn = 0;
            }
        }

        /* 임계 지역 진입시도 2단계 */
        flag[2] = inCS;
        j = 0;
        while ((j<4) && (j==2||flag[j]!=inCS)){
            j = j+1;
        }
        } while(j < 4);
        ++var;
        printf("[%d번째]thread3: %d X 3 = %d\n", i+1, var, var*3);

        flag[2] = idle;
    }

    pthread_exit(NULL);  // 3번 thread 종료 
}
void *func4(void *arg) {  // 4번 thread 실행 함수 
    for (int i=0; i<25; i++) {
        int j;
        do {
            /* 임계 지역 진입시도 1단계 */
        flag[3] = wantIn;
        while (turn != 0) {
            if (flag[turn] == idle) {
                turn = 0;
            }
        }

        /* 임계 지역 진입시도 2단계 */
        flag[3] = inCS;
        j = 0;
        while ((j<4) && (j==3||flag[j]!=inCS)){
            j = j+1;
        }
        } while(j < 4);
        ++var;
        printf("[%d번째]thread4: %d X 3 = %d\n", i+1, var, var*3);

        flag[3] = idle;
    }
    
    pthread_exit(NULL);  // 4번 thread 종료 
}

int main() {
    pthread_t tid1, tid2, tid3, tid4;

    if(pthread_create(&tid1, NULL, func1, NULL) != 0){
        fprintf(stderr, "thread create error\n");
        exit(1);
    }

    if(pthread_create(&tid2, NULL, func2, NULL) != 0) {
        fprintf(stderr, "thread create error\n");
        exit(1);
    }

    if(pthread_create(&tid3, NULL, func3, NULL) != 0) {
        fprintf(stderr, "thread create error\n");
        exit(1);
    }

    if(pthread_create(&tid4, NULL, func4, NULL) != 0) {
        fprintf(stderr, "thread create error\n");
        exit(1);
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);

    return 0;
}
