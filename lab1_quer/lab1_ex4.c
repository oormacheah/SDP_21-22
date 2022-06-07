#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Wrong number of parameters\n");
        exit(1);
    }

    int i, j, h, n;
    pid_t pid;
    h = atoi(argv[1]);
    n = atoi(argv[2]);

    for (i=0; i<h; i++) {
        for (j=0; j<n; j++) {
            pid = fork();
            if (pid == 0) { // if it's the child node,
                // break and re-enter the main loop
                break;
            } else {
                printf("%d generates %d\n", getpid(), pid); // pid is the CHILD pid, getpid() is the actual pid
                if (j == n-1) {
                    printf("exiting parent pid (after forking %d times): %d\n", n, getpid());
                    exit(0);
                }
            }
        }
    }
    printf("leave process pid: %d\n", getpid());

    return 1;
}
