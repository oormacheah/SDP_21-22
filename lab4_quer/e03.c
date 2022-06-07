/*
 *  StQ 15.05.2014
 *  StQ 01.06.2016
 *  StQ 15.05.2017
 *
 *  Compare N directory trees in a recursive way
 *  Use N threads, one for each directory tree, plus 1 to compare entries.
 *  Use string "EndOfDirectory" to deal with "equivalent but shorter"
 *  directories.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <limits.h>
#include <errno.h>
#include <stdarg.h>

#include <semaphore.h>
#include <pthread.h>

#define L 256

// Reading Threads
typedef struct threads {
  int tN;        // Thread Number
  char cdN[L];   // Current Directory Name
  char cfN[L];   // Current File Name
  sem_t se;
} threads_t;


// Comparing Thread
typedef struct threadC {
  threads_t *s;  // Pointer to the thread array
  int nt;        // Number of threads
} threadC_t;

// Semaphore for the comparing thread
sem_t se;

static void *thread_compare (void *);
static void *thread_visit (void *);
static void thread_visit_rec (threads_t *);

int main(int argc, char *argv[]) {
  int i;
  threadC_t tcD;
  threads_t *threadData;
  pthread_t *threadHandle, tcH;
  int ret;

  threadData = (threads_t *) malloc ((argc - 1) * sizeof (threads_t));
  threadHandle = (pthread_t *) malloc ((argc - 1) * sizeof (pthread_t));

  tcD.nt = argc - 1;
  tcD.s = threadData;
  sem_init (&se, 0, 0);
  ret = pthread_create (&tcH, NULL, thread_compare, (void *) &tcD);
  if (ret != 0) {
      exit (1);
  }

  for (i=0; i<argc-1; i++) {
    threadData[i].tN = i + 1;
    strcpy (threadData[i].cdN, argv[i+1]);
    sem_init (&threadData[i].se, 0, 0);

    fprintf (stdout, "Run Thread %d (out of %d)\n", i+1, argc-1);
    ret = pthread_create (&threadHandle[i], NULL, thread_visit, (void *) &threadData[i]);
    if (ret != 0) {
      exit (1);
    }
  }

  // Straighforward version: wait AT MOST 1 sec then exit
  // (INFINITE -> 1000)
  // for "equivalent but shorter" directories
  for (i=0; i<argc-1; i++) {
    pthread_join (threadHandle[i], NULL);
  }
  fprintf (stdout, "--> OK directories are equivalent.\n");

  return 0;
}

static void *thread_compare (void *par) {
  int i, nEnded;
  threadC_t *data;

  data = (threadC_t *) par;

  while (1) {
    for (i = 0; i<data->nt; i++) {
      sem_wait (&se);
    }
    fprintf (stdout, "---- Comparing Entry: %s ", data->s[0].cfN);
    nEnded = 0;
    for (i = 0; i<data->nt; i++) {
      if (strcmp (data->s[i].cfN, "EndOfDirectory") == 0) {
        nEnded++;
      } else {
        if (i>0 && (strcmp (data->s[0].cfN, data->s[i].cfN) != 0)) {
          fprintf (stdout, "- %s DIFFER.\n", data->s[i].cfN);
          fprintf (stdout, "--> KO directories differ. Stop All Threads.\n");
          exit(1);
        }
      }
    }
    if (nEnded == 0 || nEnded == data->nt) {
      fprintf (stdout, "- All OK.\n");
    } else {
      fprintf (stdout, "--> KO - ENDED directory. Stop All Threads.\n");
#if 1
      fprintf (stdout, "Go on? ");
      scanf ("%d", &i);
#endif
      exit(1);
    }
    // Release All Reading Threads
    for (i = 0; i<data->nt; i++) {
      sem_post (&data->s[i].se);
    }
  }

  pthread_exit (0);
}

static void *thread_visit (void *par) {
  threads_t *data;

  data = (threads_t *) par;

  thread_visit_rec (data);
  strcpy (data->cfN, "EndOfDirectory");
  sem_post (&se);

  pthread_exit (0);
}

static void thread_visit_rec (threads_t *data) {
  DIR *dp;
  struct stat statbuf;
  struct dirent *dirp;
  char s1[L], s2[L];

  strcpy (s1, data->cdN);
    
  if (lstat(s1, &statbuf) < 0 ) {
    fprintf (stderr, "Error Running lstat.\n");
    exit (1);
  }

  if (S_ISDIR(statbuf.st_mode) == 0) {
    fprintf (stderr, "Error NOT Entering a Directory.\n");    
    return;
  }

  if ( (dp = opendir(s1)) == NULL) {
    fprintf (stderr, "Error Opening Dir.\n");
    exit (1);
  }
  
  while ( (dirp = readdir(dp)) != NULL) {
    strcpy (data->cfN, dirp->d_name);
    fprintf (stdout, "-- Thread %d finding %s\n", data->tN, data->cfN);
    sem_post (&se);
    sem_wait (&data->se);

    /* Directory */
    if (S_ISDIR(statbuf.st_mode) == 0) {
      if (strcmp(dirp->d_name, ".") == 0  ||
        strcmp(dirp->d_name, "..") == 0)
        continue;
      sprintf (s1, "%s", data->cdN);
      sprintf (s2, "%s", data->cdN);
      strcat (s2, "\\");
      strcat (s2, dirp->d_name);
      strcpy (data->cdN, s2);
      
      thread_visit_rec (data);

      strcpy (data->cfN, s1);      
    }

  }

  if (closedir(dp) < 0) {
    fprintf (stderr, "Error.\n");
    exit (1);
  }

  return;
}
