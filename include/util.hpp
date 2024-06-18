#ifndef UTIL_HPP
#define UTIL_HPP
#include <cstdint>
using namespace std;

bool isPrime(__int128_t x);
void print_int128(__int128_t x);                                             // print a 128-bit unsigned integer with hex format
__int128_t primeGen();                                                       // generate a random large prime number
__int128_t gcd(__int128_t x, __int128_t y);                                  // the greatest common divisor of x and y
__int128_t ex_gcd(__int128_t a, __int128_t b, __int128_t &x, __int128_t &y); // the extended Euclidean algorithm
__int128_t inverse(__int128_t x, __int128_t mod);                            // the inversion of x modulo mod
__int128_t phi(__int128_t n);                                                // the Euler's function of n
__int128_t power(__int128_t base, __int128_t exp, __int128_t mod);           // base^exp modulo mod

#endif