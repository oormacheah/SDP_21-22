/*
 *  Array and matrix multiplication
 *  Sequential version
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

static float my_random ();
static float *my_malloc (int);
static float **my_malloc_2d (int, int);
static void array_display (float *, int);
static void matrix_display (float **, int, int);
static void array_product (float *, float *, int, float *);
static void matrix_product (float *, float **, float *, int);
 
int main (int argc, char **argv) {
  int n, i, r, c;
  float *v1, *v2, **mat, *v, res;

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

  matrix_product (v1, mat, v, n);

  fprintf (stdout, "v:\n");
  array_display (v, n);

  array_product (v, v2, n, &res);

  fprintf (stdout, "Result:\n%8.4f\n", res);

  free (v1);
  free (v2);
  for (i=0; i<n; i++) {
    free (mat[i]);
  }
  free (mat);
  free (v);

  return (1);
}

static void matrix_product (float *v1, float **m, float *v, int n) {
  int i, k;

  for (i=0; i<n; i++) {
    v[i] = 0;
    for (k=0; k<n; k++) {
      v[i] += v1[k] * m[k][i];
    }
  }

  return;
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
