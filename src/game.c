#include "game.h"
#include "camera.h"
#include "car.h"
#include "common.h"
#include "linked_list.h"

Texture2D   trackBackground; // Armazenam a imagem que vai ser colocada de plano de fundo
LinkedList *cars;            // Variável para armazenar a lista encadeada dos carros da corrida
Camera2D   *camera;

void load_map(Map map) {
    switch (map) {
    case INTERLAGOS:
        trackBackground =
            LoadTexture("resources/masks/interlagos_maskV2.png"); // converte em textura// Definindo
                                                                  // o frame rate em 60
        // Carregando a imagem da máscara de pixels
        Track_setMask("resources/masks/interlagos_maskV2.png");
        Track_setAreas(TRACK_AREAS, 4);
        Track_setCheckpoints(CHECKPOINTS, 3);
        Track_setOutsideColor(OUTSIDE_TRACK_COLOR);

        state.map = INTERLAGOS;
        break;
    }

    Camera_Screen_setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    Camera_Background_setSize(trackBackground.width, trackBackground.height);
}

void setup_game(Mode mode) {
    load_map(INTERLAGOS);

    cars = LinkedList_create();

    Car *player = Car_create((Vector2) {5400, 2000}, // pos
                             2.66,                   // angulo inicial do carro

                             0.3,  // aceleracao do carro
                             0.2,  // força da marcha ré
                             0.02, // força de frenagem

                             0.035, // aceleração angular (velocidade de rotação)
                             0.2,   // velocidade mínima para fazer curva

                             125, // largura
                             75,  // altura

                             "resources/cars/carroazul.png", // path da textura
                             1                               // id do carro
    );

    LinkedList_addCar(cars, player); // Adicionando o carro criado na lista encadeada

    camera = Camera_create(player->pos, (Vector2) {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, 0.0f,
                           0.5f);
}

void cleanup_game() {
    CloseWindow();                  // Fechar a janela gráfica 2d
    UnloadTexture(trackBackground); // Liberando a textura da imagem do plano de fundo
    Track_Unload();                 // função que deve liberar o trackMask e trackPixels
    Camera_free(camera);
    LinkedList_free(cars); // Libera a memória da lista encadeada de carros
}

void update_game() {
    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    Car_move(player, KEY_W, KEY_S, KEY_D,
             KEY_A); // Movendo o carro do player 2 de acordo com essas teclas

    LinkedList_forEach(
        cars,
        Car_update); // Jogando a função Car_update(Car* car); para cada carro da lista encadeada

    Camera_updateTarget(camera, player); // Atualizando a posição da camera
}

void draw_game() {
    BeginMode2D(*camera);

    DrawTexture(trackBackground, 0, 0, WHITE); // desenha pista como fundo

    Car *player = LinkedList_getCarById(cars, 1); // Pegando o carro com id 1 da lista encadeada

    Car_showInfo(player, player->pos.x - (SCREEN_WIDTH / 2), player->pos.y - (SCREEN_HEIGHT / 2),
                 50,
                 BLACK); // Mostrando as informações do carro com id 1

    LinkedList_forEach(
        cars, Car_draw); // Jogando a função Car_draw(Car* car); para cada carro da lista encadeada

    EndMode2D();
}
