#include <stdlib.h> 
#include <stdio.h> 

#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h>

#define KEY                  0x00012347

typedef struct {
    int value;
} COUNTER;

int main(void) {
    COUNTER *c;
    int memid;

    /* création ou lien avec la zone partagée */
    memid = shmget(KEY, sizeof(COUNTER), 0700 | IPC_CREAT); 
    if (memid == -1) { perror("shmget"); return (EXIT_FAILURE); }
    
    /* montage en mémoire */
    c = shmat(memid, NULL, 0);
    printf("compteur = %d\n", c->value++ );

    return (EXIT_SUCCESS); 
}