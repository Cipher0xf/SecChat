#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <iostream>
using namespace std;
#include "util.hpp"
#include "DSA.hpp"

void printParams(DSA &dsa);

int main()
{
    __int128_t msg, signature;
    bool verify_result = false;
    msg = (__uint128_t)0x0 << 64 | 0x0721145141919810;
    printf("message: "), print_int128(msg);

    DSA dsa;
    dsa.keyGen();
    signature = dsa.sign(msg, dsa.priv_key);
    verify_result = dsa.verify(msg, signature, dsa.pub_key);
    printf("signature: "), print_int128(signature);
    printf("verify-result: %s\n", verify_result == true ? "true" : "false");

    system("pause");
    return 0;
}

void printParams(DSA &dsa)
{
    printf("p="), print_int128(dsa.p);
    printf("q="), print_int128(dsa.q);
    printf("n="), print_int128(dsa.n);
    printf("phi(n)="), print_int128(dsa.phi_n);
    printf("e="), print_int128(dsa.e);
    printf("d="), print_int128(dsa.d);
    // printf("public-key: "), print_uint128(dsa.pub_key);
    // printf("private-key: "), print_uint128(dsa.priv_key);
}