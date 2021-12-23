#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include<assert.h>


sem_t mutex;


/*************************************************************
** Afficher (20 fois) une ligne composée de 40 chiffres.
**************************************************************/
void* affichage (void* ptr_num) {
    int num = *((int*) ptr_num);
    
    for(int i = 0; i < 20; i++) {
        //sem_wait(&mutex); /* prologue */
            for(int j=0; j<20; j++) printf("%d",num);
            //sleep(1); /* pour être sur d'avoir des problèmes */
            for(int j=0; j<20; j++) printf("%d",num);
            printf("\n");
            fflush(stdout);
        //sem_post(&mutex); /* épilogue */
        sleep(1);
    }
    return NULL;
}


/*************************************************************
** Initialiser le sémaphore, créer les deux threads et
** attendre leurs terminaisons.
**************************************************************/
int main (void) {
    pthread_t th0, th1;
    int num0 = 0, num1 = 1;
    
    /* ce qui compte est le 1 : valeur initiale du compteur */
    sem_init(&mutex, 0, 1);
    
    assert(pthread_create(&th0, NULL, affichage, &num0) == 0);
    assert(pthread_create(&th1, NULL, affichage, &num1) == 0);

    assert(pthread_join(th0, NULL) == 0);
    assert(pthread_join(th1, NULL) == 0);

    return (EXIT_SUCCESS);
}