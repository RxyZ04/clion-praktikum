#ifndef KEYVALSTORE_H
#define KEYVALSTORE_H

#define MAX_ITEMS 100
#define MAX_KEY_LEN 50
#define MAX_VALUE_LEN 100

int put(char* key, char* value);
int get(char* key, char* res);
int del(char* key);

#endif
