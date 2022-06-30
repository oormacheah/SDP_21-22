#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

pthread_cond_t turn = PTHREAD_COND_INITIALIZER;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int nR = 0, nW = 0;

void *readers() {
    pthread_mutex_lock(&m);
    while (nW > 0) {
        pthread_cond_wait(&turn, &m);
    }
    nR++;
    pthread_mutex_unlock(&m);
    
    reading ...

    pthread_mutex_lock(&m);
    nR--;
    if (nR == 0) {
        pthread_cond_broadcast(&turn);
    }
    pthread_mutex_unlock(&m);
    pthread_exit(NULL);
}

void *writers() {
    pthread_mutex_lock(&m);
    while (nR > 0 || nW > 0) {
        pthread_cond_wait(&turn, &m);
    }
    nW++;
    pthread_mutex_unlock(&m);

    writing ...

    pthread_mutex_lock(&m);
    nW--;
    pthread_mutex_unlock(&m);
    pthread_cond_broadcast(&turn);
    pthread_exit(NULL);
}