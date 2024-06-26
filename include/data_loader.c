#include"data_loader.h"

//从txt加载数据，保存到data_loader结构体中
data_loader load_data(const char *filename) {
    FILE *f = fopen(filename, "r");
    //打开文件
    if (f == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    data_loader res;
    fscanf(f, "%d %d %d %d", &res.n, &res.m, &res.k, &res.rvt);
    res.rvt *= 60; // 单位：s

    //动态分配空间
    res.all_times = (int**)malloc(res.k * sizeof(int *));
    res.opt_nums = (int*)malloc(res.k * sizeof(int));
    res.all_opts = (char***)malloc(res.k * sizeof(char **));

    char tmp[1024]; // 假设字符串不会超过1023字符
    fgets(tmp, sizeof(tmp), f); // 读取空白行

    //循环读取
    for (int i = 1; i <= res.k; i++) {
        int *ag_times = (int*)malloc(sizeof(int) * 4); // 动态分配内存

        char **ag_opts = (char**)malloc(sizeof(char *) * (res.m + 1)); // 动态分配内存

        // 读取agent标识，遇到\n停止
        fscanf(f, "%[^\n]\n", tmp);
        if (strncmp(tmp, "agent", 5) != 0 || atoi(tmp + 6) != i) {
            fprintf(stderr, "Input error\n");
            fclose(f);
            exit(EXIT_FAILURE);
        }

        // 读取时间参数
        for (int j = 0; j < 4; j++) {
            fscanf(f, "%[^\n]\n", tmp);
            char *sstr = tmp;
            switch (j) {
                case 0: {
                    if (strncmp(sstr, "reserve ", 8) == 0) ag_times[j] = atoi(sstr+8);
                    break;
                }
                case 1: {
                    if (strncmp(sstr, "ticket ", 7) == 0) ag_times[j] = atoi(sstr+7);
                    break;
                }
                case 2: {
                    if (strncmp(sstr, "cancel ", 7) == 0) ag_times[j] = atoi(sstr+7);
                    break;
                }
                case 3: {
                    if (strncmp(sstr, "check_customer ", 15) == 0)  ag_times[j] = atoi(sstr+15);
                    break;
                }
            }
        }
        res.all_times[i - 1] = ag_times;

        // 读取交易列表
        fscanf(f, "%[^\n]\n", tmp);
        int k = 0;
        while (strcmp(tmp, "end") != 0) {
            ag_opts[k++] = strdup(tmp); // 使用strdup复制字符串
            fscanf(f, "%[^\n]\n", tmp);
        }
        res.all_opts[i - 1] = ag_opts;
        res.opt_nums[i-1] = k;
    }

    fclose(f);
    return res;
}
