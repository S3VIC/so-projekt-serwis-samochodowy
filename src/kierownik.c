#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shared.h"
#include <sys/msg.h>

void initPidySerwisu();
int sprawdzStanSerwisu();
int initSerwis();
int initRecepcja();

int serwisPid, recepcjaPid, kierownikMsgQId;
service serwis;
reception recepcja;

int main() {
	int procesKonczacy;
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | IPC_EXCL | 0600, -1);
	initPidySerwisu();
	initRecepcja();
	initSerwis();
	if(sprawdzStanSerwisu() == 0) {
		printf("[INF] Kierownik: Status serwisu - pozytywny\n");
	}
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
	if(msgrcv(kierownikMsgQId, &serwis, sizeof(pid_t) * (1 + LICZBA_MECHANIKOW), ID_SERWIS, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o serwisie\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		return -1;
	} else {
		printf("[INF] Serwis zainicjalizowany pomyslnie\n");
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
	if(msgrcv(kierownikMsgQId, &recepcja, sizeof(pid_t) * (3 + LICZBA_ST_OBSLUGI_KLIENTA), ID_RECEPCJA, 0) == -1) {
		printf("[ERR] Blad odbioru informacji o recepcji\n");
		msgctl(kierownikMsgQId, IPC_RMID, NULL);
		exit(1);
	} else {
		printf("[INF] Recepcja zainicjalizowana pomyslnie\n");
	}
	return 0;
}

void initPidySerwisu() {
	int i;
	serwis.pid = -1;
	for(i = 0; i < LICZBA_MECHANIKOW; i++)
		serwis.mechanicy[i] = -1;
}

int sprawdzStanSerwisu() {
	int i;
	if(serwis.pid < 0) return -1;
	for(i = 0; i < LICZBA_MECHANIKOW; i++)
		if(serwis.mechanicy[i] < 0) return -1;
	return 0;
}
