#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LICZBA_STANOWISK_OBS_KL 3

int initObslugaKlienta();
int initKasjer();
int initKolejka();

int stObslKlPids[3], kasjerPid, kolejkaPid;
int main() {
	int  procesKonczacy, res, i;
	if((res = initObslugaKlienta()) != 0) {
		perror("[ERR] Blad podczas inicjalizacji obslugi klienta");
		exit(1);
	}
	if((res = initKasjer()) != 0) {
		perror("[ERR] Blad podczas inicjalizacji kasjera");
		exit(1);
	}
	printf("[INF] Recepcja zainicjalizowana\n");
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


int initObslugaKlienta() {
	int i;
	char *const obslKlArgs[] = { "./bin/obslugaKlienta", NULL };
	for(i = 0; i < LICZBA_STANOWISK_OBS_KL; i++)
		stObslKlPids[i] = -1;
	for(i = 0; i < LICZBA_STANOWISK_OBS_KL; i++) {
		stObslKlPids[i] = fork();
		if((stObslKlPids[i] = fork()) == -1) {
			perror("[ERR] Blad inicjalizacji procesu dla st. obslugi klienta");
			return -1;
		}
		if(stObslKlPids[i] == 0) {
			if(execv(obslKlArgs[0], obslKlArgs) == -1) {
				perror("[ERR] Blad uruchomienia aplikacji st. obslugi klienta");
				return -1;
			}
		}
	}
	return 0;
}

int initKasjer() {
	char *const kasjerArgs[] = { "./bin/kasjer", NULL };
	kasjerPid = fork();
	if((kasjerPid = fork()) == -1) {
		perror("[ERR] Blad inicjalizacji procesu dla kasjera");
		return -1;
	}
	if (kasjerPid == 0) {
		if(execv(kasjerArgs[0], kasjerArgs) == -1) {
			perror("[ERR] Blad uruchomienia aplikacji kasjera");
			return -1;
		}
	}
	return 0;
}
