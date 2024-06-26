#include "sem.h"


//信号量Wait函数
void Wait(int sid)
{
    struct sembuf op;
    op.sem_num=0;
    op.sem_op=-1;
    op.sem_flg=0;
    semop(sid,&op,1);
}

//信号量Signal函数
void Signal(int sid)
{
    struct sembuf op;
    op.sem_num=0;
    op.sem_op=1;
    op.sem_flg=0;
    semop(sid,&op,1);
}

// ------------unused---------
// 多个信号量同时操作
void Wait_r(int sid, int left, int right)
{
 	int len = right-left + 1;
    struct sembuf *op = (struct sembuf*)malloc( len * sizeof(struct sembuf));
    for (int i = 0; i < len; ++i){
        op[i].sem_num = left + i;
        op[i].sem_op = -1;
        op[i].sem_flg = 0;
    }
    semop(sid, op, 1);
}

void Signal_r(int sid, int left, int right)
{
 	int len = right-left + 1;
    struct sembuf *op = (struct sembuf*)malloc( len * sizeof(struct sembuf));
    for (int i = 0; i < len; ++i){
        op[i].sem_num = left + i;
        op[i].sem_op = 1;
        op[i].sem_flg = 0;
    }
    semop(sid, op, 1);
}