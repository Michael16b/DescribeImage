#define NB_MAX_WORDS 5
#define NB_MAX_LETTERS 35
#define IMAGE_FOLDER "./img/"
#define FONT_FOLDER "./font/"

typedef struct Dictionnaire {
    char* pathImg;
    char words[NB_MAX_WORDS][NB_MAX_LETTERS];
    int cptWord;
} Dictionnaire;