#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shared.h"
#include <sys/msg.h>

void initPidySerwisu(serwis *pidy);
int initSerwis();
int initRecepcja();
int serwisPid, recepcjaPid, kierownikMsgQKlucz, kierownikMsgQId;
int main() {
	serwis pidySerwisu;
	initKom recepcjaKom, kasjerKom, kolejkaKom;
	initObslugaKlientaKom obslugaKlientaKom;
	int procesKonczacy;
	if((kierownikMsgQKlucz = ftok(sSciezka, ID_KIEROWNIK)) == -1) {
		printf("[ERR] Blad generacji klucza dla kolejki kierownika\n");
		exit(1);
	}
	if((kierownikMsgQId = msgget(kierownikMsgQKlucz, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
		printf("[ERR] Blad tworzenia kolejki kierownika\n");
	}
	initPidySerwisu(&pidySerwisu);
	/*initSerwis();*/
	initRecepcja();
	if(msgrcv(kierownikMsgQId, &kasjerKom, sizeof(kasjerKom.pid), ID_KASJER, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o kasjerze\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		exit(1);
	} else {
		printf("[INF] Kasjer zainicjalizowany pomyslnie\n");
	}
	if(msgrcv(kierownikMsgQId, &kolejkaKom, sizeof(kolejkaKom.pid), ID_KOLEJKA, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o kolejce\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		exit(1);
	} else {
		printf("[INF] Kolejka zainicjalizowana pomyslnie\n");
	}
	if(msgrcv(kierownikMsgQId, &obslugaKlientaKom, sizeof(obslugaKlientaKom.pids), ID_OBSLUGA_KLIENTA, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o obsludze klienta\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		exit(1);
	} else {
		printf("[INF] Oblsuga klienta zainicjalizowana pomyslnie\n");
	}
	if(msgrcv(kierownikMsgQId, &recepcjaKom, sizeof(recepcjaKom.pid), ID_RECEPCJA, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o recepcji\n");
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
/*	procesKonczacy = waitpid(serwisPid, NULL, 0);
if(procesKonczacy == serwisPid) {
		printf("Serwis zakonczyl prace\n");
	}*/
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
	return 0;
}

int initRecepcja() {
	char *const recepcjaArgs[] = { "./bin/recepcja", NULL };
	if((recepcjaPid = fork()) == 0) {
		if(execv(recepcjaArgs[0], recepcjaArgs) == -1) {
			perror("[ERR] Blad inicjalizacji recepcji");
			exit(1);
		}
	}
	return 0;
}

void initPidySerwisu(serwis *pidy) {
	int i;
	pidy->recepcja = -1;
	pidy->kolejka = -1;
	pidy->kasjer = -1;
	for(i = 0; i < 3; i++)
		pidy->obslugaKlienta[i] = -1;
	for(i = 0; i < 7; i++)
		pidy->serwis[i] = -1;
}
