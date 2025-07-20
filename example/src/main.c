#include <stdio.h>
#include "timer.h"
#include <time.h>

static uint32_t millis() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

Timer myTimer;

int main() {
    printf("Hello, world!\n");
    fflush(stdout);

    Timer_Init(millis);
    Timer_Set(&myTimer, 5000);

    while(1) {
        if (Timer_IsExpired(&myTimer)) {
            printf("Timer expired!\n");
            fflush(stdout);
            Timer_Set(&myTimer, 2000);
        }
    }
    return 0;
}