#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LICZBA_MECHANIKOW 8

int main() {
	int mechanikPid, i;
	int mechanicy[LICZBA_MECHANIKOW];
	int procesKonczacy;
	char mechanikStrId[5];
	char *mechanikArgs[3];
	mechanikArgs[0] = "./bin/mechanik";
	mechanikArgs[1] = mechanikStrId;
	mechanikArgs[2] = NULL;
	/*Inicjalizacja mechanikow*/
	for(i = 0; i < LICZBA_MECHANIKOW; i++) {
		mechanikPid = fork();
		if(mechanikPid == -1) {
			perror("[ERR] Blad tworzenia procesu mechanika");
			continue;
		}
		if(mechanikPid == 0) {
			sprintf(mechanikStrId, "%d", i);
			if(execv(mechanikArgs[0], mechanikArgs) == -1) {
				perror("[ERR] Blad inicjalizacji mechanika");
				exit(1);
			}
		}
		mechanicy[i] = mechanikPid;
	}
	sleep(2);
	for(i = 0; i < LICZBA_MECHANIKOW; i++) {
		procesKonczacy = waitpid(mechanicy[i], NULL, 0);
		if(procesKonczacy == mechanicy[i]) {
			printf("Mechanik: %d zakonczyl prace\n", i);
		}
	}
	return 0;
}
