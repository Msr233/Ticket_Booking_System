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
#include "./include/dictionary.c"

#include"./include/data_loader.c"
#include "./include/opts.c"
#include "./include/utils.c"


int main(){
    char info_opt[9] = "\0", info_seat[8] = "\0", info_name[32] = "\0";
    char *opt = "ticket D 15 DD";
    char* opt2 = "reserve A 2c GG";
    char *opt3 = "reserve A AA BB";
    char *opt4 = "reserve A 2";
    char *opt5 = "show all j";
    char *opt6 = "show A";
    char *opt7 = "show all AA BB";
    split_opt(opt, info_opt, info_seat, info_name);
    split_opt(opt2, info_opt, info_seat, info_name);
    return 0;

}