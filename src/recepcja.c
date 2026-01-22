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

int stObslKlPids[3], kasjerPid, kolejkaPid, kierownikMsgQId;
initKom kolejkaInit, kasjerInit;
reception recepcja;
int main() {
	int procesKonczacy, i;
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | 0600, -1);
	/*TODO Error handling */
	if(initObslugaKlienta() != 0) { return 1; } 	
	if(initKasjer() != 0) { return 1; }
	if(initKolejka() != 0) { return 1; }
	recepcja.typ = ID_RECEPCJA;
	recepcja.pid = getpid();
	if(msgsnd(kierownikMsgQId, &recepcja, sizeof(pid_t) * (3 + LICZBA_ST_OBSLUGI_KLIENTA), 0) == -1) {
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
	int i, res;
	char *const obslKlArgs[] = { "./bin/obslugaKlienta", NULL };
	res = 0;
	for(i = 0; i < LICZBA_STANOWISK_OBS_KL; i++)
		recepcja.obslugaKlienta[i] = -1;
	for(i = 0; i < LICZBA_STANOWISK_OBS_KL; i++) {
		if((recepcja.obslugaKlienta[i] = fork()) == -1) {
			perror("[ERR] Blad inicjalizacji procesu dla st. obslugi klienta");
			res = -1;
			break;
		}
		if(recepcja.obslugaKlienta[i] == 0) {
			if(execv(obslKlArgs[0], obslKlArgs) == -1) {
				perror("[ERR] Blad uruchomienia aplikacji st. obslugi klienta");
				res = -1;
				break;
			}
		}
	}
	if(res == -1) {
		perror("[ERR] Blad podczas inicjalizacji obslugi klienta");
		return res;
	}
	return res;
}

int initKasjer() {
	char *const kasjerArgs[] = { "./bin/kasjer", NULL };
	kasjerInit.typ = ID_KASJER;
	kasjerInit.pid = initProcess("kasjer", kasjerArgs);
	if(msgsnd(kierownikMsgQId, &kasjerInit, sizeof(kasjerInit.pid), 0) == -1) {
		printf("[ERR] Recepcja: Blad wysylania informacji o inicjalizacji kasjera\n");
		return -1;
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
	kolejkaInit.typ = ID_KOLEJKA;
	kolejkaInit.pid = kolejkaPid;
	if(msgsnd(kierownikMsgQId, &kolejkaInit, sizeof(kolejkaInit.pid), 0) == -1) {
		printf("[ERR] Recepcja: Blad wysylania informacji o inicjalizacji kolejki\n");
		return -1;
	}
	return 0;
}
