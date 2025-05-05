#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define SCREEN_WIDTH GetScreenWidth()
#define SCREEN_HEIGHT GetScreenHeight()
#define BACKGROUND_COLOR (Color){186, 149, 127, 255}
#define MIN_TURN_SPEED 0.2    // PAra carro n girar parado
#define DRAG_COEFICIENT 0.01  // Atrito
#define BREAK_COEFICIENT 0.05 // ``Atrito para freio ativo ``

#define DRAG_FORCE (1 - DRAG_COEFICIENT)
#define BREAK_FORCE (1 - BREAK_COEFICIENT)

typedef struct {
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
Car createCar(Vector2 pos, float vel, float acc, int width, int height,
              Color color, float angle, float angularAcc);

void updateCars(Car *cars);       // Atualizar a posição do carro a cada frame
void accelerateCar(Car *car);     // Acelerar o carro
void drawCars(Car *cars);         // Desenhar o carro na tela
void turn(Car *car, float angle); // Rotacionar o carro
void turnLeft(Car *car);          // Virar o carro para esquerda
void turnRight(Car *car);         // Virar o carro para direita
bool canTurn(
    Car car); // Verificar se o carro possui velocidade mínima para girar
void breakCar(Car *car);   // Freio ativo
void reverseCar(Car *car); // Ré do carro

void setup(); // Função para carregar o cenário e variáveis globais
void draw();  // Função que é executada a cada frame

bool isOnTrack(Vector2 pos);

int players_n = 0;
Car *players;
Color *trackPixels;
Image trackMask;
Texture2D trackTex;
Texture2D trackBackground;

int main() {
  setup();

  while (!WindowShouldClose()) {
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
  if (x < 0 || x >= trackMask.width || y < 0 || y >= trackMask.height)
    return false;
  Color pixel = trackPixels[y * trackMask.width + x];
  return pixel.r > 200 && pixel.g > 200 && pixel.b > 200; // ex: branco
}

void setup() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Racer test");
  SetTargetFPS(60);

  trackMask = LoadImage("pista_mask.png");
  ImageResize(&trackMask, SCREEN_WIDTH, SCREEN_HEIGHT);
  trackPixels = LoadImageColors(trackMask);
  trackTex = LoadTextureFromImage(trackMask);

  Image bgImage = LoadImage("teste2.png");            // Pista exibida
  ImageResize(&bgImage, SCREEN_WIDTH, SCREEN_HEIGHT); // redimensiona
  trackBackground = LoadTextureFromImage(bgImage);    // converte em textura
  UnloadImage(bgImage); // libera o recurso da imagem após virar textura

  createCar((Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, // pos
            0,      // vel inicial do carro
            0.1,    // aceleracao do carro
            50,     // largura
            20,     // altura
            ORANGE, // Cor
            0.0,    // angulo inicial do carro
            0.025   // aceleraçao angular ( velocidade de rotacao)
  );

  createCar((Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, // pos
            0,    // vel inicial do carro
            0.1,  // aceleracao do carro
            50,   // largura
            20,   // altura
            BLUE, // Cor
            0.0,  // angulo inicial do carro
            0.025 // aceleraçao angular ( velocidade de rotacao)
  );

  createCar((Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, // pos
            0,     // vel inicial do carro
            0.1,   // aceleracao do carro
            50,    // largura
            20,    // altura
            GREEN, // Cor
            0.0,   // angulo inicial do carro
            0.025  // aceleraçao angular ( velocidade de rotacao)
  );
}

void draw() {
  ClearBackground(BACKGROUND_COLOR);
  DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo

  if (IsKeyDown(KEY_UP)) {
    accelerateCar(&players[0]);
  }
  if (IsKeyDown(KEY_LEFT)) {
    turnLeft(&players[0]);
  }
  if (IsKeyDown(KEY_RIGHT)) {
    turnRight(&players[0]);
  }

  if (IsKeyDown(KEY_DOWN)) {
    if (players[0].vel <= MIN_TURN_SPEED) {
      reverseCar(&players[0]);
    } else {
      breakCar(&players[0]);
    }
  }

  if (IsKeyDown(KEY_W)) {
    accelerateCar(&players[1]);
  }
  if (IsKeyDown(KEY_A)) {
    turnLeft(&players[1]);
  }
  if (IsKeyDown(KEY_D)) {
    turnRight(&players[1]);
  }

  if (IsKeyDown(KEY_S)) {
    if (players[1].vel <= MIN_TURN_SPEED) {
      reverseCar(&players[1]);
    } else {
      breakCar(&players[1]);
    }
  }

  if (IsKeyDown(KEY_I)) {
    accelerateCar(&players[2]);
  }
  if (IsKeyDown(KEY_J)) {
    turnLeft(&players[2]);
  }
  if (IsKeyDown(KEY_L)) {
    turnRight(&players[2]);
  }

  if (IsKeyDown(KEY_K)) {
    if (players[2].vel <= MIN_TURN_SPEED) {
      reverseCar(&players[2]);
    } else {
      breakCar(&players[1]);
    }
  }

  updateCars(players);
  drawCars(players);
}

Car createCar(Vector2 pos, float vel, float acc, int width, int height,
              Color color, float angle, float angularAcc) {
  Car car = {pos, vel, acc, width, height, color, angle, angularAcc};
  players = realloc(players, sizeof(Car) * ++players_n);
  players[players_n - 1] = car;
  return car;
}

void updateCars(Car *cars) {
  for (int i = 0; i < players_n; i++) {
    cars[i].vel *= DRAG_FORCE;
    cars[i].pos.x += cos(cars[i].angle) * cars[i].vel;
    cars[i].pos.y += sin(cars[i].angle) * cars[i].vel;
  }
}

void accelerateCar(Car *car) { car->vel += car->acc; }

void drawCars(Car *cars) {
  for (int i = 0; i < players_n; i++) {
    Rectangle rect = {cars[i].pos.x, cars[i].pos.y, cars[i].width,
                      cars[i].height};
    Vector2 origin = {cars[i].width * 0.2, cars[i].height / 2.0f};
    DrawRectanglePro(rect, origin, cars[i].angle * RAD2DEG, cars[i].color);
  }
}

void turn(Car *car, float angle) {
  if (canTurn(*car))
    car->angle += angle;
}

void turnLeft(Car *car) { turn(car, -car->angularAcc); }

void turnRight(Car *car) { turn(car, +car->angularAcc); }

bool canTurn(Car car) {
  return car.vel > MIN_TURN_SPEED || car.vel < -MIN_TURN_SPEED;
}

void breakCar(Car *car) { car->vel *= BREAK_FORCE; }

void reverseCar(Car *car) { car->vel -= car->acc / 5; }
