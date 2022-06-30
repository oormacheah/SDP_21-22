#include <stdlib.h>
#include <semaphore.h>
#include <stdio.h>
#include <pthread.h>

#define N 10

sem_t *sa, *sb, *sc, *sac;

void *A(void *p) {
    int n = (int) p;
    wait(sa);.3
    pthread_exit(NULL);
}

void *B(void *p) {
    int n = (int) p;
    wait(sb);
    pthread_exit(NULL);
}

void *C(void *p) {
    int n = (int) p;
    pthread_exit(NULL);
}

int main() {
    pthread_t thA[N], thB[N], thC[N];
    int i;

    sa = (sem_t *) malloc(sizeof(sem_t));
    sb = (sem_t *) malloc(sizeof(sem_t));
    sc = (sem_t *) malloc(sizeof(sem_t));
    sac = (sem_t *) malloc(sizeof(sem_t));

    sem_init(sa, 0, 1);
    sem_init(sb, 0, 0);
    sem_init(sc, 0, 0);
    sem_init(sac, 0, 0);

    for (i=0; i<N; i++) {
        pthread_create(&thA[i], NULL, A, (void *) i);
        pthread_create(&thB[i], NULL, B, (void *) i);
        pthread_create(&thC[i], NULL, C, (void *) i);
    }
    for (i=0; i<N; i++) {
        pthread_join(thA[i], NULL);
        pthread_join(thB[i], NULL);
        pthread_join(thC[i], NULL);
    }
    
    sem_destroy(sa);
    sem_destroy(sb);
    sem_destroy(sc);
    sem_destroy(sac);
    free(sa);
    free(sb);
    free(sc);
    free(sac);
}