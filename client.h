#include "./types.h"

#ifndef CLIENTDEF
#define CLIENTDEF

/*
Fonction : Envoi un message au serveur situé à l'adrese désignée par la variable ADRESS 
de types.h au port PORT de types.h

Argument : 
    - msg : chaine de caractère contenant le message à transmettre au serveur

Retour : Void
*/
void SendMessage(char msg[NB_MAX_LETTERS]);

#endif 