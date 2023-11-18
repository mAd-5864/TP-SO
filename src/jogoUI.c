#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curses.h>

#define MAX_USERNAME_SIZE 20
#define MAX_COMMAND_SIZE 100

// Function prototypes
void validateCommands(char* command, WINDOW* janela_mensagens, WINDOW* janela_comandos);
void playersCommand(WINDOW* janela_mensagens);
void msgCommand(char *username, char* msg, WINDOW* janela_mensagens);
void exitCommand();
void desenhaMoldura(int alt, int comp, int linha, int coluna);

int main(int argc, char* argv[], char* envp[]) {
    if (argc != 2) {
        printf("[ERRO]: Introduza o nome do jogador como argumento.\n");
        exit(1);
    }

    char playerName[MAX_USERNAME_SIZE];
    strncpy(playerName, argv[1], MAX_USERNAME_SIZE);
    printf("Nome do jogador: %s\n\n", playerName);

    int sair;
    WINDOW * janela_cursor, * janela_mensagens, * janela_comandos;

    initscr(); // Inicializa o modo curses
    start_color(); // Inicializa o suporte a cores
    erase(); // Limpa a tela
    noecho(); // Desabilita a exibição das teclas digitadas
    cbreak(); // Desabilita o buffer de linha (entrada de caractere imediata)

    // Área do cursor
    desenhaMoldura(16, 40, 1, 1);
    janela_cursor = newwin(16, 40, 1, 1);
    wrefresh(janela_cursor);

    // Área das mensagens
    desenhaMoldura(16, 40, 1, 45);
    janela_mensagens = newwin(16, 40, 1, 45);
    scrollok(janela_mensagens, TRUE);
    wprintw(janela_mensagens, "MENSAGENS:\n");
    wrefresh(janela_mensagens);

    // Área dos comandos
    desenhaMoldura(5, 100, 19, 1);
    janela_comandos = newwin(5, 100, 19, 1);
    box(janela_comandos, 0, 0);
    wrefresh(janela_comandos);

    char command[MAX_COMMAND_SIZE];
    int ch;
    int cursor_x = 1;
    int cursor_y = 1;

    do {
        ch = mvwgetch(janela_comandos, cursor_y, cursor_x);

        // Process the input character
        switch(ch) {
            case '\n': // Enter key
                command[cursor_x - 1] = '\0'; // Null-terminate the string
                validateCommands(command, janela_mensagens, janela_comandos);
                cursor_x = 1;
                break;
            case 127: // Backspace key
                if (cursor_x > 1) {
                    mvwaddch(janela_comandos, cursor_y, --cursor_x, ' ');
                }
                break;
            default:
                if (cursor_x < 98) {
                    command[cursor_x - 1] = ch;
                    mvwaddch(janela_comandos, cursor_y, cursor_x++, ch);
                }
                break;
        }

        wrefresh(janela_comandos);

    } while (strcmp(command, "exit") != 0);

    endwin(); // Encerra o modo curses
    return 0;
}

void validateCommands(char* command, WINDOW* janela_mensagens, WINDOW* janela_comandos) {
    char* commandAux = strtok(command, "\n");

    if (!strcmp(commandAux, "players")) {
        playersCommand(janela_mensagens);
    } else if (!strcmp(commandAux, "msg")) {
        char username[MAX_USERNAME_SIZE];
        char msg[MAX_COMMAND_SIZE - MAX_USERNAME_SIZE - 2];
        username[0] = '\0';
        msg[0] = '\0';
        strcat(username, strtok(NULL, " "));
        strcat(msg, strtok(NULL, "\n"));
        if (username[0] == '\0' || msg[0] == '\0') {
            wprintw(janela_mensagens, "[ERRO] Syntax: msg <username> <message>\n");
            wrefresh(janela_mensagens);
        } else {
            msgCommand(username, msg, janela_mensagens);
        }
    } else if (!strcmp(commandAux, "exit")) {
        exitCommand();
    } else {
        wprintw(janela_mensagens, "[ERRO]: Comando invalido.\n");
        wrefresh(janela_mensagens);
    }
}

void playersCommand(WINDOW* janela_mensagens) {
    wprintw(janela_mensagens, "Comando [players] nao implementado.\n");
    wrefresh(janela_mensagens);
}

void msgCommand(char *username, char* msg, WINDOW* janela_mensagens) {
    // Display the message in the messages window
    wprintw(janela_mensagens, "%s: %s\n", username, msg);
    wrefresh(janela_mensagens);
}

void exitCommand()
{
    printf("\nComando [exit] nao implementado.\n");
}

void desenhaMoldura(int alt, int comp, int linha, int coluna) {
    --linha;
    --coluna;
    alt += 2;
    comp += 2;
    for (int l = linha; l <= linha + alt - 1; ++l) {
        mvaddch(l, coluna, '|');
        mvaddch(l, coluna + comp - 1, '|');
    }
    for (int c = coluna; c <= coluna + comp - 1; ++c) {
        mvaddch(linha, c, '-');
        mvaddch(linha + alt - 1, c, '-');
    }
    mvaddch(linha, coluna, '+');
    mvaddch(linha, coluna + comp - 1, '+');
    mvaddch(linha + alt - 1, coluna, '+');
    mvaddch(linha + alt - 1, coluna + comp - 1, '+');
    refresh();
}
