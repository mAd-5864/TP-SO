#ifndef JOGOUI_H
#define JOGOUI_H

//Constantes
#define MAX_USERNAME_SIZE 30
#define MAX_COMMAND_SIZE 50

// Defenir estruturas
typedef struct {
    char username[MAX_USERNAME_SIZE];
    // Adicionar mais se necessário
} UserCredentials;


// Declarar funções
void validateCommands(char* command);
void playersCommand();
void msgCommand(char *username, char* msg);
void exitCommand();

void getUserCredentials(UserCredentials* credentials);

#endif

