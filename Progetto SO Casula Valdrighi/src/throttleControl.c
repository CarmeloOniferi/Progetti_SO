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
#include <time.h>

int main(int argc, char *argv[]){
    int pipeEcuFd, n, logFd, randFd;
    char buffer[14];
    unsigned int numeroRandom;      //unsigned per leggere sempre correttamente un numero int 

    pipeEcuFd = open("throttlePipe", O_RDONLY);         //pipe con la centralEcu

    logFd = open("throttle.log", O_RDWR | O_CREAT | O_TRUNC, 0666);

    //imposto la data
    time_t data;
	time (&data);
	struct tm* dataPtr;
	dataPtr = localtime(&data);
    
    //apro in randFd la sorgente di numeri casuali scelta
    if(strcmp(argv[1], "NORMALE") == 0)
        randFd = open("/dev/urandom", O_RDONLY);
    else
        randFd = open("urandomARTIFICIALE.binary", O_RDONLY);

    //finché non viene killato dalla centralEcu
    while(1){
        for(int i = 0; i < 14; i++)
            buffer[i] = '\0';

        n = read(pipeEcuFd, buffer, sizeof(buffer));        //ogni volta che legge dalla centralEcu (legge sempre e solo INCREMENTO 5)

        n = read(randFd, &numeroRandom, sizeof(numeroRandom));      //legge un numero random
        
        numeroRandom = numeroRandom % 100000;       //per calcolare la probabilità di 10^(-5)

        if(numeroRandom == 48925)           //se il numero random equivale ad un numero a caso tra 0 e 99999 (abbiamo scelto 48925)
            kill(getppid(), SIGUSR1);       //segnalo alla centralEcu l'errore di accelerazione tramite SIGUSR1
        
        dprintf(logFd, "%d/%d/%d AUMENTO 5\n", dataPtr->tm_mday, 1 + dataPtr->tm_mon, 1900 + dataPtr->tm_year);
    }
}