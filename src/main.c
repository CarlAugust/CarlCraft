#include "raylib.h"
#include <stdio.h>
#include <chunk.h>
#include <stdlib.h>
#include <time.h>

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
    
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(144);

    clock_t start = clock();
    Chunk* chunk = ChunkCreate();
    clock_t end = clock();
    double minutes = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\n\nTOTAL %f\n\n", minutes);
    Model chunkModel = LoadModelFromMesh(chunk->mesh);
    chunkModel.meshes[0].vertices;
    Vector3 rel = chunk->position;
    
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
        DrawModel(chunkModel, chunk->position, 1.0f, PURPLE);
        DrawModelWires(chunkModel, chunk->position, 1.0f, BLACK);
        DrawCube((Vector3){0,0,0}, 1, 1, 1, BLUE);

        EndMode3D();

        DrawFPS(10, 10);
        DrawText(TextFormat("X: %f, Y: %f, Z: %f", camera.position.x, camera.position.y, camera.position.z), 10, 40, 20, BLACK);
        EndDrawing();   
    }
    
    free(chunk);
    UnloadModel(chunkModel);

    FreeMeshBuffers();
    CloseWindow();
    return 0;
}


