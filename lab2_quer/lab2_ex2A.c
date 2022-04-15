#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define STRSIZE 31

typedef struct record_s {
    int id;
    long int reg_num;
    char surname[STRSIZE], name[STRSIZE];
    int mark;
}   record_t;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Wrong number of parameters.\n");
        exit(1);
    }

    record_t rec_buf;
    int fd;
    int n;
    char op;

    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "Error opening the file\n");
        exit(1);
    }
    
    while (1) {
	printf("Enter [W n] or [R n] or [E (anything) to terminate]:\n");
        fscanf(stdin, "%c %d", &op, &n);
        if ((op != 'R') && (op != 'W') && (op != 'E')) {
            fprintf(stderr, "\nEnter a valid option (R, W, E)\n");
            continue;
        }
        if (op == 'E') {
            fprintf(stdout, "\nExit program.\n\n");
            break;
        }
        if (op == 'R') {
            lseek(fd, (n-1) * sizeof(record_t), SEEK_SET);
            if (read(fd, &rec_buf, sizeof(record_t)) == 0) {
                fprintf(stderr, "Attempt to read outside file's boundaries\n\n");
                continue;
            }
            fprintf(stdout, "\n%d %ld %s %s %d\n\n", rec_buf.id, rec_buf.reg_num,
                    rec_buf.surname, rec_buf.name, rec_buf.mark);
        }
        if (op == 'W') {
            fprintf(stdout, "\nEnter data in the following format: [ID REG_NUM SURNAME NAME MARK]\n");
            fscanf(stdin, "%d %ld %s %s %d", &rec_buf.id, &rec_buf.reg_num, rec_buf.surname,
                    rec_buf.name, &rec_buf.mark); // ask for data
            lseek(fd, (n-1) * sizeof(record_t), SEEK_SET);
            if (write(fd, &rec_buf, sizeof(record_t)) != sizeof(record_t)) {
                fprintf(stderr, "\nError in number of bytes written.\n");
            }

            fprintf(stdout, "\nEntered data:\n%d %ld %s %s %d\n\n", rec_buf.id, rec_buf.reg_num,
                    rec_buf.surname, rec_buf.name, rec_buf.mark);
        }
    }

    fprintf (stdout,  "Final file:\n");
    lseek (fd, 0, SEEK_SET);
    while (read (fd, &rec_buf, sizeof(record_t)) == sizeof(record_t)) {
        fprintf (stdout, "%d %ld %s %s %d\n", rec_buf.id, rec_buf.reg_num,
                 rec_buf.surname, rec_buf.name, rec_buf.mark);
    }

    close(fd);


    return 0;
}
