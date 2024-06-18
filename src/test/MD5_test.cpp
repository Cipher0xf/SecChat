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
    MD5 md5,md5_1;
    md5.init();
    md5_1.init();
    char password[MAX_LENGTH] = "";
    printf("Please input password:\n"), scanf("%s", password);
    printf("strlen(password): %d\n", strlen(password));
    uint8_t *pwd_hash = md5.hash(password);
    uint8_t *pwd_hash_1 = md5_1.hash(password);
    md5.printHash(pwd_hash);
    md5_1.printHash(pwd_hash_1);
    // char *hash_str = md5.hash2str(hash);
    // printf("hash_str: %s\n", hash_str);
    // hash = md5.str2hash(hash_str);
    // md5.printHash(hash);

    free(pwd_hash);
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