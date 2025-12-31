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
	int procesKonczacy;
	char *const serwisArgs[] = { "./bin/serwis", NULL };
	/*pid_t serwis[LICZBA_USLUG]*/;
	if((serwisPid = fork()) == 0) {
		if(execv(serwisArgs[0], serwisArgs) == -1) {
			perror("[ERR] Blad inicjalizacji serwisu");
			exit(1);
		}
	}
	procesKonczacy = wait(NULL);
	if(procesKonczacy == serwisPid) {
		printf("Serwis zakonczyl prace\n");
	}
	return 0;
}
