#include "raylib.h"
#include <stdio.h>
#include <chunk.h>
#include <stdlib.h>
#include <time.h>
#include <arena.h>

typedef enum {
    START_SCREEN, PAUSE_MENU, PLAYING
} States;

int main(void)
{
    int screenWidth = 1920;
    int screenHeight = 1080;

    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 0.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;         
    
    InitWindow(screenWidth, screenHeight, "Raylib [core] example - basic window");

    SetTargetFPS(144);

    // SETUP ARENA BUFFER
    Arena* bufferArena = Arena_init(MB(1000));

    Chunk* chunk[4][4];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            chunk[i][j] = ChunkCreate(bufferArena, (Vector3){i * CHUNK_WIDTH * BLOCK_SIZE, 0, j * CHUNK_WIDTH * BLOCK_SIZE});
            chunk[i][j]->model = LoadModelFromMesh(chunk[i][j]->mesh);
        }
    }
    
    float speed = 1.0f;
    while (!WindowShouldClose())
    {
        speed = 1.0f;
        float dx = GetFrameTime();

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        if (IsWindowFocused && PLAYING) {
            DisableCursor();
        }

        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            speed = 5.0f;
        }

        if (IsKeyDown(KEY_SPACE)) {
            camera.position.y += 20 * dx * speed;
            camera.target.y += 20 * dx * speed;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.position.y += -20 * dx * speed;
            camera.target.y += -20 * dx * speed;
        }

        BeginDrawing();

        ClearBackground(WHITE);

        BeginMode3D(camera);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                DrawModel(chunk[i][j]->model, chunk[i][j]->position, 1.0f, (Color){200 + i * 20, 122 + j * 30, 255, 255});
                DrawModelWires(chunk[i][j]->model, chunk[i][j]->position, 1.0f, BLACK);
            }
        }
        DrawCube((Vector3){0,0,0}, 1, 1, 1, BLUE);

        EndMode3D();

        DrawFPS(10, 10);
        DrawText(TextFormat("X: %f, Y: %f, Z: %f", camera.position.x, camera.position.y, camera.position.z), 10, 40, 20, BLACK);
        EndDrawing();   
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            UnloadModel(chunk[i][j]->model);
        }
    }

    free(bufferArena);


    FreeMeshBuffers();
    CloseWindow();
    return 0;
}


