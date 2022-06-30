#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#define N 101

typedef struct record_s {
    int i;
    char s1[N], s2[N];
    float f;
} record_t;

int main(int argc, char *argv[]) {
    int i, j, fd, n_records;
    char buf[N], c;
    record_t *records;

    if (argc != 2) {
        fprintf(stderr, "Wrong number of parameters\n");
        exit(1);
    }
    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "Error opening file with open()\n");
        exit(1);
    }

    i = 0;
    read(fd, &c, sizeof(char));
    while (c != '\n') {
        buf[i] = c;
        read(fd, &c, sizeof(char));
        i++;
    }
    buf[i] = '\0';
    n_records = atoi(buf); // got the number of records

    printf("%d\n", n_records);
    records = (record_t *) malloc(n_records * sizeof(record_t));

    for (j=0; j<n_records; j++) {
        i = 0;
        do {
            if (read(fd, &c, sizeof(char)) == 0) break;
            buf[i] = c;
            i++;
        } while (c != '\n');
        buf[i] = '\0';
        printf("%s\n", buf);
        sscanf(buf, "%d %s %s %f", &records[i].i, records[i].s1, records[i].s2, &records[i].f);
    }
    for (i=0; i<n_records; i++) {
        printf("%d %s %s %.1f\n", records[i].i, records[i].s1, records[i].s2, records[i].f);
    }   
    free(records);
    close(fd);
}