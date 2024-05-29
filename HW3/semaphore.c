#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>

sem_t semaphore;
int num = 1; // 세마포 초기값 1로 설정 
int var = 0;

void *threadF1(void *org)
{
    for (int i=0;i<25;i++){
        if(num != 0)
        {
            num--;
            sem_wait(&semaphore);
            var++;
            printf("[%d번째]thread1: %d X 3 = %d\n", i+1, var, var*3);
            num++;
            sem_post(&semaphore);
        }

        else if(num == 0)
        {
            while(num == 0){}
            var++;
            printf("[%d번째]thread1: %d X 3 = %d\n", i+1, var, var*3);
            num++;
            sem_post(&semaphore);
        }
    }
    
}

void *threadF2(void *arg)
{
    for (int i=0;i<25;i++){
        if(num != 0)
        {
            num--;
            sem_wait(&semaphore);
            var++;
            printf("[%d번째]thread2: %d X 3 = %d\n", i+1, var, var*3);
            num++;
            sem_post(&semaphore);
        }

        else if(num == 0)
        {
            while(num == 0){}
            var++;
            printf("[%d번째]thread2: %d X 3 = %d\n", i+1, var, var*3);
            num++;
            sem_post(&semaphore);
        }
    }
}

void *threadF3(void *arg)
{
    for (int i=0;i<25;i++){
        if(num != 0)
        {
            num--;
            sem_wait(&semaphore);
            var++;
            printf("[%d번째]thread3: %d X 3 = %d\n", i+1, var, var*3);
            num++;
            sem_post(&semaphore);
        }

        else if(num == 0)
        {
            while(num == 0){}
            var++;
            printf("[%d번째]thread3: %d X 3 = %d\n", i+1, var, var*3);
            num++;
            sem_post(&semaphore);
        }
    }
}

void *threadF4(void *arg)
{
    for (int i=0;i<25;i++){
        if(num != 0)
        {
            num--;
            sem_wait(&semaphore);
            var++;
            printf("[%d번째]thread4: %d X 3 = %d\n", i+1, var, var*3);
            num++;
            sem_post(&semaphore);
        }

        else if(num == 0)
        {
            while(num == 0){}
            var++;
            printf("[%d번째]thread4: %d X 3 = %d\n", i+1, var, var*3);
            num++;
            sem_post(&semaphore);
        }
    }
}

int main(){
    pthread_t thread1, thread2, thread3;

    sem_init(&semaphore, 0, 1);

    //스레드 생성
    pthread_create(&thread1, NULL, threadF1, NULL);
    pthread_create(&thread2, NULL, threadF2, NULL);
    pthread_create(&thread3, NULL, threadF3, NULL);
    pthread_create(&thread3, NULL, threadF4, NULL);

    //스레드 조인
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);


    // 세마포 객체 소멸
    sem_destroy(&semaphore);

    return 0;
}