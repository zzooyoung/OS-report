#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
    Highest Response-ratio Next 스케줄링 알고리즘 구현 입니다. 
    HRN.c
*/

// 프로세스 데이터 준비
int process_ready[5][3] = {  // {도착시간, 실행시간, 우선순위}
    {0, 10},
    {1, 28},
    {2, 6},
    {3, 4},
    {4, 14}
};
double proc_priority[5] = {0, 0, 0, 0, 0};
int process_amont = 5;
// 프로세스 도착 플래그 
int arrive_flag = 0;

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
int execute_func(int proc_num) {
    printf("---P(%d) 실행 시작---\n", proc_num+1);
    int execute_time = process_ready[proc_num][1];
    for(int i = 1; i<=execute_time; i++){
        printf("P%d: %d X %d = %d\n", proc_num+1, i, proc_num+1, (proc_num+1)*i);
        usleep(100000);
    }
    return 0;
}

int wait_func(int proc_num) {
    int execute_time = process_ready[proc_num][1];
    for(int i = 1; i<=execute_time; i++){
        usleep(100000);
    }
    return 0;
}

int main() {
    Queue *queue = createQueue();
    Queue *Gantt_queue = createQueue();
    Queue *queue1 = createQueue();
    Queue *queue2 = createQueue();

    

    // 프로세스 5개 생성
    pid_t pid0, pid1, pid2;
    int proc_num = -1;
    if(pid0 = fork() == 0) {
        if(pid1 = fork() == 0) {
            proc_num = 4;
        }
        else {
            proc_num = 3;
        }
    }
    else{
        if(pid1 = fork() == 0) {
            proc_num = 2;
        }
        else {
            if(pid2 = fork() == 0){
                proc_num = 1;
            }
            else {
                proc_num = 0;
            }
        }
    }
    
    // 도착 순서대로 Enqueueing
    for(int i = 0; i<process_amont; i++){ // 도착 시간 순서대로 enqueue
        for(int j=0; j<process_amont; j++){
            if(process_ready[j][0] == i){
                if(i == 0){
                    enqueue(queue, j);
                }
                else {
                    enqueue(queue2, j);
                }
            }
        }
        
    }
    // 큐에서 차례대로 실행
    int result;
    int run_time_acc=0;
    int tmp;
    while(1){
        result = dequeue(queue);
        if(result == proc_num){
            enqueue(Gantt_queue, result);
            execute_func(result);
        }
        else{
            enqueue(Gantt_queue, result);
            wait_func(result);
            
        }
        proc_priority[result] = -1;
        run_time_acc+=process_ready[result][1];
        int max_pri=0; int max_num;
        int temp;
        while(1){
            tmp = dequeue(queue2);  
            proc_priority[tmp] = (double)(process_ready[tmp][1]+run_time_acc-process_ready[tmp][0])/process_ready[tmp][1];
            // printf("%lf\n", proc_priority[tmp]);
            if(max_pri < proc_priority[tmp]){
                max_pri = proc_priority[tmp];
                max_num = tmp;
            }
            enqueue(queue1, tmp);
            if (isEmpty(queue2)){
                break;
            }
        }
        while(1){
            tmp = dequeue(queue1);
            if(tmp == max_num){
                enqueue(queue, tmp);
            }
            else {
                enqueue(queue2, tmp);
            }
            if(isEmpty(queue1)){
                break;
            }
        }
        
        if(isEmpty(queue2)){
            result = dequeue(queue);
            if(result == proc_num){
                enqueue(Gantt_queue, result);
                execute_func(result);
            }
            else{
                enqueue(Gantt_queue, result);
                wait_func(result);
                
            }
            freeQueue(queue2);
            freeQueue(queue1);
            freeQueue(queue);
            // exit(0);
            break;
        }
    }

    // 프로세스 대기 및 종료
    int retval, status;
    retval = wait(&status);
    if(proc_num != 0) {
        exit(0);
    }
    

    // 간트 차트 출력부
    printf("--- 간트 차트 출력 ---\n");
    Queue *return_queue = createQueue();
    int time_acc = 0;
    int index = 0;
    while(1){
        result = dequeue(Gantt_queue);
        enqueue(return_queue, result);

        printf("P%d (%d-%d)\n", result+1, time_acc, time_acc+process_ready[result][1]);
        time_acc = time_acc + process_ready[result][1];
        
        if(isEmpty(Gantt_queue)){
            freeQueue(Gantt_queue);
            break;
        }
    }

    

    // 프로세스 반환시간 출력부
    printf("--- 프로세스 반환시간 출력 ---\n"); 
    time_acc = 0;
    int wait_time_acc = 0; int return_time_acc = 0;
    index = 0;
    while(1){
        result = dequeue(return_queue);
        printf("P%d 반환시간: %d,   대기시간: %d\n", result+1, (time_acc - process_ready[result][0])+process_ready[result][1], (time_acc - process_ready[result][0]));
        return_time_acc += (time_acc - process_ready[result][0])+process_ready[result][1];
        wait_time_acc = wait_time_acc + (time_acc - process_ready[result][0]);
        time_acc += process_ready[result][1];
        
        if(isEmpty(return_queue)){
            freeQueue(return_queue);
            break;
        }
        index++;
    }
    printf("전체 평균 반환시간 : %.2lf 전체 평균 대기시간 : %.2lf\n", (double)return_time_acc/5 , (double)wait_time_acc/5);


    return 0;
}
