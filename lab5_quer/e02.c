/*
 *  StQ 10.05.2015
 *  StQ 10.05.2017
 *  StQ 15.04.2022
 *  Merge N ordered files: Optimized version (parallel merge and sort)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "pthread.h"
#include "semaphore.h"

// Debug Printing (y/n) (level 1 and 2)
#define DEBUG1    1
#define DEBUG2    0

typedef struct threads_d {
  int id;
  char *fileName;
  int *vet;
  int vetN;
} threads_t;

// Used to set the id of the sorting thread which has done
int finish = 0;
// Used to wait the merge thread and the sorting threds
pthread_mutex_t mtxSorter = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtxMerger = PTHREAD_MUTEX_INITIALIZER;

static void *threadFunction (void *);
static void sort (int *, int);
static void merge (threads_t **, int, int **, int *, int **, int *);

int main(int argc, char *argv[]) {
  int i, j, fileN, *threadId;
  int fd, ret;
  int *mergeOld, *mergeNew, mergeOldN, mergeNewN;
  threads_t **threadData;
  pthread_t *threadHandle;

  fileN = argc - 2;
  threadData = (threads_t **) malloc (fileN * sizeof (threads_t *));
  threadHandle = (pthread_t *) malloc (fileN * sizeof (pthread_t));
  threadId = (int *) malloc (fileN * sizeof (int));

  // Run Thread
  for (i=0; i<fileN; i++) {
    threadData[i] = (threads_t *) malloc( 1 * sizeof(threads_t));
    if (threadData[i] == NULL) {
      fprintf (stderr, "Memory allocation error.\n");
      exit (0);
    }
    threadData[i]->id = i + 1;
    threadData[i]->fileName = argv[i + 1];
    ret = pthread_create (&threadHandle[i], NULL, threadFunction, (void *) threadData[i]);
    if (ret != 0) {
      exit (1);
    }
  }

  // Wait for all threads to complete (one for each iteration)
  mergeNew = NULL;
  mergeNewN = 0;
  pthread_mutex_lock (&mtxMerger);
  for (j=0; j<fileN; j++) {
    pthread_mutex_lock (&mtxMerger);
    i = finish;
    pthread_mutex_unlock (&mtxSorter);

#if DEBUG1    
    fprintf (stdout, "Merging File %d = %s #val=%d\n",
       i, threadData[i]->fileName, threadData[i]->vetN);
#if DEBUG2
    for (k=0; k<threadData[i]->vetN; k++) {
      fprintf (stdout, ("%d "), threadData[i]->vet[k]);
    }
    fprintf (stdout, "\n");
#endif
#endif

    // Switch Old and New Array and Size
    mergeOld = mergeNew;
    mergeOldN = mergeNewN;
    merge (threadData, i, &mergeOld, &mergeOldN, &mergeNew, &mergeNewN);
    free (threadData[i]->vet);    
  }


  fd = open (argv[argc-1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    fprintf (stderr, "Cannot open output file (%s).\n", argv[argc-1]);
    exit (1);
  }

  /* Generate output */
#if DEBUG1
  fprintf (stdout, "Output  File=%s #val=%d\n", argv[argc-1], mergeNewN);
#endif
  write (fd, &mergeNewN, sizeof(int));
  for (i = 0; i<mergeNewN; i++) {
#if DEBUG2
    fprintf (stdout, "%d ", mergeNew[i]);
#endif
    write (fd, &mergeNew[i], sizeof(int));
  }
  fprintf (stdout, "\n");

  close (fd);

  free(threadData);
  free(threadHandle);
  free(threadId);
  free(mergeNew);

  return 0;
}

void *threadFunction (void *par) {
  threads_t *data;
  int n, val, fd;

  data = (threads_t *) par;

  pthread_detach (pthread_self());
  
  fd = open (data->fileName, O_RDONLY);
  if (fd == -1) {
    fprintf (stderr, "Open file error (%s).\n", data->fileName);
    exit (1);
  }

  if (read (fd, &val, sizeof(int)) > 0) {
    data->vetN = val;
    data->vet = (int *)malloc(data->vetN * sizeof(int));
    if (data->vet == NULL) {
      fprintf (stderr, "Allocaton error.\n");
      exit (1);
    }
  }

  n = 0;
  while (read (fd, &val, sizeof(int))>0 && n<data->vetN) {
#if DEBUG && 0
    fprintf (stderr, "File=%s vet[%d]=%d\n", data->fileName, n, val);
#endif
    data->vet[n] = val;
    n++;
  }

  sort (data->vet, data->vetN);
  close (fd);

  pthread_mutex_lock (&mtxSorter);
  finish = data->id - 1;
  pthread_mutex_unlock (&mtxMerger);
  
  pthread_exit (NULL);
}

static void sort(int *vet, int n) {
  int i, j, tmp;

  for (i = 1; i < n; i++) {
    tmp = vet[i];
    j = i - 1;
    while (j >= 0 && tmp<vet[j]) {
      vet[j + 1] = vet[j];
      j--;
    }
    vet[j + 1] = tmp;
  }
  return;
}

static void merge (
  threads_t **threadData, int index, int **mergeOld,
  int *mergeOldN, int **mergeNew, int *mergeNewN
) {
  int i1, i2, i3;
  int *vet, n;

  n = (*mergeOldN) + threadData[index]->vetN;
  vet = (int *)malloc(n * sizeof(int));
  if (vet == NULL) {
    fprintf (stderr, "Allocaton error.\n");
    exit(1);
  }

  i1 = i2 = i3 = 0;
  while (i1<(*mergeOldN) && i2<threadData[index]->vetN) {
    if ((*mergeOld)[i1] < threadData[index]->vet[i2]) {
      vet[i3++] = (*mergeOld)[i1++];
    }
    else {
      vet[i3++] = threadData[index]->vet[i2++];
    }
  }


  /* deal with v1 tail */
  while (i1 < (*mergeOldN)) {
    vet[i3++] = (*mergeOld)[i1++];
  }

  /* deal with v2 tail */
  while (i2 < threadData[index]->vetN) {
    vet[i3++] = threadData[index]->vet[i2++];
  }

  free(*mergeOld);
  *mergeOldN = 0;
  *mergeOld = NULL;
  *mergeNewN = n;
  *mergeNew = vet;

  return;
}
