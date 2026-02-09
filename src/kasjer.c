#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared.h"

int kierownikMsgQId;
processReport report;
processAction pa;
size_t paMessageSize;
int main() {
	paMessageSize = 2 * sizeof(pa.stop);
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | 0600, -1);
	if(msgrcv(kierownikMsgQId, &report, sizeof(report.id), ID_KASJER, 0) == -1) {
		printf("[ERR] Kasjer: Blad przesylu wiadomosci od kierownika\n");
	}

	if(msgsnd(kierownikMsgQId, &report, sizeof(report.id), 0) == -1)
		printf("[ERR] Kasjer: Blad wysylania potwierdzenia startu pracy do kierownika\n");

	if(msgrcv(kierownikMsgQId, &pa, paMessageSize, KASJER_STOP_MESSAGE, 0) == -1) {
		printf("[ERR] Kasjer: Blad odbioru wiadomosci przerwania pracy od kierownika\n");
		return 1;
	}
	pa.type = KASJER_STOP_MESSAGE;
	if(msgsnd(kierownikMsgQId, &pa, paMessageSize, 0) == -1)
		printf("[ERR] Kasjer: Blad wysylania potwierdzenia przerwania pracy do kierownika\n");
	return 0;
}
