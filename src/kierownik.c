#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shared.h"

int initMechanicy();
int initObslugaKlienta();
int initKolejka();
int initProcesses(const char* name, char *const args[], int count, int msgType);

int kierownikMsgQId;
processDetails mechanicy[LICZBA_MECHANIKOW];
processDetails stObsKlienta[LICZBA_ST_OBSLUGI_KLIENTA];
processReport report;
int main() {
	char *const mechanikArgs[] = { "./bin/mechanik", NULL };
	char *const obslugaKlientaArgs[] = { "./bin/obslugaKlienta", NULL };
	char *const kolejkaArgs[] = { "./bin/kolejka", NULL };
	char *const kasjerArgs[] = { "./bin/kasjer", NULL };
	int res, i;
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | IPC_EXCL | 0600, -1);

	res = initProcesses("Mechanik", mechanikArgs, LICZBA_MECHANIKOW, ID_MECHANIK);
	if(res < 0) exit(1);
	res = initProcesses("Obsluga Klienta", obslugaKlientaArgs, LICZBA_ST_OBSLUGI_KLIENTA, ID_OBSLUGA_KLIENTA);
	if(res < 0) exit(2);
	res = initProcess("Kolejka", kolejkaArgs);
	if(res < 0) exit(3);
	report.type = ID_KOLEJKA;
	report.id = 0;
	if(msgsnd(kierownikMsgQId, &report, sizeof(report.id), 0) == -1)
		printf("[ERR] Kierownik: blad przesylu wiadomosci do: Kolejka\n");
	
	res = initProcess("Kasjer", kasjerArgs);
	if(res < 0) exit(4);
	report.type = ID_KASJER;
	if(msgsnd(kierownikMsgQId, &report, sizeof(report.id), 0) == -1)
		printf("[ERR] Kierownik: blad przesylu wiadomosci do: Kasjer\n");

	/* + 2 : proces kolejki + proces kasjera */
	for(i = 0; i < LICZBA_MECHANIKOW + LICZBA_ST_OBSLUGI_KLIENTA + 2; i++)
		wait(NULL);

	msgctl(kierownikMsgQId, IPC_RMID, NULL);
	return 0;
}

int initProcesses(const char* name, char *const args[], int count, int msgType) {
	int res, i;
	processReport report;
	res = initProcessGroup(name, args, count, NULL);
	if(res < 0) return res;
	report.type = msgType;
	for(i = 0; i < count; i++) {
		report.id = i;
		if(msgsnd(kierownikMsgQId, &report, sizeof(report.id), 0) == -1)
			printf("[ERR] Kierownik: blad przesylu wiadomosci do %s%d\n", name, i);
	}
	return 0;
}

