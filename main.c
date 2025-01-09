#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./client.h"


int main(int argc, char **argv) {
    char input[100]; // Buffer pour stocker l'entrée utilisateur

    printf("Entrez du texte (tapez 'Stop' pour quitter) :\n");

    while (1) { // Boucle infinie
        printf("> "); // Invite de commande
        fgets(input, sizeof(input), stdin); // Lire l'entrée utilisateur

        // Supprimer le caractère de nouvelle ligne '\n' si présent
        input[strcspn(input, "\n")] = 0;

        // Vérifier si l'utilisateur a entré "Stop"
        if (strcmp(input, "Stop") == 0) {
            printf("Arrêt du programme.\n");
            break; // Quitter la boucle
        }

        // Afficher l'entrée utilisateur
        printf("Vous avez entré : %s\n", input);
        SendMessage(input);
    }

    return 0;
}