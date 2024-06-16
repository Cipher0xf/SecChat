#ifndef AES_HPP
#define AES_HPP
#include <cstdint>
using namespace std;
#define ROUND 10

class AES
{
public:
    uint8_t key[16] = {};                            //  initial key
    uint8_t subKeys[ROUND + 1][16] = {};             // 128-bit subkeys for 10 rounds
    void keyGen(uint8_t *key);                       // generate subkeys
    uint64_t msgPadding(uint8_t *msg, uint64_t len); // padding message, return the number of 128-bit blocks
    void addRoundKey(uint8_t *block, uint8_t *key);
    void subByte(uint8_t *block);
    void re_subByte(uint8_t *block);
    void shiftRows(uint8_t *block);
    void re_shiftRows(uint8_t *block);
    uint8_t GaloisMult(uint8_t L, uint8_t R); // multiplication in GF(2^7)
    void mixColumn(uint8_t *block);
    void re_mixColumn(uint8_t *block);
    void encrypt(uint8_t *block);
    void decrypt(uint8_t *block);
    void printByteMatrix(uint8_t *block); // print the 4x4 byte matrix
};

#endif