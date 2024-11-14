#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int pidEcu;

    //controllo che si sia scritto NORMALE o ARTIFICIALE in argv[1] e che non ci sia scritto altro
    if(argc != 2 || (strcmp(argv[1], "NORMALE") != 0 && strcmp(argv[1], "ARTIFICIALE") != 0)){
        printf("Modalità d'esecuzione errata, scrivi NORMALE o ARTIFICIALE come parametro in input (argv[1])\n");
        return 0;
    }

    unlink("steerHMI");                 //pipe tra inputForHMI e SteerByWire
    mknod("steerHMI", __S_IFIFO, 0);
    chmod("steerHMI", 0660);

    if((pidEcu = fork()) == 0)
        execl("centralEcu", "centralEcu", argv[1], NULL);       //avvio la centralEcu
    else if(pidEcu < 0)
                exit(EXIT_FAILURE);  
                
    system("gnome-terminal -- ./inputForHMI");  //apro il secondo terminale per l'input

    wait(0);                                    //attendo che la centralEcu termini

    char comando[50];
    int pidHMI;
    sprintf(comando, "pidof inputForHMI");      //cerco il pid di inputForHMI per killarlo
                                                //nel caso in cui venga letto PARCHEGGIO
    FILE *cmd = popen(comando, "r");            //da frontCamera.data
    if(cmd == NULL)
        exit((EXIT_FAILURE));

    if(fscanf(cmd, "%d", &pidHMI) > 0)          //quindi solo se è sempre attivo eseguo la kill()
        kill(pidHMI, SIGTERM);

    pclose(cmd);    
    close(pidEcu);

    return 0;
}