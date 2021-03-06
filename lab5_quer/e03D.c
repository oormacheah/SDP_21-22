/*
 *  Array and matrix multiplication
 *  Concurrent version 2
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct barrier_s {
  sem_t sem;
  pthread_mutex_t mutex;
  int count;
} barrier_t;

static float my_random ();
static float *my_malloc (int);
static float **my_malloc_2d (int, int);
static void array_display (float *, int);
static void matrix_display (float **, int, int);
static void array_product (float *, float *, int, float *);

static void *matrix_product (void *);

int n;
float *v1, *v2, **mat, *v;
barrier_t barrier;

int main (int argc, char **argv) {
  int i, r, c, *id;
  pthread_t *tid;

  n = atoi (argv[1]);
  srand (time(NULL));
  //srand (1);

  v1 = my_malloc (n);
  mat = my_malloc_2d (n, n);
  v2 = my_malloc (n);
  v = my_malloc (n);

  for (i=0; i<n; i++) {
    v1[i] = my_random ();
    v2[i] = my_random ();
  }

  for (r=0; r<n; r++) {
    for (c=0; c<n; c++) {
      mat[r][c] = my_random ();
    }
  }

  fprintf (stdout, "v1:\n");
  array_display (v1, n);

  fprintf (stdout, "v2:\n");
  array_display (v2, n);

  fprintf (stdout, "mat:\n");
  matrix_display (mat, n, n);

  sem_init (&barrier.sem, 0, 0);
  pthread_mutex_init (&barrier.mutex, NULL);
  barrier.count = 0;
  
  tid = malloc (n * sizeof (pthread_t));
  id = malloc (n * sizeof (int));
  for (i=0; i<n; i++) {
    id[i] = i;
    pthread_create (&tid[i], NULL, matrix_product, (void *) &id[i]);
  }

  for (i=0; i<n; i++) {
    pthread_join (tid[i], NULL);
  }
  
  free (v1);
  free (v2);
  for (i=0; i<n; i++) {
    free (mat[i]);
  }
  free (mat);
  free (v);
  free (tid);
  free (id);

  return (1);
}

static void *matrix_product (void *arg) {
  int *pi, i, k;
  float res;

  pi = (int *) arg;
  i = *pi;
  v[i] = 0;
  for (k=0; k<n; k++) {
    v[i] += v1[k] * mat[k][i];
  }

  pthread_mutex_lock (&barrier.mutex);
  barrier.count++;
  if (barrier.count == n) {
    for (k= 0; k<n; k++) {
      sem_post (&barrier.sem);
    }
    fprintf (stdout, "v:\n");
    array_display (v, n);
    array_product (v, v2, n, &res);
    fprintf (stdout, "Result:\n%8.4f\n", res);
  }
  pthread_mutex_unlock (&barrier.mutex);

  sem_wait (&barrier.sem);
  
  pthread_exit (NULL);
}

static void array_product (float *v1, float *v2, int n, float *res) {
  int i;

  *res = 0;
  for (i=0; i<n; i++) {
    *res = *res + v1[i] * v2[i];
  }

  return;
}

static void array_display (float *v, int n) {
  int i;
  
  for (i=0; i<n; i++) { 
    fprintf (stdout, "%8.4f ", v[i]);
  }
  fprintf (stdout, "\n");

  return;
}

static void matrix_display (float **m, int r, int c) {
   int i, j;
   
   for (i=0; i<r; i++) {
     for (j=0; j<c; j++) {
       fprintf (stdout, "%8.4f ", m[i][j]);
    }
    fprintf (stdout, "\n");
  }

  return;
}

static float my_random () {
  float n;

  n = ((float) (rand()) / ((float) RAND_MAX)) - 0.5;

  return (n);
}

static float *my_malloc (int n) {
  float *ptr;

  ptr = malloc (n * sizeof (float));
  if (ptr == NULL) {
    fprintf (stderr, "Memory allocation error!\n");
    exit(EXIT_FAILURE);
  }

  return ptr;
}

static float **my_malloc_2d (int r, int c) {
  int i;
  float **mat;

  mat = (float **) malloc (r * sizeof (float *));
  if (mat == NULL) {
    fprintf (stderr, "Memory allocation error.\n");
    exit(EXIT_FAILURE);
  }
  for (i=0; i<r; i++) {
    mat[i] = my_malloc (c);
  }

  return mat;
}
