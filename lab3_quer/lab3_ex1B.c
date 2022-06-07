#define WAIT_TIME_1 1
#define WAIT_TIME_2 3
#define STR_SIZE 6
#define STR_NUM 4
#define L 100

#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>

void generateRandomString(int *, char *);
void convertToUppercase(char *);
void set_fcntl(int, int);

int main() {
    pid_t pid1, pid2;
    int fd1[2];
    int fd2[2];
    pipe(fd1);
    pipe(fd2);
    int i, l;
    char str[L];

    pid1 = fork();
    if (pid1 != 0) {
        // parent process enters here
        pid2 = fork();
        if (pid2 != 0) {
            // Parent process
            for (i=0; i<STR_NUM; i++) {
                read(fd1[0], &l, sizeof(int));
                read(fd1[0], str, l);
                convertToUppercase(str);
                fprintf(stdout, "Parent reads from Child 1 read #%d: %d %s\n", i+1, l, str);

                read(fd2[0], &l, sizeof(int));
                read(fd2[0], str, l);
                convertToUppercase(str);
                fprintf(stdout, "\t\t\t\t\t\t\tParent reads from Child 2 read #%d: %d %s\n", i+1, l, str);
            }
        }
        else {
            // Child 2
            srand(getpid());
            for (i=0; i<STR_NUM; i++) {
            sleep(WAIT_TIME_2);
            generateRandomString(&l, str);
            fprintf(stdout, "\t\t\t\t\t\t\tChild 2 write #%d: %d %s\n", i+1, l, str);
            write(fd2[1], &l, sizeof(int)); // write on the writing terminal of pipe 1
            write(fd2[1], str, l);
        }
        }
    } 
    else {
        // Child 1
        srand(getpid());
        for (i=0; i<STR_NUM; i++) {
            sleep(WAIT_TIME_1);
            generateRandomString(&l, str);
            fprintf(stdout, "Child 1 write #%d: %d %s\n", i+1, l, str);
            write(fd1[1], &l, sizeof(int)); // write on the writing terminal of pipe 1
            write(fd1[1], str, l);
        }
    }
    wait(NULL); // wait for any child
    wait(NULL); // wait for any child
    
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
}

void generateRandomString(int *length, char *str) {
    int i;
    *length = 1 + rand() % STR_SIZE; // +1 to avoid 0 length (modulus operation)
    for (i=0; i<(*length); i++) {
        str[i] = (char) ((int) 'a' + rand() % 26); 
    }
    str[i] = '\0';
    *length = *length + 1;
}

void convertToUppercase(char *str) {
    int i;
    for (i=0; i<strlen(str); i++) {
        str[i] = toupper(str[i]);
    }
}


void convertToUppercase(char *str) {
    int i;
    for (i=0; i<strlen(str); i++) {
        str[i] = toupper(str[i]);
    }
}

void set_fcntl(int fd, int flags) {
    int val; // val will store the flags

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        fprintf(stderr, "error when getting flags\n");
        exit(1);
    }
    val |= flags;

    if (fnctl(fd, F_SETFL, val) < 0) {
        fprintf(stderr, "error when setting flags\n");
        exit(1);
    }
}

void clear_fcntl(int fd, int flags) {
    int val; // val will store the flags
    
    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        fprintf(stderr, "error when getting flags\n");
        exit(1);
    }
    val &= ~flags;

    if (fnctl(fd, F_SETFL, val) < 0) {
        fprintf(stderr, "error when setting flags\n");
        exit(1);
    }
}