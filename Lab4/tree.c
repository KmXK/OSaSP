#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#define PAIRS_COUNT 8

int indexes[] = {0, 1, 2, 2, 3, 4, 6, 7};
int childs[]  = {1, 2, 3, 4, 6, 5, 7, 8};

int main() {

    int index = 0;
    for (int i = 0; i < PAIRS_COUNT; ++i) {
        if(indexes[i] == index) {
            switch (fork()) {
                case -1: {
                    perror("fork");
                    kill(0, SIGKILL);
                    return -1;
                }
                // child
                case 0: {
                    index = childs[i];
                    i = 0;
                    break;
                }
                // parent
                default: {
                    break;
                }
            }
        }
    }

    getchar();
    kill(0, SIGKILL);

    return 0;
}
