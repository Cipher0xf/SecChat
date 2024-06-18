#ifndef MD5_HPP
#define MD5_HPP
#include <cstdint>
using namespace std;

class MD5
{
public:
    uint32_t A, B, C, D;    // MD5 buffer
    uint32_t sin_table[64]; // table of sine values
    void init();
    void sinTableGen();
    uint64_t msgPadding(uint8_t *msg, uint64_t len);           // padding message with 1000...(2) and 64-bit length, return the number of 512-bit blocks
    void process(uint32_t *msg);                               // process a 512-bit block, update MD5 buffer
    uint32_t F(uint32_t x, uint32_t y, uint32_t z, int round); // unlinear function F
    uint8_t *hash(char *msg_str);                              // return the MD5 hash of message
    char *hash2str(uint8_t *hash);
    uint8_t *str2hash(char *hash_str);
    void printHash(uint8_t *hash);
};

#endif