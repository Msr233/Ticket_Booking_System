#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    int n, m, k, rvt;   //大厅排数n、每排座位数m、代理数k、预定保存时间rvt
    char ***all_opts;   //存储所有代理的所有操作
    int *opt_nums;      //存储每个代理的操作个数
    int **all_times;    //存储所有代理的延迟参数,reserve,ticket,cancel,show
} data_loader;

data_loader load_data(const char *filename);