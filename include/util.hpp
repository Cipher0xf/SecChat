#ifndef UTIL_HPP
#define UTIL_HPP
#include <cstdint>
using namespace std;

bool isPrime(uint64_t x);
uint64_t primeNumGen();                                    // generate a random large prime number
uint64_t gcd(uint64_t x, uint64_t y);                      // the greatest common divisor of x and y
uint64_t inverse(uint64_t x, uint64_t mod);                // the inversion of x modulo mod
uint64_t power(uint64_t base, uint64_t exp, uint64_t mod); // base^exp modulo mod

#endif