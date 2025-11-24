#include <stdio.h>
#include "software_timer.h"
#include <time.h>

static uint32_t millis() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

SoftwareTimer myTimer;

int main() {
    printf("Hello, world!\n");
    fflush(stdout);

    SoftwareTimer_Init(millis);
    SoftwareTimer_Set(&myTimer, 5000);

    while(1) {
        if (SoftwareTimer_IsExpired(&myTimer)) {
            printf("Timer expired!\n");
            fflush(stdout);
            SoftwareTimer_Set(&myTimer, 2000);
        }
    }
    return 0;
}