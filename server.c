#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <dirent.h>
#include "./types.h"

int server_socket;

// Variables pour les messages
Client clients[MAX_CLIENTS];
int TabScores[MAX_CLIENTS];
int MotTrouveXFois = 0;
int client_count = 0;
pthread_mutex_t client_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t points_lock = PTHREAD_MUTEX_INITIALIZER;

// Variables sur la partie en cours
bool Jeu_En_Cours = false;
bool Manche_En_Cours = false;
int Numero_Manche = 0;
int Mot_Actuel;
int Points_Joueurs[MAX_CLIENTS];
pthread_mutex_t infos_manche_lock = PTHREAD_MUTEX_INITIALIZER;
Dictionnaire* imgTable;


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
    imgTable[cpt] = count_word(imgTable[cpt], actualWord);

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

void *send_messages(void *arg) {
    char message[BUFFER_SIZE];


    while (1) {
        printf("Entrez un message à envoyer aux clients : ");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0'; // Retirer le \n

        // Debut de zone d'exclusion mutuelle
        pthread_mutex_lock(&client_lock);
        for (int i = 0; i < client_count; i++) {
            sendto(server_socket, message, strlen(message), 0,
                   (struct sockaddr *)&clients[i].addr, clients[i].addr_len);
        }
        pthread_mutex_unlock(&client_lock);
        // Fin de zone d'exclusion mutuelle
    }

    return NULL;
}

void envoi_message(char message[]){
    //sprintf(message, "Chrono : %d", secondes_restantes);
    message[strcspn(message, "\n")] = '\0';

    // Boucle sur le nombre de clients
    for(int i = 0; i < client_count; i++){
        sendto(server_socket, message, strlen(message), 0,
            (struct sockaddr *)&clients[i].addr, clients[i].addr_len);
    }
}

void* chrono(void* arg) {
    int secondes_restantes = *(int*)arg; // Cast du pointeur pour récupérer la durée
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char reponse[MAX_CAR];

    while (1){
        if(Manche_En_Cours == true){
            secondes_restantes = NB_SECONDES;
            printf("Chronomètre démarré pour %d secondes...\n", secondes_restantes);
            
            while (secondes_restantes > 0) {
                // Ecriture du message comportant les informations du timer au client
                sprintf(message, "%s%d",CHRONO, secondes_restantes);
                message[strcspn(message, "\n")] = '\0';
                envoi_message(message);

                // Purement visuel
                printf("Temps restant : %d secondes\n", secondes_restantes);

                // Décrémentation
                sleep(1); 
                secondes_restantes--;
            }

            pthread_mutex_lock(&infos_manche_lock);
            Manche_En_Cours = false;
            pthread_mutex_unlock(&infos_manche_lock);

            // Envoi des scores aux clients
            pthread_mutex_lock(&points_lock);
            for(int i = 0; i<client_count; i++){
                // Préparation de la variable à envoyer au client actuel
                sprintf(reponse, "%s%d\n", POINTS, TabScores[i]);
                printf("%sTabScore[%d] : %d%s\n", D_VERT,i,TabScores[i], F_VERT);

                sendto(server_socket, reponse, strlen(reponse), 0,
                    (struct sockaddr *)&clients[i].addr, clients[i].addr_len);
            }
            // Réinitialisation du nombre de fois que le mot à été trouvé (car fin de manche = nv mot)
            MotTrouveXFois = 0;
            pthread_mutex_unlock(&points_lock);

            printf("Chronomètre terminé !\n");

        }
        
    }

    return NULL; // Le thread retourne NULL
}

char* test_mot(char* msgClient){
    for (int i = 0; i < imgTable[Mot_Actuel].cptWord; i++){
        printf("%s\tIntra test_mot : \n\t\tMot réél -%s-\n\t\tMot de l'utilisateur -%s-\n\n%s",D_JAUNE,imgTable[Mot_Actuel].words[i],msgClient, F_JAUNE);
        if(strcasecmp(imgTable[Mot_Actuel].words[i], msgClient) == 0){
            return "OK";
        }else{
            return "KO";
        }
    }
    return "KO";
}

int getScore(){
    return 100 - (MotTrouveXFois * 10);
}

void *handle_client_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char message[BUFFER_SIZE];

    

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, 
                                (struct sockaddr *)&client_addr, &addr_len);

        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            printf("Message reçu de %s:%d -> %s\n", 
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);

            // Ajouter le client à la liste si nouveau
            // Début de zone d'exclusion mutuelle
            pthread_mutex_lock(&client_lock);
            int found = 0;
            for (int i = 0; i < client_count; i++) {
                if (clients[i].addr.sin_addr.s_addr == client_addr.sin_addr.s_addr &&
                    clients[i].addr.sin_port == client_addr.sin_port) {
                    found = 1;

                    // Cas ou le client qui décide de lancer la partie
                    if(strcmp(buffer+4, "Start")==0 && !Manche_En_Cours){
                        printf("Début de la partie. \n");
                        
                        // Choix du mot de la manche
                        int countImgFiles;
                        char* randomImg;
                        char pathImg[NB_MAX_LETTERS];
                        // Nombres d'images dans le dossier img
                        countImgFiles = count_files(IMAGE_FOLDER);
                        printf("%sNOMBRE D'IMAGES : %d%s\n",D_ROUGE, countImgFiles, F_ROUGE);
                        // Récupération (et construction) du dictionnaire des images
                        imgTable = getImgTable(countImgFiles);
                        // Choix d'un nombre aléatoire (entre 0, soit la première image du folder et countImgFiles soit la dernière image possible.)
                        countImgFiles = generate_random_int(0, countImgFiles);
                        printf("%sIMAGE [%d] : %s\nImage (mot) : %s%s\n\n", D_ROUGE,countImgFiles, imgTable[countImgFiles].pathImg , imgTable[countImgFiles].words[0],F_ROUGE);
                        Mot_Actuel = countImgFiles;

                        // Envoi du mot choisi aux clients, affichent l'image correspondante
                        snprintf(buffer, sizeof(buffer), "%s%s", NEW_IMG, imgTable[countImgFiles].pathImg);
                        envoi_message(buffer);
                        
                        // On déclanche le thread qui s'occupe de la fonction "Chrono"
                        pthread_mutex_lock(&infos_manche_lock);
                            Jeu_En_Cours = true;
                            Manche_En_Cours = true;
                            Numero_Manche += 1;
                        pthread_mutex_unlock(&infos_manche_lock);
                    }
                    // Cas ou le client envoi un message pendant une manche
                    else if(Manche_En_Cours){
                        char code[4];
                        char message[MAX_CAR];
                        char reponse[MAX_CAR];
                        
                        sprintf(code, "%.4s", buffer);
                        sprintf(message, "%s", buffer+4);

                        if (strcmp(code, MANCHE) == 0) {
                            // Affichage sur le terminal
                            printf("%s>Message du client reçu : \n\t- Code : [%s]\n\t- Message : %s%s\n\n",D_BLEU, code, message, F_BLEU);
                            printf("%sReponse test_mot: %s\n\t->%s\n%s\n",D_ROUGE, test_mot(message),message, F_ROUGE);
                            

                            // Préparation de la variable à envoyer au client actuel
                            sprintf(reponse, "%s%s", MESSAGE, test_mot(message));
                            reponse[strcspn(reponse, "\n")] = '\0';

                            sendto(server_socket, reponse, strlen(reponse), 0,
                                (struct sockaddr *)&clients[i].addr, clients[i].addr_len);

                            // MAJ Score du client et nombre de fois que le mot à été trouvé
                            pthread_mutex_lock(&points_lock);
                            printf("\t\tAVANT AUGMENTATION DES POINTS DU CLIENT %d\n\n", i);
                            if(strcmp(test_mot(message), "OK") == 0){
                                TabScores[i] += getScore();
                                printf("INTRA AUGMENTATION : GetScore = %d\n", getScore());
                                if(MotTrouveXFois<5){
                                    printf("MotTrouveXFois : %d -> %d\n",MotTrouveXFois, MotTrouveXFois+1);
                                    MotTrouveXFois ++;
                                }
                            }
                            pthread_mutex_unlock(&points_lock);
                            printf("\t\t APRES AUGMENTATION DES POINTS DU CLIENT %d\n\n", i);
                        }

                        
                    }
                }
            }
            if (!found && client_count < MAX_CLIENTS) {
                clients[client_count].addr = client_addr;
                clients[client_count].addr_len = addr_len;
                client_count++;
                printf("Nouveau client ajouté : %s:%d\n", 
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }
            pthread_mutex_unlock(&client_lock);
            // Fin de zone d'exclusion mutuelle
        }
    }

    return NULL;
}

int main() {
    struct sockaddr_in server_addr;

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur lors de la création du socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur lors du bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    pthread_t recv_thread, send_thread;
    pthread_t chrono_thread;
    int duree = NB_SECONDES; 
    memset(TabScores, 0, sizeof(TabScores));


    if(pthread_create(&recv_thread, NULL, handle_client_messages, (void *)&server_socket) != 0){
        perror("Erreur lors de la création du thread recv_thread");
        return 1;
    }
    if(pthread_create(&send_thread, NULL, send_messages, (void *)&server_socket) != 0){
        perror("Erreur lors de la création du thread send_thread");
        return 1;
    }
    if (pthread_create(&chrono_thread, NULL, chrono, (void*)&duree) != 0) {
        perror("Erreur lors de la création du thread chrono_thread");
        exit(EXIT_FAILURE);
    }
                            
    // TODO thread chrono, qui envoi toute les n périodes un signal aux clients

    if(pthread_join(recv_thread, NULL) != 0){
        perror("Erreur lors de l'attente du thread recv_thread");
        return 1;
    }
    if(pthread_join(send_thread, NULL) != 0){
        perror("Erreur lors de l'attente du thread send_thread");
        return 1;
    }
    if (pthread_join(chrono_thread, NULL) != 0) {
        perror("Erreur lors de l'attente du thread chrono_thread");
        exit(EXIT_FAILURE);
    }
    

    close(server_socket);
    pthread_mutex_destroy(&client_lock);

    return 0;
}
