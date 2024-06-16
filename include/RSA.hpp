#ifndef RSA_HPP
#define RSA_HPP
#include <cstdint>
using namespace std;

class RSA
{
public:
    uint64_t p, q, n, phi_n, e, d;
    uint64_t pub_key, priv_key;
    void keyGen();
    uint64_t encrypt(uint64_t msg, uint64_t pub_key);
    uint64_t decrypt(uint64_t cipher, uint64_t priv_key);
};

#endif