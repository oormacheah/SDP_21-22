#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// thread_data structure
typedef struct thread_s {
    int n; // amount of levels still to generate
    int size; // size of the tree so far (maybe negligible) (Actually no, we need it)
    long int *v; // array of long ints (t_ids will be stored here)
} thread_t;

// thread generation

void *myalloc(int, int);
void *thread_generation(void *);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stdout, "wrong number of parameters.\n");
        exit(1);
    }
    thread_t tstruct;
    tstruct.n = atoi(argv[1]);
    tstruct.size = 0;
    tstruct.v = NULL;
    pthread_t tid;

    pthread_create(&tid, NULL, thread_generation, &tstruct);
    pthread_join(tid, NULL);

    return 0;
}

void *thread_generation(void *passed_struct) {
    // Each thread has to create 2 threads, so tid1 and tid2
    int i;
    pthread_t tid1, tid2;
    thread_t *tstruct, ts1, ts2; // the one passed, 2 newly created
    tstruct = (thread_t *) passed_struct;
    
    // Check if a leaf node is reached (n == 0 means no more depth in the tree is required)
    if (tstruct->n == 0) {
        for (i=0; i<tstruct->size; i++) {
            fprintf(stdout, "%ld ", tstruct->v[i]);
        }
        fprintf(stdout, "\n");
        pthread_exit((void *) 1); // status 1 to be retrieved by pthread_join()
    }

    ts1.n = (tstruct->n) - 1;
    ts1.size = (tstruct->size) + 1;
    ts1.v = (long int *) myalloc(ts1.size, (int) sizeof(long int));
    for (i=0; i<tstruct->size; i++) { // if I use ts1.size I'm getting an undefined value.
        ts1.v[i] = tstruct->v[i];
    }
    ts1.v[tstruct->size] = pthread_self(); // the retrieved tstruct's size is the index of the new addition (this thread)
    pthread_create(&tid1, NULL, *thread_generation, &ts1);

    // Do the same for the second thread generated
    ts2.n = (tstruct->n) - 1;
    ts2.size = (tstruct->size) + 1;
    ts2.v = (long int *) myalloc(ts2.size, (int) sizeof(long int));
    for (i=0; i<tstruct->size; i++) {
        ts2.v[i] = tstruct->v[i];
    }
    ts2.v[tstruct->size] = pthread_self();
    pthread_create(&tid2, NULL, *thread_generation, &ts2);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    // prof's solution doesn't even cleanup memory xd
    free(ts1.v);
    free(ts2.v);
    pthread_exit((void *) 1);
}

void *myalloc(int n, int size) {
    void *p = malloc(n * size); // explicit casting to (void *) is not really necessary
    if (p == NULL) {
        fprintf(stderr, "Error in myalloc\n");
        exit(1);
    }
    return p;
}