#include <cstdlib>
#include <cstring>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <cstdio>
#include <iostream>
using namespace std;
#include "util.hpp"
#include "AES.hpp"
#define MAX_LENGTH 1000

typedef struct
{
    uint64_t seq;
    uint64_t timestamp;
    char src_name[20];
    char dst_name[20];
    char content[MAX_LENGTH];
} chatRecord;

int main()
{
    /* read session-key */
    AES aes;
    char aes_key_str[17] = "";
    FILE *fp = fopen("../src/main/client/session_key.txt", "r");
    if (fp == NULL)
        printf("ERROR(file): session_key.txt not found\n");
    fscanf(fp, "%s\n", aes_key_str);
    printf("LOG(file): read AES_key %s\n", aes_key_str);
    aes.str2key(aes_key_str);
    aes.keyGen();
    fclose(fp);

    /* read chat-address */
    char chat_address[100] = "";
    uint64_t current_seq = 0;
    FILE *fp2 = fopen("../src/main/client/chat_address.txt", "r");
    if (fp2 == NULL)
        printf("ERROR(file): chat_address.txt not found\n");
    fscanf(fp2, "%s\n", chat_address);
    printf("LOG(file): read chat_address %s\n", chat_address);
    fclose(fp2);

    /* read chat-record */
    FILE *fp3;
    fp3 = fopen(chat_address, "r");
    if (fp3 == NULL)
        printf("ERROR(file): chat_record file not found\n");
    chatRecord temp;
    // char col_names[100];
    // fscanf(fp3, "%99[^\n]", col_names); // filter out column names
    while (fscanf(fp3, "%llu,%llu,%[^,],%[^,],%[^\n]\n", &(temp.seq), &(temp.timestamp), temp.src_name, temp.dst_name, temp.content) != EOF)
    {
        current_seq = temp.seq;
        char time_str[50] = "";
        time_t t = temp.timestamp;
        struct tm *tm_info = localtime(&t);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
        printf("\n%s\n%s:\n%s\n", time_str, temp.src_name, temp.content);
    }
    fclose(fp3);

    /* refresh chat-record */
    while (true)
    {
        Sleep(3000);
        fp3 = fopen(chat_address, "r");
        // fscanf(fp3, "%99[^\n]", col_names); // filter out column names
        while (fscanf(fp3, "%llu,%llu,%[^,],%[^,],%[^\n]\n", &(temp.seq), &(temp.timestamp), temp.src_name, temp.dst_name, temp.content) != EOF)
        {
            if (current_seq < temp.seq)
            {
                char time_str[50] = "";
                time_t t = temp.timestamp;
                struct tm *tm_info = localtime(&t);
                strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
                printf("\n%s\n%s:\n%s\n", time_str, temp.src_name, temp.content);
            }
        }
    }

    system("pause");
    return 0;
}