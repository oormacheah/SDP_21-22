#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N 10

int n = 0;
sem_t sema, semb, semc;

void *A(void *p) {
    pthread_detach(pthread_self());
    int i = (int) p;

    sem_wait(&sema);
    printf("A%d ", i);
    n++;
    if (n == 5) {
        n = 0;
        sem_post(&semc);
    }
}

void *B(void *p) {
    pthread_detach(pthread_self());
    int i = (int) p;

    sem_wait(&semb);
    printf("B%d ", i);
    n++;
    if (n == 5) {
        n = 0;
        sem_post(&semc);
    }
}   

void *C(void *p) {
    pthread_detach(pthread_self());
    int i = (int) p;

    sem_wait(&semc);
    printf("C%d\n", i);

    sem_post(&sema);
    sem_post(&sema);
    sem_post(&sema);
    sem_post(&semb);
    sem_post(&semb);
}


int main() {
    pthread_t tid;
    int i;

    sem_init(&sema, 0, 3);
    sem_init(&semb, 0, 2);
    sem_init(&semc, 0, 0);

    for (i=0; i<N; i++) {
        pthread_create(&tid, NULL, A, (void *) i);
        pthread_create(&tid, NULL, A, (void *) i);
        pthread_create(&tid, NULL, A, (void *) i);
        pthread_create(&tid, NULL, B, (void *) i);
        pthread_create(&tid, NULL, B, (void *) i);
        pthread_create(&tid, NULL, C, (void *) i);
    }
    sleep(1);
    return 0;
}