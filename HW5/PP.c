#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
    Preemptive Priority 스케줄링 알고리즘 구현 입니다. 
    PP.c
*/

// 프로세스 데이터 준비
int process_ready[5][3] = {  // {도착시간, 실행시간, 우선순위}
    {0, 10, 3},
    {1, 28, 2},
    {2, 6, 4},
    {3, 4, 1},
    {4, 14, 2}
};

// 프로세스 도착 플래그 
int arrive_flag = -1;

// 임시 실행 중이던 정보 저장 큐
int tmp_queue[2] = {-1, -1};

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
    int idx = 1;
    
    if(tmp_queue[0] == -1&&tmp_queue[1] == -1) {
        idx = 1;
    }
    else if(tmp_queue[0] == proc_num && tmp_queue[1] != -1) {
        idx = tmp_queue[1] + 1;
        tmp_queue[1] = -1;
    }
    if(arrive_flag != -1){
        execute_time = arrive_flag;
        arrive_flag = -1;
        tmp_queue[0] = proc_num;
        tmp_queue[1] = execute_time;
    }
    for(idx; idx<=execute_time; idx++){
        printf("P%d: %d X %d = %d\n", proc_num+1, idx, proc_num+1, (proc_num+1)*idx);
        usleep(200000);
    }
    return 0;
}

int wait_func(int proc_num) {
    int execute_time = process_ready[proc_num][1];
    int idx = 1;
    
    if(tmp_queue[0] == -1&&tmp_queue[1] == -1) {
        idx = 1;
    }
    else if(tmp_queue[0] == proc_num && tmp_queue[1] != -1) {
        idx = tmp_queue[1] + 1;
        tmp_queue[1] = -1;
    }
    if(arrive_flag != -1){
        execute_time = arrive_flag;
        arrive_flag = -1;
        tmp_queue[0] = proc_num;
        tmp_queue[1] = execute_time;
    }
    for(idx; idx<=execute_time; idx++){
        usleep(200000);
    }
    return 0;
}

int main() {
    Queue *queue = createQueue();
    Queue *Gantt_queue = createQueue();

    

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
    int early_come;
    int priority_tmp;
    for(int i = 0; i<5; i++){  // 비선점 도착 시간 0 인 프로세스
        if(process_ready[i][0] == 0){
            enqueue(queue, i);
            early_come = i;
            priority_tmp = process_ready[i][2];
            break;
        }
    }
    for(int i = 4; i>=0; i--){  // 우선순위가 중복될 경우 도착 시간 순서대로 enqueue
        for(int j = 0; j<5; j++){  
            if(process_ready[j][2]==i){
                if(process_ready[j][0] < process_ready[early_come][1]&&priority_tmp < process_ready[j][2]){
                    enqueue(queue, j);
                    arrive_flag = j;
                    priority_tmp = process_ready[j][2];
                }
                else{
                    enqueue(queue, j);
                    priority_tmp = process_ready[j][2];
                }
            }
        }   
    }

    // 큐에서 차례대로 실행
    int result;
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

        if(isEmpty(queue)){
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
    int buf;
    int count;
    int before_flag[2] = {-1, -1};  // {해당 프로세스, 진행 중이던 count}
    buf = dequeue(Gantt_queue);
    enqueue(return_queue, buf);
    while(1){
        result = dequeue(Gantt_queue);
        enqueue(return_queue, result);
        
        if(before_flag[0] == buf){
            count = process_ready[buf][1]-before_flag[1];
            before_flag[0] = -1;
            before_flag[1] = -1;
        }
        else {
            count = process_ready[buf][1];
        }
        if(process_ready[buf][2]<process_ready[result][2]){
            count = process_ready[result][0];
            before_flag[0] = buf;
            before_flag[1] = count;
        }
        printf("P%d (%d-%d)\n", buf+1, time_acc, time_acc+count);
        time_acc += count;

        buf = result;
        
        if(isEmpty(Gantt_queue)){
            printf("P%d (%d-%d)\n", result+1, time_acc, time_acc+process_ready[result][1]);
            freeQueue(Gantt_queue);
            break;
        }
    }


    // 프로세스 반환시간 출력부
    printf("--- 프로세스 반환시간 출력 ---\n"); 
    time_acc = 0;
    int wait_time_acc = 0; int return_time_acc = 0;
    before_flag[0] = -1;
    before_flag[1] = -1;  // {해당 프로세스, 진행 중이던 count}
    buf = dequeue(return_queue);
    
    while(1){
        result = dequeue(return_queue);
        
        if(process_ready[buf][2]<process_ready[result][2]){
            before_flag[1] = process_ready[result][0];
            time_acc += process_ready[result][0];
            before_flag[0] = buf;
            buf = result;
            continue;
        }
        else if(before_flag[0]==buf){
            time_acc-= before_flag[1];
            before_flag[0] = -1;
            before_flag[1] = -1; 
        }

        printf("P%d 반환시간: %d,   대기시간: %d\n", buf+1, time_acc+process_ready[buf][1]-process_ready[buf][0], time_acc - process_ready[buf][0]);
        
        return_time_acc += time_acc+process_ready[buf][1]-process_ready[buf][0];
        wait_time_acc += time_acc - process_ready[buf][0];
        time_acc = time_acc + process_ready[buf][1];
        
        if(isEmpty(return_queue)){
            printf("P%d 반환시간: %d,   대기시간: %d\n", result+1, time_acc+process_ready[result][1]-process_ready[result][0], time_acc - process_ready[result][0]);
            freeQueue(return_queue);
            break;
        }
        buf = result;
    }
    printf("전체 평균 반환시간 : %.2lf 전체 평균 대기시간 : %.2lf\n", (double)return_time_acc/5 , (double)wait_time_acc/5);


    return 0;
}
