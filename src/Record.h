#pragma once
#include "jsoncpp/json/json.h"
#include <cstring>
#include <string>
using namespace std;
// 忽略这个文件
class Record {
    private:
    char *reply;

    Json::Value operationList;
    int rank;

    public:
    Record(char *rep);

    void move(int fx, int fy, int tx, int ty);

    void attack(int fx, int fy, int tx, int ty);

    void build(int x, int y, int type, bool real);

    void hidechange(int x, int y);

    void finish(int &len);
};
