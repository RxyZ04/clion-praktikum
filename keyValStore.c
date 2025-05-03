#include <string.h>
#include <stdio.h>
#include "keyValStore.h"

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} KeyValuePair;

static KeyValuePair store[MAX_PAIRS];
static int count = 0;

int get(const char* key, char* result) {
    for (int i = 0; i < count; ++i) {
        if (strcmp(store[i].key, key) == 0) {
            strcpy(result, store[i].value);
            return 0;
        }
    }
    return -1;
}

int put(const char* key, const char* value) {
    for (int i = 0; i < count; ++i) {
        if (strcmp(store[i].key, key) == 0) {
            strncpy(store[i].value, value, MAX_VALUE_LEN);
            return 0;
        }
    }
    if (count < MAX_PAIRS) {
        strncpy(store[count].key, key, MAX_KEY_LEN);
        strncpy(store[count].value, value, MAX_VALUE_LEN);
        count++;
        return 0;
    }
    return -1;
}

int del(const char* key) {
    for (int i = 0; i < count; ++i) {
        if (strcmp(store[i].key, key) == 0) {
            store[i] = store[count - 1];
            count--;
            return 0;
        }
    }
    return -1;
}
