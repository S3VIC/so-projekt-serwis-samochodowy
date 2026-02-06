#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "shared.h"

int kierownikMsgQId;
processReport report;
int main() {
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | 0600, -1);
	if(msgrcv(kierownikMsgQId, &report, sizeof(report.id), ID_KASJER, 0) == -1) {
		printf("[ERR] Kolejka: Blad przesylu wiadomosci od kierownika\n");
	}
	printf("Kasjer: gotowy\n");
	sleep(1);
	return 0;
}
