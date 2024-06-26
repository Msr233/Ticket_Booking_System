#include "utils.h"

// 输入完整的交易信息，返回分割好的3个部分：操作，座位，姓名,成功返回0，不成功返回-1
int split_opt(const char* opt, char *info_opt, char *info_seat, char *info_name){
    size_t len = strlen(opt);
    // char info_opt[9] = "\0", info_seat[8] = "\0", info_name[32] = "\0";
    int p1,p2;
    // printf("%s\n", opt);
    for (p1 = 0; p1 < len; ++p1) if (opt[p1] == ' ') break;
    for (p2 = len-1; p2 >= 0; --p2) if (opt[p2] == ' ') break;
    if (p1 == len){//没有空格，说明操作描述错误
        printf("ERROR commamd: [%s] SKIP!\n", opt);
        return -1;
    }
    if (p1 == p2){ // 缺项
        printf("ERROR command: [%s] SKIP!\n", opt);
        return -1;
    }

    if (opt[p1])
    if (strncmp(opt, "show", 4) == 0){
        strncpy(info_opt, opt, p1+4); info_opt[p1+4] = '\0';
        strcpy(info_name, opt + p2 + 1); info_name[strlen(info_name)] = '\0';
    }
    else{
        strncpy(info_opt, opt, p1); info_opt[p1] = '\0';
        strncpy(info_seat, opt + p1 + 1, p2 - p1); info_seat[p2 - p1-1] = '\0';
        strncpy(info_name, opt + p2 + 1, len - p2 - 1); info_name[len - p2 - 1] = '\0';
    }
    
    if (check_opt(info_opt) == -1){
        printf("ERROR operation: [%s] SKIP!\n", opt);
        return -1;
    }
    if (check_name(info_name) == -1){// 出错
        printf("ERROR NAME [%s] SKIP!\n", opt);
        return -1;
    };
    if (check_blank(info_seat) == -1){
        printf("ERROR command: [%s] SKIP!\n", opt);
        return -1;
    }
    return 0;
}

int check_name(const char* name){
    if (name[0] >= '0' && name[0] <= '9') return -1;
    return 0;
}

int check_opt(const char* opt){
    //show
    if (strncmp(opt, "reserve", 7) == 0 || strncmp(opt, "ticket", 6) == 0 || strncmp(opt, "show all", 8) == 0 || strncmp(opt, "cancel", 6) == 0){
        return 0;
    }
    return -1;
}

int check_blank(char *seat){
    char *token;
    char delim[] = " ";
    char *last;
    char *s = strdup(seat);
    token = strtok_r(s, delim, &last);
    int cnt = 0;
    int type = -1;  //用于判断是哪一种类型
    while (token != NULL){
        switch(cnt){
            case 0:{
                if (token[0] >= '1' && token[0] <= '9'){
                    type = 6;   
                }
                else{
                    type = 3;
                }
                break;
            }
            case 1:{
                if (type == 6) return -1;

                int len = strlen(token);
                if (token[len-1] == 'r'){
                    if (len < 2) return -1;
                    if (token[len-2] >= '0' && token[len-2] <= '9')
                        type = 4;
                    else return -1;
                }
                else if (token[len-1] == 'c'){
                    if (len < 2) return -1;
                    if (token[len-2] >= '0' && token[len-2] <= '9')
                        type = 5;
                    else return -1;
                }
                else{
                    if (token[len-1] >= '0' && token[len-1] <= '9')
                        type = 1;
                    else return -1;
                }
                break;
            }
            case 2:{
                int len = strlen(token);
                if (token[len-1] >= '0' && token[len-1] <= '9'){
                    type = 2;
                    break;
                }
                else return -1;
            }
            case 3:{
                return -1;
            }
        }
        token = strtok_r(NULL, delim, &last);
        cnt++;
    }
    return 0;
}