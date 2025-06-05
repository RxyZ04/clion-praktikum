#include "keyValStore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>

typedef struct {
    KeyValuePair pairs[MAX_PAIRS];
    int count;
} SharedData;

static int shm_id;
static int sem_id;
static SharedData* store;

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};

// Semaphore Operationen
void sem_lock() {
    struct sembuf op = {0, -1, 0};
    semop(sem_id, &op, 1);
}

void sem_unlock() {
    struct sembuf op = {0, 1, 0};
    semop(sem_id, &op, 1);
}

int init_store() {
    key_t shm_key = ftok(".", 'S');
    key_t sem_key = ftok(".", 'M');

    shm_id = shmget(shm_key, sizeof(SharedData), IPC_CREAT | 0666);
    if (shm_id == -1) return -1;

    sem_id = semget(sem_key, 1, IPC_CREAT | 0666);
    if (sem_id == -1) return -1;

    store = (SharedData*)shmat(shm_id, NULL, 0);
    if (store == (void*)-1) return -1;

    union semun arg;
    arg.val = 1;
    semctl(sem_id, 0, SETVAL, arg);

    return 0;
}

int get(const char* key, char* result) {
    sem_lock();
    for (int i = 0; i < store->count; i++) {
        if (strcmp(store->pairs[i].key, key) == 0) {
            strncpy(result, store->pairs[i].value, MAX_VALUE_LEN);
            sem_unlock();
            return 0;
        }
    }
    sem_unlock();
    return -1;
}

int put(const char* key, const char* value) {
    sem_lock();
    for (int i = 0; i < store->count; i++) {
        if (strcmp(store->pairs[i].key, key) == 0) {
            strncpy(store->pairs[i].value, value, MAX_VALUE_LEN);
            sem_unlock();
            return 0;
        }
    }

    if (store->count < MAX_PAIRS) {
        strncpy(store->pairs[store->count].key, key, MAX_KEY_LEN);
        strncpy(store->pairs[store->count].value, value, MAX_VALUE_LEN);
        store->count++;
        sem_unlock();
        return 0;
    }

    sem_unlock();
    return -1;
}

int del(const char* key) {
    sem_lock();
    for (int i = 0; i < store->count; i++) {
        if (strcmp(store->pairs[i].key, key) == 0) {
            for (int j = i; j < store->count - 1; j++) {
                store->pairs[j] = store->pairs[j + 1];
            }
            store->count--;
            sem_unlock();
            return 0;
        }
    }
    sem_unlock();
    return -1;
}

void detach_store() {
    shmdt((void*)store);
}

void destroy_store() {
    shmctl(shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);
}
