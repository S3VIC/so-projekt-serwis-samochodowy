#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "shared.h"

int kierownikMsgQId, mechanikId;
processReport report;

int main(void) {
	initMsgQ(&kierownikMsgQId, ID_KIEROWNIK, IPC_CREAT | 0600, -1);
	if(msgrcv(kierownikMsgQId, &report, sizeof(report.id), ID_MECHANIK, 0) == -1) {
		printf("[ERR] Mechanik: Blad przesylu wiadomosci od kierownika\n");
	} else
		mechanikId = report.id;
	printf("Mechanik: M%d gotowy\n", mechanikId);
	sleep(1);
	return 0;
}
