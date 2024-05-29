#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int var = 0;
int turn = 0; 

int proc_amount = 5;

int process[5][2] = {{0, 10}, {1, 28}, {2, 6}, {3, 4}, {4, 14}};

int time_clock=0;

int seq_count[5];
int turn_count = 0;
struct node
{
    int num;
    int arrive_time;
    int running_time;
    pthread_t tid;
    struct node *next;

};
typedef struct node Node;

Node *head = NULL;

void *func(void *arg) {  
    int proc_num = *((int*)arg) - 1;
    printf("%d번 프로세스 시작이용\n", proc_num+1);
    while(seq_count[turn_count]==proc_num+1){
        // 바쁜 대기 
    }
    printf("%d랑 %d 요고 빠져나옴 !", seq_count[turn_count], proc_num+1);
    for (int idx = 1; idx<=process[proc_num][1]; idx++){
        printf("P%d:%d X %d = %d\n", proc_num+1, idx, proc_num+1, idx*(proc_num+1));
    }
    seq_count[turn_count] = 0;
    turn_count++;
    pthread_exit(NULL);  
}




int main() {
    pthread_t tid1, tid2, tid3, tid4, tid5;
    Node proc1, proc2, proc3, proc4, proc5;

    for(int i=0;i<proc_amount;i++) {
        seq_count[i] = 0;
    }

    head = &proc1;
    // Linked List 
    proc1.num = 1;
    proc1.tid = tid1;
    proc1.arrive_time = process[0][0];
    proc1.running_time = process[0][1];
    proc1.next = &proc2;

    proc2.num = 2;
    proc2.tid = tid2;
    proc2.arrive_time = process[1][0];
    proc2.running_time = process[1][1];
    proc2.next = &proc3;

    proc3.num = 3;
    proc3.tid = tid3;
    proc3.arrive_time = process[2][0];
    proc3.running_time = process[2][1];
    proc3.next = &proc4;

    proc4.num = 4;
    proc4.tid = tid4;
    proc4.arrive_time = process[3][0];
    proc4.running_time = process[3][1];
    proc4.next = &proc5;
    
    proc5.num = 5;
    proc5.tid = tid5;
    proc5.arrive_time = process[4][0];
    proc5.running_time = process[4][1];
    proc5.next = NULL;

    Node *curNode = head;
    Node *tmp; 
    Node *zeroNode;

    // 도착시간 0 인 프로세스 체크 zeroNode
    int zero_flag = 0; 
    while(1){  
        if(head->arrive_time == 0) {
            zero_flag = 1;
            zeroNode = head; 
            curNode = head->next;
            head = curNode;
            break;
        }
        else if(curNode->next->arrive_time == 0){
            zero_flag = 1;
            if(curNode->next->next==NULL){
                zeroNode = curNode->next->next;
                curNode->next = NULL;
            }
            else {
                zeroNode = curNode->next;
                curNode->next = curNode->next->next;
            }
            break;
            
        }
        else{
            if (curNode->next == NULL){
                break; 
            }
            else {
                curNode = curNode->next;
            }
        }
    
    }
    
    if (zero_flag == 1){ // 비선점 프로세스 도착시간 0 인 경우 바로 시작
        if(pthread_create(&(zeroNode->tid), NULL, func, &(zeroNode->num)) != 0){
            fprintf(stderr, "thread create error\n");
            exit(1);
        }
        seq_count[0]=zeroNode->num;
        proc_amount--;
    }

    // 나머지 프로세스 FCFS 에 맞게 정렬
    curNode = head;
    for (int i=0; i<proc_amount-1; i++){
        if (curNode->running_time > curNode->next->running_time){ // curNode 는 head->next
            head = curNode->next;
        }
        for(int j=0;j<proc_amount-i;j++){
            if (curNode->next==NULL){
                break;
            }
            else if (curNode->running_time > curNode->next->running_time){
                curNode->next = curNode->next->next;
                curNode->next->next = curNode;
            }
            curNode = curNode->next;
        }
        if (i==0){
            curNode->next = NULL;
        }
        curNode = head;
    }
    while(1){
        if (curNode->next!=NULL){
            curNode = curNode->next;
        }
        else {
            break;
        }
    }
    // 프로세스 생성
    while(1) {
        if(pthread_create(&(curNode->tid), NULL, func, &(curNode->num)) != 0){
            fprintf(stderr, "thread create error\n");
            exit(1);
        }
        
        if(curNode->next!=NULL){
            curNode = curNode->next; 
        }
        else{break;}
    }

    // 프로세스 실행 버퍼 
    curNode = head;
    int idx=0;
    while(1){
        if(seq_count[idx]!=0){
            idx++;
        }
        else {
            seq_count[idx] = curNode->num;
            if(curNode->next != NULL){
                curNode = curNode->next;
            }
            else{break;}
        } 
        if(idx==5){
            break;
        }
    }
    printf("%d -> %d -> %d -> %d -> %d\n",seq_count[0], seq_count[1], seq_count[2], seq_count[3], seq_count[4]);

    int count = 0;
    curNode = head;

    // 쓰레드 종료 대기
    if(zero_flag == 1){
        pthread_join(zeroNode->tid, NULL);
    }
    else {
        while(1){
            pthread_join(curNode->tid, NULL);
            if(curNode->next!=NULL){
                break;
            }
            else{
                curNode = curNode->next;
            }
        }
        
    }
    
    // 간트 차트 출력
    if(zero_flag == 1) {
        int wait_timer=0;
        printf("---간트차트---\n");
        printf("P%d (%d-%d)\n", zeroNode->num, wait_timer, zeroNode->running_time);
        wait_timer = wait_timer + zeroNode->running_time;
        while(1){
            if(wait_timer>curNode->arrive_time){  // 도착시간이 앞 프로세스 종료시간 보다 짧을 경우 
                printf("P%d (%d-%d)\n", curNode->num, wait_timer, wait_timer+curNode->running_time);
                wait_timer = wait_timer + zeroNode->running_time;
            }
            else{                                // 도착시간이 앞 프로세스 종료시간보다 길 경우
                printf("P%d (%d-%d)\n", curNode->num, curNode->arrive_time, curNode->arrive_time+curNode->running_time);
                wait_timer = curNode->arrive_time + curNode->running_time;
            }
            if(curNode->next==NULL){
                break;
            }        
            else {
                curNode = curNode->next;
            }  
        }
    }
    pthread_join(zeroNode->tid, NULL);

    return 0;
}
