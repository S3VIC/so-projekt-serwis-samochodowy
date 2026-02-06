#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shared.h"

int initMechanicy();
int initObslugaKlienta();
int initKolejka();

int kierownikMsgQId;
int mechanicy[LICZBA_MECHANIKOW];
int stObsKlienta[LICZBA_ST_OBSLUGI_KLIENTA];
processReport report;
int main() {
	char *const mechanikArgs[] = { "./bin/mechanik", NULL };
	char *const obslugaKlientaArgs[] = { "./bin/obslugaKlienta", NULL };
	char *const kolejkaArgs[] = { "./bin/kolejka", NULL };
	int res, i;
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | IPC_EXCL | 0600, -1);

	res = initProcessGroup("mechanik", mechanikArgs, LICZBA_MECHANIKOW, mechanicy);
	if(res < 0) exit(1);
	report.type = ID_MECHANIK;
	for(i = 0; i < LICZBA_MECHANIKOW; i++) {
		report.id = i;
		if(msgsnd(kierownikMsgQId, &report, sizeof(report.id), 0) == -1)
			printf("[ERR] Kierownik: blad przesylu wiadomosci do mechanika %d\n", i);
	}
	res = initProcessGroup("obslugaKlienta", obslugaKlientaArgs, LICZBA_ST_OBSLUGI_KLIENTA, stObsKlienta);
	if(res < 0) exit(2);
	res = initProcess("kolejka", kolejkaArgs);
	if(res < 0) exit(3);
	for(i = 0; i < LICZBA_MECHANIKOW + LICZBA_ST_OBSLUGI_KLIENTA + 1; i++) {
		wait(NULL);
	}

	msgctl(kierownikMsgQId, IPC_RMID, NULL);
	return 0;
}

