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
#define MAX_THREAD 4

typedef struct
{
	SOCKET server_socket;
	SOCKET client_socket;
	sockaddr_in client_addr;
} Parameter;
DWORD WINAPI mainThread(LPVOID lpargs);
DWORD WINAPI subThread(LPVOID lpParameter);

typedef struct
{
	uint64_t id;
	char user_id[20];
	char user_name[20];
	char password_md5[33];
} userInfo;

typedef struct
{
	char id[20];
	char sign_time[20];
	char user_id[20];
	char rsa_public_key[64];
	char salt_value;
	char certificate[64];
} userCert;

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
		printf("ERROR: startup failed\n");
		WSACleanup();
		system("pause");
		return -1;
	}
	int response = 0;

	/* set socket address for server */
	SOCKET server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == server_socket)
	{
		printf("ERROR: socket creation failed\n");
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

	/* listen for client */
	printf("Waiting for clients...\n");
	response = listen(server_socket, MAX_THREAD);
	if (SOCKET_ERROR == response)
	{
		printf("ERROR: listen failed\n");
		closesocket(server_socket);
		WSACleanup();
		return -1;
	}

	SOCKET client_socket;
	sockaddr_in client_addr;
	int client_addr_len = sizeof(client_addr);
	while (true)
	{
		/* set socket address for client */
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
	// int thread_id = GetCurrentThreadId();
	// printf("  thread-id %d\n", thread_id);
	Parameter *param = (Parameter *)lpParameter;
	SOCKET server_socket = param->server_socket;
	SOCKET client_socket = param->client_socket;
	sockaddr_in client_addr = param->client_addr;
	printf("  ip:port %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
	int response = 0;

	/* crypto initialization */
	DSA dsa;
	dsa.keyGen();
	RSA rsa;
	AES aes;
	MD5 md5;
	md5.init();

	/* receive login request */
	char login_request[50] = "";
	char login_response[30] = "";
	char token[20] = "";
	char pwd_hash_str[33] = "";
	char rsa_pk_str[35] = "";
	response = recv(client_socket, login_request, MAX_LENGTH, 0);
	sscanf(login_request, "login_request %s %32s %34s", token, pwd_hash_str, rsa_pk_str);
	printf("LOG(login):\n  token: %s\n  pwd_hash_str: %s\n  rsa_pk_str: %s\n", token, pwd_hash_str, rsa_pk_str);
	userInfo user_info;
	FILE *fp = fopen("../src/main/server/database/user_info.csv", "r");
	if (fp == NULL)
		printf("ERROR(login): user_info.csv not found\n");
	char col_names[100];
	fscanf(fp, "%99[^\n]\n", col_names); // filter out column names
	bool valid = false;
	while (fscanf(fp, "%llu,%[^,],%[^,],%[^\n]\n", &(user_info.id), user_info.user_id, user_info.user_name, user_info.password_md5) != EOF)
	{
		// printf("DEBUG(login): Matching... user_id: %s, password_md5: %s\n", user_info.user_id, user_info.password_md5);
		if ((strcmp(token, user_info.user_name) == 0 || strcmp(token, user_info.user_id) == 0) && strcmp(pwd_hash_str, user_info.password_md5) == 0)
		{
			valid = true;
			break;
		}
	}
	if (valid)
	{
		printf("LOG(login): accept login request from %s:%d-%s-%s\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, user_info.user_id, user_info.user_name);
		__int128_t rsa_pk = rsa.str2pk(rsa_pk_str);
		__int128_t dsa_sign = dsa.sign(rsa_pk, dsa.priv_key);
		char *dsa_sign_str = dsa.sign2str(dsa_sign);
		printf("  sign a DSA-certificate %16s for RSA-public-key\n", dsa_sign_str);
		sprintf(login_response, "accept %16s", dsa_sign_str);
		response = send(client_socket, login_response, strlen(login_response), 0);
		free(dsa_sign_str);
	}
	else
	{
		printf("LOG(login): reject login request from %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
		response = send(client_socket, "reject", strlen("reject"), 0);
	}
	fclose(fp);

	/* communicate with client */
	while (true)
	{
		/* receive from client */
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
			printf("-------------------------\n%s:%d-%s-%s sent to server:\n%s-------------------------\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, user_info.user_id, user_info.user_name, buffer);
			// char reply[] = "okk!";
			// response = send(client_socket, reply, strlen(reply), 0);
		}
	}

	// pthread_detach(pthread_self());
	return 0;
}
