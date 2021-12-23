#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int une_globale;
const char* une_constante = "une-chaine";

/**************************************************
 Afficher la carte mémoire du processus
 fichier /proc/PID/maps
 **************************************************/
void dump_maps(void) {
    char command[ 1024 ];
    sprintf(command, "cat /proc/%d/maps", getpid());
    system(command);
    printf("Tapez RETURN >> ");
    getchar();
}


/**************************************************
 Afficher les adresses des variables locales sur
 trois niveaux de récursion.
 **************************************************/
void print_local(int n) {
    long une_locale;
    printf("adresse de une_locale/%d = %012lx\n", n, (unsigned long) &une_locale);
    if (n < 4) print_local(n+1);
}


/**************************************************
 Afficher les adresses de différents objets mémoire
 **************************************************/
int main(int argc, char* argv[]) {
    static long une_statique = 1;
    
    char *alloc1 = malloc(1024L * 1024L * 10L); /* 10mo */
    printf("PID = %d\n", getpid());
    printf("adresse de une_globale  = %012lx\n", (unsigned long) & une_globale);
    printf("adresse de une_statique = %012lx\n", (unsigned long) & une_statique);
    print_local(0);
    printf("adresse de alloc1       = %012lx\n", (unsigned long)   alloc1);
    printf("adresse de une_fonction = %012lx\n", (unsigned long) & main);
    printf("adresse d'une constante = %012lx\n", (unsigned long)   une_constante);

    /* afficher la carte mémoire */
    dump_maps();

    /* allouer de la mémoire en plus */
    char *alloc2 = malloc(1024L * 1024L * 100L); /* 100mo */
    
    printf("adresse de alloc1       = %012lx\n", (unsigned long)   alloc1);
    printf("adresse de alloc2       = %012lx\n", (unsigned long)   alloc2);
    dump_maps();

    return (EXIT_SUCCESS);
}