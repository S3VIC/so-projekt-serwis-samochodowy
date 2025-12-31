#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
	sleep(5);
	printf("Hello world\n");
	return 0;
}
