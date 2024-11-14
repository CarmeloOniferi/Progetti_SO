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
    int pipeEcuFd, logFd, randFd, pidSurround, surroundFd;
    unsigned char buffer[8];                //unsigned per leggere sempre correttamente tutti gli 8 byte

    pipeEcuFd = open("parkPipe", O_WRONLY);             //pipe con la centralEcu

    logFd = open("assist.log", O_RDWR | O_CREAT | O_TRUNC, 0666);

    surroundFd = open("surroundPipe", O_RDONLY);        //pipe con SurroundViewCameras

    //apro in randFd la sorgente di numeri casuali scelta 
    if(strcmp(argv[1], "NORMALE") == 0)
        randFd = open("/dev/urandom", O_RDONLY);        
    else
        randFd = open("urandomARTIFICIALE.binary", O_RDONLY);  

    //fino a che l'auto non è parcheggiata, ogni secondo
    while(1){
        int n = read(surroundFd, buffer, sizeof(buffer));       //leggo 8 byte da SurroundViewCameras
        write(pipeEcuFd, buffer, sizeof(buffer));               //e li mando alla centralEcu

        n = read(randFd, buffer, sizeof(buffer));           //quindi leggo 8 byte random dalla sorgente scelta
        write(pipeEcuFd, buffer, sizeof(buffer));           //e li mando alla centralEcu
        dprintf(logFd, "%02x %02x %02x %02x %02x %02x %02x %02x\n", buffer[0] , buffer[1] , buffer[2] , buffer[3] , buffer[4] , buffer[5] , buffer[6] , buffer[7]);
        
        //quindi ogni due write() alla centralEcu, mi fermo un secondo, così ho mandato tutti e 16 i byte compresi quelli di SurroundViewCameras 
        sleep(1);
    }
}