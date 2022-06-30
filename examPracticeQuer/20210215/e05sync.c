#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 4
#define T 40

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t m2 = PTHREAD_MUTEX_INITIALIZER;
sem_t **sems, *semsync;
pthread_t th[N];
int f = 0;
int thread_count = 0;

void *sync_loop(void *param) {
    int i, j, k;
    i = (int) param;
    int local_count = 0;

    pthread_detach(pthread_self());

    while (f < T) {
        // sleep(1);
        pthread_mutex_lock(&m1);
        printf("thread: %d: iteration number %d\n", i+1, local_count+1);
        local_count++;
        if (local_count > i) { // if it's the last ITERATION of the thread
            // pthread_mutex_lock(&m2);
            thread_count++;
            if (i == (N-1)) { // if, on top of that, it's the last THREAD
                for (j=0; j<N; j++) {
                    for (k=0; k<=j; k++) { // this loop "reinitializes" the semaphores
                        sem_post(sems[N-1-k]);
                    }
                }
                for (k=0; k<N; k++) {
                    sem_post(semsync);
                }
                thread_count = 0;
                printf("\n");
                // pthread_mutex_unlock(&m2);
            }
            else {
                pthread_mutex_unlock(&m1);
                sem_wait(semsync);
            }
            local_count = 0;
        }
        pthread_mutex_unlock(&m1);
        sem_wait(sems[i]);
        f++;
    }
}

int main() {
    int i;
    sems = (sem_t **) malloc(N * sizeof(sem_t));
    for (i=0; i<N; i++) {
        sems[i] = (sem_t *) malloc(sizeof(sem_t));
        sem_init(sems[i], 0, i);
    }

    semsync = (sem_t *) malloc(sizeof(sem_t));
    sem_init(semsync, 0, 0);

    for (i=0; i<N; i++) {
        pthread_create(&th[i], NULL, sync_loop, (void *) i);
    }

    sleep(10);
    
    for (i=0; i<N; i++) {
        sem_destroy(sems[i]);
    }
    free(sems);
    sem_destroy(semsync);
    
    return 0;
}