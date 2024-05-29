#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
    Multilevel Feedback Queue 스케줄링 알고리즘 구현 입니다. 
    MLFQ.c
*/

// 프로세스 데이터 준비
int process_ready[3] = {  // {도착시간, 실행시간, 우선순위}
    30, 20, 10
};
const int queue_rule_time[3] = {1, 2, 4};

// 프로세스 누산기 
int process_acc[3] = {0, 0, 0};

int queue_isEmpty[3] = {0, 0, 0}; 
int last_chk[3] = {0, 0, 0};

// 노드 구조체 정의
typedef struct Node {
    int data;
    struct Node *next;
} Node;

// 큐 구조체 정의
typedef struct Queue {
    Node *front;
    Node *rear;
} Queue;

// 큐 초기화 함수
Queue* createQueue() {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->front = queue->rear = NULL;
    return queue;
}

// 큐에 요소 추가 (enqueue) 함수
void enqueue(Queue *queue, int data) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (queue->rear == NULL) {
        queue->front = queue->rear = newNode;
        return;
    }

    queue->rear->next = newNode;
    queue->rear = newNode;
}

// 큐에서 요소 제거 (dequeue) 함수
int dequeue(Queue *queue) {
    if (queue->front == NULL) {
        printf("Queue is empty\n");
        return -1;
    }

    Node *temp = queue->front;
    int data = temp->data;
    queue->front = queue->front->next;

    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    return data;
}

// 큐가 비어있는지 확인하는 함수
int isEmpty(Queue *queue) {
    return queue->front == NULL;
}

// 큐의 앞 요소를 확인하는 함수
int front(Queue *queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty\n");
        return -1;
    }
    return queue->front->data;
}

// 큐 메모리 해제 함수
void freeQueue(Queue *queue) {
    while (!isEmpty(queue)) {
        dequeue(queue);
    }
    free(queue);
}

// 프로세스 실행 함수
int execute_func(int proc_num, int q_num) {
    printf("---P(%d) 실행 시작---\n", proc_num+1);
    int end_count; int retval;
    if(process_acc[proc_num]+queue_rule_time[q_num]>=process_ready[proc_num]){
        end_count=process_ready[proc_num];
        retval = 0;
    }
    else {
        end_count = process_acc[proc_num] + queue_rule_time[q_num];
        process_acc[proc_num] += queue_rule_time[q_num];
        retval = 1;
    }
    for(int i = process_acc[proc_num]; i<=end_count;i++){
        printf("P%d: %d X %d = %d\n", proc_num+1, i, proc_num+1, i*(proc_num+1));
        usleep(100000);
    }
    return retval;
}

int wait_func(int proc_num, int q_num) {
    int end_count; int retval;
    if(process_acc[proc_num]+queue_rule_time[q_num]>=process_ready[proc_num]){
        end_count=process_ready[proc_num];
        retval = 0;
    }
    else {
        end_count = process_acc[proc_num] + queue_rule_time[q_num];
        process_acc[proc_num] += queue_rule_time[q_num];
        retval = 1;
    }
    for(int i = process_acc[proc_num]; i<=end_count;i++){
        // printf("P%d: %d X %d = %d\n", proc_num, i, proc_num, i*proc_num);
        usleep(100000);
    }
    return retval;
}

int turn_chk[3]={0, 0, 0};

int main() {
    Queue *queue1 = createQueue();
    Queue *queue2 = createQueue();
    Queue *queue3 = createQueue();
    Queue *Gantt_queue = createQueue();

    int run_time_acc = 0;
    // 프로세스 3개 생성
    pid_t pid0, pid1, pid2;
    int proc_num = -1;
    if(pid0 = fork() == 0) {
        if(pid1 = fork() == 0){
            proc_num = 2;
        }
        else {
            proc_num = 1;
        }
    }
    else{
        proc_num = 0;
    }

    // P1 -> P2 -> P3 순서대로 Enqueueing

    for(int i = 0; i<3; i++){
        enqueue(queue1, i);
    }
    int alone_chk = -1;

    while(1) {
        // Queue1 에서 차례대로 실행
        int result;
        int queue_num = 0;
        enqueue(Gantt_queue, queue_num);
        while(1){
            result = dequeue(queue1);
            turn_chk[0]++;
            if(result == proc_num){
                if(execute_func(result, queue_num) == 1){ // 1 이면 진행 0 이면 끝
                    if(process_ready[result] - process_acc[result]>1){
                        if(alone_chk == result) {
                            enqueue(queue1, result);
                            alone_chk = result;
                        }
                        enqueue(queue2, result);
                    }
                    else {
                        enqueue(queue1, result);
                    }
                }
            }
            else{
                if(wait_func(result, queue_num) == 1){ // 1 이면 진행 0 이면 끝
                    process_acc[result] += queue_rule_time[queue_num];
                    if(process_ready[result] - process_acc[result]>1){
                        enqueue(queue2, result);
                    }
                    else {
                        enqueue(queue1, result);
                    }
                }
                else {
                    process_acc[result] += queue_rule_time[queue_num];
                }
            }
            if(isEmpty(queue1)){
                break;
            }
        } 

        // Queue 2 에서 실행 
        queue_num = 1;
        enqueue(Gantt_queue, queue_num);
        while(1){
            result = dequeue(queue2);
            turn_chk[1]++;
            if(result == proc_num){
                if(execute_func(result, queue_num) == 1){ // 1 이면 진행 0 이면 끝
                    if(process_ready[result] - process_acc[result]>2){
                        enqueue(queue3, result);
                    }
                    else if(1<process_ready[result] - process_acc[result]<=2){
                        enqueue(queue2, result);
                    }
                    else {
                        enqueue(queue1, result);
                    }
                }
            }
            else{
                if(wait_func(result, queue_num) == 1){ // 0 이면 진행 1 이면 끝
                    if(process_ready[result] - process_acc[result]>2){
                        enqueue(queue3, result);
                    }
                    else if(1<process_ready[result] - process_acc[result]<=2){
                        enqueue(queue2, result);
                    }
                    else {
                        enqueue(queue1, result);
                    }
                }
            }
            if(isEmpty(queue2)){
                break;
            }
        }

        // Queue 3 에서 실행 
        alone_chk = 0;
        queue_num = 2;
        enqueue(Gantt_queue, queue_num);
        while(1){
            result = dequeue(queue3);
            turn_chk[2]++;
            if(result == alone_chk && isEmpty(queue2) && isEmpty(queue1)) { // 전체 큐에 1개의 프로세스만 남아 있을 경우 
                enqueue(queue1, result);
                break;
            }
            if(result == proc_num){
                if(execute_func(result, queue_num) == 0){ // 0 이면 진행 1 이면 끝
                    if(process_ready[result] - process_acc[result]>2){
                        enqueue(queue3, result);
                        alone_chk = result;
                    }
                    else if(1<process_ready[result] - process_acc[result]<=2){
                        enqueue(queue2, result);
                    }
                    else {
                        enqueue(queue1, result);
                    }
                }
            }
            else{
                if(wait_func(result, queue_num) == 0){ // 0 이면 진행 1 이면 끝
                    if(process_ready[result] - process_acc[result]>2){
                        enqueue(queue3, result);
                    }
                    else if(1<process_ready[result] - process_acc[result]<=2){
                        enqueue(queue2, result);
                    }
                    else {
                        enqueue(queue1, result);
                    }
                }
            }
            if(isEmpty(queue2)){
                break;
            }
        }
        if(isEmpty(queue1) && isEmpty(queue2) && isEmpty(queue3)) {
            break;
        }
    }

    //
    freeQueue(queue1);
    freeQueue(queue2);
    freeQueue(queue3);
    // 프로세스 대기 및 종료
    int retval, status;
    retval = wait(&status);
    if(proc_num != 0) {
        exit(0);
    }
    

    // 간트 차트 출력부
    printf("--- 간트 차트 출력 ---\n"); 
    int time_acc = 0;
    int index = 0;
    int count;
    int result;
    int run_acc[3] = {0, 0, 0};


    while(1){
        result = dequeue(Gantt_queue);
        printf("Q%d: ", result+1);
        for(int count=0; count<turn_chk[result]/queue_rule_time[result];count++){
            for(int i = 0; i<3; i++){
                if(run_acc[i]+ queue_rule_time[i]<process_ready[i])
                {
                    printf("P%d (%d-%d),", i+1, time_acc, time_acc+queue_rule_time[result]);
                    time_acc += queue_rule_time[result];
                    run_acc[i] += queue_rule_time[result];
                }
                else {
                    printf("P%d (%d-%d),", i+1, time_acc, process_ready[i]);
                    run_acc[i] += process_ready[i] - run_acc[i];
                }
            }
        }

    }
    
    

    // // 프로세스 반환시간 출력부
    // printf("--- 프로세스 반환시간 출력 ---\n"); 
    // time_acc = 0;
    // int wait_time_acc = 0; int return_time_acc = 0;
    // index = 0;
    // while(1){

    //     result = dequeue(return_queue);

    //     printf("P%d 반환시간: %d,   대기시간: %d\n", result+1, acc[result]+(process_ready[result][1]%time_quantum) - process_ready[result][0], acc[result]-((process_ready[result][1]/time_quantum)*5)-process_ready[result][0]);
    //     return_time_acc += acc[result]+(process_ready[result][1]%time_quantum) - process_ready[result][0];
    //     wait_time_acc += acc[result]-((process_ready[result][1]/time_quantum)*5)-process_ready[result][0];
        
    //     if(isEmpty(return_queue)){
    //         freeQueue(return_queue);
    //         break;
    //     }
    //     index++;
    // }
    // printf("전체 평균 반환시간 : %.2lf 전체 평균 대기시간 : %.2lf\n", (double)return_time_acc/5 , (double)wait_time_acc/5);


    return 0;
}
