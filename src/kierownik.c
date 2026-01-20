#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shared.h"
#include <sys/msg.h>


int initSerwis();
int initRecepcja();
int serwisPid, recepcjaPid, kierownikMsgQKlucz, kierownikMsgQId;

int main() {
	initKom serwisKom, recepcjaKom;
	int procesKonczacy;
	if((kierownikMsgQKlucz = ftok(sSciezka, ID_KIEROWNIK)) == -1) {
		printf("[ERR] Blad generacji klucza dla kolejki kierownika\n");
		exit(1);
	}
	if((kierownikMsgQId = msgget(kierownikMsgQKlucz, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
		printf("[ERR] Blad tworzenia kolejki kierownika\n");
	}
	initSerwis();
	initRecepcja();
	if(msgrcv(kierownikMsgQId, &serwisKom, sizeof(serwisKom.status), 1, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o serwisie\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		exit(1);
	} else {
		printf("[INF] Recepcja zainicjalizowana pomyslnie\n");
	}
	/*if(msgrcv(kierownikMsgQId, &recepcjaKom, sizeof(recepcjaKom.status), ID_RECEPCJA, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o recepcji\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		exit(1);
	} */
	procesKonczacy = waitpid(serwisPid, NULL, 0);
	if(procesKonczacy == serwisPid) {
		printf("Serwis zakonczyl prace\n");
	}
	procesKonczacy = waitpid(recepcjaPid, NULL, 0);
	if(procesKonczacy == recepcjaPid) {
		printf("Recepcja zakonczyla prace\n");
	}
	msgctl(kierownikMsgQId, IPC_RMID, NULL);
	return 0;
}

int initSerwis() {
	char *const serwisArgs[] = { "./bin/serwis", NULL };
	if((serwisPid = fork()) == 0) {
		if(execv(serwisArgs[0], serwisArgs) == -1) {
			perror("[ERR] Blad inicjalizacji serwisu");
			exit(1);
		}
	}
}

int initRecepcja() {
	char *const recepcjaArgs[] = { "./bin/recepcja", NULL };
	if((recepcjaPid = fork()) == 0) {
		if(execv(recepcjaArgs[0], recepcjaArgs) == -1) {
			perror("[ERR] Blad inicjalizacji recepcji");
			exit(1);
		}
	}
}
