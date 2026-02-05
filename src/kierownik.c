#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shared.h"
#include <sys/msg.h>

int initMechanicy();
int initObslugaKlienta();
int initKolejka();

int kierownikMsgQId;
int mechanicy[LICZBA_MECHANIKOW];
int stObsKlienta[LICZBA_ST_OBSLUGI_KLIENTA];

int main() {
	char *const mechanikArgs[] = { "./bin/mechanik", NULL };
	char *const obslugaKlientaArgs[] = { "./bin/obslugaKlienta", NULL };
	/*char *const kolejkaArgs[] = { "./bin/kolejka", NULL };*/
	int res, i;
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | IPC_EXCL | 0600, -1);

	res = initProcessGroup("mechanik", mechanikArgs, LICZBA_MECHANIKOW, mechanicy);
	if(res < 0) exit(1);
	initProcessGroup("obslugaKlienta", obslugaKlientaArgs, LICZBA_ST_OBSLUGI_KLIENTA, stObsKlienta);
	if(res < 0) exit(1);
	for(i = 0; i < LICZBA_MECHANIKOW + LICZBA_ST_OBSLUGI_KLIENTA; i++) {
		wait(NULL);
	}

	msgctl(kierownikMsgQId, IPC_RMID, NULL);
	return 0;
}

int initKolejka() {
	return 0;
}

