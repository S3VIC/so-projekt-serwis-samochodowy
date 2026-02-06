#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared.h"

int kierownikMsgQId, obslugaKlientaId;
processReport report;
int main() {
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | 0600, -1);
	if(msgrcv(kierownikMsgQId, &report, sizeof(report.id), ID_OBSLUGA_KLIENTA, 0) == -1) {
		printf("[ERR] Mechanik: Blad przesylu wiadomosci od kierownika\n");
	} else
		obslugaKlientaId = report.id;
	printf("Obsluga klienta: OK%d gotowa\n", obslugaKlientaId);
	sleep(1);
	return 0;
}
