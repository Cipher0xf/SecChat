#ifndef DSA_HPP
#define DSA_HPP
#include <cstdint>
using namespace std;

class DSA
{
public:
    __int128_t p, q, n, phi_n, e, d;
    __int128_t pub_key, priv_key;
    void keyGen();
    __int128_t sign(__int128_t msg, __int128_t priv_key);
    bool verify(__int128_t msg, __int128_t cert, __int128_t pub_key);
    char *pk2str();
    __int128_t str2pk(char *pk_str);
    char* sign2str(__int128_t sign);
    __int128_t str2sign(char *sign_str);
};

#endif