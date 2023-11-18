#ifndef JOGOUI_H
#define JOGOUI_H

#include "estruturas.h"


// Declarar funções
void validateCommands(char* command, WINDOW* janela_mensagens, WINDOW* janela_comandos);
void playersCommand(WINDOW* janela_mensagens);
void msgCommand(char *username, char* msg, WINDOW* janela_mensagens);
void exitCommand();
void desenhaMoldura(int alt, int comp, int linha, int coluna);
void getUserCredentials(Player* credentials);

#endif

