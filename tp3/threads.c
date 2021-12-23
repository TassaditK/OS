#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h> 
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include <dispatch/dispatch.h>



/*************************************************************
** Zone d'échange entre les threads:
** La valeur '\0' indique l'absence de donnée.
**************************************************************/
#define EMPTY      ('\0')
volatile int theChar = EMPTY;
dispatch_semaphore_t NPlein, NVide;
#define BUFFER_SIZE         (10)
char buffer[ BUFFER_SIZE ];
int ptr_input = 0;
int ptr_output = 0; 


/*************************************************************
** Producteur: Lire l'entrée standard et, pour chaque
** caractère, donner le tour au consommateur.
**************************************************************/
void* read_stdin (void* _unused) {
    int nb_chars = 0;
    while (theChar != EOF) { 
        //while (theChar != EMPTY) {   /* attendre mon tour */
            /* rien à faire */
            //usleep(100);
        //}
        dispatch_semaphore_wait(NVide, DISPATCH_TIME_FOREVER);
        //theChar = getchar();
        //nb_chars++;
        theChar = getchar();
        buffer[ptr_input]=theChar;
        ptr_input = (ptr_input+1)%BUFFER_SIZE;
        //nb_chars++;
        dispatch_semaphore_signal(NPlein);
    }
    
    /* préparation d'un résultat (à titre d'exemple) */
    int* result = malloc(sizeof(int));
    assert(result != NULL);
    *result = (nb_chars - 1);
    return result;
} 


/*************************************************************
** Consommateur: Attendre son tour et, pour chaque caractère,
** l'afficher et donner le tour au producteur.
**************************************************************/
void* write_to_stdout (void* _unused) { 
    unsigned long cpt = 0; 
    while (true) {
        //while (theChar == EMPTY) {    /* attendre */
        //    cpt ++;
        //    usleep(100);
        //}
        dispatch_semaphore_wait(NPlein, DISPATCH_TIME_FOREVER);
        //if (theChar == EOF) break;
        //theChar = toupper(theChar);
        //printf("cpt=%lu, car=%c\n", cpt, theChar); 
        //theChar = EMPTY; 
        if (buffer[ptr_output] == EOF) break;
        theChar = toupper(theChar);
        printf("cpt=%lu, car=%c\n", cpt, buffer[ptr_output]); 
        ptr_output = (ptr_output + 1)%BUFFER_SIZE;               /* donner le tour */ 
        dispatch_semaphore_signal(NVide);
    }
    return NULL;
} 


/*************************************************************
** Créer les threads et attendre leurs terminaisons.
**************************************************************/
int main (void) { 
    pthread_t read_thread, write_thread;
    void *nb_chars;
    NPlein = dispatch_semaphore_create(0);
    NVide = dispatch_semaphore_create(1);
    if (pthread_create(&read_thread, NULL, read_stdin, NULL)) { 
        perror("pthread_create"); 
        exit(EXIT_FAILURE); 
    } 
    if (pthread_create(&write_thread, NULL, write_to_stdout, NULL)) { 
        perror("pthread_create"); 
        exit(EXIT_FAILURE); 
    } 

    if (pthread_join(read_thread, &nb_chars)) {
        perror("pthread_join");
        exit(EXIT_FAILURE); 
    }

    if (pthread_join(write_thread, NULL)) {
        perror("pthread_join"); 
        exit(EXIT_FAILURE); 
    }

 
    printf("Fin du pere (%d caractères lus)\n", *((int*) nb_chars)) ;
    free(nb_chars);
    return (EXIT_SUCCESS);

}