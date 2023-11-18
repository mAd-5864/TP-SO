#include "jogoUI.h"

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 2)
    {
        printf("[ERRO]: Introduza o nome do jogador como argumento.\n");
        exit(1);
    }

    char playerName[MAX_USERNAME_SIZE];
    strncpy(playerName, argv[1], MAX_USERNAME_SIZE);
    printf("Nome do jogador: %s\n\n", playerName);

    char command[MAX_COMMAND_SIZE];
    do
    {
        printf("Introduza um comando: ");
        fgets(command, MAX_COMMAND_SIZE, stdin);
        validateCommands(command, playerName);
    } while (strcmp(command, "exit"));
}

void validateCommands(char *command, char *playerName)
{
    char *commandAux = strtok(command, " \n");
    char username[MAX_USERNAME_SIZE];
    char msg[MAX_COMMAND_SIZE - MAX_USERNAME_SIZE - 2];

    printf("\nComando: [%s]\n", commandAux);
    if (!strcmp(commandAux, "players"))
    {
        playersCommand();
    }
    else if (!strcmp(commandAux, "msg"))
    {
        username[0] = '\0';
        msg[0] = '\0';
        char *userToken = strtok(NULL, " ");
        char *msgToken = strtok(NULL, "\n");

        if (userToken == NULL || msgToken == NULL)
        {
            printf("[ERRO] Syntax: msg <username> <message>\n");
        }
        else
        {
            strcat(username, userToken);
            strcat(msg, msgToken);
            printf("%s -> %s: %s\n", playerName, username, msg);
            msgCommand(username, msg);
        }
    }
    else if (!strcmp(commandAux, "exit"))
    {
        exitCommand();
        strcpy(command, "exit");
    }
    else
    {
        printf("[ERRO]: Comando invalido.\n");
    }
}

void playersCommand()
{ // Listar todos jogadores
    printf("\nComando [players] nao implementado.\n");
}

void msgCommand(char *username, char *msg)
{ // Enviar mensagem privada a outro jogador
    printf("\nComando [msg] nao implementado.\n");
}

void exitCommand()
{
    printf("\nComando [exit] nao implementado.\n");
}
