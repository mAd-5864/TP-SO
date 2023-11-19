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

    int sair;
    WINDOW *janela_jogo, *janela_comandos;

    initscr();     // Inicializa o modo curses
    start_color(); // Inicializa o suporte a cores
    erase();       // Limpa a tela
    noecho();      // Desabilita a exibição das teclas digitadas
    cbreak();      // Desabilita o buffer de linha (entrada de caractere imediata)

    // Área do jogo
    janela_jogo = newwin(16, 40, 1, 5);
    box(janela_jogo, 0, 0);
    wrefresh(janela_jogo);

    // Área dos comandos
    janela_comandos = newwin(16, 60, 1, 50);
    box(janela_comandos, 0, 0);
    wrefresh(janela_comandos);

    char command[MAX_COMMAND_SIZE];
    int ch;
    int cursor_x = 1;
    int cursor_y = 1;

    WINDOW *active_window = janela_jogo; // Defenir janela inicial

    do
    {
        ch = mvwgetch(active_window, cursor_y, cursor_x);

        if (active_window == janela_jogo)
        {
            mvwaddch(janela_jogo, cursor_y, cursor_x, ' ');
            switch (ch)
            {
            case 37: // Left
                if (cursor_x > 1)
                {
                    --cursor_x;
                }
                break;
            case 39: // Right
                if (cursor_x < 38)
                {
                    ++cursor_x;
                }
                break;
            case 38: // Up
                if (cursor_y > 1)
                {
                    --cursor_y;
                }
                break;
            case 40: // Down
                if (cursor_y < 14)
                {
                    ++cursor_y;
                }
                break;
            case ' ': // Mudar para a janela_comandos
                active_window = janela_comandos;
                break;
            default:
                break;
            }

            // Atualizar posicao do jogador depois do moviemnto
            // mvwaddch(janela_jogo, cursor_y, cursor_x, playerName[0]);
            wrefresh(janela_jogo);
        }
        else
        {
            switch (ch)
            {
            case '\n': // Enter key
                command[cursor_x - 1] = '\0';
                cursor_x = 1;
                mvwaddch(janela_comandos, ++cursor_y, cursor_x, ' ');
                if (cursor_y > 14)
                {
                    cursor_y = 1;
                    wclear(janela_comandos);
                    wrefresh(janela_comandos);
                    box(janela_comandos, 0, 0);
                    mvwaddch(janela_comandos, cursor_y++, cursor_x, ' ');
                }
                else
                {
                    cursor_y++;
                }
                validateCommands(command, playerName, janela_comandos);

                break;
            case 127: // Backspace key
                if (cursor_x > 1)
                {
                    mvwaddch(janela_comandos, cursor_y, --cursor_x, ' ');
                }
                break;
            case 27: // Esc key
                active_window = janela_jogo; // Mudar de Janela
                break;
            default:
                if (cursor_x < 98)
                {
                    command[cursor_x - 1] = ch;
                    mvwaddch(janela_comandos, cursor_y, cursor_x++, ch);
                }
                break;
            }
        }

        wrefresh(active_window);

    } while (strcmp(command, "exit"));

    endwin(); // Encerra o modo curses
    return 0;
}

void validateCommands(char *command, char *playerName, WINDOW *janela_comandos)
{
    char *commandAux = strtok(command, " \n");

    if (!strcmp(commandAux, "players"))
    {
        playersCommand(janela_comandos);
    }
    else if (!strcmp(commandAux, "msg"))
    {
        char username[MAX_USERNAME_SIZE];
        char msg[MAX_COMMAND_SIZE - MAX_USERNAME_SIZE - 2];
        username[0] = '\0';
        msg[0] = '\0';
        char *userToken = strtok(NULL, " ");
        char *msgToken = strtok(NULL, "\n");
        if (userToken == NULL || msgToken == NULL)
        {
            wprintw(janela_comandos, "[ERRO] Syntax: msg <username> <message>\n");
            wrefresh(janela_comandos);
        }
        else
        {
            strcat(username, userToken);
            strcat(msg, msgToken);
            msgCommand(playerName, username, msg, janela_comandos);
        }
    }
    else if (!strcmp(commandAux, "exit"))
    {
        exitCommand();
    }
    else
    {
        wprintw(janela_comandos, "[ERRO]: Comando invalido.");
        wrefresh(janela_comandos);
    }
}

void playersCommand(WINDOW *janela_comandos)
{
    wrefresh(janela_comandos);
    wprintw(janela_comandos, "Comando [players] nao implementado.");
}

void msgCommand(char *playerName, char *username, char *msg, WINDOW *janela_comandos)
{
    wprintw(janela_comandos, "%s -> %s: %s", playerName, username, msg);
    wrefresh(janela_comandos);
}

void exitCommand()
{
    printf("Comando [exit] nao implementado.");
}

void desenhaMoldura(int alt, int comp, int linha, int coluna)
{
    --linha;
    --coluna;
    alt += 2;
    comp += 2;
    for (int l = linha; l <= linha + alt - 1; ++l)
    {
        mvaddch(l, coluna, '|');
        mvaddch(l, coluna + comp - 1, '|');
    }
    for (int c = coluna; c <= coluna + comp - 1; ++c)
    {
        mvaddch(linha, c, '-');
        mvaddch(linha + alt - 1, c, '-');
    }
    mvaddch(linha, coluna, '+');
    mvaddch(linha, coluna + comp - 1, '+');
    mvaddch(linha + alt - 1, coluna, '+');
    mvaddch(linha + alt - 1, coluna + comp - 1, '+');
    refresh();
}
