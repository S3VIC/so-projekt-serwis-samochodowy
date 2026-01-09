#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LICZBA_STANOWISK_OBS_KL 3
int main() {
	int stObslKlPids[3],	kasjerPid, procesKonczacy, i;
	char *const obslKlArgs[] = { "./bin/obslugaKlienta", NULL };
	char *const kasjerArgs[] = { "./bin/kasjer", NULL };
	for(i = 0; i < LICZBA_STANOWISK_OBS_KL; i++) {
		stObslKlPids[i] = fork();
		if((stObslKlPids[i] = fork()) == -1) {
			perror("[ERR] Blad inicjalizacji procesu dla st. obslugi klienta");
			continue;
		}
		if(stObslKlPids[i] == 0) {
			if(execv(obslKlArgs[0], obslKlArgs) == -1) {
				perror("[ERR] Blad uruchomienia aplikacji st. obslugi klienta");
				exit(1);
			}
		}
	}
	kasjerPid = fork();
	if((kasjerPid = fork()) == -1) {
		perror("[ERR] Blad inicjalizacji procesu dla kasjera");
	}
	if (kasjerPid == 0) {
		if(execv(kasjerArgs[0], kasjerArgs) == -1) {
			perror("[ERR] Blad uruchomienia aplikacji kasjera");
			exit(1);
		}
	}

	printf("Recepcja zainicjalizowana\n");
	sleep(1);
	for(i = 0; i < LICZBA_STANOWISK_OBS_KL; i++) {
		procesKonczacy = waitpid(stObslKlPids[i], NULL, 0);
		if(procesKonczacy == stObslKlPids[i])
			printf("[INF] Stanowisko obslugi klienta zakonczylo prace\n");
	}

	procesKonczacy = waitpid(kasjerPid, NULL, 0);
	if(procesKonczacy == kasjerPid)
		printf("[INF] Kasjer zakonczyl prace\n");
	
	return 0;
}
