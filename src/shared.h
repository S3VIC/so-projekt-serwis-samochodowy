#define SLEEP_TIME_S 1
#define SLEEP_TIME_US 5000
#define MAX_ROZMIAR_KOLEJKI 50



typedef struct {
	unsigned int godzina;
	unsigned int minuty;
	unsigned int otwarteOd;
	unsigned int otwarteDo;
} zegar;

typedef struct {
	char model;
	int pid;
	int usterki[3];
} kierowca;

enum {
 	U1 = 0, U2, U3, U4, U5, U6, U7, U8,
	U9, U10, U11, U12, U13, U14, U15, U16,
	U17, U18, U19, U20, U21, U22, U23, U24,
	U25, U26, U27, U28, U29, U30, U31, U32,
	U33
};

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
