#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int my_msg_count=0;

void my_handler(int sig) {

  printf("pid=%d count=%d\n",getpid(),my_msg_count);
  my_msg_count=0;

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
        strcpy(mybuf.mtext, "This is text");
        len = strlen(mybuf.mtext)+1;
    while(1){


       if (msgsnd(msqid, (struct msgbuf *) &mybuf,
            len, 0) < 0){
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
