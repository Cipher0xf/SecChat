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

    char message[MAX_LENGTH] = "";
    char ciphertext[MAX_LENGTH] = "";
    char plaintext[MAX_LENGTH] = "";
    cout << "\nPlease input message:\n";
    scanf("%s", message);
    uint8_t msg[MAX_LENGTH] = "";
    uint64_t len = strlen(message);
    memcpy(msg, message, len);

    AES aes;
    aes.keyGen(key);
    uint64_t block_num = aes.msgPadding(msg, len);

    for (int i = 0; i < block_num; i++)
    {
        uint8_t block[16] = {};
        memcpy(block, msg + i * 16, 16);

        aes.encrypt(block);
        memcpy(ciphertext + i * 16, block, 16);
        printf("\nciphertext-block-%d\n", i);
        aes.printByteMatrix(block);

        aes.decrypt(block);
        memcpy(plaintext + i * 16, block, 16);
        printf("plaintext-block-%d\n", i);
        aes.printByteMatrix(block);
    }
    printf("\nciphertext: %s\n", ciphertext);
    printf("plaintext: %s\n", plaintext);
    
    system("pause");
    return 0;
}