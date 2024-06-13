#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <algorithm>
using namespace std;
#define RANGE 30

int64_t a, b, p; // parameters of the elliptic curve on Galois field: y^2=x^3+ax+b mod p
typedef struct
{
    int64_t x;
    int64_t y;
} point;
void init(int64_t x, int64_t y, int64_t z);     // initialize the elliptic curve, generate ecode_map/decode_map for plaintext
bool verify(int64_t x, int64_t y);              // verify whether point (x,y) is on the elliptic curve
point elliptic_curve_inverse(point P);          // return -P
point elliptic_curve_add(point P, point Q);     // return P+Q
point elliptic_curve_mult(int64_t n, point P);  // return nP
int64_t gcd(int64_t x, int64_t y);              // return the greatest common factor of x and y
int64_t inverse(int64_t a, int64_t p);          // return a^-1 mod p
int64_t power(int64_t a, int64_t n, int64_t p); // return a^n mod p

int64_t encode_map[RANGE] = {13, 16, 19, 11, 14, 12, 18, 15, 17, 3, 6, 9, 1, 4, 2, 8, 5, 7, 23, 26, 29, 21, 24, 22, 28, 25, 27, 0, 10, 20};
int64_t decode_map[RANGE] = {};
point encode[RANGE] = {};
int64_t decode[RANGE][RANGE] = {};

typedef struct
{
    point c1;
    point c2;
} cipher;
point G;                                            // generator of Galois field
point P;                                            // public key
int64_t n;                                          // private key
void key_gen(point generator, int64_t private_key); // generate public-key P=nG from private_key n
cipher ecc_encrypt(int64_t plaintext, point public_key);
int64_t ecc_decrypt(cipher ciphertext, int64_t private_key);

int main()
{
    srand(time(0));
    init(20, 4, 29);
    point generator = {1, 5};
    int64_t private_key = rand() % (RANGE - 2) + 2; // the private-key n cannot be 0 or 1
    key_gen(generator, private_key);
    point public_key = P;

    for (int i = 0; i < RANGE; i++)
    {
        point temp = elliptic_curve_mult(i, generator);
        encode[decode_map[i]] = temp;
        decode[temp.x][temp.y] = decode_map[i];
        printf("%dG=(%lld,%lld)\n", i, temp.x, temp.y);
    }
    printf("\npublic-key: G=(%lld,%lld), P=nG=(%lld,%lld)", G.x, G.y, P.x, P.y);
    printf("\nprivate-key: n=%lld", n);

    int t = 9;
    while (t--)
    {
        printf("\nPlease input the message(0~29):\n");
        int64_t msg;
        scanf("%lld", &msg);
        cipher ciphertext = ecc_encrypt(msg, public_key);
        int64_t message = ecc_decrypt(ciphertext, private_key);
        printf("ciphertext={c1,c2}={(%lld,%lld),(%lld,%lld)}\n", ciphertext.c1.x, ciphertext.c1.y, ciphertext.c2.x, ciphertext.c2.y);
        printf("message=%lld\n", message);
    }
}

void init(int64_t x, int64_t y, int64_t z)
{
    a = x % z;
    b = y % z;
    p = z;
    // std::random_shuffle(std::begin(encode), std::end(encode));
    for (int i = 0; i < RANGE; i++)
    {
        decode_map[encode_map[i]] = i;
    }
}

bool verify(int64_t x, int64_t y)
{
    if (((y * y) - (x * x * x + a * x + b)) % p == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

point elliptic_curve_inverse(point P)
{
    int64_t x = P.x, y = P.y;
    return {x, (p - y) % p};
}

point elliptic_curve_add(point P, point Q)
{
    int64_t x1 = P.x, y1 = P.y, x2 = Q.x, y2 = Q.y;
    int64_t numerator, denominator;
    bool negative = false;
    int64_t lamda;
    point R;
    if (x1 == x2 && (y1 + y2) % p == 0) // P+Q=O, return the point at infinity
    {
        R.x = R.y = 0;
        return R;
    }
    if (x1 == 0 && y1 == 0) // P is the point at infinity, return Q
    {
        return Q;
    }
    if (x2 == 0 && y2 == 0) // Q is the point at infinity, return P
    {
        return P;
    }
    if (x1 == x2 && y1 == y2) // P=Q
    {
        numerator = 3 * (x1 * x1) + a;
        denominator = 2 * y1;
    }
    else // P≠Q
    {
        numerator = y2 - y1;
        denominator = x2 - x1;
        if (numerator * denominator < 0)
        {
            negative = true;
        }
        numerator = abs(numerator);
        denominator = abs(denominator);
    }
    int64_t temp = gcd(numerator, denominator);
    numerator /= temp;
    denominator /= temp;
    lamda = numerator * inverse(denominator, p);
    if (negative)
    {
        lamda = (-lamda) + (lamda / p + 1) * p;
    }
    R.x = (lamda * lamda - x1 - x2) % p;
    if (R.x < 0)
    {
        R.x += ((-R.x) / p + 1) * p;
    }
    R.y = (lamda * (x1 - R.x) - y1) % p;
    if (R.y < 0)
    {
        R.y += ((-R.y) / p + 1) * p;
    }
    return R;
}

point elliptic_curve_mult(int64_t n, point P)
{
    if (n == 0)
    {
        return {0, 0};
    }
    point nP = {P.x, P.y};
    n--;
    while (n--)
    {
        nP = elliptic_curve_add(nP, P);
    }
    return nP;
}

int64_t gcd(int64_t x, int64_t y)
{
    while (y > 0)
    {
        int temp = x % y;
        x = y;
        y = temp;
    }
    return x;
}

int64_t inverse(int64_t a, int64_t p)
{
    a = a % p;
    if (a == 1 || a == p - 1)
    {
        return a;
    }
    if (a == 0)
    {
        printf("No inverse element of Zero!\n");
        return 0;
    }
    for (int i = 2; i <= p - 2; i++)
    {
        if ((a * i) % p == 1)
        {
            return i;
        }
    }
    printf("Inverse operation error!\n");
    return 0;
}

int64_t power(int64_t a, int64_t n, int64_t p)
{
    a = a % p;
    int64_t power = 1;
    while (n--)
    {
        power *= a;
        power %= p;
    }
    return power;
}

void key_gen(point generator, int64_t private_key)
{
    G = generator;
    n = private_key;
    P = elliptic_curve_mult(n, G);
}

cipher ecc_encrypt(int64_t message, point public_key)
{
    int random = rand() % (RANGE - 2) + 2;
    point plaintext = encode[message];
    point c1 = elliptic_curve_mult(random, G);
    point c2 = elliptic_curve_add(plaintext, elliptic_curve_mult(random, public_key));
    cipher ciphertext = {c1, c2};
    return ciphertext;
}

int64_t ecc_decrypt(cipher ciphertext, int64_t private_key)
{
    point c1 = ciphertext.c1;
    point c2 = ciphertext.c2;
    point plaintext = elliptic_curve_add(c2, elliptic_curve_inverse(elliptic_curve_mult(private_key, c1)));
    int64_t message = decode[plaintext.x][plaintext.y];
    return message;
}