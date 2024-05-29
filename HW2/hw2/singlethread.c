#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    clock_t start_clock = clock();

    for(int j=3; j<=9;j=j+2){
        for(int i=1; i<=100;i++){
            printf("%d X %d = %d\n", i, j, i*j);
        }
    }



    clock_t end_clock = clock();
    printf("소요 시간: %lf\n", (double)(end_clock-start_clock)/ CLOCKS_PER_SEC);

    return 0;
}