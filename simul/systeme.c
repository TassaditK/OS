
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "cpu.h"
#include "asm.h"
#include "systeme.h"


/**********************************************************
** Structures de données de l'ordonnanceur (représentation
** d'un ensemble de processus).
**
** SUIVEZ L'ORDRE DES QUESTIONS DU SUJET DE TP.
***********************************************************/


#define MAX_PROCESS  (20)   /* nb maximum de processus  */

typedef enum {
    EMPTY = 0,              /* processus non-prêt       */
    READY = 1,              /* processus prêt           */
    SLEEP = 1,             /* Processus endormi        */
    GETCHAR = 1         
} STATE;                    /* État d'un processus      */
enum {
    SYSC_EXIT = 100,   // fin du processus courant
    SYSC_PUTI = 200,   // afficher le contenu de Ri
    SYSC_NEW_THREAD =  300,
    SYSC_SLEEP = 400,
    SYSC_GETCHAR = 500
};


typedef struct {
    PSW    cpu;             /* mot d'état du processeur */
    STATE  state;           /* état du processus        */
    time_t wakeup_time;     /* heure de reveil d'un processus endormi*/
} PCB;                      /* Un Process Control Block */

PCB process[MAX_PROCESS];   /* table des processus      */

int current_process = -1;   /* nu du processus courant  */
int nb_ready        =  0;   /* nb de processus prêts    */
char tampon = '\0';
int getchar_counter = 0;


/**********************************************************
** Changer l'état d'un processus.
**
** SUIVEZ L'ORDRE DES QUESTIONS DU SUJET DE TP.
***********************************************************/

void change_state(int p, STATE new_state) {
    assert((0 <= p) && (p < MAX_PROCESS));
    STATE old_state = process[p].state;
    process[p].state = new_state;
    if (old_state == READY) nb_ready--;
    if (new_state == READY) nb_ready++;
}


/**********************************************************
** Ordonnancer l'exécution des processus.
**
** SUIVEZ L'ORDRE DES QUESTIONS DU SUJET DE TP.
***********************************************************/

PSW scheduler(PSW cpu) {
    wakeup();
    process[current_process].cpu = cpu;
    do
    {
       current_process = (current_process + 1) % MAX_PROCESS;
    } while (process[current_process].state != READY);
    printf("Fonction scheduler() à terminer\n");
    return process[current_process].cpu ;
}

/**********************************************************
** Démarrage du système (création d'un programme)
***********************************************************/

PSW system_init(void) {

    printf("Booting\n");
    
    
    /*** création d'un programme P1 ***/
    /*PSW P1_cpu = {
        .PC = 20, // le code démarre au début du segment
        .SB = 20, // le segment débute à l'adr physique 20
        .SE = 30, // et se termine à l'adresse 30
                  // les autres registres sont à zéro
    };
    assemble(P1_cpu.SB, "prog1.asm");
    change_state(0,READY);
    process[0].cpu= P1_cpu;
    current_process = 0;   
    return P1_cpu;*/


     /*** création d'un programme P2 ***/
    
    /*PSW P2_cpu = {
        .PC = 20, // le code démarre au début du segment
        .SB = 20, // le segment débute à l'adr physique 20
        .SE = 30, // et se termine à l'adresse 30
                  // les autres registres sont à zéro
    };
    
    assemble(P2_cpu.SB, "progThread.asm");
    change_state(1,READY);
    process[1].cpu= P2_cpu;
    current_process = 0;
    //return P2_cpu;
    */

    /*PSW P3_cpu = prepare_idle();
    process[2].cpu = P3_cpu;
    return P3_cpu;*/

    PSW P4_cpu = {
        .PC = 20, // le code démarre au début du segment
        .SB = 20, // le segment débute à l'adr physique 20
        .SE = 30, // et se termine à l'adresse 30
                  // les autres registres sont à zéro
    };
    assemble(P4_cpu.SB, "prog1.asm");
    change_state(0,READY);
    process[3].cpu= P4_cpu;
    current_process = 0; 
    return P4_cpu;  
}


/**********************************************************
** Traitement des appels au système
***********************************************************/

static PSW process_system_call(PSW cpu) {
     // suivant l'argument de sysc Ri, Rj, ARG
    switch (cpu.RI.arg) {
        case SYSC_EXIT:
            printf("Fin du processus courant %d \n",cpu.PC);
            process[current_process].state = EMPTY;

            int exit = 1;
            for (int i = 0; i <MAX_PROCESS; i++)
            {
                // verifier que tous les processus sont vides
                if(process[i].state != EMPTY ){
                    exit = 0;
                    break;
                }
            }
            if(exit == 1){
                printf("Il ne reste plus aucun processus\n" );
                sysc_exit(cpu);   // quitter le processus
            }
            
            cpu = sysc_exit(cpu);
            break;
        case SYSC_PUTI:
            cpu = sysc_puti(cpu);
            break;
        case SYSC_NEW_THREAD:
            printf("Nouveau Tread : %d \n", new_thread(cpu));
            cpu.PC +=1;
            new_thread(cpu);
            break;
        case SYSC_SLEEP:
            printf("Endormir le processus %i pendant %i secondes \n",current_process,cpu.DR[cpu.RI.i]);
            process[current_process].cpu = cpu;
            process[current_process].state = SLEEP;
            time_t current_time = time(NULL);
            // on prend le temps courant + la durée pour laquelle on veut endormir le processus
            process[current_process].wakeup_time = current_time + cpu.DR[cpu.RI.i];
            break;
        case SYSC_GETCHAR:
            if(tampon == '\0'){
                printf("Processus %d passe à l'état GETCHAR\n",current_process);
                process[current_process].cpu = cpu;
                process[current_process].state = GETCHAR;
                getchar_counter ++;
            }
            else{
                cpu.DR[cpu.RI.i] = tampon;
                // ici on consomme le tampon 
                tampon = '\0';
            }
            break;
        default:
            printf("Appel système inconnu %d\n", cpu.RI.arg);
            break;
    }
    return cpu;
}


/**********************************************************
** Traitement des interruptions par le système (mode système)
***********************************************************/

PSW process_interrupt(PSW cpu) {
    switch (cpu.IN) {
        case INT_SEGV:
            printf("Erreur d'adressage! \n");
            exit(EXIT_FAILURE);
            break;
        case INT_INST:
            printf("Erreur inconnue! \n");
            exit(EXIT_FAILURE);

            break;
        case INT_TRACE:
             printf("Interruption num = %d \n",cpu.IN);
             cpu= scheduler(cpu);
             dump_cpu(cpu); 
             //sleep(1);
            break;
        case INT_SYSC:
            cpu = process_system_call(cpu);
            break;
        case INT_KEYBOARD:
            break;
        default:
            break;
    }
    return cpu;
}

PSW sysc_exit(PSW c){   
    exit(0); 
}

PSW sysc_puti(PSW c){
    printf("Contenu de Ri = %d \n",c.DR[c.RI.i]);
    return c;
}


int new_thread(PSW cpu) { 
    cpu.DR[cpu.RI.i] = 1;
    int new_process = -1;
    //trouver une case libre dans le tableau des processus
    for (int i = 0; i < MAX_PROCESS; i++)
    {
        if(process[i].state == EMPTY){
            new_process = i;
            break;
        }
    }
    if(new_process == -1){
        printf("il n'y a plus de processus disponible!\n");
        sysc_exit(cpu);
    }
    else{     
    cpu.PC++;              //pour eviter que le nouveau thread se duplique à l'infini
    //creation d'un nouveau thread
    process[new_process].cpu = cpu;
    process[new_process].state = READY;
    cpu.RI.i = new_process;
    process[new_process].cpu.RI.i = 0;
    process[new_process].cpu.DR[cpu.RI.i] = 0;
    
    //incrémenter le nombre de prêts
    nb_ready++;
    }
    return new_process;  
}


PSW prepare_idle(void) {
    PSW idle = { .PC = 120, .SB = 120, .SE = 125, };
    assemble(idle.SB, "idle.asm");
    return idle;
}

/* réveille les processus endormis */
void wakeup(void){
    // prendre le temps courant : ici on met NULL en paramètre
    // afin de renvoyer un objet time_t content le temps actuel
    time_t current_time = time(NULL);
    for (int i = 0; i < MAX_PROCESS; i++)
    { 
        if(process[i].state == SLEEP && process[i].wakeup_time <= current_time){
            // on le met dans l'état prêt
            printf("reveil du processus : %d\n",i);
            process[i].state = READY;
            nb_ready++;
        }
    }
    
}


/**********************************************************

***********************************************************/
void wakeup_getchar(int p , char c ){
    process[p].state=READY;
    process[p].cpu.DR[process[p].cpu.RI.i]=c;
    getchar_counter--;
}
/**********************************************************

***********************************************************/
void key_board_event(void){
    char  tampon = get_keyboard_data();
    if(tampon == '\0'){
        return ; 
    }
    for(int i = 0 ; i<MAX_PROCESS;i++){
         if(process[i].state ==GETCHAR){
             wakeup_getchar(i,tampon);
         }
    }
}

