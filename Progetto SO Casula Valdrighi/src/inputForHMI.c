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

int main(int argc, char *argv[]){
    char input[10];
    int ecuFd, steerFd;

    for(int i = 0; i < 10; i++)
                input[i] = '\0';
                
    ecuFd = open("inputPipe", O_WRONLY);            //pipe con la centralEcu per gli input

    //finché non ricevo INIZIO in input
    while(strcmp(input, "INIZIO") != 0){
        printf("Per avviare la corsa digita INIZIO\n");
        scanf("%s", input);
    }

    write(ecuFd, input, sizeof(input));             //faccio partire la macchina

    steerFd = open("steerHMI", O_WRONLY);           //pipe con SteerByWire    

    FILE *fileLogSteer;

    //finché non leggo PARCHEGGIO
    do{
        printf("Digitare ARRESTO oppure PARCHEGGIO per continuare\n");
        scanf("%s", input);

        if(strcmp(input, "ARRESTO") == 0){          //se leggo ARRESTO
            printf("Attendo l'arresto...\n");
            write(ecuFd, input, sizeof(input));     //segnalo l'arresto alla centralEcu
            usleep(500000);                         //per aspettare l'eventuale ritardo di scrittura nel file di log
            
            fileLogSteer = fopen("steer.log", "r");
            fseek(fileLogSteer, -2, SEEK_END);      //mi posiziono all'ultima lettera scritta

            if(fgetc(fileLogSteer) == 'A')          //se sta sterzando, DESTRA e SINISTRA finiscono entrambe per A a differenza di NO ACTION
                write(steerFd, input, sizeof(input));   //quindi segnalo l'arresto a SteerByWire

            fclose(fileLogSteer);    //per poterlo riaprire sul momento ogni volta che viene digitato ARRESTO
            usleep(500000);    
        }

    }while(strcmp(input, "PARCHEGGIO") != 0);

    write(ecuFd, input, sizeof(input));         //quindi segnalo l'input PARCHEGGIO a centralEcu
    write(steerFd, input, sizeof(input));       //e SteerByWire

    return 0;
}