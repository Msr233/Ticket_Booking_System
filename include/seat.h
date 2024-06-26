#ifndef SEAT_H
#define SEAT_H

#include <time.h>

enum State{EMPTY, RESERVED, SOLDED};

typedef struct seat{
    time_t opt_time;// 进行最后一个操作的时间
    enum State st; // 0:empty, 1:reserve, 2:solded;
    char name[32];// 客户名字
}Seat;

#endif