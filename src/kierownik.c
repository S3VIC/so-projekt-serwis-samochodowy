#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shared.h"
int initMechanicy();
int initObslugaKlienta();
int initKolejka();
int initProcesses(const char* name, char *const args[], int count, int msgType, processDetails* details);
int stopProcesses(const char* name, int count, int msgType, processDetails *details);
void handleSigInt(int sig);

int kierownikMsgQId;
processDetails mechanicy[LICZBA_MECHANIKOW];
processDetails stObsKlienta[LICZBA_ST_OBSLUGI_KLIENTA];
processDetails kasjer;
processDetails kolejka;
processReport report;
int main() {
	char *const mechanikArgs[] = { "./bin/mechanik", NULL };
	char *const obslugaKlientaArgs[] = { "./bin/obslugaKlienta", NULL };
	char *const kolejkaArgs[] = { "./bin/kolejka", NULL };
	char *const kasjerArgs[] = { "./bin/kasjer", NULL };
	int res, i;
	signal(SIGINT, handleSigInt);
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | IPC_EXCL | 0600, -1);

	res = initProcesses("Mechanik", mechanikArgs, LICZBA_MECHANIKOW, ID_MECHANIK, mechanicy);
	if(res < 0) exit(1);
	res = initProcesses("Obsluga Klienta", obslugaKlientaArgs, LICZBA_ST_OBSLUGI_KLIENTA, ID_OBSLUGA_KLIENTA, stObsKlienta);
	if(res < 0) exit(2);
	res = initProcesses("Kolejka", kolejkaArgs, 1, ID_KOLEJKA, &kolejka);
	if(res < 0) exit(3);
	res = initProcesses("Kasjer", kasjerArgs, 1, ID_KASJER, &kasjer);
	if(res < 0) exit(4);
	/* + 2 : proces kolejki + proces kasjera */
	sleep(500);

	for(i = 0; i < LICZBA_MECHANIKOW + LICZBA_ST_OBSLUGI_KLIENTA + 2; i++)
		wait(NULL);
	msgctl(kierownikMsgQId, IPC_RMID, NULL);
	return 0;
}

int initProcesses(const char* name, char *const args[], int count, int msgType, processDetails* details) {
	int res, i;
	processReport report;
	res = initProcessGroup(name, args, count, details);
	if(res < 0) return res;
	report.type = msgType;
	for(i = 0; i < count; i++) {
		report.id = i;
		if(msgsnd(kierownikMsgQId, &report, sizeof(report.id), 0) == -1)
			printf("[ERR] Kierownik: blad przesylu wiadomosci do %s%d\n", name, i);
		if(msgrcv(kierownikMsgQId, &report, sizeof(report.id), msgType, 0) == -1)
			printf("[ERR] Kierownik: blad odbioru wiadomosci startu od %s%d\n", name, i);
		else
			printf("[INF] %s %d gotowy\n", name, i);
	}
	return 0;
}

void handleSigInt(int sig) {
	printf("[INF] Kierownik: Zamykanie symulacji... (Otrzymano sygnal: %d)\n", sig);
	/*Przyda sie zapauzowac kolejke przed stopowaniem pozostalych procesów - ale i tak zatrzymac ja na koncu*/
	stopProcesses("Mechanik", LICZBA_MECHANIKOW, MECHANIK_STOP_MESSAGE, mechanicy);
	stopProcesses("Kasjer", 1, KASJER_STOP_MESSAGE, &kasjer);
	stopProcesses("Obsluga klienta", LICZBA_ST_OBSLUGI_KLIENTA, OBSLUGA_KLIENTA_STOP_MESSAGE, stObsKlienta);
	stopProcesses("Kolejka", 1, KOLEJKA_STOP_MESSAGE, &kolejka);
	msgctl(kierownikMsgQId, IPC_RMID, NULL);
	exit(0);
}

int stopProcesses(const char* name, int count, int msgType, processDetails *details) {
	int i;
	processAction pa;
	size_t paMessageSize;
	paMessageSize = 2 * sizeof(pa.stop);
	pa.type = msgType;
	for(i = 0; i < count; i++) {
		if(msgsnd(kierownikMsgQId, &pa, paMessageSize, 0) == -1)
			printf("[ERR] Kierownik: blad przesylu wiadomosci do %s %d\n", name, i);
		if(msgrcv(kierownikMsgQId, &pa, paMessageSize, msgType, 0) == -1)
			printf("[ERR] Kierownik: blad otrzymania potwierdzenia zatrzymania od %s %d\n", name, i);
		else {
			wait(NULL);
			details[i].running = 0;
		}
	}
	return 0;
}
