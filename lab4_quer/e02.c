/*
 *  StQ 10.05.2015
 *  StQ 10.05.2017
 *  StQ 15.04.2022
 *  Merge N ordered files
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

// Debug Printing (y/n)
#define DEBUG    1
// Debug: Waiting Time (dt milliseconds)
#define dt    1000

typedef struct threads_d {
  char *fileName;
  int *vet;
  int vetN;
} threads_t;

static void *threadFunction (void *);
static void sort (int *, int);
static void merge (threads_t *, int, int **, int *);
static int findMin (threads_t *, int *, int);

int main(int argc, char *argv[]) {
  int i, j, ret, fileN, fd, *vetAll, vetAllN, *threadId;
  threads_t *threadData;
  pthread_t *threadHandle;

  fileN = argc - 2;
  threadData = (threads_t *) malloc (fileN * sizeof (threads_t));
  threadHandle = (pthread_t *) malloc (fileN * sizeof (pthread_t));
  threadId = (int *) malloc (fileN * sizeof (int));

  // Run Thread
  for (i=0; i<fileN; i++) {
    threadData[i].fileName = argv[i+1];
    ret = pthread_create (&threadHandle[i], NULL, threadFunction, (void *) &threadData[i]);
    if (ret != 0) {
      exit (1);
    }
  }

  // Wait until all threads have terminated.
  for (i=0; i<fileN; i++) {
    pthread_join (threadHandle[i], NULL);
  }

#if DEBUG
  for (i=0; i<fileN; i++) {
    fprintf (stdout, "File=%s #val=%d: ", threadData[i].fileName, threadData[i].vetN);
    for (j=0; j<threadData[i].vetN; j++) {
      fprintf (stdout, ("%d "), threadData[i].vet[j]);
    }
    fprintf (stdout, "\n");
  }
#endif

  merge(threadData, fileN, &vetAll, &vetAllN);

  fd = open (argv[argc-1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    fprintf (stderr, "Cannot open output file (%s).\n", argv[argc-1]);
    exit (1);
  }

  /* Generate output */
#if DEBUG
  fprintf (stdout, "File=%s #val=%d: ", argv[argc-1], vetAllN);
#endif
  write (fd, &vetAllN, sizeof(int));
  for (i = 0; i<vetAllN; i++) {
#if DEBUG
    fprintf (stdout, "%d ", vetAll[i]);
#endif
    write (fd, &vetAll[i], sizeof(int));
  }
  fprintf (stdout, "\n");

  close (fd);

  for (i = 0; i < fileN; i++) {
    free(threadData[i].vet);
  }
  free(threadData);
  free(threadHandle);
  free(threadId);
  free(vetAll);

  return 0;
}

void *threadFunction (void *par) {
  threads_t *data;
  int n, val, fd;

  data = (threads_t *) par;

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

static void merge (threads_t *threadData, int fileN, int **vetAll, int *vetAllN) {
  int *indices, *vet;
  int i, j, n;

  n = 0;
  for (i = 0; i < fileN; i++) {
    n = n + threadData[i].vetN;
  }
  indices = (int *)malloc(fileN * sizeof(int));
  vet = (int *)malloc(n * sizeof(int));
  if (indices == NULL || vet == NULL) {
    fprintf (stderr, "Allocaton error.\n");
    exit(1);
  }
  for (i = 0; i < fileN; i++) {
    indices[i] = 0;
  }

  j = 0;
  do {
    i = findMin (threadData, indices, fileN);
    if (i >= 0) {
      vet[j++] = threadData[i].vet[indices[i]++];
    }
  } while (i >= 0);

  *vetAll = vet;
  *vetAllN = n;
  free(indices);

  return;
}

static int findMin (threads_t *threadData, int *indices, int n) {
  int i, min, minIndex;

  minIndex = (-1);
  for (i = 0; i < n; i++) {
    if (indices[i] < threadData[i].vetN) {
      if (minIndex == (-1) ||
        threadData[i].vet[indices[i]]<min) {
        minIndex = i;
        min = threadData[i].vet[indices[i]];
      }
    }
  }

  return minIndex;
}
