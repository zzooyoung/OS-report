#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
    LRU page replacement algorithm 구현
    LRU.c
*/


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


int main() {
    Queue *queue = createQueue();
    Queue *queue_for_print = createQueue();


    int ref_idx[20] = {1, 2, 3, 2, 1, 5, 2, 1, 6, 2, 5, 6, 3, 1, 3, 6, 1, 2, 4, 3};

    int frame[3] = {0, 0, 0};
    
    int frame_num = 0;
    int idx = 0;
    int tmp;
    int page_fault_flag;
    int match;
    printf("출력1: 각 시간에 각 프레임의 stack 상태\n");
    while(idx<20) {
        page_fault_flag = -1;
        for(int i=0; i<3; i++){
            if(frame[i] == 0){
                if(i==1) {
                    frame[i] = frame[i-1];
                }
                if(i==2){
                    frame[i] = frame[i-1];
                    frame[i-1] = frame[i-2];
                }
                page_fault_flag = i;
                break;
            }
            
            else if(frame[i] == ref_idx[idx]){
                match = i;
                break;
            }
            if(i==2) {
                page_fault_flag = -2;
            }
        }
        if(page_fault_flag == -2){      // -2 -> 페이지 부재
            frame[3] = frame[2];  // Stack First Served 부분 버림
            frame[2] = frame[1];
            frame[1] = ref_idx[idx];
            enqueue(queue_for_print, idx);
        }
        else if(page_fault_flag != -1){                               // else -> 큐가 빈 상태 
            frame[0] = ref_idx[idx];
            enqueue(queue_for_print, idx);
        }
        else {
            if(match == 2){
                int tmp = frame[2];
                frame[2] = frame[1];
                frame[1] = frame[0];
                frame[0] = tmp;
            }
            else if (match == 1){
                int tmp = frame[1];
                frame[1] = frame[0];
                frame[0] = tmp;
            }
        }

        printf("%d 회차 : %d | %d | %d \n", idx+1, frame[0], frame[1], frame[2]);
        idx++;
    }

    if(isEmpty(queue)){
        freeQueue(queue);
    }

    printf("출력2: 페이지 부재 현상이 나타는 참조 문자열의 인덱스\n");
    while(1) {
        if(!isEmpty(queue_for_print)){
            idx = dequeue(queue_for_print);
            if(!isEmpty(queue_for_print)) {
                printf("%d, ", idx);
            }
            else {
                printf("%d", idx);
                break;
            }
        }
        else {
            break;
        }

    }



    return 0;
}
