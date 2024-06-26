#ifndef SEM_H
#define SEM_H

#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <stdlib.h>

//semun声明
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};



void Wait(int sid);

//信号量Signal函数
void Signal(int sid);

// 多个信号量同时操作
void Wait_r(int sid, int left, int right);

void Signal_r(int sid, int left, int right);

#endif