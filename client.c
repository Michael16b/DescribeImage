/*-----------------------------------------------------------
Client RUDP
Commande : client_rudp <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include "./types.h"

#define TIMEOUT 2       // Timeout pour attendre un ACK (en secondes)
#define MAX_RETRIES 5   // Nombre maximum de tentatives d'envoi


typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;



void SendMessage(char msg[NB_MAX_LETTERS]){
    int socket_descriptor;
    sockaddr_in adresse_serveur;
    //char buffer[256];
    char ack[256];
    char *host;
    int retries = 0;

    host = ADRESS ;

    printf("Adresse du serveur : %s\n", host);
    printf("Message envoyé     : %s\n", msg);

    /* Création de la socket UDP */
    if ((socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur : impossible de créer la socket UDP.");
        exit(1);
    }

    /* Configuration de l'adresse du serveur */
    memset(&adresse_serveur, 0, sizeof(adresse_serveur));
    adresse_serveur.sin_family = AF_INET;
    adresse_serveur.sin_port = htons(PORT);
    if (inet_pton(AF_INET, host, &adresse_serveur.sin_addr) <= 0) {
        perror("Erreur : adresse serveur invalide.");
        exit(1);
    }

    /* Mécanisme RUDP - Envoi avec ACK */
    while (retries < MAX_RETRIES) {
        /* Envoi du message au serveur */
        printf("Tentative d'envoi %d...\n", retries + 1);
        if (sendto(socket_descriptor, msg, strlen(msg), 0, (sockaddr*)&adresse_serveur, sizeof(adresse_serveur)) < 0) {
            perror("Erreur : impossible d'envoyer le message.");
            exit(1);
        }

        /* Configuration du timeout pour la socket */
        struct timeval tv;
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;
        setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        /* Attente de l'ACK */
        socklen_t len = sizeof(adresse_serveur);
        int n = recvfrom(socket_descriptor, ack, sizeof(ack) - 1, 0, (sockaddr*)&adresse_serveur, &len);
        if (n > 0) {
            ack[n] = '\0';
            if (strcmp(ack, "ACK") == 0) {
                printf("ACK reçu du serveur. Message transmis avec succès.\n");
                break;
            }
        } else {
            printf("Aucun ACK reçu. Nouvelle tentative...\n");
            retries++;
        }
    }

    if (retries == MAX_RETRIES) {
        printf("Erreur : échec de la transmission après %d tentatives.\n", MAX_RETRIES);
    }

    close(socket_descriptor);
}