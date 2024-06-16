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

bool isPrime(uint64_t x)
{
    if (x == 0 || x == 1 || x % 2 == 0)
        return false;
    for (int i = 3; i * i <= x; i += 2)
        if (x % i == 0)
            return false;
    return true;
}

uint64_t primeNumGen()
{
    bitset<64> num(1); // the least significant bit of prime number must be 1
    do
    {
        for (int i = 1; i < 16; i++) // to be optimized
        {
            num[i] = rand() % 2;
        }
    } while (!isPrime(num.to_ullong()));
    return num.to_ullong();
}

uint64_t gcd(uint64_t x, uint64_t y)
{
    uint64_t a = x;
    uint64_t b = y;
    while (b > 0)
    {
        uint64_t t = a;
        a = b;
        b = t % b;
    }
    return a;
}

uint64_t inverse(uint64_t a, uint64_t mod)
{
    a = a % mod;
    if (a == 1 || a == mod - 1)
        return a;
    if (a == 0 || gcd(a, mod) != 1)
    {
        printf("ERROR(utility.cpp inverse): inversion inexists\n");
        return 0;
    }
    for (uint64_t b = 3; b < mod - 1; b++) // to be optimized
    {
        if ((a * b) % mod == 1)
            return b;
    }
    printf("ERROR(utility.cpp inverse): inversion failed\n");
    return 0;
}

uint64_t power(uint64_t base, uint64_t exp, uint64_t mod)
{
    if(mod==0)
    {
        printf("ERROR(utility.cpp power): modulus cannot be zero\n");
        return 0;
    }
    base %= mod;
    if (base == 0 || base == 1)
        return base;
    if(exp == 0)
        return 1;
    uint64_t power = 1;
    while (exp > 0) // fast powering algorithm
    {
        if (exp % 2 == 1)
            power = (power * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return power;
}