#include <cstdlib>
#include <cstdint>
#include <thread>
#include <vector>
#include <atomic>
#include <cstdio>
#include <iostream>
using namespace std;
#include "util.hpp"

int main()
{
    __int128_t num;
    // num = (__int128_t)0xabc<<64 | 0x1145141919810721;

    // int64_t test;
    // cin >> test;
    // cout<<isPrime(test)<<endl;


    for (int64_t i = 1; i < 9; i++)
    {
        num = (__int128_t)primeGen();
        printf("success:"),print_int128(num);
        // if (isPrime(num))
        //     cout << "True" << endl;
        // else
        //     cout << "False" << endl;
        // int64_t inv = (int64_t)inverse((__int128_t)i, (__int128_t)num);
    }

    // __int128_t phi_n = phi(num);
    // print_int128(num);
    // print_int128(phi_n);

    system("pause");
    return 0;
}