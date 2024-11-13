#define NB_MAX_WORDS 5
#define NB_MAX_LETTERS 50
#define IMAGE_FOLDER "./img/"

typedef struct Dictionnaire {
    char* pathImg;
    char words[NB_MAX_WORDS][NB_MAX_LETTERS];
    int cptWord;
} Dictionnaire;