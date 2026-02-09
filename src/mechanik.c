#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "shared.h"

int kierownikMsgQId, mechanikId;
processReport report;
processAction pa;
size_t paMessageSize;
int main(void) {
	paMessageSize = 2 * sizeof(pa.stop);
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | 0600, -1);

	if(msgrcv(kierownikMsgQId, &report, sizeof(report.id), ID_MECHANIK, 0) == -1)
		printf("[ERR] Mechanik: Blad przesylu wiadomosci od kierownika\n");
	else
		mechanikId = report.id;

	if(msgsnd(kierownikMsgQId, &report, sizeof(report.id), 0) == -1)
		printf("[ERR] Mechanik: Blad wysylania potwierdzenia startu pracy do kierownika\n");

	if(msgrcv(kierownikMsgQId, &pa, paMessageSize, MECHANIK_STOP_MESSAGE, 0) == -1)
		printf("[ERR] Mechanik: Blad odbioru wiadomosci przerwania pracy od kierownika\n");

	pa.type = MECHANIK_STOP_MESSAGE;
	if(msgsnd(kierownikMsgQId, &pa, paMessageSize, 0) == -1)
		printf("[ERR] Mechanik: Blad wysylania potwierdzenia przerwania pracy do kierownika\n");
	return 0;
}
