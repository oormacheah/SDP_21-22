#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>


sem_t *sem1, *sem2, *sem3, *sem4, *m;
int n = 0;

void *T1() {
    pthread_detach(pthread_self());
    while (1) {
        sleep(1);
        sem_wait(sem1);
        printf("T1\n");
        sem_post(sem2);
        sem_post(sem3);
        sem_post(sem3);
    }
}

void *T2() {
    pthread_detach(pthread_self());
    while (1) {
        sleep(1);
        sem_wait(sem2);
        printf("T2\n");
        sem_post(sem4);
    }
}

void *T3() {
    pthread_detach(pthread_self());
    while (1) {
        sleep(1);
        sem_wait(sem3);
        printf("T3\n");
        n++;
        if (n == 1) {
            sem_wait(m);
        }
        else {
            n = 0;
            sem_post(m);
        }
        sem_post(sem4);
    }
}

void *T4() {
    pthread_detach(pthread_self());
    while (1) {
        sleep(1);
        sem_wait(sem4);
        sem_wait(sem4);
        sem_wait(sem4);
        printf("T4\n\n");
        sem_post(sem1);
    }
}

int main() {

    pthread_t tid;

    sem1 = (sem_t *) malloc(sizeof(sem_t));
    sem2 = (sem_t *) malloc(sizeof(sem_t));
    sem3 = (sem_t *) malloc(sizeof(sem_t));
    sem4 = (sem_t *) malloc(sizeof(sem_t));
    m = (sem_t *) malloc(sizeof(sem_t));

    sem_init(sem1, 0, 1);
    sem_init(sem2, 0, 0);
    sem_init(sem3, 0, 0);
    sem_init(sem4, 0, 0);
    sem_init(m, 0, 0);

    pthread_create(&tid, NULL, T3, NULL);
    pthread_create(&tid, NULL, T4, NULL);
    pthread_create(&tid, NULL, T3, NULL);
    pthread_create(&tid, NULL, T2, NULL);
    pthread_create(&tid, NULL, T1, NULL);

    sleep(5);

    sem_destroy(sem1);
    sem_destroy(sem2);
    sem_destroy(sem3);
    sem_destroy(sem4);
    sem_destroy(m);

    free(sem1);
    free(sem2);
    free(sem3);
    free(sem4);
    free(m);

    return 0;
}