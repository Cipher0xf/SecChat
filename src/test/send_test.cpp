#include <cstdlib>
#include <cstring>
#include <chrono>
#include <Windows.h>
#include <cstdio>
#include <iostream>
#include <fstream>
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
    uint8_t src_id = 101;
    uint8_t dst_id = 102;
    uint64_t last_seq = 0;
    while (true)
    {
        FILE *fp;
        fp = fopen("chat_records.csv", "r");
        if (fp == NULL)
        {
            cout << "Error: chat records file not found" << endl;
            return 1;
        }
        chatRecord temp;
        char col_names[100];
        fscanf(fp, "%99[^\n]", col_names); // filter out the column names
        while (fscanf(fp, "%llu,%llu,%u,%u,%999[^,\n]", &(temp.seq), &(temp.time), &(temp.src_id), &(temp.dst_id), temp.msg) == 5)
        {
            last_seq = temp.seq;
        }
        fclose(fp);
        // cout << "last_seq: " << last_seq << endl;

        fp = fopen("chat_records.csv", "a");
        if (fp == NULL)
        {
            cout << "Error: chat records file not found" << endl;
            return 1;
        }
        char msg[1000] = {};
        memset(msg, 0, sizeof(msg));
        scanf("%s", msg);
        auto now = chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        uint64_t timestamp = chrono::duration_cast<chrono::milliseconds>(duration).count();
        fprintf(fp, "%llu,%llu,%u,%u,%s\n", last_seq + 1, timestamp, src_id, dst_id, msg);
        fclose(fp);
    }
}