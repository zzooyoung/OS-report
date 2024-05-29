#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int var = 0;
int flag[2] = {0, 0};  // flag 1, 2번을 내림
int turn = 0; 
// 공유변수 turn 은 두 쓰레드 T1, T2가 동시에 임계 영역으로 들어가려고 충돌하는 것 방지 


void *func1(void *arg) {                // 1번 thread 실행 함수 
    for (int i=0; i<50; i++) {          // 동일한 양의 숫자 반복 50회
        flag[0] = 1;                    // T0 이 깃발을 들어 임계 영역 진입 의사 표시 
        while(flag[1] == 1){            // 1번이 깃발을 들고 있을 경우 
            if (turn == 1){             // 1번이 하고 있으면 
                flag[0] = 0;            // 깃발을 내려 P1에 진입 순서 양보 
                while (turn == 1) {  
                    // 바쁜 대기 : 임계영역 진입 불가 
                }
                flag[0] = 1;            // 깃발을 들어 임계 영역에 재진입 시도
            }
        }
        /* 임계 영역 */
        ++var;
        printf("[%d번째]thread1: %d X 3 = %d\n", i+1, var, var*3);
        /* 임계 영역 탈출*/
        turn = 1;                       // T1 에 진입순서 제공
        flag[0] = 0;                    // T0의 임계 영역 사용 완료 지정
    }

    pthread_exit(NULL);  // 1번 thread 종료
}
void *func2(void *arg) {  // 2번 thread 실행 함수 
    for (int i=0; i<50; i++) {
        flag[1] = 1;
        while (flag[0] == 1) {
            if (turn == 0) {
                flag[1] = 0;
                while (turn == 0) {
                    // 바쁜 대기
                }
                flag[1] = 1;
            }
        }
        /* 임계 영역 */
        ++var;
        printf("[%d번째]thread2: %d X 3 = %d\n", i+1, var, var*3);

        turn = 0;
        flag[1] = 1;
    }
    /* 임계 영역 */

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
