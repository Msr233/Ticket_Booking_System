#pragma once
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#define INIT_SIZE 4

typedef struct entry {
    void* key;
    void* val;
    struct entry* next;
}entry;

entry* create_entry(void* key, void* val, entry* next) {
    entry* ret = (entry*)malloc(sizeof(entry));
    ret->key = key;
    ret->val = val;
    ret->next = next;
    return ret;
}

entry* create_entries(int n) {
    return (entry*)calloc(n, sizeof(entry));
}

void lput(entry* head, entry* item, bool (*equals)(void* a, void* b)) {
    while(head->next != NULL) {
        entry* now = head->next;
        if((*equals)(now->key, item->key)) {
            now->val = item->val;
            return;
        }
        head = head->next;
    }
    head->next = item;
    item->next = NULL;
}

void* lget(entry* head, void* key, bool (*equals)(void* a, void* b)) {
    void* ret = NULL;
    while(head->next != NULL) {
        entry* now = head->next;
        if((*equals)(now->key, key)) {
            ret = now->val;
            break;
        }
        head = head->next;
    }
    return ret;
}

void* ldel(entry* head, void* key, bool (*equals)(void* a, void* b)) {
    while(head->next != NULL) {
        entry* now = head->next;
        if((*equals)(now->key, key)) {
            head->next = now->next;
            return now;
        }
        head = head->next;
    }
    return NULL;
}

