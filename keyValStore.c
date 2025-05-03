#include <string.h>
#include "keyValStore.h"

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} KeyValue;

static KeyValue store[MAX_ITEMS];
static int count = 0;

int put(char* key, char* value) {
    for (int i = 0; i < count; i++) {
        if (strcmp(store[i].key, key) == 0) {
            strncpy(store[i].value, value, MAX_VALUE_LEN);
            return 1; // überschrieben
        }
    }
    if (count < MAX_ITEMS) {
        strncpy(store[count].key, key, MAX_KEY_LEN);
        strncpy(store[count].value, value, MAX_VALUE_LEN);
        count++;
        return 0; // neu hinzugefügt
    }
    return -1; // kein Platz mehr
}

int get(char* key, char* res) {
    for (int i = 0; i < count; i++) {
        if (strcmp(store[i].key, key) == 0) {
            strncpy(res, store[i].value, MAX_VALUE_LEN);
            return 1;
        }
    }
    return -1;
}

int del(char* key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(store[i].key, key) == 0) {
            for (int j = i; j < count - 1; j++) {
                store[j] = store[j + 1];
            }
            count--;
            return 1;
        }
    }
    return -1;
}
