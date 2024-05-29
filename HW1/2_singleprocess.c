#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

int main(void){

    clock_t start = clock();

    int count;
    
    for(count=1;count<=1000;count++){
        printf("%d\n",count*7);
    }

    clock_t end = clock();
    printf("소요 시간: %lf\n", (double)(end-start)/ CLOCKS_PER_SEC);

    return 0;
}