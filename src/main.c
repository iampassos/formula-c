#include "client.h"
#include "server.h"

int main() {
    Server_Init(); 
    Client_Init();
    while (!WindowShouldClose()) {
        Client_Update();
        Client_Draw();
    }
    Client_Cleanup();
    Server_exit();
    return 0;
}