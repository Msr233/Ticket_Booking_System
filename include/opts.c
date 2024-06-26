#include "opts.h"

//AA转十进制
int Atoi(const char *str){
    int decimal = 0;
    int len = strlen(str);
    int weight = 1;
    if (len > 0) {
        // 从后向前遍历字符串
        for (int i = len - 1; i >= 0; --i) {
            int digit = str[i] - 'A' + 1;
            // 将当前字符的值乘以其权重，然后加到结果上
            decimal += digit * weight;
            weight *= 26;
        }
    }
    return decimal-1;
}

//输入座位描述，返回座位起始点ss，终点ee，座位描述类型type
void parse_seat(char *seat, int n, int m, int *ss, int *ee, int *type){
    char *token;
    char delim[] = " ";
    char *last;
    char *seat_cpy = strdup(seat);
    token = strtok_r(seat_cpy, delim, &last);
    int cnt = 0;
    *type = -1;  //用于判断是哪一种类型
    char row[8] = "\0";
    char start[8] = "\0";
    char end[8] = "\0";
    //分割字符串，并确定座位描述类型
    while (token != NULL){
        // printf("%d token:%s\n", cnt, token);
        switch(cnt){
            case 0:{
                if (token[0] >= '1' && token[0] <= '9'){
                    *type = 6;   
                    strcpy(start, token);
                }
                else{
                    *type = 3;
                    strcpy(row, token);
                }
                break;
            }
            case 1:{
                int len = strlen(token);
                if (token[len-1] == 'r'){
                    *type = 4;
                    strncpy(start, token, len-1);
                }
                else if (token[len-1] == 'c'){
                    *type = 5;
                    strncpy(start, token, len-1);
                }
                else{
                    *type = 1;
                    strncpy(start, token, len);
                }
                break;
            }
            case 2:{
                *type = 2;
                strcpy(end, token);
                break;
            }
        }
        token = strtok_r(NULL, delim, &last);
        cnt++;
    }
    //转换字母为数字，A = 0, B = 1, AA = 26
    int r = Atoi(row);
    int s = atoi(start);
    int e = atoi(end);
    if (*type == 3){// 订整行
        *ss = r*m;
        *ee = *ss + m -1;
    }
    else if (*type == 6){//任意个位置，ss表示找几个位置
        *ss = s;
    }
    else if (*type == 1){   //订1个指定位置
        if (s > m) *type = 7;
        *ss = r*m + s -1;
    }
    else if (*type == 2){   //指定同行连坐
        *ss = r*m + s - 1;
        *ee = r*m + e - 1;
    }
    else if (*type == 4){   //指定连续行
        *ss = r*m;
        *ee = (r+s)*m - 1;
    }
    else if (*type == 5){   //指定行，任意个数，ss表示开始位置，ee表示订几个
        *ss = r*m;
        *ee = s;
    }
}

// 检查是否合法
int sold_check(Seat* Hall, int start, int end, char *name, int cancel_check, int rvt){
    for (int i = start; i <= end; ++i){
        if (Hall[i].st == SOLDED){
            printf("These seats were solded!\n");
            return -1;        
        }
        // printf("%d", Hall[i].st);
        else if (Hall[i].st == RESERVED){//如果被预定了，比较预定人姓名
            // 如果超过了预定时间
            time_t t = time(NULL);
            if (t > Hall[i].opt_time + rvt){
                Hall[i].st = EMPTY;
                continue;
            }
            // printf("cmp\n");
            if (strcmp(Hall[i].name, name)!=0){
                printf("old:%s new:%s\n", Hall[i].name, name);
                printf("You can ONLY do this for your own seats!\n");
                return -1;
            }
        }
        else if (cancel_check && Hall[i].st == EMPTY){
            printf("Please cancel those your have had reserved!\n");
            return -1;
        }
    }
    return 0;
}

//针对type5，6
int sold_find(Seat* Hall, int row, int num, char *name, int len, int *to_do, int cancel_check){
    int cnt = 0;
    if (num > len) return -1;
    if (cancel_check){
        for (int i = row; i <= row + len; ++i){
            if (cnt >= num) break;
            if (Hall[i].st == RESERVED && strcmp(Hall[i].name, name)==0){
                to_do[cnt++] = i;
            }
        }
    }
    else{
        for (int i = row; i <= row + len; ++i){
            if (cnt >= num) break;
            if (Hall[i].st == SOLDED){
                continue;  
            }
            else if (Hall[i].st == RESERVED){//如果被预定了，比较预定人姓名
                if (strcmp(Hall[i].name, name)!=0){
                    continue;
                }
            }
            else{
                // printf("%d ", i);
                to_do[cnt++] = i;
            }
        }
    }
    // printf("cnt = %d, num = %d, len = %d\n", cnt,num, len);
    if (cnt == num) return 0;
    printf("NOT enough seats to do\n");
    return -1;
}

int check_range(int type, int start, int end, int len){
    if (type == 1 && start >= len) return -1;
    if (type >= 2 && type <= 4){
        if (end >= len) return -1;
    }
    if (type == 5 && (start >= len || end > len)) return -1;
    if (type == 6 && start > len) return -1;
    return 0;
}


//成功返回0，不成功返回-1
int do_reserve(char *seat, char *name, int n, int m, Seat* Hall, enum State st, char* opt, int rvt){
    int start = 0, end = 0, type = -1;
    int cancel_check = 0;
    if (st == EMPTY) cancel_check = 1;
    pid_t pid = getpid();
    printf("[%d] %s %s %s\n",pid,name, opt, seat);
    parse_seat(seat, n, m, &start, &end, &type);
    if (type == 7 || check_range(type, start, end, n*m) == -1){
        printf("ERROR out of range! [%s] SKIP!\n", seat);
        return -1;
    }
    // printf("start=%d, end=%d, type=%d\n", start, end, type);
    time_t t = time(NULL);
    switch(type){
        case 1:{
            if (sold_check(Hall, start, start, name, cancel_check, rvt) != -1){
                Hall[start].opt_time = t;
                strcpy(Hall[start].name, name);
                Hall[start].st = st;   
                printf("success\n-----------------------------------------\n");
                return 0;
            }
            else{
                printf("fail\n-----------------------------------------\n");
                return -1;
            }
        }
        case 2:{
            if (sold_check(Hall, start, start, name, cancel_check, rvt) != -1){
                for (int i = start; i <= end; ++i){
                    Hall[i].opt_time = t;
                    strcpy(Hall[i].name, name);
                    Hall[i].st = st;      
                }
                printf("success\n");
                return 0;
            }
            else{
                printf("fail\n");
                return -1;
            }
        }
        case 3:{
            if (sold_check(Hall, start, start, name, cancel_check, rvt) != -1){
                for (int i = start; i <= end; ++i){
                    Hall[i].opt_time = t;
                    strcpy(Hall[i].name, name);
                    Hall[i].st = st;   
                }
                printf("success\n-----------------------------------------\n");
                return 0;
            }
            else{
                printf("fail\n-----------------------------------------\n");
                return -1;
            }
        }
        case 4:{
            if (sold_check(Hall, start, start, name, cancel_check, rvt) != -1){
                for (int i = start; i <= end; ++i){
                    Hall[i].opt_time = t;
                    strcpy(Hall[i].name, name);
                    Hall[i].st = st;   
                }
                printf("success\n-----------------------------------------\n");
                return 0;
            }
            else{
                printf("fail\n-----------------------------------------\n");
                return -1;
            }
        }
        case 5:{
            // start开始位置，end个数
            int *to_do = (int*)malloc(sizeof(int) * end);
            if (!sold_find(Hall, start, end, name, m, to_do, cancel_check)){
                for (int i = 0; i < end; ++i){
                    int n = to_do[i];
                    Hall[n].opt_time = t;
                    strcpy(Hall[n].name, name);
                    Hall[n].st = st;   
                }
                printf("success\n-----------------------------------------\n");
                return 0;
            }
            else{
                printf("fail\n-----------------------------------------\n");
                return -1;
            }
        }
        case 6:{
            int *to_do = (int*)malloc(sizeof(int) * start);
            if (!sold_find(Hall, 0, start, name, n*m-1, to_do, cancel_check)){
                for (int i = 0; i < start; ++i){
                    int n = to_do[i];
                    // printf("%d",n);
                    Hall[n].opt_time = t;
                    strcpy(Hall[n].name, name);
                    Hall[n].st = st;   
                }
                printf("success\n-----------------------------------------\n");
                return 0;
            }
            else{
                printf("fail\n-----------------------------------------\n");
                return -1;
            }
        }
    }
    return -1;
}

// 对于show all
int do_show(char *name, Dictionary *dic){
    int p = dic_search(dic, name);
    printf("-----------------------------------------\n");
    printf("%s opts:\n%s\n", name, dic->items[p].value);
    printf("-----------------------------------------\n");
    return 0;
}
