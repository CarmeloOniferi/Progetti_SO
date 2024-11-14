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
    int pipeEcuFd, logFd, hmiFd, n = 0;
    char buffer[11], input[10];

    pipeEcuFd = open("steerPipe", O_RDONLY | O_NONBLOCK);           //pipe con la centralEcu

    logFd = open("steer.log", O_RDWR | O_CREAT | O_TRUNC, 0666);

    hmiFd = open("steerHMI", O_RDONLY | O_NONBLOCK);                // pipe con inputForHMI

    while(1){
        for(int j = 0; j < 11; j++)
            buffer[j] = '\0';
        for(int j = 0; j < 7; j++)
            input[j] = '\0';    

        n = read(hmiFd, input, sizeof(input));      //legge sempre da inputForHMI senza bloccarsi

        if(read(pipeEcuFd, buffer, sizeof(buffer)) > 0){        //quindi se legge DESTRA o SINISTRA dalla centralEcu
            for(int i = 0; i < 4; i++){                         //per 4 secondi

                if(n > 0 && input[0] != 'P'){                   //se prima ha letto ARRESTO da inputForHMI
                    dprintf(logFd, "NO ACTION\n");
                    sleep(1);                       //per eseguire l'arresto
                    dprintf(logFd, "NO ACTION\n");
                    sleep(1);                       //per riprendere la marcia prima di sterzzare
                    printf("Riprendo la sterzata a %s", buffer);
                    i--;                    //per non contare come una delle 4 sterzate l'operazione di ARRESTO
                }
                else{                               //se non ha letto niente o se ha letto PARCHEGGIO
                    dprintf(logFd, "STO GIRANDO A %s", buffer);     //segnalo la sterzata
                    if(input[0] == 'P')     //se ho letto PARCHEGGIO interrompo la sterzata
                        break;
                    sleep(1);  
                }

                n = read(hmiFd, input, sizeof(input));      //per leggere durante una sterzata
            }
        }else{          //se non deve sterzare
            dprintf(logFd, "NO ACTION\n");      //scrive NO ACTION sul file di log
            sleep(1);
        }
    }
}
