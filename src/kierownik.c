#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef enum Pracownik {
	SERWIS,
	LICZBA_USLUG
} Pracownik;

int main() {
	int serwisPid;
	int recepcjaPid;
	int procesKonczacy;
	char *const serwisArgs[] = { "./bin/serwis", NULL };
	char *const recepcjaArgs[] = { "./bin/recepcja", NULL };
	/*Inicjalizacja recepcji*/
	recepcjaPid = fork();
	if(recepcjaPid == 0) {
		if(execv(recepcjaArgs[0], recepcjaArgs) == -1) {
			perror("[ERR] Blad inicjalizacji recepcji");
			exit(1);
		}
	}
	/*Inicjalizacja serwisu*/
	if((serwisPid = fork()) == 0) {
		if(execv(serwisArgs[0], serwisArgs) == -1) {
			perror("[ERR] Blad inicjalizacji serwisu");
			exit(1);
		}
	}
	procesKonczacy = waitpid(serwisPid, NULL, 0);
	if(procesKonczacy == serwisPid) {
		printf("Serwis zakonczyl prace\n");
	}
	procesKonczacy = waitpid(recepcjaPid, NULL, 0);
	if(procesKonczacy == recepcjaPid) {
		printf("Recepcja zakonczyla prace\n");
	}
	return 0;
}
