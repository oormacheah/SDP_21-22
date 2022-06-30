#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>

#define N 1000

void P1(int *pipe12, int *pipe13) {
    int i;
    char str_buf[N];
    char c;
    FILE *fp;

    close(pipe12[0]);
    close(pipe13[0]);

    if ((fp = fopen("input.txt", "r")) == NULL) {
        fprintf(stderr, "Error opening the file\n");
    }
    while (fgets(str_buf, N, fp) != NULL) {
        for (i = 0; i < N; i++) {
            c = str_buf[i];
            if (c == '\n') {
                c = '\0';
                break;
            }
        }
        if (islower(str_buf[0])) {
            write(pipe12[1], str_buf, N * sizeof(char));
        }
        else {
            write(pipe13[1], str_buf, N * sizeof(char));
        }
    }
    close(pipe12[1]);
    close(pipe13[1]);
}

void P2(int *pipe12, int *pipe24) {
    int i;
    char str_buf[N];
    while (read(pipe12[0], str_buf, N * sizeof(char)) > 0) {
        for (i = 0; i < strlen(str_buf); i++) {
            str_buf[i] = tolower(str_buf[i]);
        }
        write(pipe24[1], str_buf, N * sizeof(char));
    }
    close(pipe12[0]);
    close(pipe12[1]);
    close(pipe24[0]);
    close(pipe24[1]);
}

void P3(int *pipe13, int *pipe34) {
    int i;
    char str_buf[N];
    while (read(pipe13[0], str_buf, N * sizeof(char)) > 0) {
        for (i = 0; i < strlen(str_buf); i++) {
            str_buf[i] = toupper(str_buf[i]);
        }
        write(pipe34[1], str_buf, N * sizeof(char));
    }
    close(pipe13[0]);
    close(pipe13[1]);
    close(pipe34[0]);
    close(pipe34[1]);
}

void P4(int *pipe24, int *pipe34) {
    int i;
    char str_buf[N];
    FILE *fp;

    close(pipe24[1]);
    close(pipe34[1]);
    
    fcntl(pipe24[0], F_SETFL, O_NONBLOCK);
    fcntl(pipe34[0], F_SETFL, O_NONBLOCK);

    if ((fp = fopen("output.txt", "w")) == NULL) {
        fprintf(stderr, "Error opening output file\n");
        exit(1);
    }
    
    while (i < 10) {
        if (read(pipe24[0], str_buf, N * sizeof(char)) > 0) {
            puts(str_buf);
            fputs(str_buf, fp);
            fprintf(fp, "\n");
        }
        if (read(pipe34[0], str_buf, N * sizeof(char)) > 0) {
            puts(str_buf);
            fputs(str_buf, fp);
            fprintf(fp, "\n");
        }
        i++;
    }
    fclose(fp);
    close(pipe24[0]);
    close(pipe34[0]);
    exit(0);
}

int main() {
    int pipe12[2], pipe13[2], pipe24[2], pipe34[2];

    pipe(pipe12);
    pipe(pipe13);
    pipe(pipe24);
    pipe(pipe34);
    
    // implement process forking

    // Parent does all the forks because it doesn't really matter who is the parent of who

    if (fork()) {
        if (fork()) {
            if (fork()) {
                P1(pipe12, pipe13);
            }
            else {
                P4(pipe24, pipe34);
            }
        }
        else {
            P3(pipe13, pipe34);
        }
    }
    else {
        P2(pipe12, pipe24);
    }

    return 0;

}