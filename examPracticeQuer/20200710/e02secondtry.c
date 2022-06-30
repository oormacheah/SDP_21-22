#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>

#define N 6
#define C 8

void my_barrier(int);
void *usebarrier(void *);

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int count = 0;

sem_t *sem, *sem2;


void my_barrier(int n) {
    sem_wait(sem);

    printf("T%d in\n", n);

    pthread_mutex_lock(&m);
    count++;
    if (count == N) {
        for (int i=0; i<N; i++) {
            sem_post(sem2);
        }
    }
    pthread_mutex_unlock(&m);
    sem_wait(sem2);

    printf("T%d out\n", n);

    pthread_mutex_lock(&m);
    count--;
    if (count == 0) {
        for (int i=0; i<N; i++) {
            sem_post(sem);
        }
    }
    pthread_mutex_unlock(&m);
}

void *usebarrier(void *p) {
    int n = (int) p;
    pthread_detach(pthread_self());

    for (int j=0; j<C; j++) {
        my_barrier(n);
    }
}

int main() {
    sem = (sem_t *) malloc(sizeof(sem_t));
    sem2 = (sem_t *) malloc(sizeof(sem_t));
    pthread_t tid;

    sem_init(sem, 0, N);
    sem_init(sem2, 0, 0);

    for (int i=0; i<N; i++) {
        pthread_create(&tid, NULL, usebarrier, (void *) i);
    }

    sleep(3);

    sem_destroy(sem);
    sem_destroy(sem2);
    free(sem);
    free(sem2);
    return 0;
}