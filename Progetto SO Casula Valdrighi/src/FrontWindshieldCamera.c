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

int main(int argc, char *argv[]){
    int serverLen, clientFd, cameraFd, n, logFd, result, i;
    char buffer[11];

    cameraFd = open("frontCamera.data", O_RDONLY);          //apro il file con i dati in input
    logFd = open("camera.log", O_RDWR | O_CREAT | O_TRUNC, 0666);

    struct sockaddr_un socketServerAddress;
    struct sockaddr* socketServerPtr;
    socketServerPtr = (struct sockaddr*) &socketServerAddress;
    serverLen = sizeof(socketServerAddress);
    clientFd = socket(AF_UNIX, SOCK_STREAM, 0);
    socketServerAddress.sun_family = AF_UNIX;
    strcpy(socketServerAddress.sun_path, "socketProgetto");
    
    //mi collego alla centralEcu tramite la socket socketProgetto
    do{
        result = connect(clientFd, socketServerPtr, serverLen);
        if (result == -1){
            printf("In attesa del server...\n");
            sleep(1);
        } 
    }while(result == -1);

    while(1){       //fino a che non viene killato dalla centralEcu
        i = 0;
        for(int j = 0; j < 11; j++)
            buffer[j] = '\0';
            
        //leggo una riga dal file in input    
        do{
            n = read(cameraFd, &buffer[i], 1);
            i++;
        }while(n > 0 && buffer[i-1] != '\n' && buffer[i-1] != EOF);

        write(clientFd, buffer, sizeof(buffer));        //mando la riga corrente alla centralEcu
        dprintf(logFd, "%s", buffer);
        sleep(1);
    }
}