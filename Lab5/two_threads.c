#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define THREADS_COUNT 2

void outputHeader();
void outputInfo(char *name);
void *thread_func(void *);

int main() {
    pthread_t threads[THREADS_COUNT];

    outputHeader();
    outputInfo("Main");

    for (int i = 0; i < THREADS_COUNT; ++i) {
        if(pthread_create(&threads[i], NULL, thread_func, NULL)) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < THREADS_COUNT; ++i) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

void outputHeader() {

    printf("%-12s | %15s | %6s | %s\n",
           "Name", "Thread id", "PID", "Time");
}

void outputInfo(char *name) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    time_t sec = ts.tv_sec;
    int ms = ts.tv_nsec / 1e6;

    struct tm *tmp = localtime(&sec);

    printf( "%-12s | %12lu | %6d | %02i:%02i:%02i:%03i\n",
           name, pthread_self(), getpid(),
           tmp->tm_hour, tmp->tm_min, tmp->tm_sec, ms);
}

void* thread_func(void *arg) {
    outputInfo("Thread");
}
