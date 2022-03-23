#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void mngr(int);

int n = 0;
int s1, s2, s3;

int main() {
    signal(SIGUSR1, mngr);
    signal(SIGUSR2, mngr);
    while(1) {
        pause();
    }
    return 0;
}

void mngr(int sigal) {
    s1 = s2;
    s2 = s3;
    // rotating shit
    if (n == 0) {
        n++;
        return;
    }
    else {
        if (sigal == SIGUSR1) s3 = 1;
        else if (sigal == SIGUSR2) s3 = 2;
        if (s1 == s2 && s2 == s3) {
            printf("... BASTA\n");
            exit(0);
        }
        else if (s2 == s3) printf("... ERROR\n");
        else if (s2 != s3) printf("... SUCCESS\n");
    }
}

