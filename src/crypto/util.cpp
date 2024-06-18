#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <bitset>
#include <vector>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <cstdio>
#include <iostream>
using namespace std;
#include "util.hpp"

void print_int128(__int128_t x)
{
    printf("0x%016llx%016llx\n", (uint64_t)(x >> 64), (uint64_t)(x & 0xFFFFFFFFFFFFFFFF));
}

bool isPrime(__int128_t n) // to be optimized
{
    if (n < 2)
        return false;
    if (n == 2)
        return true;
    if (n % 2 == 0 || n % 3 == 0 || n % 5 == 0 || n % 7 == 0 || n % 11 == 0 || n % 13 == 0 || n % 17 == 0 || n % 19 == 0 || n % 23 == 0 || n % 29 == 0 || n % 31 == 0)
        return false;
    __int128_t d = n - 1;
    int r = 0;
    while ((d & 1) == 0)
    {
        d >>= 1;
        r++;
    }
    __int128_t test_base[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31};
    for (__int128_t a : test_base)
    {
        if (a > n - 2)
            continue;
        __int128_t x = power(a, d, n);
        if (x == 1 || x == n - 1)
            continue;
        bool composite = true;
        for (int i = 0; i < r - 1; i++)
        {
            x = power(x, 2, n);
            if (x == n - 1)
            {
                composite = false;
                break;
            }
        }
        if (composite)
            return false;
    }
    return true;
}

__int128_t primeGen() // to be optimized
{
    __int128_t num;
    do
    {
        num = (__int128_t)1 << 31 | 1; // the least significant bit of prime number must be 1
        uint64_t high =
            0;
        // rand() << 48 |
        // rand() << 32 |
        // rand() << 16 |
        // rand();
        uint64_t low =
            // rand() << 48 |
            // rand() << 32 |
            rand() << 16 |
            rand();
        num |=
            (__int128_t)high << 64 |
            (__int128_t)low;
        // printf("primeGen-test:"),print_int128(num);
    } while (!isPrime(num));
    return num;
}

__int128_t gcd(__int128_t x, __int128_t y)
{
    __int128_t a = x;
    __int128_t b = y;
    while (b > 0)
    {
        __int128_t t = a;
        a = b;
        b = t % b;
    }
    return a;
}

__int128_t ex_gcd(__int128_t a, __int128_t b, __int128_t &x, __int128_t &y)
{
    if (b == 0)
    {
        x = 1, y = 0;
        return a;
    }
    __int128_t r = ex_gcd(b, a % b, y, x);
    y -= (a / b) * x;
    return r;
}

__int128_t inverse(__int128_t a, __int128_t mod) // to be optimized
{

    // printf("\ninverse(a, mod)\n");
    // printf("a="), print_int128(a);
    // printf("mod="), print_int128(mod);
    a %= mod;
    if (a == 1 || a == mod - 1)
        return a;
    if (a == 0 || gcd(a, mod) != 1)
    {
        printf("ERROR(util.cpp inverse): inversion inexists\n");
        return 0;
    }

    __int128_t inv = 0;
    // inv = power(a, phi(mod) - 1, mod); // function phi() to be optimized 
    __int128_t x, y;
    __int128_t b = mod;
    __int128_t d = ex_gcd(a, b, x, y);
    if (x < 0)
        x += b;
    inv = x % mod;

    // printf("a^-1="), print_int128(inv);
    // printf("a*a^-1="), print_int128((a * inv) % mod);
    if ((a * inv) % mod != 1)
        printf("ERROR(util.cpp inverse): extended Euclid algorithm failed\n");
    // printf("\n");
    return inv;
}

__int128_t phi(__int128_t n) // to be optimized
{
    if (isPrime(n))
        return n - 1;
    __int128_t res = n;
    __int128_t n0 = n;
    for (__int128_t i = 2; i * i <= n0; i++)
    {
        if (n % i == 0)
        {
            while (n % i == 0)
                n /= i;
            res -= res / i;
        }
    }
    if (n > 1)
        res -= res / n;
    return res;
}

__int128_t power(__int128_t base, __int128_t exp, __int128_t mod)
{
    if (mod == 0)
    {
        printf("ERROR(util.cpp power): modulus cannot be zero\n");
        return 0;
    }
    base %= mod;
    if (base == 0 || base == 1)
        return base;
    if (exp == 0)
        return 1;
    __int128_t power = 1;
    while (exp > 0) // fast powering algorithm
    {
        if (exp % 2 == 1)
            power = (power * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return power;
}