#define N 10

typedef struct sem_s {
    int fd;
    char *path;
} sem_t;

void sem_init(sem_t *sem, int n) {
    int fd;
    sem->path = strdup("/temp/fifo");
    mkfifo(sem->path, 0666);

    sem->fd = open(sem->path, O_RDWR);

    for (int i=0; i<n, i++) {
        sem_wait(sem);
    }
}

void sem_wait(sem_t *sem) {
    char c;
    read(sem->fd, &c, sizeof(char));
}

void sem_post(sem_t *sem) {
    char c = 'x';
    write(sem->fd, &c, sizeof(char));
}

