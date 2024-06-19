#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <bitset>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <iostream>
using namespace std;
#include "util.hpp"
#include "DSA.hpp"

void DSA::keyGen()
{
    srand(time(NULL));
    do // p,q are distinct primes, and n is large enough
    {
        p = primeGen();
        q = primeGen();
        n = p * q;
    } while (gcd(p, q) != 1 || n >> 63 != 1);
    phi_n = (p - 1) * (q - 1);

    do // gcd(e,φ(n))=1 and e^2≠1(mod φ(n)) ( i.e.public-key(e)≠private-key(d) )
    {
        uint64_t high = 0;
        // rand() << 48 |
        // rand() << 32 |
        // rand() << 16 |
        // rand();
        uint64_t low =
            // rand() << 48 |
            // rand() << 32 |
            rand() << 16 |
            rand();
        e =
            (__int128_t)high << 64 |
            (__int128_t)low;
        e %= phi_n;
    } while (gcd(e, phi_n) != 1 || (e * e) % phi_n == 1);
    d = inverse(e, phi_n);

    pub_key = e;
    priv_key = d;
}

__int128_t DSA::sign(__int128_t msg, __int128_t priv_key)
{
    return power(msg, priv_key, n);
}

bool DSA::verify(__int128_t msg, __int128_t cert, __int128_t pub_key)
{
    __int128_t temp = power(cert, pub_key, n);
    // printf("temp: "), print_int128(temp);
    if (msg == temp)
        return true;
    else
        return false;
}

char *DSA::pk2str()
{
    char *pk_str = (char *)malloc(35 * sizeof(char));
    sprintf(pk_str, "E%016llxN%016llx", (uint64_t)e, (uint64_t)n);
    pk_str[34] = '\0';
    return pk_str;
}

__int128_t DSA::str2pk(char *pk_str)
{
    uint64_t param1, param2;
    sscanf(pk_str, "E%016llxN%016llx", &param1, &param2);
    e = (__int128_t)param1;
    n = (__int128_t)param2;
    return e;
}

char *DSA::sign2str(__int128_t sign)
{
    char *sign_str = (char *)malloc(17 * sizeof(char));
    sprintf(sign_str, "%016llx", (uint64_t)sign);
    sign_str[16] = '\0';
    return sign_str;
}