#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

void *TA(void *);
void *TB(void *);
void *TC(void *);

void waitRandomTime(int max) {
    sleep((int) (rand() % max + 1));
}

int n;
int num_threads;
sem_t *sa, *sb, *sc, *sac;
pthread_t *th;

int main(int argc, char *argv[])
{
    int i;
    if (argc != 2) {
        fprintf(stderr, "Wrong number of parameters\n");
        exit(1);
    }

    num_threads = atoi(argv[1]);
    printf("n_threads: %d\n", num_threads);

    th = (pthread_t *) malloc(num_threads * 3 * sizeof(pthread_t));

    sa = (sem_t *) malloc(sizeof(sem_t));
    sb = (sem_t *) malloc(sizeof(sem_t));
    sc = (sem_t *) malloc(sizeof(sem_t));
    sac = (sem_t *) malloc(sizeof(sem_t));
    
    if (sa == NULL || sb == NULL || sc == NULL || sac == NULL) {
        fprintf(stderr, "Malloc error in th\n");
        exit(1);
    }

    sem_init(sa, 0, 1);
    sem_init(sb, 0, 0);
    sem_init(sc, 0, 1);
    sem_init(sac, 0, 1);

    n = 0;

    for (i = 0; i < 3*num_threads; i+=3) {
        srand(getpid());
        pthread_create(&th[i], NULL, TB, (void *)(i/3));
        pthread_create(&th[i+2], NULL, TC, (void *)(i/3));
        pthread_create(&th[i+1], NULL, TA, (void *)(i/3));
    }
    sem_destroy(sa);
    sem_destroy(sb);
    sem_destroy(sc);
    sem_destroy(sac);
    
    for (i=0; i<3*num_threads; i++) {
        pthread_join(th[i], NULL);
    }

    free(th);

    return 0;
}

void *TA(void *par)
{
    int i;
    i = (int) par;
    // pthread_detach(pthread_self());
    waitRandomTime(3);
    sem_wait(sa);
    sem_wait(sac);
    printf("A%d ", i);
    n++;
    if (n == 1) {
        sem_post(sb);
    }
    else {
        n = 0;
        sem_post(sa);
        sem_post(sc);
        sem_post(sac);
    }
    pthread_exit(0);
}

void *TB(void *par)
{
    int i;
    i = (int) par;
    // pthread_detach(pthread_self());
    waitRandomTime(3);
    sem_wait(sb);
    printf("B%d ", i);
    sem_post(sac);
    pthread_exit(0);
}

void *TC(void *par)
{
    int i;
    i = (int) par;
    waitRandomTime(3);
    // pthread_detach(pthread_self());
    sem_wait(sc);
    sem_wait(sac);
    printf("C%d ", i);
    n++;
    if (n == 1) {
        sem_post(sb);
    }
    else {
        n = 0;
        sem_post(sc);
        sem_post(sa);
        sem_post(sac);
    }
    pthread_exit(0);
}