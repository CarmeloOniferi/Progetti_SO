# **Client TCP Semplice**

### Descrizione

Questo progetto è un semplice client che permette di registrarsi, effettuare il login e gestire una lista di contatti (aggiungere, eliminare, modificare e visualizzare i contatti).
Utilizzando il protocollo TCP, il client si connette al server e invia comandi per eseguire le operazioni di gestione dei contatti.
[Il server da utilizzare.](https://github.com/sultansozoev/server-SO)

### Installazione e Compilazione

1. Clona il repository:
   ```bash
   git clone https://github.com/sultansozoev/client-SO.git
   cd  client-SO
    ```
2. Compila il progetto:
   ```bash
   make
   ```
   oppure utilizza il comando seguente:
   ```bash
   gcc -o client src/client.c src/commands.c -I./include
   ```

### Utilizzo
1. Avvia il client:
   ```bash
   ./client
   ```
2. Comandi:
    * `login`: Effettua il login con nome utente e password;
    * `register`: Registra un nuovo utente con nome utente e password;
    * `add`: Aggiunge un nuovo contatto (richiede autenticazione);
    * `delete`: Elimina un contatto per nome (richiede autenticazione);
    * `modify`: Modifica il numero di telefono di un contatto esistente (richiede autenticazione);
    * `print`: Elenca tutti i contatti;
    * `close`: Chiudi la connessione con il server.

### Pulizia
Per eliminare i file .o e l'eseguibile, esegui il comando:
   ```bash
   make clean
   ```