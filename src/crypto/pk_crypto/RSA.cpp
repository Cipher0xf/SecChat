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
#include "RSA.hpp"

void RSA::keyGen()
{
    srand(time(NULL));
    do // p≠q
    {
        p = primeNumGen();
        q = primeNumGen();
    } while (p == q);
    n = p * q;
    phi_n = (p - 1) * (q - 1);
    do // gcd(e,φ(n))=1 AND e≠1 AND e≠φ(n)-1 ( i.e. public-key(e) ≠ private-key(d) )
    {
        e = rand() % (phi_n - 2) + 1;
    } while (gcd(e, phi_n) != 1 || (e * e) % phi_n == 1);
    d = inverse(e, phi_n);
    pub_key = e;
    priv_key = d;
}

uint64_t RSA::encrypt(uint64_t msg, uint64_t pub_key)
{
    return power(msg, pub_key, n);
}

uint64_t RSA::decrypt(uint64_t cipher, uint64_t priv_key)
{
    return power(cipher, priv_key, n);
}
