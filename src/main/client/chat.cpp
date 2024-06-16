#include <cstdlib>
#include <cstring>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <cstdio>
#include <iostream>
using namespace std;

typedef struct
{
    uint64_t seq;
    uint64_t time;
    uint8_t src_id;
    uint8_t dst_id;
    char msg[];
} chatRecord;

int main()
{
    
    system("pause");
    return 0;
}