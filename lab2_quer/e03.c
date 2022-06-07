/*
 *
 * StQ 07.04.2011 & 14.05.2012
 * Copy Dir Recursive
 * Keep a Copy of File Types and Permissios
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

#define N        255
#define SPACE    2
#define BUFFSIZE 4096

void visitDirRecur (char *, char *, int);
void myCopy (char *, char *, char *, mode_t);
void myPrint (int);

int main (int argc, char *argv[]) {
  if (argc != 3) {
    fprintf (stderr, "usage: pgrm <pathnameR> <pathnameW");
    exit (1);
  }

  visitDirRecur (argv[1], argv[2], 1);

  return (0);
}

void visitDirRecur (char *fullnameR, char *fullnameW, int level) {
  DIR *dp;
  struct stat statbuf;
  struct dirent *dirp;
  char *nameR, *nameW;

  myPrint (level);
  fprintf (stdout, "Entering %s; level %d\n", fullnameR, level);

  if (lstat(fullnameR, &statbuf) < 0 ) {
    fprintf (stderr, "Error Running lstat.\n");
    exit (1);
  }

  if (S_ISDIR(statbuf.st_mode) == 0) {
    return;
  }

  /* Create New Directory */
  myPrint (level);
  fprintf (stdout, "Creating %s; level %d\n", fullnameW, level);
  mkdir (fullnameW, statbuf.st_mode);
  /*
  mkdir (fullnameW, S_IRUSR | S_IWUSR | S_IXUSR);
  */

  /* Visit Old Directory */
  if ( (dp = opendir(fullnameR)) == NULL) {
    fprintf (stderr, "Error Opening Dir.\n");
    exit (1);
  }

  while ( (dirp = readdir(dp)) != NULL) {
    nameR = malloc ((strlen(fullnameR)+strlen(dirp->d_name)+2) * sizeof(char));
    nameW = malloc ((strlen(fullnameW)+strlen(dirp->d_name)+2) * sizeof(char));
    // Check miising nameR && nameW != NULL
    sprintf (nameR, "%s/%s", fullnameR, dirp->d_name);
    sprintf (nameW, "%s/%s", fullnameW, dirp->d_name);

    if (lstat(nameR, &statbuf) < 0 ) {
      free (nameR);
      free (nameW);
      fprintf (stderr, "Error Running lstat.\n");
      exit (1);
    }

    if (S_ISDIR(statbuf.st_mode) == 0) {
      /* File */
      myPrint (level+1);
      fprintf (stdout, "Reading %s; level %d\n", nameR, level+1);
      myPrint (level+1);
      fprintf (stdout, "Copying %s to %s; level %d\n", nameR, nameW, level+1);
      myCopy (nameR, nameW, dirp->d_name, statbuf.st_mode);
    } else {
      /* Directory */
      if (strcmp(dirp->d_name, ".") == 0  ||
        strcmp(dirp->d_name, "..") == 0)
        continue;

      visitDirRecur (nameR, nameW, level+1);
    }

    free (nameR);
    free (nameW);
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

void myCopy (char *nameR, char *nameW, char *name, mode_t mode) {
  char buf[BUFFSIZE];
  int nR, nW, fdR, fdW;
  struct stat sbuf;
  
  fdR = open (nameR, O_RDONLY);
  /*
  fdW = open (nameW, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  */
  fdW = open (nameW, mode | O_WRONLY | O_CREAT);
  if (fdR==(-1) || fdW==(-1)) {
    fprintf (stdout, "File Open Error (R=%d)(W=%d).\n", fdR, fdW);
    exit (1);
  }

  // Add name and size
  write (fdW, name, strlen(name));
  fstat (fdR, &sbuf);
  write (fdW, &sbuf.st_size, sizeof(int));    

  // Copy file
  while ((nR = read (fdR, buf, BUFFSIZE)) > 0) {
    /* Write on stdout */
    /* write (1, buf, nR); */
    nW = write (fdW, buf, nR);
    if (nR != nW)
      fprintf (stderr, "Write Error (read %d, write %d).\n", nR, nW);
  }

  if (nW < 0)
    fprintf (stderr, "Write Error.\n");

  close (fdR);
  close (fdW);

  return;
}
