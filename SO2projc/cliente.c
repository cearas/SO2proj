#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#define _CRT_SECURE_NO_WARNINGS
#define PIPE_NAME TEXT("\\\\.\\pipe\\teste")
#define PIPE_NAME_ACTUALIZA TEXT("\\\\.\\pipe\\teste2")
DWORD WINAPI ThreadActualiza(LPVOID param);


struct MSG{
	int op;
	int resultado;
	TCHAR nome[30];
	DWORD pid;
}typedef msg;

int _tmain(int argc, LPTSTR argv[]){
	TCHAR buf[256];
	HANDLE hPipe;
	BOOL ret;
	DWORD n;
	msg m;


//#ifdef UNICODE 
//	_setmode(_fileno(stdin), _O_WTEXT);
//	_setmode(_fileno(stdout), _O_WTEXT);
//	_setmode(_fileno(stderr), _O_WTEXT);
//#endif

	_tprintf(TEXT("[CLIENTE]Esperar pelo pipe '%s'(WaitNamedPipe)\n"), PIPE_NAME);
	if (!WaitNamedPipe(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'... (WaitNamedPipe)\n"), PIPE_NAME);
		exit(-1);
	}

	_tprintf(TEXT("[CLIENTE] Ligação ao escritor... (CreateFile)\n"));
	//Alterar sentido da comunicação
	hPipe = CreateFile(PIPE_NAME, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPipe == NULL) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'... (CreateFile)\n"), PIPE_NAME);
		exit(-1);
	}	
	_tprintf(TEXT("[CLIENTE]Liguei-me...\n"));
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadActualiza, 0, 0, NULL);
	m.pid = GetCurrentProcessId();
	while (1) {
		_tprintf(TEXT("digite um comando\n"));
		_fgetts(buf, 256, stdin);
	
	
	if(_tcscmp(buf, TEXT("criar\n"))==0){
		m.op=1;
		WriteFile(hPipe,& m,  sizeof(msg), &n, NULL);
		////Recebe resposta
		ret = ReadFile(hPipe, &m, sizeof(msg), &n, NULL);
		
		if(m.resultado == 1)//ssucesso 
		{
		_tprintf(TEXT("[CLIENTE] %s(ReadFile)\n"), TEXT("Deixa criar jogo"));
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadActualiza, 0, 0, NULL);
		}
		else
			_tprintf(TEXT("[CLIENTE] %s(ReadFile)\n"),TEXT("Não Deixa criar jogo"));
	}
	else if(_tcscmp(buf, TEXT("comecar\n"))==0){
		m.op=2;
		WriteFile(hPipe,& m,  sizeof(msg), &n, NULL);
		////Recebe resposta
		ret = ReadFile(hPipe, &m, sizeof(msg), &n, NULL);
		if(m.resultado == 1)//ssucesso 
			_tprintf(TEXT("[CLIENTE] %s(ReadFile)\n"),TEXT("Começou jogo"));
		else
			_tprintf(TEXT("[CLIENTE] %s(ReadFile)\n"),TEXT("Nao pode começar jogo"));
	}
	else if(_tcscmp(buf, TEXT("juntar\n"))==0)
	{
		m.op=3;
		WriteFile(hPipe,& m,  sizeof(msg), &n, NULL);
		////Recebe resposta
		ret = ReadFile(hPipe, &m, sizeof(msg), &n, NULL);
		if(m.resultado == 1)//ssucesso 
		{
		_tprintf(TEXT("[CLIENTE] %s(ReadFile)\n"), TEXT("Deixa Juntar-se a um jogo"));
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadActualiza, 0, 0, NULL);
		}
		else
			_tprintf(TEXT("[CLIENTE] %s(ReadFile)\n"),TEXT("Não Deixa Juntar-se a um jogo"));
	}
	else
		m.op=0;
  
		//
  ////Escreve no Pipe
		//WriteFile(hPipe,& m,  sizeof(msg), &n, NULL);
		////Recebe resposta
		//
		//ret = ReadFile(hPipe, &m, sizeof(msg), &n, NULL);
		////buf[n / sizeof(TCHAR)] = '\0';
		//if (!ret || !n)
		//	break;
		//_tprintf(TEXT("[CLIENTE] Recebi %d bytes: %d... %s(ReadFile)\n"), n,m.op,m.nome);
	}
	CloseHandle(hPipe);

	Sleep(200);
	return 0;
}

DWORD WINAPI ThreadActualiza(LPVOID param){
	HANDLE hPipe = INVALID_HANDLE_VALUE;
	DWORD n;
	msg m;
	BOOL ret= FALSE;
	_tprintf(TEXT("Thread actualiza\n"));
	if (!WaitNamedPipe(PIPE_NAME_ACTUALIZA, NMPWAIT_WAIT_FOREVER)) {
		_tprintf(TEXT("[ERRO] Ligar ao pipe '%s'... (WaitNamedPipe)\n"), PIPE_NAME_ACTUALIZA);
		exit(-1);
	}
	hPipe = CreateFile(PIPE_NAME_ACTUALIZA, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	while(1){
	ret=ReadFile(hPipe, &m, sizeof(msg), &n, NULL);
	if (!ret || !n)
			break;
	_tprintf(TEXT("[CLIENTE ThreadActualiza] Recebi %d bytes: %d... %s(ReadFile)\n"), n,m.op,m.nome);
	}
	return 0;
}