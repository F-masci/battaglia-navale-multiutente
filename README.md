# battaglia-navale-multiutente
Repository per lo sviluppo della tesina di Sistemi Operativi (a.a. 2022/2023) presso l'università di Tor Vergata - autori: Elisa Cacace, Francesco Masci

## Specifiche di progetto

Realizzazione di una versione elettronica del famoso gioco "battaglia
navale" con un numero di giocatori arbitrario. In questa versione piu'
processi client (residenti in generale su macchine diverse) sono l'interfaccia tra i
giocatori e il server (residente in generale su una macchina separata dai
client). Un client, una volta abilitato dal server, accetta come input una
mossa, la trasmette al server, e riceve la risposta dal server. In questa
versione della battaglia navale una mossa consiste oltre alle due coordinate
anche nell'identificativo del giocatore contro cui si vuole far fuoco.
Il server a sua volta quando riceve una mossa, comunica ai client se
qualcuno e' stato colpito se uno dei giocatori e' il vincitore (o se e' stato
eliminato), altrimenti abilita il prossimo client a spedire una mossa.
La generazione della posizione delle navi per ogni client e' lasciata alla
discrezione dello studente.

## Installazione
Per procedere all'installazione del software è necessario avere a disposzione i tool di compilazione per il linguaggio C (con le relative librerie standard) e il software Make.

Per prima cosa aprire una shell e lanciare il comando ```make init```. Questo comando controllerà se esistono le cartelle necessarie alla compilazione del programma, in caso non vengano trovare saranno generate.

Per compilare il programma basterà eseguire il comando ```make``` che procederà alla creazione dei due eseguibili (il ```server``` e il ```client```) di cui si compone il software.

## Avvio
Per avviare il software basterà eseguire il file ```server``` su una macchina e collegarsi ad essa utilizzando il ```client```.