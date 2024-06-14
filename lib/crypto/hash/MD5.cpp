#include <iostream>
#include <cstdlib>
#include <cstring>
#include <bitset>
#include <ctime>
#include <cmath>
#include <algorithm>
using namespace std;

// MD5 buffer
uint32_t A = 0x67452301;
uint32_t B = 0xEFCDAB89;
uint32_t C = 0x98BADCFE;
uint32_t D = 0x10325476;

// sin table
uint32_t sin_table[64] = {};
// shift constant
uint32_t shift[4][4] = {
    {7, 12, 17, 22},
    {5, 9, 14, 20},
    {4, 11, 16, 23},
    {6, 10, 15, 21}};

uint32_t message_padding(uint8_t *msg, uint64_t length); // pad the message with 1000... and 64-bit length, return the number of 512-bit blocks
void sin_table_generation();
void MD5_process(uint32_t *msg);                           // process a 512-bit block, update MD5 buffer
uint32_t F(uint32_t x, uint32_t y, uint32_t z, int round); // the unlinear function F
void hash_output();                                        // output the MD5 hash value

int main()
{
    sin_table_generation();
    int t = 9;
    while (t--)
    {
        char message[1000] = {};
        printf("\n\nPlease enter the message:\n");
        scanf("%s", message);
        uint8_t msg[1000];
        memcpy(msg, message, sizeof(message));

        uint32_t block_num = message_padding(msg, strlen(message));
        for (int i = 0; i < block_num; i++)
        {
            uint32_t block[16] = {}; // restore the i-th 512-bit block
            for (int j = 0; j < 16; j++)
            {
                block[j] = (uint32_t)msg[i * 64 + j * 4] << 24 | (uint32_t)msg[i * 64 + j * 4 + 1] << 16 | (uint32_t)msg[i * 64 + j * 4 + 2] << 8 | (uint32_t)msg[i * 64 + j * 4 + 3];
            }
            MD5_process(block);
        }
        hash_output();
    }
    return 0;
}

uint32_t message_padding(uint8_t *msg, uint64_t length)
{
    uint64_t padding_bit_length = 512 - (length * 8 + 64) % 512;
    msg[length] = 0x80;
    for (uint64_t i = 1; i < (uint64_t)ceil(padding_bit_length / 8); i++)
    {
        msg[length + i] = 0;
    }
    bitset<64> bit_length(length * 8);
    for (int i = 0; i < 64; i++)
    {
        msg[length + (uint64_t)ceil(padding_bit_length / 8) + i / 8] |= bit_length[63 - i] << (7 - i % 8);
    }
    printf("\nbit-length = %llu\npadding-bit-length = %llu", length * 8, padding_bit_length);
    printf("\npadded-bit-length = %llu = %d*512", length * 8 + padding_bit_length + 64, (length * 8 + padding_bit_length + 64) / 512);
    return (length * 8 + padding_bit_length + 64) / 512;
}

void sin_table_generation()
{
    for (int i = 1; i <= 64; i++)
    {
        uint32_t temp = (unsigned int)(fabs(sin(i) * 4294967296UL));
        sin_table[i] = temp;
        // printf("0x%x\n", temp);
    }
}

void MD5_process(uint32_t *msg)
{
    for (int r = 0; r < 4; r++)
    {
        for (int i = 0; i < 16; i++)
        {
            uint32_t a = A;
            uint32_t b = B;
            uint32_t c = C;
            uint32_t d = D;
            uint32_t temp = a + F(b, c, d, r) + msg[i] + sin_table[i];
            A = d;
            B = b + (temp << shift[r][i % 4] | temp >> (32 - shift[r][i % 4]));
            C = b;
            D = c;
        }
    }
}

uint32_t F(uint32_t x, uint32_t y, uint32_t z, int round)
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
        printf("Error: invalid round number\n");
        return 0;
        break;
    }
}

void hash_output()
{
    uint8_t hash[16] = {};
    for (int i = 0; i < 4; i++)
    {
        hash[i] = (A >> (i * 8)) & 0xff;
        hash[i + 4] = (B >> (i * 8)) & 0xff;
        hash[i + 8] = (C >> (i * 8)) & 0xff;
        hash[i + 12] = (D >> (i * 8)) & 0xff;
    }
    printf("\nMD5-hash: ");
    for (int i = 0; i < 16; i++)
    {
        printf("%02x", hash[i]);
    }
}