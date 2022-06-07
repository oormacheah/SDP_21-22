/*
 *  StQ 24.03.2022
 *  Map a file and run 2 threads to work on the local memory in parallel
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>

#define L (30+1)

typedef struct student_s {
    int id;
    long int rn;
    char n[L];
    char s[L];
    int mark;
} student_t;

typedef struct my_thread_s {
    void *src;
    size_t copysz;
    int up_down;
} my_thread_t;

void display (char *);
static void *visit (void *);

int main(int argc, char *argv[]) {
    int fd;
    void *src;
    size_t copysz;
    struct stat sbuf;
    my_thread_t thread_d[2];
    pthread_t th[2];

    if (argc != 2) {
        fprintf(stderr, "Run as: pgrm file_name.\n");
        exit(0);
    }

    // Debug printing
    fprintf (stdout, "Source file:\n");
    display(argv[1]);

    // Core
    fd = open(argv[1], O_RDWR);
    if (fd<0) {
        fprintf(stderr, "Error opening output file.\n");
        exit(0);
    }
    /* need size of input file */
    if (fstat(fd, &sbuf) < 0) {
        fprintf(stderr, "Error in fstat.\n");
        exit(0);
    }

    copysz = sbuf.st_size;
    // Omar: Each time you increase src, you will get a new student because it is a casted pointer 
    src = (student_t * ) mmap (0, copysz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (src == MAP_FAILED) {
        fprintf(stderr, "Error in mmpap\n");
        exit(0);
    }

    thread_d[0].src = thread_d[1].src = src; // each thread has the pointer to the starting address of the mapped file
    thread_d[0].copysz = thread_d[1].copysz = copysz;
    thread_d[0].up_down = 1;
    thread_d[1].up_down = 0;

    pthread_create (&th[0], NULL, visit, (void *) &thread_d[0]);
    pthread_create (&th[1], NULL, visit, (void *) &thread_d[1]);
    pthread_join (th[0], NULL);
    pthread_join (th[1], NULL);

    munmap(src, copysz);

    // Debug printing
    fprintf (stdout, "Final file:\n");
    display(argv[1]);

    return (0);
}

void display (char *name) {
    int fd, n;
    student_t student_d;

    fd = open (name, O_RDONLY);
    if (fd == -1){
        fprintf (stderr, "Error opening input file (%s).\n", name);
        exit (1);
    }

    while ((n = read (fd, &student_d, sizeof (student_t))) == sizeof (student_d)) {
        fprintf (stdout, "%d %ld %s %s %d\n", student_d.id, student_d.rn,
                 student_d.n, student_d.s, student_d.mark);
    }

    close (fd);

    return;
}

void *visit (void *par) {
    my_thread_t *my_thread_d;
    student_t *student_d; // pointer to memory will be stored here
    int n, nrecord, up_down;

    my_thread_d = (my_thread_t *) par;
    nrecord = my_thread_d->copysz / sizeof(student_t);
    student_d = my_thread_d->src;
    up_down = my_thread_d->up_down;

    if (up_down==1) {
        for (n = 0; n < nrecord; n++) {
            student_d->rn = student_d->rn + 1;
            student_d++; // makes sense since we talking about pointers, it's like an array of student_t's.
                         // Pointer points to a struct with sizeof(struct student_s)
        }
    } else {
        student_d = student_d + nrecord - 1;
        for (n = 0; n < nrecord; n++) {
            student_d->mark = student_d->mark - 1;
            student_d--;
        }
    }

    pthread_exit (NULL);
}
