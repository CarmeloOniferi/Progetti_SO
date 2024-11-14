#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/un.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]){
    int parkFd, logFd, randFd;
    unsigned char buffer[8];                //unsigned per leggere sempre correttamente tutti gli 8 byte

    parkFd = open("surroundPipe", O_WRONLY);        //pipe con parkAssist

    logFd = open("cameras.log", O_RDWR | O_CREAT | O_TRUNC, 0666);
    
    //apro in randFd la sorgente di numeri casuali scelta
    if(strcmp(argv[1], "NORMALE") == 0)
        randFd = open("/dev/urandom", O_RDONLY);
    else
        randFd = open("randomARTIFICIALE.binary", O_RDONLY);

    //fino a che l'auto non è parcheggiata, ogni secondo
    while(1){
        int n = read(randFd, buffer, sizeof(buffer));   //leggo 8 byte random dalla sorgente scelta
        write(parkFd, buffer, sizeof(buffer));          //e li mando a parkAssist
        dprintf(logFd, "%02x %02x %02x %02x %02x %02x %02x %02x\n", buffer[0] , buffer[1] , buffer[2] , buffer[3] , buffer[4] , buffer[5] , buffer[6] , buffer[7]);
        sleep(1);
    }
}