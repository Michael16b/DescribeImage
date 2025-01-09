/*----------------------------------------------
Serveur RUDP multiclient
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "./types.h"
#include <signal.h>
#include <sys/time.h>
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define BUFFER_SIZE 256

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

void timer_handler(int signum) {
    static int count = 0;
    printf("Timer déclenché %d fois\n", ++count);
}

/* Structure pour passer les paramètres au thread */
typedef struct {
    int socket_descriptor;
    sockaddr_in client_address;
    socklen_t client_length;
    char message[BUFFER_SIZE];
} client_info_t;

/* Fonction pour gérer un client */
void* gerer_client(void* arg) {
    client_info_t* client_info = (client_info_t*)arg;
    int socket_descriptor = client_info->socket_descriptor;
    sockaddr_in client_address = client_info->client_address;
    socklen_t client_length = client_info->client_length;
    char* message = client_info->message;
    //char response[BUFFER_SIZE];
    char *ip_address = inet_ntoa(client_address.sin_addr);
    int port = ntohs(client_address.sin_port);

    printf("Nouveau client connecté.\n");
    printf("Message reçu : %s\n", message);
    printf("Adresse IP du client : %s\n", ip_address);
    printf("Port du client : %d\n", port);

    /* Envoi de l'ACK au client */
    if (sendto(socket_descriptor, "ACK", strlen("ACK"), 0, (sockaddr*)&client_address, client_length) < 0) {
        perror("Erreur : envoi de l'ACK échoué.");
        pthread_exit(NULL);
    } else {
        printf("ACK envoyé au client.\n");
    }

    /* Traitement du message */
    message[0] = 'R';
    message[1] = 'E';
    int length = strlen(message);
    message[length] = '#';
    message[length + 1] = '\0';

    printf("Message après traitement : %s\n", message);

    /* Envoi de la réponse au client */
    if (sendto(socket_descriptor, message, strlen(message), 0, (sockaddr*)&client_address, client_length) < 0) {
        perror("Erreur : envoi de la réponse échoué.");
    } else {
        printf("Réponse envoyée au client.\n");
    }

    printf("Client traité. Fermeture du thread.\n");
    free(client_info);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    int socket_descriptor;
    sockaddr_in adresse_locale, adresse_client;
    char buffer[BUFFER_SIZE];
    struct sigaction sa;
    struct itimerval timer;

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

    printf("Serveur RUDP multiclient en attente sur le port %d...\n", PORT);

    // Configurer le gestionnaire de signal
    sa.sa_handler = &timer_handler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa, NULL);

    // Configurer le timer (délai initial et intervalle)
    timer.it_value.tv_sec = 1; // 1 seconde avant le premier déclenchement
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1; // Répétition toutes les 1 seconde
    timer.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL, &timer, NULL);

    /* Boucle infinie pour attendre les clients */
    while (1) {
        socklen_t client_len = sizeof(adresse_client);
        memset(buffer, 0, BUFFER_SIZE);

        /* Réception d'un message */
        int n = recvfrom(socket_descriptor, buffer, BUFFER_SIZE - 1, 0, (sockaddr*)&adresse_client, &client_len);
        if (n < 0) {
            perror("Erreur : réception du message échouée.");
            continue;
        }

        buffer[n] = '\0';
        printf("Message reçu d'un client : %s\n", buffer);

        /* Allocation de mémoire pour stocker les informations du client */
        client_info_t* client_info = (client_info_t*)malloc(sizeof(client_info_t));
        if (!client_info) {
            perror("Erreur : échec de l'allocation mémoire.");
            continue;
        }

        client_info->socket_descriptor = socket_descriptor;
        client_info->client_address = adresse_client;
        client_info->client_length = client_len;
        strncpy(client_info->message, buffer, BUFFER_SIZE);

        /* Création d'un thread pour gérer ce client */
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, gerer_client, (void*)client_info) != 0) {
            perror("Erreur : impossible de créer un thread pour le client.");
            free(client_info);
        } else {
            pthread_detach(thread_id); // Libère automatiquement les ressources à la fin du thread
        }
    }

    close(socket_descriptor);
    return 0;
}
