#ifndef RSA_HPP
#define RSA_HPP
#include <cstdint>
using namespace std;

class RSA
{
public:
    __int128_t p, q, n, phi_n, e, d;
    __int128_t pub_key, priv_key;
    void keyGen();
    __int128_t encrypt(__int128_t msg, __int128_t pub_key);
    __int128_t decrypt(__int128_t cipher, __int128_t priv_key);
    char *pk2str();
    __int128_t str2pk(char *pk_str);
};

#endif