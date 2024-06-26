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

#include "./include/sem.c"
#include "./include/seat.h"
#include "./include/dictionary_shm.c"

#include"./include/data_loader.c"
#include "./include/opts.c"
#include "./include/utils.c"


typedef unsigned short ushort;

key_t get_key(const char *path){
    key_t key;
    for (int i = 1; i <= 128; ++i){
        key = ftok(path, i);
        if (key != -1){
            break;
        }
    }
    return key;
}

// 父进程循环等待子进程全部结束
pid_t wait_for_all_children() {
    int status;
    pid_t child_pid;
    int child_count = 0;

    while ((child_pid = wait(&status)) > 0) {
        child_count++;
        if (WIFEXITED(status)) {
            printf("-----------------------------------------\n");
            printf("Child process %d terminated with exit status: %d\n", child_pid, WEXITSTATUS(status));
            printf("-----------------------------------------\n");
        }
        // 可以添加其他状态检查，如WIFSIGNALED(status)等
    }
    printf("Total child processes terminated: %d\n", child_count);
    return child_count;
}


int main(int argc, char const *argv[]){
    char *path = "./txt/show.txt";
    //从txt中读取数据
    if (argc == 2){
        path = strdup(argv[1]);
        if (access(path, F_OK) == -1) {
            printf("%s\n", path);
            perror("File does not exist");
            return 0;
        }
    }

    data_loader data = load_data(path);
    

    //创建共享内存,并初始化
    //所有座位信息
    key_t key_seats = get_key("./include/seat.h");
    int size = sizeof(Seat) * (data.n * data.m);
    int shm_seat = shmget(key_seats, size, IPC_CREAT | 0666); //座位初始化
    if (shm_seat==-1){
        printf("shmget Failed!\n");
        exit(1);
    }
    Seat* seats = (Seat*)shmat(shm_seat, NULL, 0);
    for (int i = 0; i < data.n * data.m; ++i){
        seats->name[0] = '\0';
        seats->opt_time = -1;
        seats->st = EMPTY;
    }

    //创建用户操作记录的共享内存
    key_t key_dic = get_key("./include/dictionary.h");
    int shm_dic = shmget(key_dic, sizeof(Dictionary), IPC_CREAT | 0666); //初始化
    if (shm_dic==-1){
        printf("shmget Failed!\n");
        exit(1);
    }
    Dictionary* dic = (Dictionary*) shmat(shm_dic, NULL, 0);  
    initDictionary(dic);
    //创建信号量，每一个座位单独一个
    // key_t key_sem_seat = get_key("./utils.c");
    // int sem_seat = semget(key_sem_seat, data.n * data.m, 0666|IPC_CREAT|IPC_EXCL);//每个座位设置一个信号量
    // //全部初始化为1
    // ushort *p = (ushort*)malloc(sizeof(ushort) * data.n * data.m);
    // union semun ops;
    // ops.array = p;
    // for (int i = 0; i < data.m * data.n; ++i) ops.array[i] = 1;
    // if (semctl(sem_seat, 0, SETALL, ops) == -1) {
    //     perror("semctl SETALL");
    //     exit(1);
    // }

    //为了保证检查信号量操作是原子操作，设置信号量atomopt
    key_t key_sem_atom = get_key("./input.txt");
    int sem_atomopt = semget(key_sem_atom, data.n * data.m, 0666|IPC_CREAT|IPC_EXCL);
    if (sem_atomopt==-1){
        printf("semget Failed!\n");
        exit(1);
    }
    union semun op;
    op.val = 1;
    if (semctl(sem_atomopt, 0, SETVAL, op) == -1) {
        perror("semctl SETVAL\n");
        exit(1);
    }

    printf("Finish initialized Hall:(%d, %d)\nCreating processes....\n", data.n, data.m);
    //创建所有代理进程
    pid_t pid;
    int agent = 1;
    for (agent; agent <= data.k; ++agent){
        pid = fork();
        //child
        if(pid==0){
            break;
        }
    }
    //child
    if (pid == 0){ 
        printf("Process %d start working! Dealing with Agent %d\n", getpid(), agent);
        
        char **opts = data.all_opts[agent-1];
        int opt_num = data.opt_nums[agent-1];
        int *delay = data.all_times[agent-1];
        Seat* seats = (Seat*)shmat(shm_seat, NULL, 0); //子进程连接座位共享内存
        Dictionary* dict = (Dictionary*)shmat(shm_dic, NULL, 0);
        //遍历所有操作
        for (int i = 0; i < opt_num; ++i){
            char *opt = opts[i];
            char info_opt[9] = "\0", info_seat[8] = "\0", info_name[32] = "\0";
            char tip[32] = "\0";
            strcpy(tip, opt);
            if (split_opt(opt, info_opt, info_seat, info_name) == -1){
                continue;
            }
            // printf("test = %s %s %s\n", info_opt, info_name, info_seat);
            switch(opt[0]){
                case 'r':{ //reserve
                    Wait(sem_atomopt);
                    sleep(delay[0]);
                    int st = do_reserve(info_seat, info_name, data.n, data.m, seats, RESERVED, info_opt, data.rvt);
                    if (st == 0){
                        strcat(tip, " Successfully!");
                        dic_insert(dic,info_name, tip);
                    }
                    if (st == -1){
                        strcat(tip, " Failed!");
                        dic_insert(dic,info_name, tip);
                    }
                    Signal(sem_atomopt);
                    break;
                }
                case 't':{ //ticket
                    Wait(sem_atomopt);
                    sleep(delay[1]);
                    int st = do_reserve(info_seat, info_name, data.n, data.m, seats, SOLDED, info_opt, data.rvt);
                    if (st == 0){
                        strcat(tip, " Successfully!");
                        dic_insert(dic,info_name, tip);
                    }
                    if (st == -1){
                        strcat(tip, " Failed!");
                        dic_insert(dic,info_name, tip);
                    }
                    Signal(sem_atomopt);
                    break;
                }
                case 'c':{ //cancel
                    Wait(sem_atomopt);
                    sleep(delay[2]);
                    int st = do_reserve(info_seat, info_name, data.n, data.m, seats, EMPTY, info_opt, data.rvt);
                    if (st == 0){
                        strcat(tip, " Successfully!");
                        dic_insert(dic,info_name, tip);
                    }
                    if (st == -1){
                        strcat(tip, " Failed!");
                        dic_insert(dic,info_name, tip);
                    }
                    Signal(sem_atomopt);
                    break;
                }
                case 's':{ //show
                    Wait(sem_atomopt);
                    sleep(delay[3]);
                    // printf("%s\n", info_name);
                    int st = do_show(info_name, dic);
                    if (st == 0){
                        strcat(tip, " Successfully!");
                        dic_insert(dic,info_name, tip);
                    }
                    if (st == -1){
                        strcat(tip, " Failed!");
                        dic_insert(dic,info_name, tip);
                    }
                    Signal(sem_atomopt);
                    break;
                }
            }
        }
        if(shmdt(seats) == -1) { // 断开共享内存区的连接
            perror("shmdt");
            exit(1);
        }
        if(shmdt(dic) == -1) { // 断开共享内存区的连接
            perror("shmdt");
            exit(1);
        }
    }
    else{
        // 使用wait()等待子进程结束
        wait_for_all_children();
        //更新所有座位状态
        for (int i = 0; i < data.n * data.m; ++i){
            time_t t = time(NULL);
            if (seats[i].st == RESERVED && seats[i].opt_time + data.rvt < t){
                seats[i].st = EMPTY;
            }
        }
        //结束打印Hall所有座位状态
        printf("==============================================================================\n");
        printf("       EMPTY:   .       RESERVED:   @       SOLDED:     $       \n");
        printf("==============================================================================\n");
        for (int i = 0; i < data.n; ++i){
            printf("row:%d", i);
            for(int j = 0; j < data.m; ++j){
                int p = i*data.m + j;
                if (seats[p].st == EMPTY){
                    printf("\t.");
                }
                if (seats[p].st == RESERVED){
                    printf("\t@");
                }
                if (seats[p].st == SOLDED){
                    printf("\t$");
                }
            }
            printf("\n");
        }
        printf("==============================================================================\n");
        int cnt = 1;
        for (int i = 0; i < data.n; ++i){
            for(int j = 0; j < data.m; ++j){
                int p = i*data.m + j;
                if (seats[p].st != EMPTY){
                    time_t now = seats[p].opt_time;
                    struct tm *local_time = localtime(&now);
                    char buffer[80];
                    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local_time);
                    if (seats[p].st == RESERVED)
                        printf("name:%s  RESERVED(%d, %d)  time:%s\n", seats[p].name, i+1, j+1, buffer);
                    if (seats[p].st == SOLDED)
                        printf("name:%s   BOUGHT (%d, %d)  time:%s\n", seats[p].name, i+1, j+1, buffer);
                }
            }
        }
        printf("==============================================================================\n");
        //删除信号量与共享内存资源
        // if (semctl(sem_seat, 0, IPC_RMID) == -1) {
        //     perror("semctl IPC_RMID");
        //     exit(1);
        // }
        if (semctl(sem_atomopt, 0, IPC_RMID) == -1) {
            perror("semctl IPC_RMID");
            exit(1);
        }
        if (shmctl(shm_seat, IPC_RMID, NULL) == -1) {
            perror("shmctl IPC_RMID");
            exit(1);
        }
        //删除字典
        if (shmctl(shm_dic, IPC_RMID, NULL) == -1) {
            perror("shmctl IPC_RMID");
            exit(1);
        }
    }
    return 0;
}
