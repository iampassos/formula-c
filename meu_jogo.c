#include "raylib.h"
#include <stdio.h>
#include <math.h>

#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()
#define BACKGROUND_COLOR (Color){186, 149, 127, 255}
#define MIN_TURN_SPEED 0.2 // PAra carro n girar parado
#define DRAG_COEFICIENT 0.01 // Atrito
#define BREAK_COEFICIENT 0.05 // ``Atrito para freio ativo `` 

#define DRAG_FORCE (1 - DRAG_COEFICIENT)
#define BREAK_FORCE (1 - BREAK_COEFICIENT)

typedef struct
{
    Vector2 pos;
    float vel;
    float acc;
    int width;
    int height;
    Color color;
    float angle;
    float angularAcc;
} Car;

// Contrutor do carro
Car createCar(Vector2 pos, float vel, float acc, int width, int height, Color color, float angle, float angularAcc);

void updateCar(Car *car); // Atualizar a posição do carro a cada frame
void accelerateCar(Car *car); // Acelerar o carro
void drawCar(Car car); // Desenhar o carro na tela
void turn(Car *car, float angle); // Rotacionar o carro
void turnLeft(Car *car); // Virar o carro para esquerda
void turnRight(Car *car); // Virar o carro para direita
bool canTurn(Car car); // Verificar se o carro possui velocidade mínima para girar
void breakCar(Car *car); //Freio ativo
void reverseCar(Car *car); //Ré do carro

void setup(); // Função para carregar o cenário e variáveis globais
void draw(); // Função que é executada a cada frame

bool isOnTrack(Vector2 pos);

Car player;
Color *trackPixels;
Image trackMask;
Texture2D trackTex;
Texture2D trackBackground;

int main()
{
    setup();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        draw();
        EndDrawing();
    }

    CloseWindow();
    UnloadTexture(trackBackground);
    UnloadTexture(trackTex); // se quiser liberar também a textura da máscara
    UnloadImage(trackMask);

    return 0;
}

bool isOnTrack(Vector2 pos) {
    int x = (int)pos.x;
    int y = (int)pos.y;
    if (x < 0 || x >= trackMask.width || y < 0 || y >= trackMask.height) return false;
    Color pixel = trackPixels[y * trackMask.width + x];
    return pixel.r > 200 && pixel.g > 200 && pixel.b > 200; // ex: branco
}

void setup()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Racer test");
    SetTargetFPS(60);

    trackMask = LoadImage("pista_mask.png");
    ImageResize(&trackMask, SCREEN_WIDTH, SCREEN_HEIGHT); 
    trackPixels = LoadImageColors(trackMask);
    trackTex = LoadTextureFromImage(trackMask); 

    Image bgImage = LoadImage("teste2.png"); // Pista exibida
    ImageResize(&bgImage, SCREEN_WIDTH, SCREEN_HEIGHT); // redimensiona
    trackBackground = LoadTextureFromImage(bgImage); // converte em textura
    UnloadImage(bgImage); // libera o recurso da imagem após virar textura

    //trackBackground = LoadTexture("pista_mask.png");  // <- imagem da pista visível

    player = createCar(
        (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, // pos
        0, // vel inicial do carro
        0.1, // aceleracao do carro
        50, // largura 
        20, // altura
        ORANGE, // Cor
        0.0, //angulo inicial do carro
        0.025 // aceleraçao angular ( velocidade de rotacao)
    );
}

void draw()
{
    ClearBackground(BACKGROUND_COLOR);
    DrawTexture(trackBackground, 0, 0, WHITE);  // desenha pista como fundo

    if (IsKeyDown(KEY_UP)) {
        accelerateCar(&player);
    }
    if (IsKeyDown(KEY_LEFT)) {
        turnLeft(&player);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        turnRight(&player);
    }

    if(IsKeyDown(KEY_DOWN)){
        if (player.vel<=MIN_TURN_SPEED){
            reverseCar(&player);
        }
        else{
        breakCar(&player);          
        }
    }

    updateCar(&player);
    drawCar(player);
}

Car createCar(Vector2 pos, float vel, float acc, int width, int height, Color color, float angle, float angularAcc)
{
    Car car = {pos, vel, acc, width, height, color, angle, angularAcc};
    return car;
}

void updateCar(Car *car)
{
    car->vel *= DRAG_FORCE;
    car->pos.x += cos(car->angle) * car->vel;
    car->pos.y += sin(car->angle) * car->vel;
}

void accelerateCar(Car *car)
{
    car->vel += car->acc;
}

void drawCar(Car car)
{   
    Texture2D textureCar = LoadTexture("minha_imagem.png");
    Rectangle rect = {car.pos.x, car.pos.y, car.width, car.height};
    Vector2 origin = {car.width * 0.8, car.height / 2.0f};
    DrawRectanglePro(rect, origin, car.angle * RAD2DEG, car.color);
    //DrawTexturePro
}

void turn(Car *car, float angle)
{
    if(canTurn(*car)) 
        car->angle += angle;
}

void turnLeft(Car *car)
{
    turn(car, -car->angularAcc);
}

void turnRight(Car *car)
{
    turn(car, +car->angularAcc);
}
bool canTurn (Car car){
    return car.vel > MIN_TURN_SPEED || car.vel < -MIN_TURN_SPEED;
}

void breakCar (Car *car){
    car->vel *= BREAK_FORCE;
}

void reverseCar(Car *car){
    car->vel -= car->acc/5;
}
