#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define N 10
#define M 15

/*
    Adopted the producer-consumer scheme.
*/

sem_t semfull, semempty;
pthread_mutex_t mC = PTHREAD_MUTEX_INITIALIZER; // mutex for consumers
q_t q;

void *worker() {
    pthread_detach(pthread_self());
    task_t tmp_task;

    while (1) {
        sem_wait(&semfull);

        pthread_mutex_lock(&mC); // to make sure they don't extract the same, or something like that
        tmp_task = q.extract();
        pthread_mutex_unlock(&mC);

        sem_post(&semempty);
        if (is_task_stop(tmp_task)) {
            break;
        }
        consume_task(tmp_task);
    }
}

int main() {
    int i;
    pthread_t tid;
    task_t task_tmp;
    
    sem_init(&semfull, 0, 0);
    sem_init(&semempty, 0, M);

    q = create_queueueueue();
    
    for (i=0; i<N; i++) {
        pthread_create(tid, NULL, worker, NULL);
    }

    while (1) {
        srand(getpid());
        sleep((rand() % 5));

        sem_wait(semempty);
        task_tmp = generate_task();
        q.queue_add(task_tmp);
        if ((rand() % 5) == 3) { // literally randomly end the program
            for (i=0; i<N; i++) {
                q.queue_add(STOPTASK);
            }
            break;
        }
    }
}