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

    struct flock lock;

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
        if (op == 'E') {
            fprintf(stdout, "Exit program.\n\n");
            break;
        }

	if ((op != 'R') && (op != 'W') && (op != 'E')) {
            fprintf(stderr, "Enter a valid option (R, W, E)\n\n");
            continue;
        }

        lock.l_whence = SEEK_SET;
        lock.l_start = (n-1) * sizeof(record_t);
        lock.l_len = sizeof(record_t);

        if (op == 'R') {
            lseek(fd, (n-1) * sizeof(record_t), SEEK_SET);
            lock.l_type = F_RDLCK;
            fcntl(fd, F_SETLK, &lock); // locked for reading

            if (read(fd, &rec_buf, sizeof(record_t)) == 0) {
                fprintf(stderr, "Attempt to read outside file's boundaries\n\n");
                continue;
            }
            fprintf(stdout, "\n%d %ld %s %s %d\n\n", rec_buf.id, rec_buf.reg_num,
                    rec_buf.surname, rec_buf.name, rec_buf.mark);

            // release the lock
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);  // WITH OR WITHOUT '&' -> 100% has to be &

        }
        if (op == 'W') {
            fprintf(stdout, "Enter data in the following format: [ID REG_NUM SURNAME NAME MARK]\n");
            fscanf(stdin, "%d %ld %s %s %d", &rec_buf.id, &rec_buf.reg_num, rec_buf.surname,
                    rec_buf.name, &rec_buf.mark); // ask for data
            lseek(fd, (n-1) * sizeof(record_t), SEEK_SET);
            lock.l_type = F_WRLCK;
            fcntl(fd, F_SETLK, &lock);

            if (write(fd, &rec_buf, sizeof(record_t)) != sizeof(record_t)) {
                fprintf(stderr, "Error in number of bytes written.\n\n");
            }

            fprintf(stdout, "Entered data:\n%d %ld %s %s %d\n\n", rec_buf.id, rec_buf.reg_num,
                    rec_buf.surname, rec_buf.name, rec_buf.mark);

            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
        }
    }

    fprintf (stdout,  "Final file:\n\n");
    lseek (fd, 0, SEEK_SET);
    while (read (fd, &rec_buf, sizeof(record_t)) == sizeof(record_t)) {
        fprintf (stdout, "%d %ld %s %s %d\n", rec_buf.id, rec_buf.reg_num,
                 rec_buf.surname, rec_buf.name, rec_buf.mark);
    }

    close(fd);

    return 0;
}
