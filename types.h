#ifndef TYPES_H
#define TYPES_H

// Constantes
#define SERVER_PORT 8888
#define SERVER_ADRESS "127.0.0.1"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10
#define MAX_CAR 50
#define NB_SECONDES 20

// Messages :
#define MSG_DEB_MANCHE "La manche commence !"
#define MSG_FIN_MANCHE "La manche est terminée !"
#define MSG_FIN_PARTIE "La partie est terminée !"
#define MSG_ATTENTE "En attente d'une partie"
#define MSG_WAIT_CHRONO "Plus que "
#define MSG_SECS " secondes !"
#define MSG_SEC " seconde !"


// Codes : 
#define CHRONO "1093"
#define MESSAGE "9240"
#define MANCHE "2356"
#define NEW_IMG "1203"

// Couleurs d'écritures : 
#define D_ROUGE "\033[0;31m"
#define F_ROUGE "\033[0m"
#define D_VERT "\033[0;32m"
#define F_VERT "\033[0m"
#define D_JAUNE "\033[0;33m"
#define F_JAUNE "\033[0m"
#define D_BLEU "\033[0;34m"
#define F_BLEU "\033[0m"

/*
printf("\033[0;35mTexte en magenta\033[0m\n");   // Magenta
printf("\033[0;36mTexte en cyan\033[0m\n");      // Cyan
printf("\033[0;37mTexte en blanc\033[0m\n");     // Blanc
*/

#define NB_MAX_WORDS 5
#define NB_MAX_LETTERS 35
#define IMAGE_FOLDER "./img/"
#define FONT_FOLDER "./font/"

typedef struct Dictionnaire {
    char* pathImg;
    char words[NB_MAX_WORDS][NB_MAX_LETTERS];
    int cptWord;
} Dictionnaire;

// Structure 
typedef struct {
    struct sockaddr_in addr;
    int addr_len;
} Client;

#endif 