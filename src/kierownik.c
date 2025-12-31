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
	/*pid_t serwis[LICZBA_USLUG]*/;
	if((serwisPid = fork()) == 0) {
		sleep(5);
		exit(0);
	}
	procesKonczacy = wait(NULL);
	if(procesKonczacy == serwisPid) {
		printf("Serwis zakonczyl prace\n");
	}
	return 0;
}
