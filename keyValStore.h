#ifndef KEYVALSTORE_H
#define KEYVALSTORE_H

#define MAX_KEY_LEN 128
#define MAX_VALUE_LEN 128
#define MAX_PAIRS 100

int get(const char* key, char* result);
int put(const char* key, const char* value);
int del(const char* key);

#endif
