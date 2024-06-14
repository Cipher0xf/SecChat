#include <iostream>
#include <cstring>
#include <cmath>
#include <ctime>
#include <chrono>
#include <bitset>
using namespace std;

int findMSD(const bitset<1024> &x)
{
    for (int i = 1023; i >= 0; --i)
    {
        if (x.test(i))
        {
            return i;
        }
    }
    return -1;
}

int bitsetCompare(const bitset<1024> &x, const bitset<1024> &y)
{
    int x_MSD = findMSD(x) == 1024 ? -1 : findMSD(x);
    int y_MSD = findMSD(y) == 1024 ? -1 : findMSD(y);
    int MSD = max(x_MSD, y_MSD);
    for (int i = MSD; i >= 0; i--)
    {
        if (x[i] ^ y[i]) // x≠y
        {
            return x[i] ? 1 : -1;
        }
    }
    return 0; // x=y
}

bitset<1024> bitsetAdd(const bitset<1024> &x, const bitset<1024> &y)
{
    bitset<1024> sum;
    bool C = false;
    int x_MSD = findMSD(x) == 1024 ? -1 : findMSD(x);
    int y_MSD = findMSD(y) == 1024 ? -1 : findMSD(y);
    int MSD = max(x_MSD, y_MSD);
    for (int i = 0; i <= min(1024, MSD + 1); i++)
    {
        sum[i] = x[i] ^ y[i] ^ C;
        C = (x[i] && y[i]) || (C && (x[i] ^ y[i]));
    }
    return sum;
}

bitset<1024> bitsetMinus(const bitset<1024> &x, const bitset<1024> &y)
{
    bitset<1024> diff(0);
    bool B = false;
    int x_MSD = findMSD(x) == 1024 ? -1 : findMSD(x);
    for (int i = 0; i <= x_MSD; i++)
    {
        diff[i] = x[i] ^ y[i] ^ B;
        B = (!x[i] && y[i]) || (B && !(x[i] ^ y[i]));
    }
    return diff;
}

bitset<1024> bitsetMultiply(const bitset<1024> &x, const bitset<1024> &y)
{
    bitset<1024> product(0);
    int x_MSD = findMSD(x) == 1024 ? -1 : findMSD(x);
    int y_MSD = findMSD(y) == 1024 ? -1 : findMSD(y);
    for (int i = 0; i <= y_MSD; i++)
    {
        if (y[i])
        {
            product = bitsetAdd(product, x << i);
        }
    }

    return product;
}

bitset<1024> bitsetDivide(const bitset<1024> &x, const bitset<1024> &y)
{
    bitset<1024> quotient(0);
    bitset<1024> remainder = x;
    int x_MSD = findMSD(x) == 1024 ? -1 : findMSD(x);
    int y_MSD = findMSD(y) == 1024 ? -1 : findMSD(y);
    if (x_MSD < y_MSD)
    {
        return 0;
    }
    bitset<1024> divisor = y << (x_MSD - y_MSD);
    for (int i = x_MSD - y_MSD; i >= 0; i--)
    {
        if (bitsetCompare(remainder, divisor) >= 0)
        {
            remainder = bitsetMinus(remainder, divisor);
            quotient.set(i);
        }
        divisor >>= 1;
    }
    return quotient;
}

bitset<1024> bitsetMod(const bitset<1024> &x, const bitset<1024> &y)
{
    bitset<1024> result = x;
    int x_MSD = findMSD(x) == 1024 ? -1 : findMSD(x);
    int y_MSD = findMSD(y) == 1024 ? -1 : findMSD(y);
    if (x_MSD < y_MSD)
    {
        return result;
    }
    bitset<1024> temp = y << (x_MSD - y_MSD);

    while (bitsetCompare(temp, y) >= 0)
    {
        if (bitsetCompare(result, temp) >= 0)
        {
            result = bitsetMinus(result, temp);
        }
        temp >>= 1;
    }
    return result;
}

bitset<1024> bitsetPower(const bitset<1024> &a, const bitset<1024> &b, const bitset<1024> &n) // using Fast Power Algorithm
{
    bitset<1024> power(1);
    bitset<1024> base = bitsetMod(a, n);
    bitset<1024> exponent = b;
    while (exponent.count() > 0)
    {
        if (exponent[0])
        {
            power = bitsetMod(bitsetMultiply(power, base), n);
        }
        base = bitsetMod(bitsetMultiply(base, base), n);
        exponent >>= 1;
    }
    return bitsetMod(power, n);
    // return power;
}

bitset<1024> bitset_inverse(const bitset<1024> &A, const bitset<1024> &N) // using Extended Euclidean Algorithm
{
    if (bitsetCompare(A, 1) == 0 || bitsetCompare(bitsetAdd(A, 1), N) == 0)
    {
        return A;
    }
    int64_t a = A.to_ullong();
    int64_t n = N.to_ullong();
    int64_t x = 0, y = 1, u = 1, v = 0, gcd = n, m, q, t;
    while (a != 0)
    {
        q = gcd / a;
        t = gcd % a;
        m = x - u * q;
        n = y - v * q;
        gcd = a;
        a = t;
        x = u;
        y = v;
        u = m;
        v = n;
    }
    if (x < 0)
        x += N.to_ullong();
    if (gcd == 1)
    {
        return x % N.to_ullong();
    }
    else
    {
        return 0;
    }
}

bool isPrime(const bitset<1024> &num)
{
    if (num == 0)
    {
        // cout << "\n 0 is not prime ";
        return false;
    }

    if (num == 1)
    {
        // cout << "\n 1 is not prime ";
        return false;
    }

    if (num == 2)
    {
        // cout << "\n 2 is prime ";
        return true;
    }

    if (!num[0])
    {
        // cout << "\n even number is not prime ";
        return false;
    }
    bitset<1024> temp(3);
    auto start_time = std::chrono::steady_clock::now();
    while (bitsetCompare(bitsetMultiply(temp, temp), num) <= 0)
    {
        if (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start_time).count() > 3)
        {
            break;
        }
        if (bitsetMod(num, temp) == 0)
        {
            // cout << "\n" << num.to_ullong() << " is divisible by " << temp.to_ullong() << " ";
            return false;
        }
        temp = bitsetAdd(temp, 2);
    }
    return true;
}

bitset<1024> gen_prime_number()
{
    bitset<1024> result(1);
    while (!isPrime(result))
    {
        for (int i = 1; i < 32; i++)
        {
            result[i] = rand() % 2;
        }
    }
    return result;
}

bitset<1024> bitset_gcd(const bitset<1024> &x, const bitset<1024> &y)
{
    bitset<1024> a = x;
    bitset<1024> b = y;
    while (b.count() > 0)
    {
        bitset<1024> temp = a;
        a = b;
        b = bitsetMod(temp, b);
    }
    return a;
}

bitset<1024> p, q, n, phi_n, e, d;

void keyGen()
{
    srand(time(0));
    cout << "key-generation: \n";
    p = gen_prime_number();
    cout << "p = " << p.to_ullong() << endl;
    q = gen_prime_number();
    cout << "q = " << q.to_ullong() << endl;
    // p = 3, q = 11;
    n = bitsetMultiply(p, q);
    cout << "n = p * q = " << n.to_ullong() << endl;
    phi_n = bitsetMultiply(bitsetMinus(p, 1), bitsetMinus(q, 1));
    cout << "phi(n) = " << phi_n.to_ullong() << endl;
    do
    {
        e = bitset<1024>(rand() % (phi_n.to_ullong() - 2) + 2);
    } while (bitset_gcd(e, phi_n).to_ullong() != 1);
    cout << "e = " << e.to_ullong() << endl;
    d = bitset_inverse(e, phi_n);
    cout << "d = " << d.to_ullong() << endl;

    cout << "public-key: " << e.to_ullong() << endl;
    cout << "private-key: " << d.to_ullong() << endl;
}

bitset<1024> encrypt(const bitset<1024> &msg, const bitset<1024> &pk)
{
    bitset<1024> cipher = bitsetPower(msg, pk, n);
    return cipher;
}

bitset<1024> decrypt(const bitset<1024> &cipher, const bitset<1024> &sk)
{
    bitset<1024> plain = bitsetPower(cipher, sk, n);
    return plain;
}

int main()
{
    keyGen();
    char message[] = "math";
    // char ciphertext[] = "\0";
    // char plaintext[] = "\0";
    bitset<1024> msg;
    bitset<1024> cipher;
    bitset<1024> plain;
    for (int i = 0; i < strlen(message); i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (message[i] & (1 << j))
            {
                msg.set(i * 8 + j);
            }
        }
    }
    // msg = 2911;
    cout << "\nmessage: " << message << endl;
    cout << "message(bit-string):\n"
         << msg << endl;

    cipher = encrypt(msg, e);
    plain = decrypt(cipher, d);
    cout << "\nciphertext(bit-string):\n"
         << cipher << endl;
    cout << "\nplaintext(bit-string):\n"
         << plain << endl;
    system("pause");
    return 0;
}