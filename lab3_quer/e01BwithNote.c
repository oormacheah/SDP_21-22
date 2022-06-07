/*
 *  StQ 24.03.2022
 *  Parent process with two children connected with 2 pipes
 *  Children write; parent read
 *  NON-Blocking READ
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define L         100
#define STR_SIZE   10
#define STR_NUM     5
#define WAIT_TIME_1 4
#define WAIT_TIME_2 2

void parent (int [], int []);
void child1 (int []);
void child2 (int []);
void generate (int *, char []);
void convert (char []);
void get_time (char []);

int main (int argc, char ** argv) {
    pid_t pid1, pid2;
    int fd1[2], fd2[2];

    // no buffering
    setbuf (stdout, 0);

    // Create pipe
    pipe(fd1);
    pipe(fd2);

    pid1 = fork();
    if (pid1==0) {
        // Child 1
        srand (getpid());
        child1(fd1);
    } else {
        pid2 = fork();
        if (pid2==0) {
            // Child 2
            srand (getpid());
            child2(fd2);
        } else {
            // Parent
            close (fd1[1]);
            close (fd2[1]);
            parent (fd1, fd2);
        }
    }

    wait (NULL);
    wait (NULL);

    close (fd1[0]);
    close (fd1[0]);

    return (0);
}

void parent (int fd1[2], int fd2[2]) {
    int i1, i2, n, nR, val;
    char str[L], my_time[L];

    // Set NONBLOCKING flag
    val = fcntl (fd1[0], F_GETFL,0);
    val |= O_NONBLOCK;
    fcntl (fd1[0], F_SETFL, val);
    val = fcntl (fd2[0], F_GETFL,0);
    val |= O_NONBLOCK;
    fcntl (fd2[0], F_SETFL, val);

    i1=i2=0;
    while (i1<STR_NUM || i2<STR_NUM) {
        nR = read(fd1[0], &n, sizeof(int));
        if (nR <= 0) {
            get_time(my_time);
            fprintf(stdout, "NOT READY From Child1 R [%d][%s]\n", i1 + 1, my_time);
        } else {
            read(fd1[0], str, n); // This is not blocking as well but we assume it always completes the
                                  // reading before the other functions, if this doesn't happen the fprintf won't work
            get_time(my_time);
            convert(str);
            fprintf(stdout, "From Child1 R [%d][%s]: %d %s\n", i1 + 1, my_time, n, str);
            i1++;
        }

        nR = read(fd2[0], &n, sizeof(int));
        if (nR <= 0) {
            get_time(my_time);
            fprintf(stdout, "\t\t\t\t\t\tNOT READY From Child2 R [%d][%s]\n", i2 + 1, my_time);
        } else {
            read(fd2[0], str, n); 
            get_time(my_time);
            convert(str);
            fprintf(stdout, "\t\t\t\t\t\tFrom Child2 R [%d][%s]: %d %s\n", i2 + 1, my_time, n, str);
            i2++;
        }

        // Slow-down the process to avoid to much polling !!!
        sleep (1);
    }

    return;
}

void child1 (int fd[2]) {
    int i, l;
    char str[L], my_time[L];

    close (fd[0]);
    for (i=0; i<STR_NUM; i++) {
        sleep (WAIT_TIME_1);
        generate (&l, str);
        get_time (my_time);
        fprintf(stdout, "---- Child1 W [%d][%s]: %d %s\n", i+1, my_time, l, str);
        write (fd[1], &l, sizeof(int));
        write (fd[1], str, l);
    }
    return;
}

void child2 (int fd[2]) {
    int i, l;
    char str[L], my_time[L];

    close (fd[0]);
    for (i=0; i<STR_NUM; i++) {
        sleep (WAIT_TIME_2);
        generate (&l, str);
        get_time(my_time);
        fprintf(stdout, "\t\t\t\t\t\t---- Child2 W [%d][%s]: %d %s\n", i+1, my_time, l, str);
        write (fd[1], &l, sizeof(int));
        write (fd[1], str, l);
    }
    return;
}

void generate (int *l, char str[]) {
    int i;
    *l = 1 + rand()%STR_SIZE;
    for (i=0; i<*l; i++) {
        str[i] = (char) (((int) 'a') + rand()%26);
    }
    str[i] = '\0';
    *l = *l + 1;
    return;
}

void convert (char str[]) {
    int i;
    for (i=0; i<strlen(str); i++) {
        str[i] = toupper (str[i]);
    }
    return;
}

void get_time (char my_time[L]) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    sprintf(my_time, "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    return;
}
