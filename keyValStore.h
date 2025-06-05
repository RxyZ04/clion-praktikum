#ifndef KEYVALSTORE_H
#define KEYVALSTORE_H

#define MAX_KEY_LEN 128
#define MAX_VALUE_LEN 128
#define MAX_PAIRS 100

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} KeyValuePair;

// Funktionen
int init_store();       // Shared Memory + Semaphore initialisieren
int get(const char* key, char* result);
int put(const char* key, const char* value);
int del(const char* key);
void detach_store();    // shared memory & semaphore trennen
void destroy_store();   // nur am Ende/Exit aufrufen

#endif
