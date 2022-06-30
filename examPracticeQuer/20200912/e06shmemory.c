#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <ctype.h>

#define N 100

#define SIZE 4096 // 4 KByte

// pass the "written" int through the pipe, for P2 to know how many strings to handle

// 2 pipes, one for signaling P2, one for signaling P1

// synchronizing using pipes' blocking property

void P1(int *pipe1, int *pipe2) {
    int written = 0;
    char buf[N], *tmp_p;
    key_t key = ftok("./hello.txt", 69);
    int shmid = shmget(key, SIZE, 0644 | IPC_CREAT);

    char *base_p = shmat(shmid, 0, 0);
    tmp_p = base_p;

    close(pipe1[1]);
    close(pipe2[0]);
    
    while (1) {
        fscanf(stdin, "%s", buf);
        while (strcmp(buf, "STOP")) {
            strcpy(tmp_p, buf);
            tmp_p += (strlen(buf) + 1); // will point to byte after the '\0'
            written++;
            fscanf(stdin, "%s", buf);
        }
        write(pipe2[1], &written, sizeof(int));
        read(pipe1[0], &written, sizeof(int)); // this is blocking because pipe is empty
    }

    close(pipe1[0]);
    close(pipe2[1]);
    shmdt(base_p);
}

void P2(int *pipe1, int *pipe2) {
    int i, j, tohandle; // number of strings to handle
    char *tmp_p, buf[N];
    key_t key = ftok("./hello.txt", 69);
    int shmid = shmget(key, SIZE, 0644 | IPC_CREAT);

    char *base_p = shmat(shmid, 0, 0);
    tmp_p = base_p;

    close(pipe1[0]);
    close(pipe2[1]);

    while (1) {
        read(pipe2[0], &tohandle, sizeof(int));
        for (i=0; i<tohandle; i++) {
            strcpy(buf, tmp_p);
            for (j=0; j<strlen(buf); j++) {
                if (islower(buf[j])) {
                    buf[j] = toupper(buf[j]);
                }
                else {
                    buf[j] = tolower(buf[j]);
                }
            }
            printf("%s\n", buf);
            tmp_p += (strlen(buf) + 1);
        }
        tohandle = 0;
        write(pipe1[1], &tohandle, sizeof(int));
    }

    close(pipe1[1]);
    close(pipe2[0]);
    shmdt(base_p);
}

int main() {
    int pipe1[2], pipe2[2];
    pipe(pipe1);
    pipe(pipe2);

    if (fork()) {
        P1(pipe1, pipe2);
    }
    else {
        P2(pipe1, pipe2);
    }

    return 0;
}