#pragma once
#include "hash_entry.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#define INIT_SIZE 4


typedef struct con_hashmap {
    size_t cap;
    size_t load;
    size_t size;
    entry** data;
    size_t hash_offset;
    uint32_t (*hash_func)(void* v);
    bool (*equals)(void* a, void* b);
    pthread_spinlock_t lock;
}con_hashmap;

con_hashmap* create_hashmap(uint32_t (*hash_func)(void* v), bool (*equals)(void* a, void* b)) {
    con_hashmap* ret = (con_hashmap*)malloc(sizeof(con_hashmap));
    ret->cap = (1 << INIT_SIZE);
    ret->load = ret->size = ret->hash_offset = 0;
    ret->data = (entry**)malloc(sizeof(entry*) * (1 << INIT_SIZE));
    entry* heads = create_entries(1 << INIT_SIZE);
    for(int i = 0; i < (1 << INIT_SIZE); i ++) {
        ret->data[i] = &heads[i];
    }
    ret->hash_func = hash_func;
    ret->equals = equals;
    pthread_spin_init(&ret->lock, PTHREAD_PROCESS_PRIVATE);
    return ret;
}

uint32_t _hv(con_hashmap* s, void* key) {
    return ((*s->hash_func)(key)) >> s->hash_offset;
}

void* _get_bucket(con_hashmap* s, uint32_t hv) {
    uint32_t bucket_id = hv & (s->cap - 1);
    return s->data[bucket_id];
}

void* _get_bucket_raw(entry** data, size_t cap, uint32_t hv) {
    uint32_t bucket_id = hv & (cap - 1);
    return data[bucket_id];
}

void _resize(con_hashmap* s) {
    size_t new_cap = s->cap << 1;
    size_t new_load = 0;
    entry* heads = create_entries(new_cap);
    entry** new_data = (entry**)malloc(sizeof(entry*) * new_cap);
    for(int i = 0; i < new_cap; i ++) {
        new_data[i] = &heads[i];
    }

    for(int i = 0; i < s->cap; i ++) {
        entry* h = s->data[i];
        while(h->next != NULL) {
            entry* now = h->next;
            h->next = now->next;
            entry* bucket = (entry*)_get_bucket_raw(new_data, new_cap, _hv(s, now->key));
            if(bucket->next == NULL){
                new_load ++;
            }
            lput(bucket, now, s->equals);
        }
    }

    entry* old_heads = s->data[0];
    entry** old_data = s->data;
    s->data = new_data;
    s->cap = new_cap;
    s->load = new_load;

    free(old_heads);
    free(old_data);
}

void* _get(con_hashmap* s, uint32_t hv, void* key) {
    entry* bucket = (entry*)_get_bucket(s, hv);
    return lget(bucket, key, s->equals);
}

void* get(con_hashmap* s, void* key) {
    pthread_spin_lock(&s->lock);
    void* ret = _get(s, _hv(s, key), key);
    pthread_spin_unlock(&s->lock);
    return ret;
}

void _put(con_hashmap*s, uint32_t hv, void* key, void* val) {
    entry* bucket = (entry*)_get_bucket(s, hv);
    if(bucket->next == NULL) {
        s->load ++;
    }
    if(s->cap * 4 <= s->load * 5) {
        _resize(s);
        bucket = (entry*)_get_bucket(s, hv);
        if(bucket->next == NULL) {
            s->load ++;
        }
    }
    entry* item = create_entry(key, val, NULL);
    lput(bucket, item, s->equals);
    s->size ++;
}

void put(con_hashmap* s, void* key, void* val) {
    pthread_spin_lock(&s->lock);
    _put(s, _hv(s, key), key, val);
    pthread_spin_unlock(&s->lock);
}

void* _del(con_hashmap* s, uint32_t hv, void* key) {
    entry* bucket = (entry*)_get_bucket(s, hv);
    void* ret = ldel(bucket, key, s->equals);
    if(bucket->next == NULL) {
        s->load --;
    }
    s->size --;
    return ret;
}

void* del(con_hashmap* s, void* key) {
    pthread_spin_lock(&s->lock);
    void* ret = _del(s, _hv(s, key), key);
    pthread_spin_unlock(&s->lock);
    return ret;
}

size_t size(con_hashmap* s) {
    return s->size;
}

