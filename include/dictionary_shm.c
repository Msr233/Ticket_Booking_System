#include "dictionary_shm.h"

// 初始化字典
int initDictionary(Dictionary* dict) {
    dict->size = CLIENT_NUM_MAX;
    dict->count = 0;
    return 0;
}


int dic_search(Dictionary *dict, const char *key) {
    for (int i = 0; i < dict->count; ++i){
        if (strcmp(dict->items[i].key, key) == 0){
            return i;
        }
    }
    return -1;
}

void dic_insert(Dictionary *dict, const char *key, const char *value) {
    if (dict->count >= dict->size - 1){
        printf("ERROR dictionart is FULL!\n");
    }
    int p = dic_search(dict, key);
    if (p == -1){
        // 正确分配 key 的内存并复制字符串
        strcpy(dict->items[dict->count].key,key);
        
        // 为 value 分配初始内存，并复制字符串
        dict->items[dict->count].size = VALUE_MAX_SIZE; // 设置 value 的初始大小
        strcpy(dict->items[dict->count].value, value);
        int len = strlen(value);
        dict->items[dict->count].value[len] = '\n'; 
        dict->items[dict->count].value[len+1] = '\0'; // 确保字符串正确终止
        dict->count++;
    } else {
        // 计算新内容的长度
        int new_len = strlen(dict->items[p].value) + strlen(value) + 1; // +1 为了空字符
        // // 如果需要，扩容 value
        // if (new_len > dict->items[p].size) {
        //     dict->items[p].size = new_len; // 更新 size
        //     dict->items[p].value = (char*)realloc(dict->items[p].value, dict->items[p].size);
        // }
        // 追加新 value 并添加换行符
        strcat(dict->items[p].value, value);
        dict->items[p].value[new_len - 1] = '\n'; // 只在字符串末尾添加换行符
        dict->items[p].value[new_len] = '\0'; // 确保字符串正确终止
    }
}

void dic_show(Dictionary *dic){
    for (int i = 0; i < dic->count; ++i){
        KeyValuePair p = dic->items[i];
        printf("key = %s\n%s", p.key,p.value);
        printf("-------------------------------------\n");
    }
}
