
/************************************************************
 Utilisation du SGF
 ************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "sgf-header.h"


int main() {
    OFILE* file;
    int c;
    
    init_sgf();
    
    printf("\nLISTE DES FICHIERS\n\n");
    list_directory();


    file = sgf_open_write("essai.txt");
    sgf_puts(file, "Ceci est un petit texte qui occupe\n");
    sgf_puts(file, "quelques blocs sur ce disque fictif.\n");
    sgf_puts(file, "Le bloc faisant 128 octets, il faut\n");
    sgf_puts(file, "que je remplisse pour utiliser\n");
    sgf_puts(file, "plusieurs blocs.\n");
    sgf_close(file); 


    printf("\nCONTENU DE essai.txt\n\n");
    file = sgf_open_read("essai.txt");
    while ((c = sgf_getc(file)) > 0) {
        putchar(c);
    }
    sgf_close(file);
    
    
    return (EXIT_SUCCESS);
}

