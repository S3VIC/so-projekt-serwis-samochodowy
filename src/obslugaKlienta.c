#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared.h"

int kierownikMsgQId, obslugaKlientaId;
processReport report;
processAction pa;
size_t paMessageSize;
int main() {
	paMessageSize = 2 * sizeof(pa.stop);
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | 0600, -1);
	if(msgrcv(kierownikMsgQId, &report, sizeof(report.id), ID_OBSLUGA_KLIENTA, 0) == -1) {
		printf("[ERR] Obsluga Klienta: Blad przesylu wiadomosci od kierownika\n");
	} else
		obslugaKlientaId = report.id;
	if(msgsnd(kierownikMsgQId, &report, sizeof(report.id), 0) == -1)
		printf("[ERR] Obsluga Klienta: Blad wysylania potwierdzenia startu pracy do kierownika\n");
	if(msgrcv(kierownikMsgQId, &pa, paMessageSize, ID_OBSLUGA_KLIENTA + 10, 0) == -1) {
		printf("[ERR] Obsluga Klienta %d: Blad odbioru wiadomosci przerwania pracy od kierownika\n", obslugaKlientaId);
		return 1;
	}
	pa.type = OBSLUGA_KLIENTA_STOP_MESSAGE;
	if(msgsnd(kierownikMsgQId, &pa, paMessageSize, 0) == -1)
		printf("[ERR] Obsluga Klienta %d: Blad wysylania potwierdzenia przerwania pracy do kierownika\n", obslugaKlientaId);
	return 0;
}
