#include <cstdlib>
#include <cstring>
#include <bitset>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <iostream>
using namespace std;
#include "util.hpp"
#include "MD5.hpp"
#define MAX_LENGTH 1000

uint32_t shift[4][4] = { // shift constant
    {7, 12, 17, 22},
    {5, 9, 14, 20},
    {4, 11, 16, 23},
    {6, 10, 15, 21}};

void MD5::init()
{
    A = 0x67452301;
    B = 0xEFCDAB89;
    C = 0x98BADCFE;
    D = 0x10325476;
    sinTableGen();
}

void MD5::sinTableGen()
{
    for (int i = 1; i <= 64; i++)
        sin_table[i - 1] = (uint32_t)(fabs(sin(i) * 4294967296UL));
    // printf("sin_table[%d] = %x\n", i - 1, temp);
}

uint64_t MD5::msgPadding(uint8_t *msg, uint64_t len)
{
    uint64_t pad = 512 - (8 * len + 64) % 512;
    msg[len] = 0x80;
    for (uint64_t i = 1; i < (uint64_t)ceil(pad / 8); i++)
        msg[len + i] = 0;

    bitset<64> bit_len(8 * len);
    for (int i = 0; i < 64; i++)
        msg[len + (uint64_t)ceil(pad / 8) + i / 8] |= bit_len[63 - i] << (7 - i % 8);
    // printf("\nlength: %llu-bit\npadding-length: %llu-bit\npadded-length: %llu-bit\n", bit_len.to_ullong(), pad + 64, 8 * len + pad + 64);
    return (8 * len + pad + 64) / 512;
}

void MD5::process(uint32_t *msg)
{
    for (int r = 0; r < 4; r++)
    {
        for (int i = 0; i < 16; i++)
        {
            uint32_t a = A;
            uint32_t b = B;
            uint32_t c = C;
            uint32_t d = D;
            uint32_t temp = a + F(b, c, d, r) + msg[i] + sin_table[r * 16 + i];
            A = d;
            B = b + (temp << shift[r][i % 4] | temp >> (32 - shift[r][i % 4]));
            C = b;
            D = c;
        }
    }
}

uint32_t MD5::F(uint32_t x, uint32_t y, uint32_t z, int round)
{
    switch (round)
    {
    case 0:
        return (x & y) | (~x & z);
        break;
    case 1:
        return (x & z) | (y & ~z);
        break;
    case 2:
        return x ^ y ^ z;
        break;
    case 3:
        return y ^ (x | ~z);
        break;
    default:
        printf("ERROR(MD5.cpp F): invalid round number\n");
        return 0;
        break;
    }
}

uint8_t *MD5::hash(char *msg_str)
{
    uint8_t msg[MAX_LENGTH] = {0};
    memcpy(msg, msg_str, strlen(msg_str));
    uint64_t len = strlen(msg_str);
    uint64_t block_num = msgPadding(msg, len);
    for (int i = 0; i < block_num; i++)
    {
        uint32_t block[16] = {}; // store the i-th 512-bit block
        for (int j = 0; j < 16; j++)
            block[j] = (uint32_t)msg[i * 64 + j * 4] << 24 | (uint32_t)msg[i * 64 + j * 4 + 1] << 16 | (uint32_t)msg[i * 64 + j * 4 + 2] << 8 | (uint32_t)msg[i * 64 + j * 4 + 3];
        process(block);
    }

    uint8_t *hash = (uint8_t *)malloc(16 * sizeof(uint8_t));
    for (int i = 0; i < 4; i++)
    {
        hash[i] = (A >> (i * 8)) & 0xff;
        hash[i + 4] = (B >> (i * 8)) & 0xff;
        hash[i + 8] = (C >> (i * 8)) & 0xff;
        hash[i + 12] = (D >> (i * 8)) & 0xff;
    }
    return hash;
}

char *MD5::hash2str(uint8_t *hash)
{
    char *hash_str = (char *)malloc(33 * sizeof(char));
    for (int i = 0; i < 16; i++)
        sprintf(hash_str + 2 * i, "%02x", hash[i]);
    hash_str[32] = '\0';
    return hash_str;
}

uint8_t *MD5::str2hash(char *hash_str)
{
    uint8_t *hash = (uint8_t *)malloc(16 * sizeof(uint8_t));
    for (int i = 0; i < 16; i++)
        sscanf(hash_str + i * 2, "%02x", &hash[i]);
    return hash;
}

void MD5::printHash(uint8_t *hash)
{
    printf("MD5-hash: ");
    for (int i = 0; i < 16; i++)
        printf("%02x", hash[i]);
    printf("\n");
}