#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "types.h"

void *receive_messages(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, NULL, NULL);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            printf(">%s\n", buffer);
        }
    }

    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(SERVER_PORT);

    // Envoyer un premier message pour s'identifier au serveur
    strcpy(message, "Bonjour, serveur !");
    sendto(client_socket, message, strlen(message), 0, 
           (struct sockaddr *)&server_addr, sizeof(server_addr));

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, (void *)&client_socket);

    while (1) {
        printf("<");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0'; // Retirer le \n

        sendto(client_socket, message, strlen(message), 0, 
               (struct sockaddr *)&server_addr, sizeof(server_addr));
    }

    close(client_socket);
    return 0;
}
