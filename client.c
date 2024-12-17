/*-----------------------------------------------------------
Client UDP
Commande : client_udp <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

int main(int argc, char **argv) {
    int socket_descriptor;
    sockaddr_in adresse_serveur;
    char buffer[256];
    char *host, *mesg;

    if (argc != 3) {
        perror("usage : client_udp <adresse-serveur> <message-a-transmettre>");
        exit(1);
    }

    host = argv[1];
    mesg = argv[2];

    printf("Adresse du serveur : %s\n", host);
    printf("Message envoyé     : %s\n", mesg);

    /* Création de la socket UDP */
    if ((socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur : impossible de créer la socket UDP.");
        exit(1);
    }

    /* Configuration de l'adresse du serveur */
    memset(&adresse_serveur, 0, sizeof(adresse_serveur));
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port = htons(5000);
    if (inet_pton(AF_INET, host, &adresse_serveur.sin_addr) <= 0) {
        perror("Erreur : adresse serveur invalide.");
        exit(1);
    }

    /* Envoi du message au serveur */
    if (sendto(socket_descriptor, mesg, strlen(mesg), 0, (sockaddr*)&adresse_serveur, sizeof(adresse_serveur)) < 0) {
        perror("Erreur : impossible d'envoyer le message.");
        exit(1);
    }

    printf("Message envoyé au serveur.\n");

    /* Réception de la réponse du serveur */
    socklen_t len = sizeof(adresse_serveur);
    int n = recvfrom(socket_descriptor, buffer, sizeof(buffer)-1, 0, (sockaddr*)&adresse_serveur, &len);
    if (n < 0) {
        perror("Erreur : réception échouée.");
        exit(1);
    }

    buffer[n] = '\0';
    printf("Réponse du serveur : %s\n", buffer);

    close(socket_descriptor);
    return 0;
}
