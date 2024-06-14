#include <stdlib.h>
#include <WINSOCK2.H>
#include <iostream>
using namespace std;
#pragma comment(lib, "WS2_32.lib")
#define MAX_LENGTH 5000
#define MAX_THREAD 5

typedef struct
{
	SOCKET server_socket;
	SOCKET client_socket;
	sockaddr_in client_addr;
} Parameter;

DWORD WINAPI mainThread(LPVOID lpargs);
DWORD WINAPI subThread(LPVOID lpParameter);

int main()
{
	int port = 7701; // server port
	CreateThread(NULL, 0, mainThread, &port, 0, NULL);
	Sleep(INFINITE);
	system("pause");
	return 0;
}

DWORD WINAPI mainThread(LPVOID lpargs)
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("ERROR: WSAStartup failed\n");
		WSACleanup();
		return -1;
	}
	int response = 0;

	/* set socket address for server */
	SOCKET server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_socket)
	{
		printf("ERROR: create socket failed\n");
		WSACleanup();
		return -1;
	}
	int *p = (int *)lpargs;
	int port = *p;
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(port);
	response = bind(server_socket, (const struct sockaddr *)&server_addr, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == response)
	{
		printf("ERROR: bind failed\n");
		closesocket(server_socket);
		WSACleanup();
		return -1;
	}

	/* listening for client */
	// printf("waiting for client...\n");
	response = listen(server_socket, MAX_THREAD);
	if (SOCKET_ERROR == response)
	{
		printf("ERROR: listen failed\n");
		closesocket(server_socket);
		WSACleanup();
		return -1;
	}

	SOCKET client_socket;
	while (true)
	{
		sockaddr_in client_addr;
		int client_addr_len = sizeof(client_addr);
		client_socket = accept(server_socket, (sockaddr FAR *)&client_addr, &client_addr_len);
		if (INVALID_SOCKET == client_socket)
		{
			printf("ERROR: accept failed\n");
			WSACleanup();
			system("pause");
			return -1;
		}
		else
		{
			printf("LOG: client connected\n");
			Parameter param;
			param.server_socket = server_socket;
			param.client_socket = client_socket;
			param.client_addr = client_addr;
			CreateThread(NULL, 0, subThread, &param, 0, NULL);
		}
	}

	closesocket(server_socket);
	closesocket(client_socket);
	WSACleanup();
	system("pause");
	return 0;
}

DWORD WINAPI subThread(LPVOID lpParameter)
{
	int id = GetCurrentThreadId();
	// printf("thread-id %d\n", id);
	Parameter *param = (Parameter *)lpParameter;
	SOCKET server_socket = param->server_socket;
	SOCKET client_socket = param->client_socket;
	sockaddr_in client_addr = param->client_addr;
	printf("  ip:port %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);

	while (true)
	{
		/* receive data from client */
		int response = 0;
		char buffer[MAX_LENGTH];
		ZeroMemory(buffer, MAX_LENGTH);
		response = recv(client_socket, buffer, MAX_LENGTH, 0);
		if (SOCKET_ERROR == response)
		{
			printf("LOG: client disconnected\n  ip:port %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
			closesocket(client_socket);
			return -1;
		}
		else
		{
			printf("\nclient(%s:%d) sent to server:\n%s\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, buffer);
			char reply[]="okk!";
			response = send(client_socket, reply, strlen(reply), 0);
			if (SOCKET_ERROR == response)
			{
				printf("ERROR: reply failed\n");
				continue;
			}
		}
	}

	// pthread_detach(pthread_self());
	return 0;
}
