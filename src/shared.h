#include <sys/msg.h>
#include <string.h>
#define SLEEP_TIME_S 1
#define SLEEP_TIME_US 5000
#define ID_KIEROWNIK 0
#define ID_RECEPCJA 1
#define ID_SERWIS 2
#define ID_KOLEJKA 3
#define ID_OBSLUGA_KLIENTA 4
#define ID_KASJER 5

#define LICZBA_MECHANIKOW 8
#define LICZBA_ST_OBSLUGI_KLIENTA 3

const char* sSciezka = "./bin";

typedef struct {
	pid_t recepcja;
	pid_t kolejka;
	pid_t kasjer;
	pid_t obslugaKlienta[LICZBA_ST_OBSLUGI_KLIENTA];
	pid_t serwis[LICZBA_MECHANIKOW];
} serwis;

typedef struct {
	long int typ;
	int pids[LICZBA_MECHANIKOW];
} initSerwisKom;

typedef struct {
	long int typ;
	int pids[LICZBA_ST_OBSLUGI_KLIENTA];
} initObslugaKlientaKom;

typedef struct {
	long int typ;
	int pid;
} initKom;

typedef struct {
	char model;
	int pid;
	int usterki[LICZBA_ST_OBSLUGI_KLIENTA];
} kierowca;

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

int initKierownikMsgQ(int *id, int flags) {
	int klucz;
	if((klucz = ftok(sSciezka, ID_KIEROWNIK)) == -1) {
		printf("[ERR] Blad generacji klucza dla kolejki kierownika\n");
		return -1;
	}
	if((*id = msgget(klucz, flags)) == -1) {
		printf("[ERR] Blad inicjalizacji kolejki kierownika\n");
		return -1;
	}
	return 0;
}
