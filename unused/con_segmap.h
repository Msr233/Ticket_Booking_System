#pragma once
#include "con_hashmap.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#define INIT_SIZE 4


typedef struct con_segmap {
    con_hashmap** submaps;
    uint32_t (*hash_func)(void* v);
    size_t con_level;
}con_segmap;

con_segmap* create_segmap(uint32_t (*hash_func)(void* v), bool (*equals)(void* a, void* b), size_t con_level) {
    con_segmap* ret = (con_segmap*)malloc(sizeof(con_segmap));
    ret->con_level = con_level;
    ret->hash_func = hash_func;
    ret->submaps = (con_hashmap**)malloc(sizeof(con_hashmap*) * (1 << con_level));
    for(int i = 0; i < (1 << con_level); i ++) {
        ret->submaps[i] = create_hashmap(hash_func, equals);
        ret->submaps[i]->hash_offset = con_level;
    }
    return ret;
}

uint32_t __hv(con_segmap* s, void* x) {
    return (*s->hash_func)(x);
}

void sput(con_segmap* s, void* key, void* val) {
    uint32_t hv = __hv(s, key);
    uint32_t sub_id = hv & ((1 << s->con_level) - 1);
    pthread_spin_lock(&s->submaps[sub_id]->lock);
    _put(s->submaps[sub_id], hv >> s->con_level, key, val);
    pthread_spin_unlock(&s->submaps[sub_id]->lock);
}

void* sget(con_segmap* s, void* key) {
    uint32_t hv = __hv(s, key);
    uint32_t sub_id = hv & ((1 << s->con_level) - 1);
    pthread_spin_lock(&s->submaps[sub_id]->lock);
    void* ret = _get(s->submaps[sub_id], hv >> s->con_level, key);
    pthread_spin_unlock(&s->submaps[sub_id]->lock);
    return ret;
}

void* sdel(con_segmap* s, void* key) {
    uint32_t hv = __hv(s, key);
    uint32_t sub_id = hv & ((1 << s->con_level) - 1);
    pthread_spin_lock(&s->submaps[sub_id]->lock);
    void* ret = _del(s->submaps[sub_id], hv >> s->con_level, key);
    pthread_spin_unlock(&s->submaps[sub_id]->lock);
    return ret;
}

size_t ssize(con_segmap* s) {
    size_t ret = 0;
    for(int i = 0; i < (1 << s->con_level); i ++) {
        ret += s->submaps[i]->size;
    }
    return ret;
}

