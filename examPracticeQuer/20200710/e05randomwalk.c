#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#define N 30

typedef struct record_s {
    int current_id, next_id;
    char s1[N], s2[N];
    float f;
} record_t;

int main() {
    record_t rec_buf;
    int fd, i, n;
    FILE *fp;

    fd = open("file1.bin", O_WRONLY | O_CREAT);
    fp = fopen("e05potter.txt", "r");
    while (fscanf(fp, "%d %s %s %f %d", &rec_buf.current_id, rec_buf.s1, rec_buf.s2, &rec_buf.f, &rec_buf.next_id) != EOF) {
        write(fd, &rec_buf, sizeof(record_t));
    }
    
    fclose(fp);
    close(fd);

    fd = open("file1.bin", O_RDONLY);

    n = 3;

    lseek(fd, n * sizeof(record_t), SEEK_SET);
    while (read(fd, &rec_buf, sizeof(record_t)) == sizeof(record_t)) {
        printf("%s %s %.1f\n", rec_buf.s1, rec_buf.s2, rec_buf.f);
        if (rec_buf.next_id < 0) {
            break;
        }
        lseek(fd, rec_buf.next_id * sizeof(record_t), SEEK_SET);
    }

    close(fd);

    return 0;
}

