#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>
#include <chrono>
#include <WINSOCK2.H>
#include <Windows.h>
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
	uint64_t id;
	char sign_time[20];
	char user_id[20];
	char rsa_public_key[35];
	char salt_value;
	char dsa_certificate[17];
} userCert;

typedef struct
{
	uint64_t seq;
	uint64_t timestamp;
	char src_name[20];
	char dst_name[20];
	char content[MAX_LENGTH];
} chatRecord;

DSA dsa;
map<char *, char *> chatRequestMap; // dst_id-->chat_request
map<char *, bool> authResultMap;	// id-->auth_result

int main()
{
	/* crypto initialization */
	dsa.keyGen();
	/* server configuration */
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
			printf("LOG(connect): client connected\n");
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
	char *dsa_pk_str = dsa.pk2str();
	RSA rsa;
	char rsa_pk_str[35] = "";
	// AES aes;
	MD5 md5;
	md5.init();

	/* receive login_request & sign DSA-certificate */
	char login_request[100] = "";
	char login_response[100] = "";
	char token[20] = "";
	char pwd_hash_str[33] = "";
	response = recv(client_socket, login_request, sizeof(login_request), 0);
	// printf("DEBUG(login): recv (%s)\n", login_request);
	sscanf(login_request, "login_request %s %32s %34s", token, pwd_hash_str, rsa_pk_str);
	printf("LOG(login):\n  token: %s\n  pwd_hash_str: %s\n  rsa_pk_str: %s\n", token, pwd_hash_str, rsa_pk_str);
	userInfo src;
	FILE *fp = fopen("../src/main/server/database/user_info.csv", "r");
	if (fp == NULL)
		printf("ERROR(login): user_info.csv not found\n");
	char col_names[100];
	fscanf(fp, "%99[^\n]\n", col_names); // filter out column names
	bool valid = false;
	while (fscanf(fp, "%llu,%[^,],%[^,],%[^\n]\n", &(src.id), src.user_id, src.user_name, src.password_md5) != EOF)
	{
		// printf("DEBUG(login): Matching... user_id: %s, password_md5: %s\n", src.user_id, src.password_md5);
		if ((strcmp(token, src.user_name) == 0 || strcmp(token, src.user_id) == 0) && strcmp(pwd_hash_str, src.password_md5) == 0)
		{
			valid = true;
			break;
		}
	}
	fclose(fp);
	if (valid)
	{

		printf("LOG(login): accept login request from %s:%d-%s-%s\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, src.user_id, src.user_name);
		__int128_t rsa_pk = rsa.str2pk(rsa_pk_str);
		__int128_t dsa_sign = dsa.sign(rsa_pk, dsa.priv_key);
		char *dsa_sign_str = dsa.sign2str(dsa_sign);
		printf("  sign a RSA-public-key DSA-certificate: %16s\n", dsa_sign_str);
		sprintf(login_response, "accept %16s %34s", dsa_sign_str, dsa_pk_str);
		response = send(client_socket, login_response, strlen(login_response), 0);
		free(dsa_sign_str);
	}
	else
	{
		printf("LOG(login): reject login request from %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
		response = send(client_socket, "reject", strlen("reject"), 0);
	}
	free(dsa_pk_str);

	/* receive chat_request from src
	(dst-name + RSA-public-key + DSA-certificate) */
	char chat_request[100] = "";
	char src_name[20] = "";
	char dst_name[20] = "";
	response = recv(client_socket, chat_request, sizeof(chat_request), 0);
	sscanf(chat_request, "chat_request %s %s", src_name, dst_name);
	printf("LOG(chat): recv (%s) from %s\n", chat_request, src_name);
	chatRequestMap.insert(pair<char *, char *>(dst_name, chat_request));

	/* waiting for chat_request from dst */
	bool both_request = false;
	do
	{
		// printf("DEBUG(chat): Checking chat_request...\n");
		for (auto it = chatRequestMap.begin(); it != chatRequestMap.end(); it++)
		{
			// printf("  dst_name: %s\n", src.user_name);
			if (strcmp(it->first, src.user_name) == 0)
			{
				/* send authentication-info */
				// printf("DEBUG(chat): find chat_request from %s\n", src.user_name);
				response = send(client_socket, it->second, strlen(it->second), 0);
				printf("LOG(chat): send (%s) to %s\n", it->second, src.user_name);
				chatRequestMap.erase(it);
				both_request = true;
				break;
			}
		}
		Sleep(3000);
	} while (!both_request);

	/* wating for verification from dst */
	char auth_result[10] = "";
	bool src_auth = false;
	bool dst_auth = false;
	response = recv(client_socket, auth_result, sizeof(auth_result), 0);
	if (strcmp(auth_result, "accept") == 0)
	{
		src_auth = true;
		authResultMap.insert(pair<char *, bool>(src.user_name, true));
	}
	else
	{
		src_auth = false;
		authResultMap.insert(pair<char *, bool>(src.user_name, false));
	}
	bool both_auth = false;
	do
	{
		for (auto it = authResultMap.begin(); it != authResultMap.end(); it++)
		{
			if (strcmp(it->first, dst_name) == 0)
			{
				if (it->second)
				{
					printf("LOG(auth): %s accepts %s\n", src.user_name, dst_name);
					dst_auth = true;
					both_auth = true;
					break;
				}
				else
				{
					printf("LOG(auth): %s rejects %s\n", src.user_name, dst_name);
					dst_auth = false;
					both_auth = true;
					break;
				}
			}
		}
	} while (!both_auth);
	if (src_auth && dst_auth)
	{
		printf("LOG(auth): both sides authenticated\n");
	}
	else
	{
		printf("LOG(auth): identity authentication failed\n");
	}

	/* send chat-address to both sides */
	char chat_address[100] = "";
	if (strcmp(src.user_name, dst_name) > 0)
	{
		sprintf(chat_address, "../src/main/server/database/chat_record/%s_%s.csv", src_name, dst_name);
	}
	else
	{
		sprintf(chat_address, "../src/main/server/database/chat_record/%s_%s.csv", dst_name, src_name);
	}
	printf("LOG(file): create chat_address %s\n", chat_address);
	FILE *fp2 = fopen(chat_address, "w");
	if (fp == NULL)
	{
		printf("ERROR(file) file not found\n");
	}
	response = send(client_socket, chat_address, strlen(chat_address), 0);
	printf("LOG(file): send chat_address to %s\n", src.user_name);
	fclose(fp2);

	/* transfer chat-content(ciphertext) */
	while (true)
	{
		char buffer[MAX_LENGTH];
		ZeroMemory(buffer, MAX_LENGTH);
		response = recv(client_socket, buffer, MAX_LENGTH, 0);
		if (SOCKET_ERROR == response)
		{
			printf("LOG(connect): client disconnected\n  ip:port %s:%d\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
			closesocket(client_socket);
			return -1;
		}
		else
		{
			printf("<<--------------------------------------------------\n%s send to %s:\n%s\n-------------------------------------------------->>\n", src.user_name, dst_name, buffer);
			uint64_t last_seq = 0;
			FILE *fp3 = fopen(chat_address, "r");
			chatRecord temp;
			char col_names[100];
			fscanf(fp3, "%99[^\n]", col_names); // filter out column names
			while (fscanf(fp3, "%llu,%llu,%[^,],%[^,],%[^\n]\n", &(temp.seq), &(temp.timestamp), temp.src_name, temp.dst_name, temp.content) != EOF)
				last_seq = temp.seq;
			fclose(fp3);
			fp3 = fopen(chat_address, "a");
			auto now = chrono::system_clock::now();
			auto duration = now.time_since_epoch();
			uint64_t timestamp = chrono::duration_cast<chrono::milliseconds>(duration).count() / 1000;
			fprintf(fp3, "%llu,%llu,%s,%s,%s\n", last_seq + 1, timestamp, src_name, dst_name, buffer);
			fclose(fp3);
		}
	}

	// pthread_detach(pthread_self());
	return 0;
}
