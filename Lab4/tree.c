#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define QUEUE_LENGTH 8

typedef struct Pairtag{
    int from;
    int to;
} Pair;

Pair queue[] = {
        {0, 1},
        {1, 2},
        {2, 3},
        {2, 4},
        {4, 5},
        {3, 6},
        {6, 7},
        {7, 8}
};

int nodeIndex;
int queueIndex;

void action(int sig);

int main() {
    queueIndex = -1;
    signal(SIGUSR1, action);
    action(SIGUSR1);

    if(nodeIndex == 0) {
        getchar();
        kill(0, SIGKILL);
    }
    else {
        while(1) pause();
    }

    return 0;
}

void action(int sig) {
    queueIndex++;

    if(queueIndex == QUEUE_LENGTH)
        return;
    if(queue[queueIndex].from == nodeIndex) {
        printf("CREATE NEW. MY INDEX = %d\n", nodeIndex);
        switch(fork()) {
            case -1: {
                perror("fork");
                kill(0, SIGKILL);
                exit(-1);
            }
            case 0: {
                signal(SIGUSR1, action);
                nodeIndex = queue[queueIndex].to;
                printf("IM CREATED. MY INDEX = %d\n", nodeIndex);
                kill(0, SIGUSR1);
                break;
            }
            default: {
                break;
            }
        }
    }
}
