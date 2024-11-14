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

int pid[6], serverFd, velocita = 0, throttleFd, brakeFd, inputFd, steerFd, parkFd;

//gestione dell'errore di accelerazione 

void erroreAccelerazione(int segnale){
    if(segnale == SIGUSR1){
        velocita = 0;
        printf("Ops...errore durante l'accelerazione, ARRESTO ISTANTANEO DELLA CORSA\n");
                            
        for(int i = 0; i < 4; i++)
            kill(pid[i], SIGINT);   

        close(serverFd);
        close(throttleFd);
        close(brakeFd);
        close(inputFd);
        close(steerFd);
        close(parkFd);

        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]){
    int hmiFd, HMI = 0, nuovaVel = 0, serverLen, clientLen, clientFd, byteletti, n = 1;
    char input[10], buffer[11], messaggio[50];
    unsigned char byte[8];

    struct sockaddr_un socketServerAddress;
    struct sockaddr* socketServerPtr;
    struct sockaddr_un socketClientAddress;
    struct sockaddr* socketClientPtr;
    socketServerPtr = (struct sockaddr*) &socketServerAddress;
    serverLen = sizeof(socketServerAddress);
    socketClientPtr = (struct sockaddr*) &socketClientAddress;
    clientLen = sizeof(socketClientAddress);
    serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    socketServerAddress.sun_family = AF_UNIX;
    strcpy(socketServerAddress.sun_path, "socketProgetto");
    unlink("socketProgetto");
    bind(serverFd, socketServerPtr, serverLen);
    listen(serverFd, 1);

    unlink("throttlePipe");                 //pipe con throttleControl
    mknod("throttlePipe", __S_IFIFO, 0);
    chmod("throttlePipe", 0660);
    unlink("brakePipe");                    //pipe con BrakeByWire
    mknod("brakePipe", __S_IFIFO, 0);
    chmod("brakePipe", 0660);
    unlink("steerPipe");                    //pipe con SteerByWire   
    mknod("steerPipe", __S_IFIFO, 0);
    chmod("steerPipe", 0660);
    unlink("parkPipe");                     //pipe con parkAssist
    mknod("parkPipe", __S_IFIFO, 0);
    chmod("parkPipe", 0660);
    unlink("inputPipe");                    //pipe con inputForHMI
    mknod("inputPipe", __S_IFIFO, 0);
    chmod("inputPipe", 0660);
    unlink("surroundPipe");                 //pipe tra parkAssist e SurroundViewCameras
    mknod("surroundPipe", __S_IFIFO, 0);
    chmod("surroundPipe", 0660);

    inputFd = open("inputPipe", O_RDONLY | O_NONBLOCK);
    printf("Per avviare la corsa digita INIZIO sul terminale per l'input\n");
    
    do{
        HMI = read(inputFd, input, sizeof(input));      //attendo di ricevere INIZIO da inputForHMI
    }while(HMI <= 0);

    char *nomiProcessi[] = {"FrontWindshieldCamera", "throttleControl", "BrakeByWire", "SteerByWire", "parkAssist", "SurroundViewCameras"};
    for(int i = 0; i < 4; i++){
        pid[i] = fork();
        if(pid[i] == 0)
            execl(nomiProcessi[i], nomiProcessi[i], argv[1], NULL);     //avvio gli attuatori e il sensore FrontWindshielCamera
        else if(pid[i] < 0)
                exit(EXIT_FAILURE);  
        continue;          
    }

    signal(SIGUSR1, erroreAccelerazione);
 
    throttleFd = open("throttlePipe", O_WRONLY);
    brakeFd = open("brakePipe", O_WRONLY);
    steerFd = open("steerPipe", O_WRONLY);

    clientFd = accept(serverFd, socketClientPtr, &clientLen);   //mi collego con FrontWindshieldCamera
    printf("Avvio la corsa\n");

    //finché non trovo PARCHEGGIO da inputForHMI o da FrontWindshieldCamera
    while((buffer[0] != 'P' || buffer[1] != 'A') && input[0] != 'P'){
        for(int i = 0; i < 11; i++)
            buffer[i] = '\0';
        for(int i = 0; i < 10; i++)
            input[i] = '\0';    

        HMI = read(inputFd, input, sizeof(input));
        n = read(clientFd, buffer, sizeof(buffer));

        //se trovo PERICOLO da FrontwindshieldCamera o ARRESTO da inputForHMI
        if((buffer[0] == 'P' && buffer[1] == 'E') || input[0] == 'A'){
            velocita = 0;

            if(buffer[0] == 'P')
                printf("Pericolo in vista, ");
            printf("ARRESTO AUTO, VELOCITÀ = %d km/h\n", velocita);
            write(brakeFd, "ARRESTO", strlen("ARRESTO") + 1);   //segnalo l'arresto a BrakeByWire

            //se ho letto un numero da FrontWindshieldCamera, svuoto il buffer di input per
            //evitare che rifaccia la procedura di arresto subito prima di accelerare (più avanti)
            if(isdigit(buffer[0]) != 0){
                for(int i = 0; i < 10; i++)
                    input[i] = '\0';
            }

            sleep(1);
            printf("Riprendo la corsa...\n");
        }

        //se trovo DESTRA o SINISTRA da FrontwindshieldCamera
        if(buffer[0] == 'D' || buffer[0] == 'S'){
            if(velocita == 0){      //se ho appena fatto un arresto
                velocita += 5;
                printf("Incremento la velocità di 5 km/h prima di sterzare, VELOCITÀ = %d km/h\n", velocita);
                write(throttleFd, "INCREMENTO 5", strlen("INCREMENTO 5") + 1);
                sleep(1);
            }
    
            write(steerFd, buffer, sizeof(buffer));     //segnalo la svolta a SteerByWire
            printf("Sto girando a %d km/h a %s", velocita, buffer);
        }

        //se trovo un numero da FrontwindshieldCamera
        if(isdigit(buffer[0]) != 0){
            nuovaVel = atoi(buffer);        //lo salvo in una variabile nuovaVel

            if(nuovaVel != velocita){
                if(nuovaVel > velocita)
                    printf("Arrivo a %d km/h da %d km/h, accelero di %d km/h\n", nuovaVel, velocita, nuovaVel - velocita);

                while(nuovaVel > velocita){
                    HMI = read(inputFd, input, sizeof(input));
                    if(HMI > 0){ //se nel mentre che accelero leggo dal secondo terminale
                        if(strcmp(input, "ARRESTO") == 0){      //se è ARRESTO eseguo la procedura di arresto 
                            velocita = 0;                                   
                            write(brakeFd, "ARRESTO", strlen("ARRESTO") + 1);       //richiamando BrakeByWire
                            printf("ARRESTO AUTO, VELOCITÀ = %d km/h\n", velocita);
                            sleep(1);
                            printf("Riprendo la corsa...\nArrivo a %d km/h da %d km/h, accelero di %d km/h\n", nuovaVel, velocita, nuovaVel - velocita);
                        }
                        if(input[0] == 'P')         //se è PARCHEGGIO interrompo il ciclo per poi far eseguire la procedura di parcheggio
                            break;     
                    }

                    write(throttleFd, "INCREMENTO 5", strlen("INCREMENTO 5") + 1);  //quindi richiamo throttleControl per accelerare
                    velocita += 5;
                    printf("Incremento la velocità di 5 km/h, VELOCITÀ = %d km/h\n", velocita);
                    sleep(1);
                }  
                
                if(nuovaVel < velocita)
                    printf("Arrivo a %d km/h da %d km/h, freno di %d km/h\n", nuovaVel, velocita, velocita - nuovaVel);
            
                while(nuovaVel < velocita){
                    HMI = read(inputFd, input, sizeof(input));
                    if(HMI > 0){   //se nel mentre che freno leggo dal secondo terminale
                        if(strcmp(input, "ARRESTO") == 0){// se è ARRESTO eseguo l'arresto
                            velocita = 0;
                            write(brakeFd, "ARRESTO", strlen("ARRESTO") + 1);   //richiamando BrakeByWire
                            printf("ARRESTO AUTO, VELOCITÀ = %d km/h\n", velocita);
                            sleep(1);
                            printf("Riprendo la corsa...\n");
                        }
                        break;      //in ogni caso esco dal ciclo per poter parcheggiare o ricominciare ad accelerare
                    }

                    write(brakeFd, "FRENO 5", strlen("FRENO 5") + 1);       //quindi richiamo BrakeByWire per frenare
                    velocita -= 5;
                    printf("Decremento la velocità di 5 km/h, VELOCITÀ = %d km/h\n", velocita);
                    sleep(1);
                }  

                if(velocita > 0 && input[0] != 'P')
                    printf("Mantengo la velocità di %d km/h\n", velocita);
            }
        }

        //se trovo PARCHEGGIO da FrontwindshieldCamera o da inputForHMI
        if((buffer[0] == 'P' && buffer[1] == 'A') || input[0] == 'P'){
            printf("Avvio la procedura di parcheggio\n");

            for(int i = 0; i < 2; i++)      //uccido prima solo throttleControl e FrontWindshielCamera
                kill(pid[i], SIGINT); 

            while(velocita > 0){
                write(brakeFd, "FRENO 5", strlen("FRENO 5") + 1);       //porto la velocità a 0 richiamando BrakeByWire
                velocita -= 5;
                printf("Decremento la velocità di 5 km/h, VELOCITÀ = %d km/h\n", velocita);
                sleep(1);
            }

            for(int i = 2; i < 4; i++)      //poi uccido anche BrakeByWire e SteerByWire che intanto scriveva NO ACTION nel suo file di log 
                kill(pid[i], SIGINT);

            printf("Attendo che l'auto sia parcheggiata correttamente...\n");

            close(throttleFd);
            close(brakeFd);
            close(inputFd);
            close(steerFd);                

            for(int i = 4; i < 6; i++){     //avvio parkAssist e SurroundViewCameras
                pid[i] = fork();
                if(pid[i] == 0)
                    execl(nomiProcessi[i], nomiProcessi[i], argv[1], NULL);
                else if(pid[i] < 0)
                    exit(EXIT_FAILURE);
            }

            parkFd = open("parkPipe", O_RDONLY);
            
            for(int i = 0; i < 60; i++){    //2 read al secondo viste le 2 write prima della sleep(1) in parkAssist, quindi dura 30 secondi
                byteletti = read(parkFd, byte, sizeof(byte));   //leggo 8 byte da parkAssist
                for(int j = 0; j < 4 ; j++){
                    sprintf(messaggio, "%02x%02x", byte[2*j], byte[2*j + 1]);   //li accoppio a due a due preparandoli in messaggio

                    //se leggo un valore che corrisponde ai valori di errore
                    if(strcmp(messaggio, "172A") == 0 || strcmp(messaggio, "D693") == 0 || strcmp(messaggio, "0000") == 0 || strcmp(messaggio, "BDD8") == 0 || strcmp(messaggio, "FAEE") == 0 || strcmp(messaggio, "4300") == 0){
                        i = 0;      //azzero il contatore per far ripartire i 30 secondi, riavviando il parcheggio
                        printf("Riavvio la procedura di parcheggio\nAttendo che l'auto sia parcheggiata...\n");
                    }    
                }  
            }

            for(int i = 4; i < 6; i++)      //infine uccido  parkAssist e SurroundViewCameras
                kill(pid[i], SIGINT);
            close(parkFd);
                
            printf("Auto parcheggiata!\n");
        }   
    }

    return 0;
}