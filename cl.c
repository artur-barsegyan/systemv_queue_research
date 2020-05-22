#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int my_msg_count=0;
int declined_msg = 0;

void my_handler(int sig) {
    printf("pid=%d count=%d declined=%d\n", getpid(), my_msg_count, declined_msg);
    my_msg_count = 0;
    declined_msg = 0;

    alarm(1);
}

int main()
{
    int msqid;
    key_t key=0x112233;
    int i,len;
    struct mymsgbuf
    {
        long mtype;
        char mtext[81];
    } mybuf;

        signal(SIGALRM, my_handler);
   alarm(1);

    if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0){
        printf("Can\'t get msqid\n");
        exit(-1);
    }
        mybuf.mtype = 1;
        strcpy(mybuf.mtext, "This is");
        len = strlen(mybuf.mtext)+1;
    while(1){


       if (msgsnd(msqid, (struct msgbuf *) &mybuf,
            len, IPC_NOWAIT) < 0) {
            if (errno == EAGAIN) {
                // Queue is full
                declined_msg++;
            } else {
                printf("%s %d %d\n", strerror(errno), errno, EAGAIN);
            }

            continue;
            printf("Can\'t send message to queue! Error: %s\n", strerror(errno));
            // msgctl(msqid, IPC_RMID,
            //     (struct msqid_ds *) NULL);
            // exit(-1);
        }

        my_msg_count++;
    }

    return 0;
}
