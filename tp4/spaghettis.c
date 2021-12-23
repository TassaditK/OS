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
typedef enum {LIBRE,PRISE} Occupations;
char* occupation_name[] = {"LIBRE", "PRISE"};
#define NEXT_OCCUPATION(c)   (((c) == LIBRE) ?PRISE : LIBRE)
#define NBPHILO (3)

Occupations occupataionFoutchette[] = {LIBRE, LIBRE};
int nb_philosophes = 0;



/**********************************************************************
** Chaque peintre va récupérer un numéro unique et une couleur
** et peindre trois fois en alternant les couleurs.
**********************************************************************/
void* philosoph (void* _unused) {

    /* Chaque peintre prend un numéro et une couleur. */
    pthread_mutex_lock(&mutex);
        int my_number = (++nb_philosophes);
    pthread_mutex_unlock(&mutex);
   
   int gauche = my_number;
   int droite = (my_number+1) % NBPHILO;
   printf("le philosphe %d pense... \n",my_number);
    /* il y a trois zones à peindre */
    for(int i=0; i < 2; i++) {
   
        /* il faut attendre pour alterner les couleurs */
        pthread_mutex_lock(&mutex);
        while (occupataionFoutchette[gauche] != LIBRE && occupataionFoutchette[droite] != LIBRE) {
            printf("le philosophe %d cherche  les fourchettes ... \n",my_number);
            /* je m'endors car la condition est fausse */
            pthread_cond_wait(&condition, &mutex);
        }
        occupataionFoutchette[gauche] = PRISE;
        occupataionFoutchette[droite] = PRISE;
        printf("le philosophe %d prend  les fourchettes \n",my_number);

        printf("le philosophe %d commence à manger\n",my_number);
        sleep(1);

        occupataionFoutchette[gauche] = LIBRE;
        occupataionFoutchette[droite] = LIBRE;
        printf("le philosophe %d  repose les fourchettes\n", my_number);
        pthread_cond_broadcast(&condition);  /* réveil des peintres */
        pthread_mutex_unlock(&mutex);        /* libération */
    }
   
    --nb_philosophes;
    return NULL; /* pas de résultat */
}

/**********************************************************************
** Initialiser les peintres et attendre la fin du travail.
**********************************************************************/

int main (void) {
    //int n =3;
    pthread_t philos[NBPHILO];
   
    for(int i=0; (i<NBPHILO); i++) {
        assert(pthread_create(&philos[i], NULL, philosoph, NULL) == 0);
    }
    for(int i=0; (i<NBPHILO); i++) {
        assert(pthread_join(philos[i], NULL) == 0);
    }

    printf("Fin du pere\n") ;
    return (EXIT_SUCCESS);
}