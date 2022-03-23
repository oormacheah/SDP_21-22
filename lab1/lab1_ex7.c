#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// thread generation

void *thread_generation(void *)

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stdout, "wrong number of parameters.\n");
        exit(1);
    }
    int n = atoi(argv[1]);
    pthread_t tid;
    int i;
    for (i=0; i<n; i++) {

    }
    return 0;
}

void *thread_generation(void *parameter) {
    pthread_t tid;
    int rc, j, *nPointer;
    long int n;
    nPointer = (int *) parameter;
    n = *nPointer;
    for (j=0; j<n; j++) {
        rc = pthread_create(&tid, NULL, thread_generation, (void *) n);
        if (rc != 0) {
            printf("Error in thread generation\n");
            exit(1);
        }
        printf()
    }
    pthread_exit(NULL);
}

