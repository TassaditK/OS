#include <stdlib.h> 
#include <stdio.h> 

#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <sys/msg.h>

#define KEY                  0x00012347
#define REQUEST_KEY         0x00012345
#define REPONSE_KEY          0x00012346
#define LG_MAX                512

struct msgform {
    long mtype;
    char mtext[LG_MAX];
}msg;

int main(void) {
    int res;
    int request, response;

    request = msgget(REQUEST_KEY,0700 | IPC_CREAT);
    if(request == -1){
        perror("msgget");
    }
    
   
    response = msgrcv(request,&msg,LG_MAX,0,0);
    printf("recu : %c\n",msg.mtext[0]);

    return (EXIT_SUCCESS); 
}

