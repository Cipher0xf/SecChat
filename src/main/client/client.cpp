#include <cstdlib>
#include <WINSOCK2.H>
#include <cstdio>
#include <iostream>
using namespace std;
#pragma comment(lib, "WS2_32.lib")
#include "util.hpp"
#include "RSA.hpp"
#include "DSA.hpp"
#include "AES.hpp"
#include "MD5.hpp"
#define MAX_LENGTH 1000

int msgBuf(char *buffer);

int main()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("ERROR: startup failed\n");
		WSACleanup();
		system("pause");
		return -1;
	}
	int response = 0;

	/* set socket address for client */
	SOCKET client_socket;
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == client_socket)
	{
		printf("ERROR: socket creation failed\n");
		WSACleanup();
		system("pause");
		return -1;
	}

	/* set socket address for server */
	SOCKET server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_socket)
	{
		printf("ERROR: create socket failed\n");
		WSACleanup();
		system("pause");
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

	/* crypto initialization */
	DSA dsa;
	RSA rsa;
	rsa.keyGen();
	AES aes;
	
	// aes.keyGen();
	MD5 md5;
	md5.init();

	/* send login request */
	char login_request[50] = "";
	char token[20] = "";
	char password[20] = "";
	printf("Please input user-id or user-name:\n"), scanf("%s", token);
	printf("Please input password:\n"), scanf("%s", password);
	printf("token: %s\npassword: %s\n", token, password);
	uint8_t *pwd_hash = md5.hash(password);
	char *pwd_hash_str = md5.hash2str(pwd_hash);
	// printf("DEBUG(password hash): %s\n", pwd_hash_str);
	sprintf(login_request, "login_request %s %32s", token, pwd_hash_str);
	// printf("DEBUG(login_request): %s\n", login_request);
	response = send(client_socket, login_request, strlen(login_request), 0);
	free(pwd_hash);
	free(pwd_hash_str);
	char login_response[20];
	response = recv(client_socket, login_response, sizeof(login_response), 0);
	// printf("DEBUG(login_response): %s\n", login_response);
	if (strcmp(login_response, "accept") == 0)
	{
		printf("LOG(login_request): login succeeded\n");
	}
	else
	{
		printf("ERROR(login_request): login failed\n");
		system("pause");
		return -1;
	}

	/* communicate with server */
	while (true)
	{
		/* send to server */
		printf("Please input message:\n");
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

		/* listen for server response */
		// ZeroMemory(buffer, MAX_LENGTH);
		// response = recv(client_socket, buffer, sizeof(buffer), 0);
		// printf("server response: %s\n", buffer);
	}

	closesocket(client_socket);
	closesocket(server_socket);
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
