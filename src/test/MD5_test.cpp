#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <iostream>
using namespace std;
#include "MD5.hpp"
#define MAX_LENGTH 512

void preimage_attack(MD5 md5, uint8_t *target);

int main()
{
    MD5 md5;
    md5.init();
    printf("Please input message:\n");
    char msg_str[MAX_LENGTH] = "";
    scanf("%s", msg_str);
    uint8_t *hash_str = md5.hash(msg_str);

    printf("MD5-hash: ");
    for (int i = 0; i < 16; i++)
        printf("%02x", hash_str[i]);
    printf("\n");

    // preimage_attack(md5, hash_str);
    free(hash_str);
    system("pause");
    return 0;
}

void preimage_attack(MD5 md5, uint8_t *target)
{
    srand(time(NULL));
    char preimage[MAX_LENGTH] = "";
    uint8_t *hash = NULL;
    uint64_t count = 0;
    do
    {
        // count++;
        // if (count % (1ULL<<32) == 0)
        //     printf(".");
        for (int i = 0; i < MAX_LENGTH; i++)
            preimage[i] = rand() % 256;
        hash = md5.hash(preimage);
    } while (memcmp(hash, target, 16) != 0);
    free(hash);
    printf("find a preimage: %s\n", preimage);
}