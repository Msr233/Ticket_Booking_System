#ifndef DICT_H
#define DICT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定义键值对结构体
typedef struct {
    char *key;
    char *value;
    int size;
} KeyValuePair;

// 定义字典结构体
typedef struct {
    KeyValuePair *items;
    int size;
    int count;
} Dictionary;



// 初始化字典
Dictionary *initDictionary(int size) ;

// 释放字典内存
void freeDictionary(Dictionary *dict);

int dic_search(Dictionary *dict, const char *key);

void dic_insert(Dictionary *dict, const char *key, const char *value);

void dic_show(Dictionary *dic);

#endif