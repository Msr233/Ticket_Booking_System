#include "./include/utils.c"
#include <stdio.h>
#include <string.h>

int main(){
    char *opt = "ticket D 2r DD";
    char info_opt[9] = "\0", info_seat[8] = "\0", info_name[32] = "\0";
    split_opt(opt, info_opt, info_seat, info_name);
    printf("test = %s %s %s\n", info_opt, info_name, info_seat);
    return 0;
}