#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/msg.h>

#define N 10
#define L 30

/*

1)  FIFOs are "named pipes" which implies it works in a very similar manner to the pipes.
A FIFO is a pseudo-file created in a given directory which then can be handled like a standard file
It is byte-oriented, not suited for very big amounts of data. Processes communicating don't need to be related
(i.e. have common ancestors).

2)  Message queues are an IPC mechanism that allows structured data to be sent between processes in the form of
messages. A message is a struct containing the message and the type of the message (represented by a long int).
M. q. is storing the messages in a FIFO manner. They are retrieved with msgrcv. 

*/

typedef struct record_s {
    int id;
    char s1[N], s2[N];
    float f;
} record_t;

struct message_buffer {
    long m_type;
    record_t m_record;
};

void PW_fifo() {
    int i, n;
    record_t records[N]; // saved recordsd
    mkfifo("/tmp/fifo", 0666);
    int fd1 = open("tmp/fifo", O_WRONLY);
    i = 0;
    while ((n = write(fd1, &records[i], sizeof(record_t))) == sizeof(record_t) && (i < N)) i++;
}

void PR_fifo() {
    int i, n;
    record_t rec_buf;
    mkfifo("/tmp/fifo", 0666);
    int fd2 = open("tmp/fifo", O_RDONLY);
    i = 0;
    while ((n = read(fd2, &rec_buf, sizeof(record_t))) == sizeof(record_t)) i++;
}

void PW_msgq() {

    struct message_buffer msg;

    int i, n;
    record_t records[N];
    key_t key;
    key = ftok("progfile", 20); // arbitrary
    int msgid = msgget(key, 0666 | IPC_CREAT);

    i = 0;
    msg.m_type = 1;
    while (i < N) {
        msg.m_record = records[i];
        msgsnd(msgid, (void *) &msg, sizeof(record_t), 0);
    }
}

void PR_msgq() {

    struct message_buffer msg;

    int i, n;
    record_t rec_buf;
    key_t key;
    key = ftok("progfile", 20); // arbitrary
    int msgid = msgget(key, 0666 | IPC_CREAT);
    
    i = 0;

    while (i < N) {
        msgrcv(msgid, (void *) &rec_buf, sizeof(record_t), 1, 0);
        i++;
    }
    msgctl(msgid, IPC_RMID, NULL);
}