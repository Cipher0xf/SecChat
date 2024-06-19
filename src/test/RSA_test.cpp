#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <iostream>
using namespace std;
#include "util.hpp"
#include "RSA.hpp"

void printParams(RSA &rsa);

int main()
{
    __int128_t msg, cipher;
    msg = (__uint128_t)0x0 << 64 | 0x0721145141919810;
    printf("message: "), print_int128(msg);

    RSA rsa;
    rsa.keyGen();
    cipher = rsa.encrypt(msg, rsa.pub_key);
    msg = rsa.decrypt(cipher, rsa.priv_key);
    printParams(rsa);
    printf("ciphertext: "), print_int128(cipher);
    printf("plaintext: "), print_int128(msg);

    char *pk_str=rsa.pk2str();
    printf("public-key: %s\n", pk_str);
    rsa.str2pk(pk_str);
    printParams(rsa);

    system("pause");
    return 0;
}

void printParams(RSA &rsa)
{
    printf("p="), print_int128(rsa.p);
    printf("q="), print_int128(rsa.q);
    printf("n="), print_int128(rsa.n);
    printf("phi(n)="), print_int128(rsa.phi_n);
    printf("e="), print_int128(rsa.e);
    printf("d="), print_int128(rsa.d);
    // printf("public-key: "), print_uint128(rsa.pub_key);
    // printf("private-key: "), print_uint128(rsa.priv_key);
}