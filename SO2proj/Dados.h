#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <strsafe.h>

struct Posicao{
	char bloco;
	BOOL permissao;
}typedef posicao;

struct Jogador{
	DWORD pid;
	int x, y;
	int bombas;
	int vidas;
	
}typedef jogador;

struct Bomba{
	int x, y;
	int potencia;
}typedef bomba;
typedef struct Bomba *pbomba;

struct Nivel{
	int i;
}typedef nivel;

struct Jogo{
	nivel nivel_atual;
	posicao Mapa[100][100];
	int maxLin, maxCol;
	jogador ListaJogadores[4];
}typedef jogo;

struct Msg{
	int op;
	int resultado;
	TCHAR nome[30];
	DWORD pid;
}typedef msg;

HANDLE hMutex, hMutexbomba;

int total_jogadores = 0;
jogo novojogo;
void novo_jogo();
void cria_jogo();
void constroiLabirinto(jogo *pdados, int x, int y);
void mostraLabirinto(jogo dados);
void criarJogador(jogo *pdados, int pidCliente);
void moveBaixo(jogo *pdados, int pidCliente);
void moveCima(jogo *pdados, int pidCliente);
void moveDireita(jogo *pdados, int pidCliente);
void moveEsquerda(jogo *pdados, int pidCliente);
void colocaBomba(jogo *pdados, int pidCliente);
DWORD WINAPI ThreadBomba(LPVOID param);


//prototipos de Threads

//prototipos de funcções

