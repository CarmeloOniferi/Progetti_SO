# **Server TCP Semplice con Gestione Utenti e Contatti**

### Descrizione

Questo progetto è un semplice server TCP multi-thread scritto in C che supporta l'autenticazione degli utenti e la gestione dei contatti. Il server permette ai client di registrarsi, effettuare il login e gestire una lista di contatti (aggiungere, eliminare, modificare e visualizzare i contatti).
[Il client da utilizzare.](https://github.com/sultansozoev/client-SO)

### Funzionalità

* Registrazione Utenti: I client possono registrarsi con username e password.
* Autenticazione Utenti: I client devono effettuare il login per eseguire operazioni di gestione dei contatti.
* Aggiungi Contatto: Aggiungi un nuovo contatto con nome e numero di telefono.
* Elimina Contatto: Rimuovi un contatto per nome.
* Modifica Contatto: Cambia il numero di telefono di un contatto esistente.
* Visualizza Contatti: Elenca tutti i contatti.

### Installazione e Compilazione

1. Clona il repository:
   ```bash
   git clone https://github.com/sultansozoev/server-SO.git
   cd  server-SO
    ```
2. Compila il progetto:
   ```bash
   make
   ```
   oppure utilizza il comando seguente
   ```bash
   gcc -o server src/main.c src/server.c src/commands.c src/user.c src/contact.c -I./include -pthread
   ```

### Utilizzo
1. Avvia il server:
   ```bash
   ./server
   ```
2. Comandi dei Client:
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