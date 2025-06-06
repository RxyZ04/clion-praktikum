#include <stdio.h>
#include "main.h"
#include "keyValStore.h"
#include "sub.h"

int main() {
    printf("Starte Socket-Server auf Port 5679...\n");
    start_server(5679);
    return 0;
}
