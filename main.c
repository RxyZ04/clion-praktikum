#include <stdio.h>
#include "main.h"
#include "keyValStore.h"
#include "sub.h"

int main() {
    printf("Starte Socket-Server auf Port 5678...\n");
    start_server(5678);
    return 0;
}
