#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#define N 100

typedef struct record_s {
    int i;
    char s1[N], s2[N];
    float f;
} record_t;

int main() {
    int i, fd, n_rec, j;
    record_t *records;
    char c, buf[N];

    fd = open("file1.txt", O_RDONLY);
    i = 0;

    read(fd, &c, sizeof(char));
    while (c != '\n') {
        buf[i] = c;
        read(fd, &c, sizeof(char));
        i++;
    }
    buf[i] = '\0';

    n_rec = atoi(buf);
    records = (record_t *) malloc(n_rec * sizeof(record_t));

    for (j=0; j<n_rec; j++) {
        i = 0;
        read(fd, &c, sizeof(char));
        while (c != '\n') {
            buf[i] = c;
            i++;
            if (read(fd, &c, sizeof(char)) == 0) {
                break;
            }
        }
        buf[i] = '\0';
        sscanf(buf, "%d %s %s %f", &records[j].i, records[j].s1, records[j].s2, &records[j].f);
    }

    for (i=0; i<n_rec; i++) {
        printf("%d %s %s %.1f\n", records[i].i, records[i].s1, records[i].s2, records[i].f);
    }
    free(records);
    close(fd);
    return 0;
}