#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, const char* argv[]) {
	int id;
	sleep(1);
	if(argc < 2) {
		perror("[ERR] Brak id mechanika");
		sleep(1);
	} else {
		id = atoi(argv[1]);
		printf("Mechanik: %d\n", id);
	}
	return 0;
}
