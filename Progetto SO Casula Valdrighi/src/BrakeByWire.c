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
    int pipeEcuFd, n, logFd;
    char buffer[9];

    pipeEcuFd = open("brakePipe", O_RDONLY);                //pipe con la centralEcu

    logFd = open("brake.log", O_RDWR | O_CREAT | O_TRUNC, 0666);

    //impostiamo la data
    time_t data;
	time (&data);
	struct tm* dataPtr;
	dataPtr = localtime(&data);


    while(1){
        for(int i = 0; i < 9; i++)
            buffer[i] = '\0';

        n = read(pipeEcuFd, buffer, sizeof(buffer));        //ogni volta che legge dalla centralEcu

        if(buffer[0] == 'F')        //se legge FRENO 5, scrive la frenata sul file di log
            dprintf(logFd, "%d/%d/%d FRENO 5\n", dataPtr->tm_mday, 1 + dataPtr->tm_mon, 1900 + dataPtr->tm_year);
        //altrimenti la centralEcu mi ha segnalato un arresto, quindi scrivo l'arresto sul file di log
        else dprintf(logFd, "%d/%d/%d ARRESTO AUTO\n", dataPtr->tm_mday, 1 + dataPtr->tm_mon, 1900 + dataPtr->tm_year);    
        
    }
}