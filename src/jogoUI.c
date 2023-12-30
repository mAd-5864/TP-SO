#include "jogoUI.h"

Game localGameInfo;
Player this;

int main(int argc, char *argv[], char *envp[])
{
    if (argc != 2)
    {
        printf("[ERRO]: Introduza o nome do jogador como argumento.\n");
        exit(1);
    }

    char playerName[MAX_USERNAME_SIZE];
    strncpy(this.username, argv[1], MAX_USERNAME_SIZE);
    createFIFOs(&this);           // Criar FIFOs para dupla comunicação com o motor
    registarMotor(this.username); // Registar o novo jogador no motor

    pthread_t motorlistenerThread;
    pthread_create(&motorlistenerThread, NULL, gameStateListener, &this);
    esperarInicioMotor(this); // Esperar que o motor inicie o jogo

    int sair;
    WINDOW *janela_jogo, *janela_comandos, *janela_mensagens;

    initscr();     // Inicializa o modo curses
    start_color(); // Inicializa o suporte a cores
    erase();       // Limpa a tela
    noecho();      // Desabilita a exibição das teclas digitadas
    cbreak();      // Desabilita o buffer de linha (entrada de caractere imediata)
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);

    // Área do jogo
    janela_jogo = newwin(18, 80, 1, 2);
    printMaze(janela_jogo, localGameInfo.maze);

    // Área dos comandos
    janela_comandos = newwin(12, 80, 18, 2);
    box(janela_comandos, 0, 0);
    wrefresh(janela_comandos);

    // Área de mensagens
    janela_mensagens = newwin(29, 35, 1, 85);
    box(janela_mensagens, 0, 0);
    wrefresh(janela_mensagens);

    pthread_t msgListenerThread;
    pthread_create(&msgListenerThread, NULL, listenForMessages, (void *)janela_mensagens);

    char command[MAX_COMMAND_SIZE];
    int ch;
    int cursor_x = 1;
    int cursor_y = 1;
    int x_jogo = 1;
    int y_jogo = 1;

    WINDOW *active_window = janela_jogo; // Definir janela inicial
    keypad(janela_comandos, FALSE);

    do
    {
        ch = mvwgetch(active_window, cursor_y, cursor_x);
        if (active_window == janela_jogo)
        {
            keypad(janela_jogo, TRUE);
            curs_set(0);
            switch (ch)
            {
            case KEY_LEFT: // Left
                
                break;
            case KEY_RIGHT: // Right
                
                break;
            case KEY_UP: // Up
                
                break;
            case KEY_DOWN: // Down
                
                break;
            case ' ': // Mudar para a janela_comandos
                active_window = janela_comandos;
                curs_set(1);
                break;
            default:
                break;
            }

            // Atualizar posicao do jogador depois do moviemnto
            mvwaddch(janela_jogo, y_jogo, x_jogo, this.username[0]);
            wrefresh(janela_jogo);
        }
        else
        {
            switch (ch)
            {
            case '\n': // Enter key
                command[cursor_x - 1] = '\0';
                getyx(janela_comandos, cursor_y, cursor_x);
                cursor_x = 1;
                mvwaddch(janela_comandos, cursor_y, cursor_x, ' ');
                if (cursor_y >= 10)
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
                validateCommands(command, this, janela_comandos);

                break;
            case 127: // Backspace key
                if (cursor_x > 1)
                {
                    mvwaddch(janela_comandos, cursor_y, --cursor_x, ' ');
                }
                break;
            case 27:                         // Esc key
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

    pthread_cancel(motorlistenerThread);
    pthread_cancel(msgListenerThread);
    endwin();            // Encerra o modo curses
    cleanupFIFOs(&this); // Apagar os FIFOs
    return 0;
}

void enviarMovimentoJogador(char direction, const char *fifo) {
    int fd = open(fifo, O_WRONLY | O_NONBLOCK);
    if (fd != -1) {
        // Escrever char de direção
        // C-Cima B-Baixo E-Esquerda D-Direita
        write(fd, &direction, sizeof(direction)); 
        close(fd);
    } else {
        perror("open");
    }
}

void createFIFOs(Player *this)
{

    sprintf(this->fifo_to_motor, "%s_to_motor.fifo", this->username);
    sprintf(this->fifo_from_motor, "motor_to_%s.fifo", this->username);

    // Criar FIFOs
    if (mkfifo(this->fifo_to_motor, 0666) == -1)
    {
        perror("mkfifo");
        printf("Nome de Utilizador já ocupado\n");
        exit(1);
    }

    if (mkfifo(this->fifo_from_motor, 0666) == -1)
    {
        perror("mkfifo");
        exit(1);
    }
}

void cleanupFIFOs(Player *this)
{
    // Apagar FIFOs
    unlink(this->fifo_to_motor);
    unlink(this->fifo_from_motor);
}

void registarMotor(const char *username)
{
    int fd = open("registo.fifo", O_WRONLY);
    if (fd == -1)
    {
        printf("[ERRO] Motor não está a correr\n");
        exit(1);
    }

    ssize_t numBytes = write(fd, username, strlen(username) + 1);
    if (numBytes == -1)
    {
        perror("write");
    }
    else if (numBytes < strlen(username) + 1)
    {
        fprintf(stderr, "Incomplete write\n");
    }

    close(fd);
}

void validateCommands(char *command, Player user, WINDOW *janela_comandos)
{
    char *commandAux = strtok(command, " \n");
    if (commandAux != NULL)
    {
        if (!strcmp(commandAux, "players"))
        {
            playersCommand(janela_comandos);
        }
        else if (!strcmp(commandAux, "msg"))
        {
            ChatMessage chat;
            strncpy(chat.remetente, user.username, MAX_USERNAME_SIZE);
            chat.destinatario[0] = '\0';
            chat.msg[0] = '\0';
            char *userToken = strtok(NULL, " ");
            char *msgToken = strtok(NULL, "\n");
            if (userToken == NULL || msgToken == NULL)
            {
                wprintw(janela_comandos, "[ERRO] Syntax: msg <username> <message>\n");
                wrefresh(janela_comandos);
            }
            else
            {
                strcat(chat.destinatario, userToken);
                strcat(chat.msg, msgToken);
                msgCommand(chat, janela_comandos);
            }
        }
        else
        {
            wprintw(janela_comandos, "[ERRO]: Comando invalido.");
            wrefresh(janela_comandos);
        }
    }
}

void playersCommand(WINDOW *janela_comandos)
{
    wprintw(janela_comandos, "Players: ");
    for (size_t i = 0; i < localGameInfo.nPlayers; i++)
    {
        if (i == localGameInfo.nPlayers - 1)
        {
            wprintw(janela_comandos, "%s", localGameInfo.players[i].username);
        }
        else
        {
            wprintw(janela_comandos, "%s, ", localGameInfo.players[i].username);
        }
    }
    wrefresh(janela_comandos);
}

void msgCommand(ChatMessage chat, WINDOW *janela_comandos)
{
    // Serializar the message
    char buffer[sizeof(ChatMessage)];
    memcpy(buffer, &chat, sizeof(ChatMessage));

    // Enviar mensagem para o FIFO do destinatario
    char fifoPath[MAX_FIFO_NAME_SIZE];
    sprintf(fifoPath, "motor_to_%s.fifo", chat.destinatario); // Assuming the naming convention for FIFOs
    int fd = open(fifoPath, O_WRONLY | O_NONBLOCK);
    if (fd != -1)
    {
        write(fd, buffer, sizeof(buffer));
        close(fd);
        // Confirmar envio da mensagem
        wprintw(janela_comandos, "%s -> %s: %s", chat.remetente, chat.destinatario, chat.msg);
        wrefresh(janela_comandos);
    }
    else
    {
        wprintw(janela_comandos, "Erro ao enviar mensagem a %s", chat.destinatario);
    }
}

void *listenForMessages(void *arg)
{
    WINDOW *janela_mensagens = (WINDOW *)arg;

    int fd = open(this.fifo_from_motor, O_RDONLY | O_NONBLOCK);
    char buffer[sizeof(ChatMessage)];

    while (true)
    {
        ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
        if (bytesRead > 0)
        {
            // Deserializar a mensagem
            ChatMessage chatMsg;
            memcpy(&chatMsg, buffer, sizeof(ChatMessage));

            // Mostrar mensagem recebida
            //int y = getcury(janela_mensagens);
            mvwprintw(janela_mensagens, getcury(janela_mensagens)+1, 1, "%s: %s", chatMsg.remetente, chatMsg.msg);
            wrefresh(janela_mensagens);
        }
        usleep(100000); // 100 ms
    }

    close(fd);
    return NULL;
}

void printMaze(WINDOW *win, char maze[ROWS][COLS])
{
    for (int i = 0; i < ROWS; i++)
    {
        int k = 0;
        for (int j = 0; j < COLS; j++)
        {
            if (maze[i][j] != ' ' && maze[i][j] != 'X')
            {
                wattron(win, COLOR_PAIR(1)); // Turn on color for player
                mvwaddch(win, i, j + k, maze[i][j]);
                wattroff(win, COLOR_PAIR(1)); // Turn off color
            }
            else
            {
                mvwaddch(win, i, j + k, maze[i][j]); // Print a cada caracter do labirinto
            }
            mvwaddch(win, i, j + ++k, ' ');
        }
    }
    wrefresh(win);
}

void deserializeGameInfo(const char *buffer, Game *gameInfo)
{
    size_t offset = 0;

    // Deserializar num de utilizadores
    memcpy(&gameInfo->nPlayers, buffer + offset, sizeof(gameInfo->nPlayers));
    offset += sizeof(gameInfo->nPlayers);

    // Deserializar usernames
    for (int i = 0; i < gameInfo->nPlayers; i++)
    {
        memcpy(gameInfo->players[i].username, buffer + offset, MAX_USERNAME_SIZE);
        gameInfo->players[i].username[MAX_USERNAME_SIZE - 1] = '\0'; // Ensure null termination
        offset += MAX_USERNAME_SIZE;
    }

    // Deserializar maze
    memcpy(gameInfo->maze, buffer + offset, ROWS * COLS);
    offset += ROWS * COLS;
}

void *gameStateListener(void *arg)
{
    Player *user = (Player *)arg;
    int bufferSize = BUFFER_SIZE;
    char buffer[bufferSize];

    int fd = open(user->fifo_from_motor, O_RDONLY);
    if (fd != -1)
    {
        while (true)
        {
            ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
            if (bytesRead > 0)
            {
                deserializeGameInfo(buffer, &localGameInfo);
                // Update the UI or game logic based on the new game state
            }
            else
            {
                break;
            }
        }
        close(fd);
    }
    else
    {
        perror("open");
    }
    return NULL;
}

void esperarInicioMotor(Player this)
{
    printf("A espera do motor...\n");
    // Abrir FIFO para ler o sinal de começar
    int fd_from_motor = open(this.fifo_from_motor, O_RDONLY);
    if (fd_from_motor == -1)
    {
        perror("open");
        exit(1);
    }

    close(fd_from_motor);
}
