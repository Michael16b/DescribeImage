/*----------------------------------------------
Serveur UDP
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

#define PORT 5000
#define BUFFER_SIZE 256

int main(int argc, char **argv) {
    int socket_descriptor;
    sockaddr_in adresse_locale, adresse_client;
    char buffer[BUFFER_SIZE];

    /* Création de la socket UDP */
    if ((socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur : impossible de créer la socket UDP.");
        exit(1);
    }

    /* Configuration de l'adresse locale */
    memset(&adresse_locale, 0, sizeof(adresse_locale));
    adresse_locale.sin_family = AF_INET;
    adresse_locale.sin_addr.s_addr = INADDR_ANY;
    adresse_locale.sin_port = htons(PORT);

    /* Liaison de la socket */
    if (bind(socket_descriptor, (sockaddr*)&adresse_locale, sizeof(adresse_locale)) < 0) {
        perror("Erreur : impossible de lier la socket.");
        exit(1);
    }

    printf("Serveur UDP en attente sur le port %d...\n", PORT);

    /* Attente des messages */
    for (;;) {
        socklen_t client_len = sizeof(adresse_client);
        memset(buffer, 0, BUFFER_SIZE);

        /* Réception d'un message */
        int n = recvfrom(socket_descriptor, buffer, BUFFER_SIZE-1, 0, (sockaddr*)&adresse_client, &client_len);
        if (n < 0) {
            perror("Erreur : réception du message échouée.");
            continue;
        }

        buffer[n] = '\0';
        printf("Message reçu : %s\n", buffer);

        /* Traitement du message */
        buffer[0] = 'R';
        buffer[1] = 'E';
        buffer[n] = '#';
        buffer[n+1] = '\0';

        printf("Message après traitement : %s\n", buffer);

        /* Envoi de la réponse */
        if (sendto(socket_descriptor, buffer, strlen(buffer), 0, (sockaddr*)&adresse_client, client_len) < 0) {
            perror("Erreur : impossible d'envoyer la réponse.");
        } else {
            printf("Réponse envoyée au client.\n");
        }
    }

    close(socket_descriptor);
    return 0;
}
