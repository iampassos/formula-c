#include "client.h"
#include "server.h"

int main() {
    Client_Init();
    while (!WindowShouldClose()) {
        Client_Update();
        Client_Draw();
    }
    Client_Cleanup();
    return 0;
}