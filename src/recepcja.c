#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include "shared.h"
#define LICZBA_STANOWISK_OBS_KL 3

int initObslugaKlienta();
int initKasjer();
int initKolejka();

int stObslKlPids[3], kasjerPid, kolejkaPid;
int main() {
	int kierownikMsgQKlucz, kierownikMsgQId;
	initKom recepcjaInit;
	recepcjaInit.typ = ID_RECEPCJA;
	recepcjaInit.status = 1;
	if((kierownikMsgQKlucz = ftok(sSciezka, ID_KIEROWNIK)) == -1) {
		printf("[ERR] Recepcja: Blad generacji klucza dla kolejki kierownika\n");
		exit(1);
	}
	if((kierownikMsgQId = msgget(kierownikMsgQKlucz, IPC_CREAT | 0600)) == -1) {
		printf("[ERR] Recepcja: Blad tworzenia kolejki kierownika\n");
		exit(1);
	}
	int  procesKonczacy, res, i;
	if((res = initObslugaKlienta()) != 0) {
		perror("[ERR] Blad podczas inicjalizacji obslugi klienta");
		return 1;
	}
	if((res = initKasjer()) != 0) {
		perror("[ERR] Blad podczas inicjalizacji kasjera");
		return 1;
	}
	if((res = initKolejka()) != 0) {
		perror("[ERR] Blad podczas inicjalizacji kolejki");
		return 1;
	}
	sleep(1);
	recepcjaInit.typ = ID_RECEPCJA;
	recepcjaInit.status = 1;
	if(msgsnd(kierownikMsgQId, &recepcjaInit, sizeof(recepcjaInit.status), 0) == -1) {
		printf("[ERR] Recepcja: Blad wysylania informacji o inicjalizacji recepcji\n");
		exit(1);
	}

	for(i = 0; i < LICZBA_STANOWISK_OBS_KL; i++) {
		procesKonczacy = waitpid(stObslKlPids[i], NULL, 0);
		if(procesKonczacy == stObslKlPids[i])
			printf("[INF] Stanowisko obslugi klienta zakonczylo prace\n");
	}

	procesKonczacy = waitpid(kasjerPid, NULL, 0);
	if(procesKonczacy == kasjerPid)
		printf("[INF] Kasjer zakonczyl prace\n");
	
	procesKonczacy = waitpid(kolejkaPid, NULL, 0);
	if(procesKonczacy == kolejkaPid)
		printf("[INF] Kolejka zostala zamknieta\n");
	return 0;
}


int initObslugaKlienta() {
	int i;
	char *const obslKlArgs[] = { "./bin/obslugaKlienta", NULL };
	for(i = 0; i < LICZBA_STANOWISK_OBS_KL; i++)
		stObslKlPids[i] = -1;
	for(i = 0; i < LICZBA_STANOWISK_OBS_KL; i++) {
		if((stObslKlPids[i] = fork()) == -1) {
			perror("[ERR] Blad inicjalizacji procesu dla st. obslugi klienta");
			return -1;
		}
		if(stObslKlPids[i] == 0) {
			if(execv(obslKlArgs[0], obslKlArgs) == -1) {
				perror("[ERR] Blad uruchomienia aplikacji st. obslugi klienta");
				return -1;
			}
		}
	}
	return 0;
}

int initKasjer() {
	char *const kasjerArgs[] = { "./bin/kasjer", NULL };
	if((kasjerPid = fork()) == -1) {
		perror("[ERR] Blad inicjalizacji procesu dla kasjera");
		return -1;
	}
	if (kasjerPid == 0) {
		if(execv(kasjerArgs[0], kasjerArgs) == -1) {
			perror("[ERR] Blad uruchomienia aplikacji kasjera");
			return -1;
		}
	}
	return 0;
}

int initKolejka() {
	char *const kolejkaArgs[] = { "./bin/kolejka", NULL };
	if((kolejkaPid = fork()) == -1) {
		perror("[ERR] Blad inicjalizacji procesu dla kolejki");
		return -1;
	}
	if (kolejkaPid == 0) {
		if(execv(kolejkaArgs[0], kolejkaArgs) == -1) {
			perror("[ERR] Blad uruchomienia aplikacji kolejki");
			return -1;
		}
	}
	return 0;
}
