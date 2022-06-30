#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define Nthreads 5
#define Nsyncs 3

void *f(void *);

int count = 0;
pthread_mutex_t *me;
sem_t *sem, *sem2;

int main() {
    int v[Nthreads];
    int i;
    pthread_t th[Nthreads];
    me = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    sem = (sem_t *) malloc(sizeof(sem_t));
    sem2 = (sem_t *) malloc(sizeof(sem_t));
    pthread_mutex_init(me, NULL);
    sem_init(sem, 0, 0);
    sem_init(sem2, 0, Nthreads);

    for (i=0; i<Nthreads; i++) {
        v[i] = i;
        pthread_create(&th[i], NULL, f, (void *) &v[i]);
    }
    for (i=0; i<Nthreads; i++) {
        pthread_join(th[i], NULL);
    }
    pthread_mutex_destroy(me);
    sem_destroy(sem);
    sem_destroy(sem2);

    return 0;
}

void *f(void *par) {
    int id, *intPar, i, j;
    intPar = (int *) par;
    id = *intPar;
    // printf("created thread id: %d\n", id);
    for (j=0; j<Nsyncs; j++) {
        sem_wait(sem2);
        pthread_mutex_lock(me); // this mutex IS necessary BEFORE count++, otherwise any thread could acquire
                                // the mutex with count == Nthreads before every input prints.
        count++;
        printf("%d) T%d in\n", j+1, id);
        if (count == Nthreads) {
            for (i=0; i<Nthreads; i++) {
                sem_post(sem);
            }
        }
        pthread_mutex_unlock(me);
        sem_wait(sem);
        printf("%d) T%d out\n", j+1, id);
        // pthread_mutex_lock(me); // not necessary because after printing every thread waits at the beginning
                                   // of the loop anyway
        count--;
        if (count == 0) {
            for (i=0; i<Nthreads; i++) {
                sem_post(sem2);
            }
        }
        // pthread_mutex_unlock(me);
    }
    pthread_exit(NULL);
}       
