#include "./include/data_loader.c"
#include <stdio.h>


int test_dataloader(char *file){
    data_loader data;
    data = load_data(file);
    printf("n = %d m = %d k = %d rvt = %d\n", data.n, data.m, data.k, data.rvt);
    for (int i = 0; i < data.k; ++i){
        printf("=================================\n");
        printf("agent %d\n", i+1);
        printf("---------------------------------\n");
        printf("delay:\nreserve = %d ticket = %d cancel = %d check = %d\n", data.all_times[i][0],data.all_times[i][1],data.all_times[i][2],data.all_times[i][3]);
        printf("---------------------------------\n");
        for (int k = 0; k < data.opt_nums[i]; ++k){
            printf("%s\n", data.all_opts[i][k]);
        }
    }
    return 0;
}

int main()
{
    char *path = "./input3.txt";
    test_dataloader(path);
    return 0;
}
