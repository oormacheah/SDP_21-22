#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define N 3

void myAlarm (int sig) {
    if (sig==SIGUSR1)
        printf("Alarm on ...\n");
    return;
}

int main() {
    pid_t pid;
    signal(SIGUSR1, myAlarm);
    if (fork()) {
        sleep(N);
        kill(getpid(), SIGUSR1);
        exit(0);
    }
    return 0;
}
