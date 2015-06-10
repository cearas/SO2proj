#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <strsafe.h>
#include "Dados.h"

#define _CRT_SECURE_NO_WARNINGS
#define N 4
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")
#define PIPE_NAME_ACTUALIZA TEXT("\\\\.\\pipe\\teste2")
DWORD WINAPI ThreadAtendeCliente(LPVOID param);




//dados jogo

struct JOGADOR{
	DWORD pid;
	int x, y;
}typedef Jogador;

Jogador ListaJogadores[N];
int total_jogadores=0;
BOOL JOGO=FALSE, JOGO_COMECOU=FALSE;
//dados actualiza
HANDLE hPipeA[N]; 
DWORD total=0;


int _tmain(int argc, LPTSTR argv[]){
	HANDLE hThread=NULL;
	BOOL fConnected = FALSE;
	DWORD dwThreadID = 0;
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	int i;
	
//#ifdef UNICODE 
//	_setmode(_fileno(stdin), _O_WTEXT);
//	_setmode(_fileno(stdout), _O_WTEXT);
//	_setmode(_fileno(stderr), _O_WTEXT);
//#endif
	

		for(i = 0;i<N;i++){
		//Duplo sentido na comunicação
			_tprintf(TEXT("[SERVIDOR] Vou criação o pipe '%s' de recepcao... (CreateNamedPipe)\n"), PIPE_NAME);
		hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE 
		| PIPE_READMODE_MESSAGE, N, sizeof(msg), sizeof(msg),
		1000, NULL);
		if (hPipe == INVALID_HANDLE_VALUE){
			_tprintf(TEXT("CreateNamedPipe falhou, erro=%d\n"),GetLastError());
			exit(-1);
		}
		
		/*hPipeA[i] = CreateNamedPipe(PIPE_NAME_ACTUALIZA, PIPE_ACCESS_OUTBOUND|FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE
		| PIPE_READMODE_MESSAGE, N, sizeof(msg), 0,1000,NULL);
		total++;*/
		_tprintf(TEXT("[SERVIDOR] Esperar ligação de um cliente... (ConnectNamedPipe)\n"));

		fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		
		//Lancar thread para cada novo cliente (hPipe[i] ou i)
		if(fConnected){
			_tprintf(TEXT("[SERVIDOR] Um cliente ligou-se...Vou criar uma thread p/ ele\n"));;
			hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadAtendeCliente, (LPVOID) hPipe, 0, NULL);
		
		if(hThread == NULL){
			_tprintf(TEXT("Erro na criação da thread. Erro=%d. \n"),GetLastError());
			exit(-1);
		}
		else
			CloseHandle(hThread);
		}
	else
		CloseHandle(hPipe);
	
}
	_tprintf(TEXT("Sistema vai encerrar!"));
	
	return 0;
}

DWORD WINAPI ThreadAtendeCliente(LPVOID param){
	DWORD nlidos,nescritos,i;
	BOOL ret = FALSE;
	msg m;
	
	//trabalhar somente com este cliente
	HANDLE cliente = (HANDLE)param;
	do{
				ret = ReadFile(cliente, &m, sizeof(msg), &nlidos, NULL);		
		if (!ret || !nlidos)
			break;
				
		if(m.op == 1)
		{
			if(JOGO==FALSE){
			hPipeA[total] = CreateNamedPipe(PIPE_NAME_ACTUALIZA, PIPE_ACCESS_OUTBOUND|FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, N, sizeof(msg), 0,1000,NULL);
			total++;
			ListaJogadores[total_jogadores].pid = m.pid;
			total_jogadores++;
			m.resultado = 1;
			JOGO = TRUE;
			novo_jogo();
			}
			else
			m.resultado = 0;
		}
	
		else if(m.op == 2)
		{
			if(JOGO == TRUE && JOGO_COMECOU == FALSE && m.pid == ListaJogadores[0].pid){
				m.resultado = 1;
				JOGO_COMECOU = TRUE;
			}
			else
				m.resultado = 0;
			
		}
		else if(m.op == 3)
		{
			if(JOGO ==TRUE && JOGO_COMECOU == FALSE) {
				hPipeA[total] = CreateNamedPipe(PIPE_NAME_ACTUALIZA, PIPE_ACCESS_OUTBOUND|FILE_FLAG_OVERLAPPED, PIPE_WAIT | PIPE_TYPE_MESSAGE
			| PIPE_READMODE_MESSAGE, N, sizeof(msg), 0,1000,NULL);
			total++;
				m.resultado = 1;
				ListaJogadores[total_jogadores].pid = m.pid;
				total_jogadores++;
				
			}
			
			else
				m.resultado = 0;
		}
		else if(m.op == 4)
		{
			_tcscpy_s(m.nome,30,TEXT("move baixo"));
		}
		else if(m.op == 5){
			
			_tcscpy_s(m.nome,30,TEXT("move direita"));
		}
		else if(m.op == 6)
		{
			_tcscpy_s(m.nome,30,TEXT("move esquerda"));
		}
		else
			_tcscpy_s(m.nome,30,TEXT("nenhuma opcao valida"));

		WriteFile(cliente, &m, sizeof(msg),&nescritos, NULL);
		for(i=0;i<total;i++)
			WriteFile(hPipeA[i], &m, sizeof(msg),&nescritos, NULL);
		
	} while (1);

	

	_tprintf(TEXT("[SERVIDOR] Vou desligar o pipe(DisconnectNamedPipe/CloseHandle)\n"));
	

		if (!DisconnectNamedPipe(cliente)){
			_tperror(TEXT("Erro ao desligar o pipe!"));
			exit(-1);
		}
	
		//Sleep(2000);
		_tprintf(TEXT("[SERVIDOR] Vou desligar o pipe... (CloseHandle)\n"));
		CloseHandle(cliente);
	
return 0;
}

///FUNCOES DE JOGO

void constroiLabirinto(jogo *pdados, int x, int y){
	//Quantidade de inimigos
	//Haver mais blocos para destruir
	//Quantidade de Objectos para apanhar

	int i, j, r;
	pdados->maxLin = x;
	pdados->maxCol = y;
	for (i = 0; i<x; i++)
	for (j = 0; j < y; j++){
		pdados->Mapa[i][j].bloco = 'D';
		pdados->Mapa[i][j].permissao = TRUE;
	}
	for (i = 0; i<x; i++){
		pdados->Mapa[i][0].bloco = 'I';
		pdados->Mapa[i][y - 1].bloco = 'I';
		pdados->Mapa[i][0].permissao = FALSE;
		pdados->Mapa[i][y - 1].permissao = FALSE;
	}
	for (i = 1; i<y - 1; i++){
		pdados->Mapa[0][i].bloco = 'I';
		pdados->Mapa[0][i].permissao = FALSE;
		pdados->Mapa[x - 1][i].bloco = 'I';
		pdados->Mapa[x - 1][i].permissao = FALSE;
	}

	for (i = 0; i<x; i++)
	for (j = 0; j < y; j++){
		i++; j++;
		pdados->Mapa[i][j].bloco = 'I';
		pdados->Mapa[i][j].permissao = FALSE;
	
	}
	for (i = 0; i<x; i++)
	for (j = 0; j < y; j++){
		pdados->Mapa[i][j].bloco = 'D';
		pdados->Mapa[i][j].permissao = TRUE;
	}

	for (i = 1; i<x - 1; i++)
	for (j = 1; j<y - 1; j++)
	{
		r = 1 + (rand() % 100);
		if (r > 35){
			pdados->Mapa[i][j].bloco = 'C';
			pdados->Mapa[i][j].permissao = TRUE;
		}
	}
}

void mostraLabirinto(jogo dados){
	int l, c;
	for (l = 0; l<dados.maxLin; l++)
	{
		printf("\n");

		for (c = 0; c<dados.maxCol; c++)
			printf(" %c", dados.Mapa[l][c].bloco);
		printf("\n");
	}
}

void criarJogador(jogo *pdados, int pidCliente){

	pdados->ListaJogadores[total_jogadores].pid = pidCliente;
	pdados->ListaJogadores[total_jogadores].vidas = 3;

	pdados->ListaJogadores[total_jogadores].x = 1;
	pdados->ListaJogadores[total_jogadores].y = 1 + total_jogadores;
	pdados->Mapa[1][1 + total_jogadores].bloco = 'J';
	pdados->Mapa[1][1 + total_jogadores].permissao = 0;
	total_jogadores++;

}
void moveBaixo(jogo *pdados, int pidCliente){
	int i;
	for (i = 0; i<total_jogadores; i++)
	if (pdados->ListaJogadores[i].pid == pidCliente){
		if (pdados->Mapa[pdados->ListaJogadores[i].x + 1][pdados->ListaJogadores[i].y].bloco == 'C'
			&& pdados->Mapa[pdados->ListaJogadores[i].x + 1][pdados->ListaJogadores[i].y].permissao == 1 && pdados->ListaJogadores[i].x + 1 < pdados->maxLin){

			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y].bloco = 'C';
			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y].permissao = 1;
			pdados->Mapa[pdados->ListaJogadores[i].x + 1][pdados->ListaJogadores[i].y].bloco = 'J';
			pdados->Mapa[pdados->ListaJogadores[i].x + 1][pdados->ListaJogadores[i].y].permissao = 0;
			pdados->ListaJogadores[i].x = pdados->ListaJogadores[i].x + 1;
		}
	}

}
void moveCima(jogo *pdados, int pidCliente){
	int i;
	for (i = 0; i<total_jogadores; i++)
	if (pdados->ListaJogadores[i].pid == pidCliente){
		if (pdados->Mapa[pdados->ListaJogadores[i].x - 1][pdados->ListaJogadores[i].y].bloco == 'C'
			&& pdados->Mapa[pdados->ListaJogadores[i].x - 1][pdados->ListaJogadores[i].y].permissao == 1 && pdados->ListaJogadores[i].x - 1 > 0){

			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y].bloco = 'C';
			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y].permissao = 1;
			pdados->Mapa[pdados->ListaJogadores[i].x - 1][pdados->ListaJogadores[i].y].bloco = 'J';
			pdados->Mapa[pdados->ListaJogadores[i].x - 1][pdados->ListaJogadores[i].y].permissao = 0;
			pdados->ListaJogadores[i].x = pdados->ListaJogadores[i].x - 1;
		}
	}
}
void moveDireita(jogo *pdados, int pidCliente){
	int i;
	for (i = 0; i<total_jogadores; i++)
	if (pdados->ListaJogadores[i].pid == pidCliente){
		if (pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y + 1].bloco == 'C'
			&& pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y + 1].permissao == 1 && pdados->ListaJogadores[i].y + 1 < pdados->maxCol){

			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y].bloco = 'C';
			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y].permissao = 1;
			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y + 1].bloco = 'J';
			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y + 1].permissao = 0;
			pdados->ListaJogadores[i].y = pdados->ListaJogadores[i].y + 1;
		}
	}
}
void moveEsquerda(jogo *pdados, int pidCliente){
	int i;
	for (i = 0; i<total_jogadores; i++)
	if (pdados->ListaJogadores[i].pid == pidCliente){
		if (pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y - 1].bloco == 'C'
			&& pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y - 1].permissao == 1 && pdados->ListaJogadores[i].y - 1 > 0){

			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y].bloco = 'C';
			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y].permissao = 1;
			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y - 1].bloco = 'J';
			pdados->Mapa[pdados->ListaJogadores[i].x][pdados->ListaJogadores[i].y - 1].permissao = 0;
			pdados->ListaJogadores[i].y = pdados->ListaJogadores[i].y - 1;
		}
	}
}
void colocaBomba(jogo *pdados, int pidCliente){
	int i;
	HANDLE t;
	pbomba b = (pbomba)malloc(sizeof (struct Bomba));



	////preencheBomba
	for (i = 0; i<total_jogadores; i++)
	if (pdados->ListaJogadores[i].pid == pidCliente){
		b->x = pdados->ListaJogadores[i].x++;
		b->y = pdados->ListaJogadores[i].y++;
		b->potencia = 3;
		break;
	}

	printf("BOMBA ACTIVADA(%d %d)\n", b->x, b->y);


	t = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadBomba, (LPVOID)b, 0, NULL);
	//WaitForSingleObject(t,INFINITE);

}

DWORD WINAPI ThreadBomba(LPVOID param){

	pbomba j = (pbomba)param;
	int i;
	Sleep(2000);


	//Explode epicentro

	//Explode direita
	for (i = 1; i<j->potencia; i++){
		if (novojogo.Mapa[j->x][j->y + i].bloco == 'D' && j->y + i < novojogo.maxCol)
		{
			novojogo.Mapa[j->x][j->y + i].bloco = 'C';
			break;
		}
	}
	//Explode esquerda
	for (i = 1; i<j->potencia; i++){
		if (novojogo.Mapa[j->x][j->y - 1].bloco == 'D' && j->y - 1 > 0){
			novojogo.Mapa[j->x][j->y - 1].bloco = 'C';
			break;
		}
	}
	//Explode cima
	for (i = 1; i<j->potencia; i++){
		if (novojogo.Mapa[j->x - 1][j->y].bloco == 'D' && j->x - 1 > 0){
			novojogo.Mapa[j->x - 1][j->y].bloco = 'C';
			break;
		}
	}
	//Explode baixo
	for (i = 1; i<j->potencia; i++){
		if (novojogo.Mapa[j->x + 1][j->y].bloco == 'D' && j->x + 1 < novojogo.maxLin)
		{
			novojogo.Mapa[j->x + 1][j->y].bloco = 'C';
			break;
		}
	}
	printf("BOMBA Explodida(%d %d)\n", j->x, j->y);
	free(j);


	return 0;
}

void novo_jogo(){

	hMutexbomba = CreateMutex(NULL, FALSE, NULL);
	/*t1 =CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadBomba,(LPVOID)hMutex,0,NULL);
	WaitForSingleObject(t1,INFINITE);*/

	constroiLabirinto(&novojogo, 10, 10);
	criarJogador(&novojogo, 1);
	mostraLabirinto(novojogo);
	colocaBomba(&novojogo, 1);
	colocaBomba(&novojogo, 1);

	while (1){

	}


}