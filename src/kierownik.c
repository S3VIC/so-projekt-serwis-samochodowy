#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shared.h"
#include <sys/msg.h>

void initPidySerwisu(serwis *pidy);
int sprawdzStanSerwisu(serwis *pidy);
int initSerwis();
int initRecepcja();
int serwisPid, recepcjaPid, kierownikMsgQId;

initKom recepcjaKom, kasjerKom, kolejkaKom;
initObslugaKlientaKom obslugaKlientaKom;
serwis pidySerwisu;

int main() {
	int procesKonczacy;
	initKierownikMsgQ(&kierownikMsgQId);
	initPidySerwisu(&pidySerwisu);
	/*initSerwis();*/
	initRecepcja();
	if(sprawdzStanSerwisu(&pidySerwisu) == 0) {
		printf("[INF] Kierownik: Status serwisu - pozytywny\n");
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
	int i;
	char *const recepcjaArgs[] = { "./bin/recepcja", NULL };
	if((recepcjaPid = fork()) == 0) {
		if(execv(recepcjaArgs[0], recepcjaArgs) == -1) {
			perror("[ERR] Blad inicjalizacji recepcji");
			exit(1);
		}
	}
	if(msgrcv(kierownikMsgQId, &kasjerKom, sizeof(kasjerKom.pid), ID_KASJER, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o kasjerze\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		exit(1);
	} else {
		printf("[INF] Kasjer zainicjalizowany pomyslnie\n");
		pidySerwisu.kasjer = kasjerKom.pid;
	}
	if(msgrcv(kierownikMsgQId, &kolejkaKom, sizeof(kolejkaKom.pid), ID_KOLEJKA, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o kolejce\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		exit(1);
	} else {
		printf("[INF] Kolejka zainicjalizowana pomyslnie\n");
		pidySerwisu.kolejka = kolejkaKom.pid;
	}
	if(msgrcv(kierownikMsgQId, &obslugaKlientaKom, sizeof(obslugaKlientaKom.pids), ID_OBSLUGA_KLIENTA, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o obsludze klienta\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		exit(1);
	} else {
		printf("[INF] Oblsuga klienta zainicjalizowana pomyslnie\n");
		for(i = 0; i < 3; i++)
			pidySerwisu.obslugaKlienta[i] = obslugaKlientaKom.pids[i];
	}
	if(msgrcv(kierownikMsgQId, &recepcjaKom, sizeof(recepcjaKom.pid), ID_RECEPCJA, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o recepcji\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		exit(1);
	} else {
		printf("[INF] Recepcja zainicjalizowana pomyslnie\n");
		pidySerwisu.recepcja = recepcjaKom.pid;
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

int sprawdzStanSerwisu(serwis *pidy) {
	int i;
	if(pidy->recepcja < 0) return -1;
	if(pidy->kolejka < 0) return -1;
	if(pidy->kasjer < 0) return -1;
	for(i = 0; i < 3; i++)
		if(pidy->obslugaKlienta[i] < 0) return -1;
	/*for(i = 0; i < 7; i++)
		if(pidy->serwis[i] < 0) return -1;*/
	return 0;
}
