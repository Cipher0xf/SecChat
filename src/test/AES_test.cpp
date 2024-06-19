#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <iostream>
using namespace std;
#include "AES.hpp"
#define MAX_LENGTH 1000

int main()
{
    srand(time(NULL));
    char key_str[17] = "1234567890abcdef";
    AES aes;
    aes.str2key(key_str);
    for (int i = 0; i < 16; i++)
        printf("%02x ", aes.key[i]);
    aes.keyGen();

    char msg[MAX_LENGTH] = "";
    printf("\nPlease input message:\n");
    scanf("%s", msg);
    char *cipher_ptr = aes.encrypt(msg);
    char *msg_ptr = aes.decrypt(cipher_ptr);
    printf("\nciphertext:\n%s\n", cipher_ptr); // to be optimized
    printf("plaintext:\n%s\n", msg_ptr);

    free(msg_ptr);
    free(cipher_ptr);
    system("pause");
    return 0;
}