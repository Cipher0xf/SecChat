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
    uint8_t key[16] = {};
    printf("initial-key:\n");
    for (int i = 0; i < 16; i++)
    {
        key[i] = rand() % 256;
        printf("%x ", key[i]);
        if (i % 4 == 3)
            printf("\n");
    }

    char msg[MAX_LENGTH] = "";
    char *msg_ptr = NULL;
    char *cipher_ptr = NULL;
    printf("\nPlease input message:\n");
    scanf("%s", msg);

    AES aes;
    aes.keyGen(key);
    cipher_ptr = aes.encrypt(msg);
    msg_ptr = aes.decrypt(cipher_ptr);

    printf("\nciphertext:\n%s\n", cipher_ptr); // to be optimized
    printf("plaintext:\n%s\n", msg_ptr);

    free(msg_ptr);
    free(cipher_ptr);
    system("pause");
    return 0;
}