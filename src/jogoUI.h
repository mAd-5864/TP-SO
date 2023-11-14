#ifndef JOGOUI_H
#define JOGOUI_H

#include "estruturas.h"


// Declarar funções
void validateCommands(char* command);
void playersCommand();
void msgCommand(char *username, char* msg);
void exitCommand();

void getUserCredentials(Player* credentials);

#endif

