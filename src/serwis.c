#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shared.h"

int initMechanicy();
int kierownikMsgQId;
initSerwisKom serwisKom;
int main() {
	int i, procesKonczacy;
	initKierownikMsgQ(&kierownikMsgQId, IPC_CREAT | 0600);
	initMechanicy();
	sleep(1);
	for(i = 0; i < LICZBA_MECHANIKOW; i++) {
		procesKonczacy = waitpid(serwisKom.pids[i], NULL, 0);
		if(procesKonczacy == serwisKom.pids[i]) {
			printf("Mechanik: %d zakonczyl prace\n", i);
		}
	}
	return 0;
}

int initMechanicy() {
	int i, mechanikPid, res;
	char mechanikStrId[2], *mechanikArgs[3];
	mechanikArgs[0] = "./bin/mechanik";
	mechanikArgs[1] = mechanikStrId;
	mechanikArgs[2] = NULL;
	res = 0;
	/*Inicjalizacja mechanikow*/
	for(i = 0; i < LICZBA_MECHANIKOW; i++) {
		if((mechanikPid = fork()) == -1) {
			perror("[ERR] Blad tworzenia procesu mechanika");
			continue;
		}
		if(mechanikPid == 0) {
			sprintf(mechanikStrId, "%d", i);
			if(execv(mechanikArgs[0], mechanikArgs) == -1) {
				perror("[ERR] Blad inicjalizacji mechanika");
				res = -1;
			}
		}
		serwisKom.pids[i] = mechanikPid;
	}
	serwisKom.typ = ID_SERWIS;
	if(msgsnd(kierownikMsgQId, &serwisKom, sizeof(serwisKom.pids), 0) == -1) {
		printf("[ERR] Serwis: Blad wysylania informacji o inicjalizacji serwisu\n");
		return -1;
	}
	return res;
}
