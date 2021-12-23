#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

/**********************************************************************
** Outils de synchronisation.
**********************************************************************/
pthread_mutex_t mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition = PTHREAD_COND_INITIALIZER;

/**********************************************************************
** Définition des couleurs et du nombre de peintres
**********************************************************************/
typedef enum {RED, BLUE} COLOR;
char* colors_name[] = {"RED", "BLUE"};
#define NEXT_COLOR(c)   (((c) == RED) ? BLUE : RED)

COLOR color = RED;
int nb_painters = 0;

/**********************************************************************
** Chaque peintre va récupérer un numéro unique et une couleur
** et peindre trois fois en alternant les couleurs.
**********************************************************************/
void* painter (void* _unused) {

    /* Chaque peintre prend un numéro et une couleur. */
    pthread_mutex_lock(&mutex);
        int my_number = (++nb_painters);
        COLOR my_color = ((my_number % 2) ? RED : BLUE);
    pthread_mutex_unlock(&mutex);
   
    /* il y a trois zones à peindre */
    for(int i=0; i < 3; i++) {
   
        /* il faut attendre pour alterner les couleurs */
        pthread_mutex_lock(&mutex);
        while (color != my_color && nb_painters>= 2) {
            /* je m'endors car la condition est fausse */
            pthread_cond_wait(&condition, &mutex);
        }
       
        printf("numéro=%d, couleur=%s\n", my_number, colors_name[my_color]);
        printf("nbr de paintres = %d \n",nb_painters);
        sleep(1);
       
        color = NEXT_COLOR(color);           /* couleur suivante */
        pthread_cond_broadcast(&condition);  /* réveil des peintres */
        pthread_mutex_unlock(&mutex);        /* libération */
    }
   
    --nb_painters;
    return NULL; /* pas de résultat */
}

/**********************************************************************
** Initialiser les peintres et attendre la fin du travail.
**********************************************************************/

int main (void) {
    int n =3;
    pthread_t peintres[ n ];
   
    for(int i=0; i<n; i++) {
        assert(pthread_create(&peintres[i], NULL, painter, NULL) == 0);
    }
    for(int i=0; i<n; i++) {
        assert(pthread_join(peintres[i], NULL) == 0);
    }

    printf("Fin du pere\n") ;
    return (EXIT_SUCCESS);
}