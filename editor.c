#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include "types.h"

int count_files(const char *directory) {
    struct dirent *entry;
    DIR *dir = opendir(directory);
    int count = 0;

    // Check if the directory opens correctly
    if (dir == NULL) {
        perror("Error opening directory");
        return -1;  // Return -1 if there's an error
    }

    // Traverse each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Ignore entries "." and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Check if the entry is a regular file (DT_REG means regular file)
        if (entry->d_type == DT_REG) {
            count++;
        }
    }

    // Close the directory after reading
    closedir(dir);

    return count;
}
char* GetRidOfString(char* wordOrSentence, const char* delim) {
    int len_delim = strlen(delim);
    char *pos;

    // Créer une copie modifiable de `wordOrSentence` pour ne pas modifier l'original
    char *result = malloc(strlen(wordOrSentence) + 1);
    if (result == NULL) {
        return NULL; // Gestion de l'erreur d'allocation
    }
    strcpy(result, wordOrSentence);

    // Tant que la sous-chaîne est trouvée dans la chaîne principale
    while ((pos = strstr(result, delim)) != NULL) {
        // Décaler tout le texte après la sous-chaîne vers la gauche
        memmove(pos, pos + len_delim, strlen(pos + len_delim) + 1);
    }

    return result;
}
Dictionnaire count_word(Dictionnaire imgTable, char* words) {
    // int cpt = 0;

    const char delim[] = "-";
    char *token;
    token = strtok(words, delim);
    // Enlever le .jpg plus tard
    imgTable.cptWord = 0;
    // Boucle pour récupérer tous les tokens
    while (token != NULL) {
        // Enlevage des extensions
        // TODO Liste de chaines plutot qu'un appel multiple
        token = GetRidOfString(token, ".jpg");
        token = GetRidOfString(token, ".png");
        token = GetRidOfString(token, ".svg");
        // Affectation du mot x dans le tableau de mots de l'image
        strncpy(imgTable.words[imgTable.cptWord], token, NB_MAX_LETTERS - 1);
        imgTable.words[imgTable.cptWord][NB_MAX_LETTERS - 1] = '\0';
        // Incrémentation du compteur
        imgTable.cptWord ++;
        token = strtok(NULL, delim);
    }

    // Commentaire
    printf("Nom de fichier : %s\n", imgTable.pathImg);
    for(int i = 0 ; i < imgTable.cptWord; i++){
        printf("\t");
        printf("Mot numéro [%d] = %s\n", i, imgTable.words[i]);
    }
    printf("\n\n");

    return imgTable;
}
Dictionnaire* getImgTable(int countImgFiles) {
    const char* imgFolder = "./img";
    struct dirent * entree;
    DIR *repo = opendir(imgFolder);
    Dictionnaire* imgTable ;
    int cpt = 0;
    

    imgTable = (Dictionnaire*)malloc(countImgFiles*sizeof(Dictionnaire));

    if (repo == NULL) {
        perror("Erreur lors de l'ouverture du dossier");
        return imgTable;
    }

    while ((entree = readdir(repo)) != NULL) {
        char actualWord[NB_MAX_LETTERS];
        if (strcmp(entree-> d_name, ".") == 0 || strcmp(entree-> d_name, "..") == 0) {
            continue;
        }
    imgTable[cpt].pathImg = entree->d_name;
    printf("%s\n", imgTable[cpt].pathImg);
    strcpy(actualWord,entree->d_name);
    count_word(imgTable[cpt], actualWord);

    cpt ++;
    }
    closedir(repo);

    return imgTable;
} 
int generate_random_int(int min, int max) {
    int random_number = min + rand() % (max - min + 1);
    printf("Nombre aléatoire entre %d et %d : %d\n", min, max, random_number);

    return random_number;
}

int main(int argc, char *argv[]) {
    Dictionnaire* imgTable;
    int countImgFiles;
    char* randomImg;
    char pathImg[NB_MAX_LETTERS];
    countImgFiles = count_files(IMAGE_FOLDER);
    imgTable = getImgTable(countImgFiles);
    srand(time(NULL));

    randomImg = imgTable[generate_random_int(0,countImgFiles-1)].pathImg;
    strcpy(pathImg, IMAGE_FOLDER);
    strcat(pathImg,randomImg);
    printf("%s\n",pathImg);

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Initialiser SDL_image
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("Erreur SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    // Créer une fenêtre
    SDL_Window *window = SDL_CreateWindow("Charger une image PNG avec SDL2", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur création fenêtre: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Créer un renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) {
        printf("Erreur création renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Charger une image PNG
    SDL_Surface *imageSurface = IMG_Load(pathImg);
    if (!imageSurface) {
        printf("Erreur chargement image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Créer une texture à partir de la surface
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);  // On peut libérer la surface après avoir créé la texture

    if (!texture) {
        printf("Erreur création texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Boucle d'événements
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Effacer l'écran
        SDL_RenderClear(renderer);

        // Dessiner la texture
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // Afficher
        SDL_RenderPresent(renderer);
    }

    
    printf("%d\n", NB_MAX_WORDS);
    printf("%d\n", NB_MAX_LETTERS);

    // Nettoyer
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}



