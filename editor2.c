#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include "types.h"

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


