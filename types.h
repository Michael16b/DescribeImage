#ifndef TYPESDEF
#define TYPESDEF


#define NB_MAX_WORDS 5
#define NB_MAX_LETTERS 50
#define IMAGE_FOLDER "./img/"
#define ADRESS "127.0.0.1"
#define PORT 5000

typedef struct Dictionnaire {
    char* pathImg;
    char words[NB_MAX_WORDS][NB_MAX_LETTERS];
    int cptWord;
} Dictionnaire;


#endif