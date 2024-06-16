#include <cstring>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <iostream>
using namespace std;
#define ROUND 16
#define MAX_LENGTH 1000

uint8_t IP[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7};
uint8_t _IP[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25};
uint8_t Ebox[48] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1};
uint8_t Sbox[8][4][16] = {
    {{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
     {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
     {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
     {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}}, // S1

    {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
     {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
     {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
     {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}}, // S2

    {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
     {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
     {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
     {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}}, // S3

    {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
     {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
     {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
     {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}}, // S4

    {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
     {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
     {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
     {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}}, // S5

    {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
     {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
     {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
     {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}}, // S6

    {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
     {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
     {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
     {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}}, // S7

    {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
     {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
     {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
     {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}} // S8
};
uint8_t Pbox[32] = {
    16, 7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2, 8, 24, 14,
    32, 27, 3, 9,
    19, 13, 30, 6,
    22, 11, 4, 25};
uint8_t PC1[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,

    3, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4};
uint8_t PC2[48] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32};
uint8_t keyShift[ROUND] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

uint64_t key = 0;
uint64_t genKeys[ROUND] = {};
char message[MAX_LENGTH] = "";
char ciphertext[MAX_LENGTH] = "";
char plaintext[MAX_LENGTH] = "";

void messagePadding(char *msg);
bool keyCheck(uint64_t key);
void keyGen(uint64_t key); // generate 16 48-bit keys
uint64_t initialPermutation(uint64_t block);
uint64_t inversePermutation(uint64_t block);
uint64_t expansion(uint64_t block, uint64_t key); // 32bits-->48bits
uint64_t substitution(uint64_t block); // 48bits-->32bits
uint64_t permutation(uint64_t block); // 32bits-->32bits
uint64_t F(uint64_t block, uint64_t key); // round-function 32bits-->48bits-->32bits
uint64_t feistel(uint64_t leftBlock, uint64_t rightBlock, uint64_t *genKeys, uint8_t round);
uint64_t des(uint64_t input, uint64_t *genKeys);

int main()
{
    keyGen(key);
    cout << "\nPlease input message:\n";
    cin >> message;
    for (int i = 0; i < strlen(message); i++)
    {
        message[i] = 'a';
    }
    messagePadding(message);
    for (int i = 0; i < strlen(message); i += 8)
    {
        uint64_t block = 0;
        for (int j = 0; j < 8; j++)
        {
            block |= ((uint64_t)message[i + j] << (56 - j * 8));
        }
        cout << "(" << i / 8 + 1 << ")" << endl;
        cout << "message-block-" << i / 8 + 1 << ": " << block << endl;
        cout << "ciphertext-block-" << i / 8 + 1 << ": " << des(block, genKeys) << endl;
        cout << "plaintext-block-" << i / 8 + 1 << ": " << des(des(block, genKeys), genKeys) << endl;
    }
    system("pause");
    return 0;
}

void messagePadding(char *msg)
{
    int padding = 8 - strlen(msg) % 8;
    if (padding != 8)
    {
        for (int i = 0; i < padding; i++)
        {
            msg[strlen(msg) + i] = 0;
        }
    }
}

bool keyCheck(uint64_t key)
{
    return true;
}
void keyGen(uint64_t key) // generate 16 48-bit keys
{
    uint64_t pcKey1 = 0;
    for (int i = 0; i < 56; i++)
    {
        pcKey1 |= ((key >> (64 - PC1[i])) & 1) << (55 - i);
    }
    uint64_t C = pcKey1 >> 28;
    uint64_t D = pcKey1 & 0xFFFFFFF;
    for (int r = 0; r < ROUND; r++)
    {
        C = (C << keyShift[r]) | (C >> (28 - keyShift[r]));
        D = (D << keyShift[r]) | (D >> (28 - keyShift[r]));
        uint64_t cdKey = (C << 28) | D;
        uint64_t pcKey2 = 0;
        for (int i = 0; i < 48; i++)
        {
            pcKey2 |= ((cdKey >> (56 - PC2[i])) & 1) << (47 - i);
        }
        genKeys[r] = pcKey2;
    }
}

uint64_t initialPermutation(uint64_t block)
{
    uint64_t initBlock = 0;
    for (int i = 0; i < 64; i++)
    {
        initBlock |= ((block >> (64 - IP[i])) & 1) << (63 - i);
    }
    return initBlock;
}
uint64_t inversePermutation(uint64_t block)
{
    uint64_t invBlock = 0;
    for (int i = 0; i < 64; i++)
    {
        invBlock |= ((block >> (64 - _IP[i])) & 1) << (63 - i);
    }
    return invBlock;
}

uint64_t expansion(uint64_t block, uint64_t key) // 32bits-->48bits
{
    uint64_t exBlock = 0;
    for (int i = 0; i < 48; i++)
    {
        exBlock |= ((block >> (32 - Ebox[i])) & 1) << (47 - i);
    }
    return exBlock ^ key;
}
uint64_t substitution(uint64_t block) // 48bits-->32bits
{
    uint64_t sBlock = 0;
    for (int i = 0; i < 8; i++)
    {
        uint8_t row = ((block >> (42 - i * 6)) & 0b100000) | ((block >> (47 - i * 6)) & 0b1);
        uint8_t col = (block >> (43 - i * 6)) & 0b1111;
        uint8_t temp = Sbox[i][row][col];
        sBlock |= (uint64_t)temp << (28 - i * 4);
    }
    return sBlock;
}
uint64_t permutation(uint64_t block) // 32bits-->32bits
{
    uint64_t pBlock = 0;
    for (int i = 0; i < 32; i++)
    {
        pBlock |= ((block >> (32 - Pbox[i])) & 1) << (31 - i);
    }
    return pBlock;
}
uint64_t F(uint64_t block, uint64_t key) // round-function 32bits-->48bits-->32bits
{
    return permutation(substitution(expansion(block, key)));
}

uint64_t feistel(uint64_t leftBlock, uint64_t rightBlock, uint64_t *genKeys, uint8_t round)
{
    uint64_t fBlock = 0;
    for (int r = 0; r < round; r++)
    {
        uint64_t tempBlock = F(rightBlock, genKeys[r]) ^ leftBlock;
        leftBlock = rightBlock;
        rightBlock = tempBlock;
    }
    return (rightBlock << 32) | leftBlock;
}

uint64_t des(uint64_t input, uint64_t *genKeys)
{
    uint64_t leftBlock = initialPermutation(input) >> 32;
    uint64_t rightBlock = initialPermutation(input) & 0xFFFFFFFF;
    return inversePermutation(feistel(leftBlock, rightBlock, genKeys, ROUND));
}
