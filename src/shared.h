#include <sys/msg.h>
#include <string.h>
#define SLEEP_TIME_S 1
#define SLEEP_TIME_US 5000

#define LICZBA_MECHANIKOW 8
#define LICZBA_ST_OBSLUGI_KLIENTA 3
/*
	ID wiadomosci przekazywanych do kolejki kierownika
*/
#define ID_KIEROWNIK 0
#define ID_MECHANIK 1
#define ID_OBSLUGA_KLIENTA 2
#define ID_KOLEJKA 3
#define ID_KASJER 4

#define MECHANIK_STOP_MESSAGE 11
#define OBSLUGA_KLIENTA_STOP_MESSAGE 12
#define KOLEJKA_STOP_MESSAGE 13
#define KASJER_STOP_MESSAGE 14


const char* sSciezka = "./bin";

typedef struct {
	pid_t pid;
	int running;
} processDetails;

typedef struct {
	long int type;
	unsigned int id;
} processReport;

typedef struct {
	long int type;
	int stop;
	int confirmation;
} processAction;

typedef enum {
 	U1 = 0, U2, U3, U4, U5, U6, U7, U8,
	U9, U10, U11, U12, U13, U14, U15, U16,
	U17, U18, U19, U20, U21, U22, U23, U24,
	U25, U26, U27, U28, U29, U30, U31, U32,
	U33
} usluga;

int uslugiKoszty[33] = {
	5, 10, 15, 20, 25, 30, 35, 40,
	45, 50, 55, 60, 65, 70, 75, 80,
	85, 90, 95, 100, 105, 110, 115,
	120, 125, 130, 135, 140, 145, 150,
	250, 300, 350
};

int uslugiCzas[33] = {
	5, 5, 10, 5, 15, 25, 10, 15, 10, 15,
	20, 25, 35, 20, 25, 15, 35, 40, 30, 15,
	30, 35, 30, 45, 40, 20, 25, 30, 35, 20,
	50, 55, 50
};

const char* moduly[6] = {
	"Kierownik",
	"Recepcja",
	"Serwis",
	"Kolejka",
	"Obsluga klienta",
	"Kasjer"
};

int initMsgQ(int *qid, int id, int flags, int id2) {
	int klucz;
	if((klucz = ftok(sSciezka, id)) == -1) {
		if(id2 >= 0)
			printf("[ERR] Blad generacji klucza kolejki dla: %s-%s\n", moduly[id], moduly[id2]);
		else
			printf("[ERR] Blad generacji klucza kolejki dla: %s\n", moduly[id]);
		return -1;
	}
	if((*qid = msgget(klucz, flags)) == -1) {
		if(id2 >= 0)
			printf("[ERR] Blad inicjalizacji kolejki: %s-%s\n", moduly[id], moduly[id2]);
		else
			printf("[ERR] Blad inicjalizacji kolejki: %s\n", moduly[id]);
		return -1;
	}
	return 0;
}

int initProcess(const char* name, char *const args[]) {
	int pid;
	if((pid = fork()) == -1) {
		printf("[ERR] Blad inicjalizacji procesu: %s", name);
		return pid;
	}
	if(pid == 0) {
		if(execv(args[0], args) == -1) {
			printf("[ERR] Blad uruchomienia aplikacji: %s\n", name);
			return -1;
		}
	}
	return pid;
}

int initProcessGroup(const char* name, char *const args[], int count, processDetails* details) {
	int i, res;
	for(i = 0; i < count; i++) {
		res = initProcess(name, args);
		if(details) {
			details[i].pid = res;
			details[i].running = 1;
		}

	}
	return 0;
}

