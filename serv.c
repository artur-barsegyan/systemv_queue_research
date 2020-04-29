
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int my_msg_count=0;

void my_handler(int sig) {
    printf("pid=%d count=%d\n", getpid(), my_msg_count);
    my_msg_count=0;

    alarm(1);
}

int main() {
    int msqid;

    key_t key=0x112233;
    int len, maxlen;
    struct mymsgbuf {
        long mtype;
        char mtext[81];
    } mybuf;

    signal(SIGALRM, my_handler);
    alarm(1);

    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0){
        printf("Can\'t get msqid\n");
        exit(-1);
    }

    struct msqid_ds buf;
    msgctl(msqid, IPC_STAT, buf);

    // The maximum number of outstanding messages in queue
    buf.msg_tql = 100;

    // The maximum number of bytes allowed in messages outstanding on the associated message queue.
    buf.msg_qbytes = 2750;
    msgctl(msqid, IPC_SET, buf);

    while(1) {
        maxlen = 81;
        if ((len = msgrcv(msqid, (struct msgbuf *) &mybuf, maxlen, 0, 0)) > 0){
	       my_msg_count++;
        }
    }
    return 0;
}
