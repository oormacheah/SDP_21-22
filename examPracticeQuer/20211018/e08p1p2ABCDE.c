#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
    

void *P1();
void *P2();

sem_t *sem1, *sem2, *sem3;
int nAB = 0;
int nED = 0;

void *P1() {
    pthread_detach(pthread_self());
    while (1) {
        sem_wait(sem1);
        nAB++;
        printf("A");
        if (nAB == 1) {
            sem_post(sem1);
        }
        else {
            nAB = 0;
            sem_post(sem2);
        }
        sem_wait(sem2);
        printf("C");
        sem_post(sem3);
        sem_wait(sem3);
        nED++;
        printf("D");
        if (nED == 1) {
            sem_post(sem3);
        }
        else {
            nED = 0;
            printf("\n");
            sem_post(sem1);
        }
    }
}

void *P2() {
    pthread_detach(pthread_self());
    while(1) {
        sem_wait(sem1);
        nAB++;
        printf("B");
        if (nAB == 1) {
            // first
            sem_post(sem1);
        }
        else {
            nAB = 0;
            sem_post(sem2);
        }
        sem_wait(sem3);
        nED++;
        printf("E");
        if (nED == 1) {
            sem_post(sem3);
        }
        else {
            nED = 0;
            printf("\n");
            sem_post(sem1);
        }
    }
}

int main() {
    sem1 = (sem_t *) malloc(sizeof(sem_t));
    sem2 = (sem_t *) malloc(sizeof(sem_t));
    sem3 = (sem_t *) malloc(sizeof(sem_t));
    sem_init(sem1, 0, 1);
    sem_init(sem2, 0, 0);
    sem_init(sem3, 0, 0);

    pthread_t tA, tB;
    pthread_create(&tA, NULL, P1, NULL);
    pthread_create(&tB, NULL, P2, NULL);

    sem_destroy(sem1);
    sem_destroy(sem2);
    return 0;
}