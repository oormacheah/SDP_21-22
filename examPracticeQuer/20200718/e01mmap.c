#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>

#define N 10
#define C 5
#define oneG 1024 * 1024 * 1024

int fd;

void *thread_f(void *p) {
    int offset = (int) p;
    pthread_detach(pthread_self());
    char *base_data, *moving_p;
    char tmp;
    
    int count = 0;

    base_data = mmap(0, oneG, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset * oneG);
    moving_p = base_data;

    while (count < oneG) {
        char tmp = *moving_p;
        tmp = (tmp + C) % 256;
        *moving_p = tmp;
        moving_p++;
        count++;
    }
    
    munmap(base_data, oneG);
}

int main() {
    fd = open("bigfile.bin", O_RDONLY);
    pthread_t tid;
    for (int i=0; i<N; i++) {
        pthread_create(&tid, NULL, thread_f, (void *) i);
    }
}