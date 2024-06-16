#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cstdio>
#include <iostream>
using namespace std;
#include "RSA.hpp"

int main()
{
    uint64_t msg, cipher;
    msg = 0x114514;

    RSA rsa;
    rsa.keyGen();
    cipher = rsa.encrypt(msg, rsa.pub_key);
    msg = rsa.decrypt(cipher, rsa.priv_key);

    printf("p=%llu, q=%llu, n=%llu, phi(n)=%llu, e=%llu, d=%llu\n", rsa.p, rsa.q, rsa.n, rsa.phi_n, rsa.e, rsa.d);
    printf("public-key: %#llx\nprivate-key: %#llx\n", rsa.pub_key, rsa.priv_key);
    printf("ciphertext: %#llx\n", cipher);
    printf("plaintext: %#llx\n", msg);

    system("pause");
    return 0;
}