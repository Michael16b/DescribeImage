#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "types.h"

int seconds;
SDL_Surface *imageSurface;
SDL_Surface* chronoSurface;
SDL_Texture *chronoTexture;
SDL_Texture *texture;
SDL_Window *window;
SDL_Renderer *renderer;
TTF_Font *font;


const char *trueText;
int validText;

SDL_Color textColor = {0, 0, 0};
SDL_Rect chronoProperties = {380, 0, 440, 50};
char chronoText[256] = MSG_ATTENTE;


void *receive_messages(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    char code[4];
    char message[MAX_CAR];
    char chemin_image[NB_MAX_LETTERS];

    // BOUCLE DE RECEPTION DES MESSAGES DU SERVEUR
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            
            sprintf(code, "%.4s", buffer);
            sprintf(message, "%s", buffer+4);

            // SWITCH SUR LE CODE -> CHRONO - NEW_IMG - MESSAGE
            // CHRONO
            if (strcmp(code, CHRONO) == 0) {
                printf("%s>Message du serveur reçu : \n\t- Code : [%s]\n\t- Message : %s%s\n\n",D_BLEU, code, message, F_BLEU);
                snprintf(chronoText, sizeof(chronoText), "%s%s%s", MSG_WAIT_CHRONO, message, MSG_SECS);
                if (strcmp(message,"1") == 0) {
                    snprintf(chronoText, sizeof(chronoText), "%s%s%s", MSG_WAIT_CHRONO, message, MSG_SEC);
                    sleep(1);
                    snprintf(chronoText, sizeof(chronoText), "%s", MSG_ATTENTE);
                }

                
            // NEW_IMG
            }else if(strcmp(code, NEW_IMG) == 0){
                printf("%s>Message du serveur reçu : \n\t- Code : [%s]\n\t- Message : %s%s\n\n",D_ROUGE, code, message, F_ROUGE);
                    // Charger une image PNG
                    sprintf(chemin_image, "%s%s", IMAGE_FOLDER, message);
                    printf("%s>Chargement de l'image : %s%s\n\n\n", D_ROUGE, chemin_image, F_ROUGE);
                    imageSurface = IMG_Load(chemin_image);
                    if (!imageSurface) {
                        printf("Erreur chargement image: %s\n", IMG_GetError());
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        IMG_Quit();
                        SDL_Quit();
                        pthread_exit(NULL);
                    }

                    // Créer une texture à partir de la surface
                    texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
                    SDL_FreeSurface(imageSurface);  // On peut libérer la surface après avoir créé la texture

                    if (!texture) {
                        printf("Erreur création texture: %s\n", SDL_GetError());
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        IMG_Quit();
                        SDL_Quit();
                        pthread_exit(NULL);
                    }

                    // trueText = chemin_image;
                    // validText = 0; // Boolean (1 = true, 0 = false)*/

            // MESSAGE
            }else if(strcmp(code, MESSAGE) == 0){
                printf("%s>Message du serveur reçu : \n\t- Code : [%s]\n\t- Message : %s%s\n\n",D_JAUNE, code, message, F_JAUNE);
                if(strcmp(message, "OK") == 0){
                    printf("%s>Chargement de l'image de base : ./img_acceuil/Fond_Gris.jpg%s\n\n\n", D_ROUGE, F_ROUGE);
                    imageSurface = IMG_Load("./img_acceuil/Fond_Gris.jpg");
                    if (!imageSurface) {
                        printf("Erreur chargement image: %s\n", IMG_GetError());
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        IMG_Quit();
                        SDL_Quit();
                        pthread_exit(NULL);
                    }

                    // Créer une texture à partir de la surface
                    texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
                    SDL_FreeSurface(imageSurface);  // On peut libérer la surface après avoir créé la texture

                    if (!texture) {
                        printf("Erreur création texture: %s\n", SDL_GetError());
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        IMG_Quit();
                        SDL_Quit();
                        pthread_exit(NULL);
                    }
                }
            // AUTRE
            }else{
                printf(">Message du serveur reçu : \n\t- Message : %s\n\n",buffer);
            }
        }
    }

    return NULL;
}

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

int checkWord(const Dictionnaire* dict, const char* mot) {
    if (dict == NULL || dict->pathImg == NULL || mot == NULL) {
        return 0; // Erreur ou mot non trouvé
    }
    if (strstr(dict->pathImg, mot) != NULL) {
        return 1; // Mot trouvé
    }
    return 0; // Mot non trouvé
}

int init_sdl(SDL_Window **window, SDL_Renderer **renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur SDL: %s\n", SDL_GetError());
        return 0;
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {
        printf("Erreur SDL_image: %s\n", IMG_GetError());
        SDL_Quit();
        return 0;
    }

    if (TTF_Init() != 0) {
        printf("Erreur SDL_ttf: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return 0;
    }

    return 1;
}



int main(int argc, char *argv[]) {

    // Window Game    
    window = SDL_CreateWindow("DescribeImage", 
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                          640, 480, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur création fenêtre: %s\n", SDL_GetError());
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Créer un renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) {
        printf("Erreur création renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }


    Dictionnaire* imgTable;
    int countImgFiles;
    char* randomImg;
    char pathImg[NB_MAX_LETTERS];
    countImgFiles = count_files(IMAGE_FOLDER);
    imgTable = getImgTable(countImgFiles);
    srand(time(NULL));    
    char fontName[NB_MAX_LETTERS];
    strcpy(fontName, FONT_FOLDER);
    strcat(fontName, "arial.ttf");
    printf("FontName: %s\n", fontName);

    // Texte entré par l'utilisateur
    char inputText[NB_MAX_LETTERS] = "";
    int inputActive = 1;

    int running = 1;
    SDL_Event event;
    
    // Rectangle pour réduire la taille de l'image
    SDL_Rect imageRect = {220, 50, 200, 150}; // Position (x, y) et taille (w, h)

    randomImg = imgTable[generate_random_int(0,countImgFiles-1)].pathImg;
    strcpy(pathImg, IMAGE_FOLDER);
    strcat(pathImg,randomImg);
    if (!init_sdl(&window, &renderer)) {
        return 1;
    }

    
    

    // Charger la police pour afficher du texte
    font = TTF_OpenFont(fontName, 24);
    if (!font) {
        printf("Erreur chargement police: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    
    
    // Charger une image PNG
    imageSurface = IMG_Load("./img_acceuil/Fond_Gris.jpg");
    if (!imageSurface) {
        printf("Erreur chargement image: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    

    // Créer une texture à partir de la surface
    texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);  // On peut libérer la surface après avoir créé la texture

    if (!texture) {
        printf("Erreur création texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    trueText = pathImg;
    validText = 0; // Boolean (1 = true, 0 = false)

    

    // Avant la boucle infinie, on créer les threads d'écoute et on prépare l'envoi de messages au serv
    int client_socket;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADRESS);
    server_addr.sin_port = htons(SERVER_PORT);
    
    // Envoyer un premier message pour s'identifier au serveur
    strcpy(message, "Bonjour, serveur !");
    sendto(client_socket, message, strlen(message), 0, 
           (struct sockaddr *)&server_addr, sizeof(server_addr));

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, (void *)&client_socket);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (event.type == SDL_TEXTINPUT && inputActive) {
                strncat(inputText, event.text.text, sizeof(inputText) - strlen(inputText) - 1);
            }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_BACKSPACE && inputActive) {
                size_t len = strlen(inputText);
                if (len > 0) {
                    inputText[len - 1] = '\0';
                }
            } else if (event.key.keysym.sym == SDLK_RETURN && inputActive) {
                if (strlen(inputText) > 0) { // Vérifier si le champ de texte n'est pas vide
                    printf("Texte valide(Utilisateur): %s\n", inputText);
                    printf("Texte valide(Server): %s\n", trueText);
                    // Envoi du résultat au serv
                    sprintf(message, "%s%s", MANCHE, inputText);
                    sendto(client_socket, message, strlen(message), 0, 
                        (struct sockaddr *)&server_addr, sizeof(server_addr));
                    validText = strcmp(inputText, trueText) == 0 ? 1 : 0; // Comparer avec trueText
                    inputText[0] = '\0'; // Réinitialiser le texte saisi
                    printf("Texte valide(Utilisateur = Server): %d\n", validText); // Debug: afficher le résultat
                }
            }
        }
        }

        



        // Effacer l'écran
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Dessiner l'image
        SDL_RenderCopy(renderer, texture, NULL, &imageRect);

        // Dessiner le champ de texte
        SDL_Rect inputRect = {100, 300, 440, 50};
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderFillRect(renderer, &inputRect);

        

        // Afficher le texte
        
        SDL_Surface *textSurface = TTF_RenderText_Blended(font, inputText, textColor);
        if (textSurface) {
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {inputRect.x + 5, inputRect.y + (inputRect.h - textSurface->h) / 2, textSurface->w, textSurface->h};
            
            SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
            SDL_FreeSurface(textSurface);     
            SDL_DestroyTexture(textTexture);
        }

        chronoSurface = TTF_RenderText_Solid(font, chronoText, textColor);
        if (chronoSurface) {
            SDL_Texture *chronoTexture = SDL_CreateTextureFromSurface(renderer, chronoSurface);
            SDL_Rect chronoRect = {chronoProperties.x + 5, chronoProperties.y + (chronoProperties.h - chronoSurface->h) / 2, chronoSurface->w, chronoSurface->h};
        
            SDL_RenderCopy(renderer, chronoTexture, NULL, &chronoRect);
            
            SDL_FreeSurface(chronoSurface);     
            SDL_DestroyTexture(chronoTexture); 
        }
        
        
        



        

        // Afficher le rendu
        SDL_RenderPresent(renderer);
        
    }


    close(client_socket);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}