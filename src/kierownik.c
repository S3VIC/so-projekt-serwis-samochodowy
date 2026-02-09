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
	int i;
	processAction pa, paResponse;
	size_t paMessageSize;
	paMessageSize = 2 * sizeof(pa.stop);
	printf("[INF] Kierownik: Zamykanie symulacji... (Otrzymano sygnal: %d)\n", sig);
	pa.type = MECHANIK_STOP_MESSAGE;
	for(i = 0; i < LICZBA_MECHANIKOW; i++) {
		if(msgsnd(kierownikMsgQId, &pa, paMessageSize, 0) == -1)
			printf("[ERR] Kierownik: blad przesylu wiadomosci do %s%d\n", "Mechanik", i);
		if(msgrcv(kierownikMsgQId, &paResponse, paMessageSize, MECHANIK_STOP_MESSAGE, 0) == -1)
			printf("[ERR] Kierownik: blad otrzymania potwierdzenia zatrzymania od %s%d\n", "Mechanik", i);
		else {
			wait(NULL);
			mechanicy[i].running = 0;
		}
	}
	pa.type = KASJER_STOP_MESSAGE;
	if(msgsnd(kierownikMsgQId, &pa, paMessageSize, 0) == -1)
		printf("[ERR] Kierownik: blad przesylu wiadomosci do %s\n", "Kasjer");
	if(msgrcv(kierownikMsgQId, &pa, paMessageSize, KASJER_STOP_MESSAGE, 0) == -1)
		printf("[ERR] Kierownik: blad otrzymania potwierdzenia zatrzymania od %s\n", "Kasjer");
	else
		kasjer.running = 0;
	pa.type = OBSLUGA_KLIENTA_STOP_MESSAGE;
	for(i = 0; i < LICZBA_ST_OBSLUGI_KLIENTA; i++) {
		if(msgsnd(kierownikMsgQId, &pa, paMessageSize, 0) == -1)
			printf("[ERR] Kierownik: blad przesylu wiadomosci do %s%d\n", "Obsluga Klienta", i);
		if(msgrcv(kierownikMsgQId, &pa, paMessageSize, OBSLUGA_KLIENTA_STOP_MESSAGE, 0) == -1)
			printf("[ERR] Kierownik: blad otrzymania potwierdzenia zatrzymania od %s%d\n", "Obsluga Klienta", i);
		else
			stObsKlienta[i].running = 0;
	}
	pa.type = KOLEJKA_STOP_MESSAGE;
	if(msgsnd(kierownikMsgQId, &pa, paMessageSize, 0) == -1)
		printf("[ERR] Kierownik: blad przesylu wiadomosci do %s\n", "Kolejka");
	if(msgrcv(kierownikMsgQId, &pa, paMessageSize, KOLEJKA_STOP_MESSAGE, 0) == -1)
		printf("[ERR] Kierownik: blad otrzymania potwierdzenia zatrzymania od %s%d\n", "Obsluga Klienta", i);
	for(i = 0; i < LICZBA_ST_OBSLUGI_KLIENTA + 2; i++)
		wait(NULL);
	msgctl(kierownikMsgQId, IPC_RMID, NULL);
	exit(0);
}

