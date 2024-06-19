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
	char dsa_pk_str[35] = "";
	char dsa_sign_str[17] = ""; // DSA-certificate for user's RSA-public-key
	RSA rsa;
	rsa.keyGen();
	char *rsa_pk_str = rsa.pk2str(); // user's RSA-public-key to encrypt AES-key for session
	AES aes;
	// aes.keyGen();
	MD5 md5;
	md5.init();

	/* send login request
	(src_name + password-MD5 + RSA-public-key) */
	char login_request[100] = "";
	char login_response[100] = "";
	char src_name[20] = "";
	char password[20] = "";
	printf("Please input user-name:\n"), scanf("%s", src_name);
	printf("Please input password:\n"), scanf("%s", password);
	printf("LOG(login):\n  token: %s\n  password: %s\n  RSA-public-key: %s\n", src_name, password, rsa_pk_str);
	uint8_t *pwd_hash = md5.hash(password);
	char *pwd_hash_str = md5.hash2str(pwd_hash);
	printf("DEBUG(password-MD5): %s\n", pwd_hash_str);
	sprintf(login_request, "login_request %s %32s %34s", src_name, pwd_hash_str, rsa_pk_str);
	printf("DEBUG(login): send (%s) to server\n", login_request);
	response = send(client_socket, login_request, strlen(login_request), 0);
	free(pwd_hash);
	free(pwd_hash_str);

	/* recv login_response
	(DSA-certificate + DSA-public-key) */
	response = recv(client_socket, login_response, sizeof(login_response), 0);
	printf("DEBUG(login): recv (%s) from server\n", login_response);
	if (strstr(login_response, "accept") != NULL)
	{
		sscanf(login_response, "accept %16s %34s", dsa_sign_str, dsa_pk_str);
		printf("LOG(login): success\n  recv DSA-certificate: %s\n  recv DSA-public-key: %s\n", dsa_sign_str, dsa_pk_str);
	}
	else
	{
		printf("ERROR(login): fail\n");
		system("pause");
		return -1;
	}

	/* send chat request
	(src-name + dst-name + RSA-public-key + DSA-certificate) */
	char chat_request[100] = "";
	char dst_name[20] = "";
	printf("Who do you want to chat with?\n");
	scanf("%s", dst_name);
	sprintf(chat_request, "chat_request %s %s %34s %16s", src_name, dst_name, rsa_pk_str, dsa_sign_str);
	response = send(client_socket, chat_request, sizeof(chat_request), 0);
	printf("DEBUG(chat): send (%s) to %s\n", chat_request, dst_name);

	/* identity authentication */
	char chat_response[100] = "";
	__int128_t dsa_pk = dsa.str2pk(dsa_pk_str);
	char dst_rsa_pk_str[35] = "";
	char dst_dsa_sign_str[17] = "";
	response = recv(client_socket, chat_response, sizeof(chat_response), 0);
	sscanf(chat_response, "chat_request %s %s %34s %16s", dst_name, src_name, dst_rsa_pk_str, dst_dsa_sign_str);
	printf("DEBUG(auth): recv (%s) from %s\n", chat_response, dst_name);
	__int128_t dst_rsa_pk = rsa.str2pk(dst_rsa_pk_str);
	__int128_t dst_dsa_sign = dsa.str2sign(dst_dsa_sign_str);
	bool auth_result = dsa.verify(dst_rsa_pk, dst_dsa_sign, dsa_pk);
	if (auth_result)
	{
		printf("LOG(auth): success\n");
		response = send(client_socket, "accept", sizeof("accept"), 0);
	}
	else
	{
		printf("LOG(auth): fail\n");
		response = send(client_socket, "reject", sizeof("reject"), 0);
	}

	/* receive chat-address */

	free(rsa_pk_str);

	/* start chatting */
	while (true)
	{
		printf("Please input message:\n");
		char buffer[MAX_LENGTH];
		ZeroMemory(buffer, MAX_LENGTH);
		if (msgBuf(buffer) != 0)
			continue;
		response = send(client_socket, buffer, strlen(buffer), 0);
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
