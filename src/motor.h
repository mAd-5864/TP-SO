#include "estruturas.h"

void validateAdminCommands(char* command);
void usersCommand();
void kickCommand(char *username);
void botsCommand();
void bmovCommand();
void rbmCommand();
void beginCommand();
void endCommand();
void testBotCommand(char* interval, char* duration);

/* Listar jogadores atualmente a usar a plataforma: comando users
● Banir um jogador atualmente registado (porque sim): comando kick <nome do jogador>
Tem o mesmo efeito que o comando exit feito pelo jogador (pode voltar a entrar). O utilizador em questão é
informado, e o seu programa jogoUI deve terminar automaticamente.
Exemplo: kick rogerio66
● Listar os bots atualmente ativos: comando bots
São apresentados os bots e respetiva configuração.
● Insere um bloqueio móvel. Um bloqueio móvel é um pedaço de parede que se vai movimentando a cada segundo
para uma posição adjacente. Apenas se move para posições livres, não atropelando jogadores nem outros obstáculos
que haja no labirinto. Serve para complicar a vida aos jogadores. A posição inicial é sorteada. Comando bmov
● Elimina um bloqueio móvel. Caso haja vários é removido o que foi criado em primeiro lugar. Comando rbm
● Inicia manualmente o jogo, mesmo que não exista um número mínimo de jogadores: comando begin
● Encerrar a jogo: comando end
Encerra o jogo (se estiver a decorrer) e termina o motor. Os processos a correr o jogo são notificados, devendo
também terminar. Os processos bots a correr devem também terminar. Os recursos do sistema em uso são libertados. */