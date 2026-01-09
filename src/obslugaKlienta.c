#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
	int working; 	/* flaga na okreslenie czy stanowisko pracuje */
	int id; 		/* 0-2 (3 stanowiska) */
} stanowisko;

int working = 0;
int main() {
	stanowisko s = {0};
	s.id = 1;
	printf("[INF] Proces stanowiska %d obslugi klienta zaincjalizowany pomyslnie\n", s.id);
	sleep(2);
	return 0;
}
