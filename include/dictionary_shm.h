#ifndef DICT_H
#define DICT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define VALUE_MAX_SIZE 1024
#define NAME_MAX 16
#define CLIENT_NUM_MAX 64

// 定义键值对结构体
typedef struct {
    char key[NAME_MAX];
    char value[VALUE_MAX_SIZE];
    int size;
} KeyValuePair;

// 定义字典结构体
typedef struct {
    KeyValuePair items[CLIENT_NUM_MAX];
    int size;
    int count;
} Dictionary;


// 初始化字典
Dictionary *initDictionary(int size) ;

int dic_search(Dictionary *dict, const char *key);

void dic_insert(Dictionary *dict, const char *key, const char *value);

void dic_show(Dictionary *dic);

#endif