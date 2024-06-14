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
    uint64_t current_seq = 0;
    FILE *fp;
    fp = fopen("chat_records.csv", "r");
    if (fp == NULL)
    {
        cout << "Error: chat records file not found" << endl;
        return 1;
    }
    // vector<chatRecord> records;
    chatRecord temp;
    char col_names[100];
    fscanf(fp, "%99[^\n]", col_names); // filter out the column names
    while (fscanf(fp, "%llu,%llu,%u,%u,%s\n", &(temp.seq), &(temp.time), &(temp.src_id), &(temp.dst_id), temp.msg) == 5)
    {
        current_seq = temp.seq;
        printf("\ntime-%llu\nuser-%u:%s\n", temp.time, temp.src_id, temp.msg);
        // records.push_back(temp);
    }
    fclose(fp);

    while (true)
    {
        // records.clear();
        // cout << "\nrefreshing..." << endl;
        Sleep(2000);
        fp = fopen("chat_records.csv", "r");
        fscanf(fp, "%99[^\n]", col_names); // filter out the column names
        if (fp == NULL)
        {
            cout << "Error: chat records file not found" << endl;
            return 1;
        }
        while (fscanf(fp, "%llu,%llu,%u,%u,%s\n", &(temp.seq), &(temp.time), &(temp.src_id), &(temp.dst_id), temp.msg) == 5)
        {
            if (current_seq < temp.seq)
            {
                printf("\ntime-%llu\nuser-%u:%s\n", temp.time, temp.src_id, temp.msg);
            }
        }
        current_seq = temp.seq;
        // cout << "\nrefreshed" << endl;
        fclose(fp);
    }
}
