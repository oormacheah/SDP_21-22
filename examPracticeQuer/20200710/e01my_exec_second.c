#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define L_STR 20
#define N_COMMANDS 10

void my_exec(char *);

void my_exec(char *name) {
    int fd, i, j;
    char c;
    char path_bin[L_STR + 5];
    char **cmd_buffer;
    
    if ((fd = open(name, O_RDONLY)) < 0) {
        fprintf(stderr, "error opening the file\n");
        exit(1);
    }

    cmd_buffer = (char **) malloc(N_COMMANDS * sizeof(char *));
    for (i=0; i<N_COMMANDS; i++) {
        cmd_buffer[i] = (char *) malloc(L_STR * sizeof(char));
    }

    i = 0;
    j = 0;

    while (read(fd, &c, sizeof(char)) > 0) {
        printf("%c\n", c);
        if (c == '\n') {
            i++;
            cmd_buffer[i] = (char *) 0;
            i = 0;
            j = 0;
            if (!fork()) {
                sprintf(path_bin, "/bin/%s", cmd_buffer[0]);
                execv(path_bin, cmd_buffer);
            }
            for (i=0; i<N_COMMANDS; i++) {
                free(cmd_buffer[i]);
            }
            free(cmd_buffer);

            cmd_buffer = (char **) malloc(N_COMMANDS * sizeof(char *));
            for (i=0; i<N_COMMANDS; i++) {
                cmd_buffer[i] = (char *) malloc(L_STR * sizeof(char));
            }
        }
        else if (c == ' ') {
            cmd_buffer[i][j] = '\0';
            i++;
            j = 0;
        }
        else {
            cmd_buffer[i][j] = c;
            j++;
        }
    }
    close(fd);
    for (i=0; i<N_COMMANDS; i++) {
        free(cmd_buffer[i]);
    }
    free(cmd_buffer);
}

int main() {
    
    my_exec("e01_file.txt");

    return 0;
}