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
    if (argc != 4) {
        printf("Wrong number of parameters.\n");
        exit(1);
    }

    FILE *fp1, *fp2, *fp3;
    int fd2; //Didn't really need to have 3 fd's, 1 was enough.
    int n; // for taking account of the written/read bytes of each write() / open() call

    fd2 = open(argv[2], O_WRONLY || O_TRUNC);


    if (fd2 == -1) {
        fprintf(stderr, "Error opening a file with open()\n");
        exit(1);
    }

    fp1 = fopen(argv[1], "r");
    fp3 = fopen(argv[3], "w");

    if (fp1 == NULL || fp3 == NULL) {
        fprintf(stderr, "Error opening file with fopen()\n");
    }

    record_t rec_buf; // Declare an instance of record_t to use as "buffer".

    while (fscanf(fp1, "%d%ld%s%s%d", &rec_buf.id, &rec_buf.reg_num,
                  rec_buf.surname, rec_buf.name, &rec_buf.mark) != EOF) {
        n = write(fd2, &rec_buf, sizeof(record_t));
        if (n != sizeof(record_t)) {
            fprintf(stderr, "Error in writing bytes to file_2 (wrong number of bytes written)\n");
            exit(1);
        }
    }
    
    close(fd2);

    // part 2, writing the binary file to file_3 in TEXT format

    fd2 = open(argv[2], O_RDONLY, O_TRUNC);
    if (fd2 == -1) {
        fprintf(stderr, "Wrong open() call\n");
    }

    while ((n = read(fd2, &rec_buf, sizeof(record_t))) == sizeof(record_t)) {
        fprintf(fp3, "%d %ld %s %s %d\n", rec_buf.id, rec_buf.reg_num, rec_buf.surname,
                rec_buf.name, rec_buf.mark);
    }
    
    close(fd2);
    fclose(fp1);
    fclose(fp3);

    return 0;
}
