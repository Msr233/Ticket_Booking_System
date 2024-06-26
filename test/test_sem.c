#include<stdio.h>
//fork
#include <unistd.h>
#include <sys/types.h>
//wait
#include <sys/wait.h>
//exit
#include <stdlib.h>
//IPC&sem
#include <sys/ipc.h>
#include <sys/sem.h>
//share memery
#include <sys/shm.h>

// #include "con_segmap.h"
#include "./include/dictionary.h"
#include "./include/sem.h"

typedef unsigned short ushort;

int main(){
    int n = 2, m = 5;
    int len = m*n;
    key_t key_sem_seat = ftok("./utils.c", 6);
    int sem_seat = semget(key_sem_seat, n * m, 0666|IPC_CREAT|IPC_EXCL);//每个座位设置一个信号量
    //全部初始化为1
    ushort *p = (ushort*)malloc(sizeof(ushort) * n * m);
    union semun op;
    op.array = p;
    for (int i = 0; i < m * n; ++i) op.array[i] = 3;
    if (semctl(sem_seat, 0, SETALL, op) == -1) {
        perror("semctl SETALL");
        exit(1);
    }

    if (semctl(sem_seat, 0, GETALL, op) == -1) {
        perror("semctl GETALL");
        exit(1);
    }
    for (int i = 0; i < len; ++i) printf("Sem %d : %hu\n", i, p[i]);

    int sstart = 2, eend = 5;
    struct sembuf *ops = (struct sembuf*)malloc( (len) * sizeof(struct sembuf));
    for (int i = 0; i < len; ++i){
        ops[i].sem_num = i;
        if (i >= sstart && i <= eend){
            ops[i].sem_op = 1;
        }
        ops[i].sem_op = 0;
        ops[i].sem_flg = 0;
    }
    if (semop(sem_seat, ops, len) == -1) {
        perror("semop");
        exit(1);
    }

    printf("--------------------------------\n");
    if (semctl(sem_seat, 0, GETALL, op) == -1) {
        perror("semctl GETALL");
        exit(1);
    }
    for (int i = 0; i < len; ++i) printf("Sem %d : %hu\n", i, p[i]);

    if (semctl(sem_seat, 0, IPC_RMID) == -1) {
            perror("semctl IPC_RMID");
            exit(1);
    }

}