/*
 *  StQ 07.04.2011 & 12.05.2012
 *  Read mumtiple-directories recursively and concurrently
 *  Version A: Interleaved output
 */

#include  <sys/types.h>
#include  <sys/stat.h>
#include  <dirent.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <pthread.h>

#include  <limits.h>
#include  <errno.h>
#include  <stdarg.h>

#define N      100
#define SPACE    2

typedef struct thread_s {
    int id;
    char *name;
    int level;
} thread_t;

static void *thread_f (void *);
static void visit_dir_recur (int, char *, int);
static void myPrint (int);

int main (int argc, char *argv[]) {
    pthread_t *th;
    thread_t *ts;
    int i;

    if (argc == 1) {
        fprintf (stderr, "Usage: pgrm <pathname1> <pathname2> ...");
        exit (1);
    }
    setbuf (stdout, 0);
    
    th = (pthread_t *) malloc ((argc-1) * sizeof (pthread_t));
    ts = (thread_t *) malloc ((argc-1) * sizeof (thread_t));
    if (th==NULL || ts==NULL) {
        fprintf (stderr, "Allocation error.\n");
        exit (1);
    }
    for (i=1; i<argc; i++) {
      ts[i-1].id = i;
      ts[i-1].name = strdup (argv[i]);
      ts[i-1].level = 1;
      //fprintf (stdout, "TC %d %d %s %d\n", i, ts[i-1].id, ts[i-1].name, ts[i-1].level);
      pthread_create (&th[i-1], NULL, thread_f, (void *) &ts[i-1]);
    }
    for (i=1; i<argc; i++) {
      pthread_join (th[i-1], NULL);
    }

    free (ts);
    free (th);

    return (0);
}

static void *thread_f (void *ptr) {
    thread_t *thread_d_p;
    int id, level;
    char *name;

    thread_d_p = (thread_t *) ptr;
    id = thread_d_p->id;
    name = thread_d_p->name;
    level = thread_d_p->level;

    //fprintf (stdout, "TF %d %s %d\n", id, name, level);

    visit_dir_recur (id, name, level);
    
    pthread_exit (NULL);
}

static void visit_dir_recur (int id, char *name, int level) {
    DIR *dp;
    struct stat statbuf;
    struct dirent *dirp;
    char *nameR;

    myPrint (level);
    fprintf (stdout, "Thread %d Entering %s; level %d\n", id, name, level);

    if (stat(name, &statbuf) < 0 ) {
        fprintf (stderr, "Error Running stat.\n");
        exit (1);
    }

    /* File */
    if (S_ISDIR(statbuf.st_mode) == 0) {
        return;
    }

    /* Directory */
    if ( (dp = opendir(name)) == NULL) {
        fprintf (stderr, "Error Opening Dir.\n");
        exit (1);
    }

    while ( (dirp = readdir(dp)) != NULL) {
        nameR = malloc ((strlen(name)+strlen(dirp->d_name)+2) * sizeof(char));
        if (nameR==NULL) {
            fprintf (stderr, "Allocation error.\n");
            exit (1);
        }
        sprintf (nameR, "%s/%s", name, dirp->d_name);

        if (stat(nameR, &statbuf) < 0 ) {
            fprintf (stderr, "Error Running lstat.\n");
            exit (1);
        }

        if (S_ISDIR(statbuf.st_mode) == 0) {
            /* File */
            myPrint (level+1);
            fprintf (stdout, "Reading %s; level %d\n", dirp->d_name, level+1);
        } else {
            /* Directory */
            if (strcmp(dirp->d_name, ".") ==0 || strcmp(dirp->d_name, "..") == 0)
                continue;
            visit_dir_recur (id, nameR, level+1);
        }
        free (nameR);
    }

    if (closedir(dp) < 0) {
        fprintf (stderr, "Error.\n");
        exit (1);
    }

    return;
}

void myPrint (int n) {
    int i;

    for (i=0; i<(n-1)*SPACE; i++) {
        fprintf (stdout, " ");
    }

    return;
}
