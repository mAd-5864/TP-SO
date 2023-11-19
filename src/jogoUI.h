#ifndef JOGOUI_H
#define JOGOUI_H

#include "estruturas.h"

// Declarar funções
void validateCommands(char *command, char *playerName, WINDOW *janela_comandos);
void playersCommand(WINDOW *janela_comandos);
void msgCommand(char *playerName, char *username, char *msg, WINDOW *janela_comandos);
void exitCommand();

#endif
