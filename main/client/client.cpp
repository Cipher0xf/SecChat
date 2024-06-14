#include <stdlib.h>
#include <WINSOCK2.H>
#include <iostream>
using namespace std;
#pragma comment(lib, "WS2_32.lib")
#define MAX_LENGTH 5000

int msgBuf(char *buffer);

int main()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("ERROR: WSAStartup failed\n");
		WSACleanup();
		return -1;
	}
	int response = 0;

	/* set socket address for client */
	SOCKET client_socket;
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == client_socket)
	{
		printf("ERROR: create socket failed\n");
		WSACleanup();
		return -1;
	}

	/* set socket address for server */
	SOCKET server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_socket)
	{
		printf("ERROR: create socket failed\n");
		WSACleanup();
		return -1;
	}
	SOCKADDR_IN server_addr;
	char ip[] = "127.0.0.1";
	int port = 7701;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	/* connect to server */
	response = connect(client_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr));
	if (SOCKET_ERROR == response)
	{
		printf("ERROR: connect failed\n");
		closesocket(client_socket);
		WSACleanup();
		system("pause");
		return -1;
	}

	/* communication */
	while (true)
	{
		/* send message to server */
		printf("\nplease input message:\n");
		char buffer[MAX_LENGTH];
		ZeroMemory(buffer, MAX_LENGTH);
		if (msgBuf(buffer) != 0)
			continue;
		response = send(client_socket, buffer, strlen(buffer), 0);
		if (SOCKET_ERROR == response)
		{
			printf("ERROR: send failed\n");
			continue;
		}

		/* listening for server response */
		ZeroMemory(buffer, MAX_LENGTH);
		response = recv(client_socket, buffer, sizeof(buffer), 0);
		if (SOCKET_ERROR == response)
		{
			printf("ERROR: server reciption failed\n");
			continue;
		}
		printf("server response: %s\n", buffer);
	}

	closesocket(client_socket);
	WSACleanup();
	system("pause");
	return 0;
}

int msgBuf(char *buffer)
{
	ZeroMemory(buffer, MAX_LENGTH);
	int i = 0;
	char c;
	while (c = getchar())
	{
		if (c == '\n' && i > 0)
		{
			if (buffer[i - 1] == '\n')
				break;
		}
		buffer[i++] = c;
	}
	if (i > MAX_LENGTH)
	{
		printf("ERROR: message too long\n");
		return -1;
	}
	return 0;
}
