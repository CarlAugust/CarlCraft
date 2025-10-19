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

    SetTargetFPS(1440);

    // SETUP ARENA BUFFER
    Arena* bufferArena = Arena_init(MB(100));
    if (bufferArena == NULL) {
        goto exit_program;
    }
    Texture atlasTexture = BlockLoadTexturePackAtlas();
    ChunkManager* chunkManager = ChunkManagerCreate(bufferArena, CHUNK_RENDER_DISTACE, atlasTexture);
    if (chunkManager == NULL) {
        goto exit_program;
    }
    
    float speed = 1.0f;

    while (!WindowShouldClose())
    {
        speed = 1.0f;
        float dt = GetFrameTime();

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        if (IsWindowFocused && PLAYING) {
            DisableCursor();
        }

        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            speed = 5.0f;
        }

        if (IsKeyDown(KEY_SPACE)) {
            camera.position.y += 20 * dt * speed;
            camera.target.y += 20 * dt * speed;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.position.y += -20 * dt * speed;
            camera.target.y += -20 * dt * speed;
        }
                        



        BeginDrawing();

        ClearBackground(WHITE);

        BeginMode3D(camera);

        for (int i = 0; i < chunkManager->count; i++) {
            DrawModel(chunkManager->chunks[i]->model, chunkManager->chunks[i]->position, 1.0f, WHITE);
			Vector3 pos = chunkManager->chunks[i]->position;
			pos.x += (CHUNK_WIDTH * BLOCK_SIZE) / 2;
			pos.z += (CHUNK_WIDTH * BLOCK_SIZE) / 2;
			pos.y += (CHUNK_HEIGHT * BLOCK_SIZE) / 2;
			DrawCubeWires(pos, CHUNK_WIDTH * BLOCK_SIZE, CHUNK_HEIGHT * BLOCK_SIZE, CHUNK_WIDTH * BLOCK_SIZE, RED);
        }
        
        EndMode3D();
        DrawFPS(10, 10);
        DrawText(TextFormat("X: %f, Y: %f, Z: %f", camera.position.x, camera.position.y, camera.position.z), 10, 40, 20, BLACK);
        EndDrawing();   
    }

    goto exit_program;

    exit_program:
        UnloadTexture(atlasTexture);
        if (chunkManager != NULL) {
            for (int i = 0; i < chunkManager->count; i++) {
                UnloadModel(chunkManager->chunks[i]->model);
            }
        }
        free(bufferArena);
        FreeMeshBuffers();
        CloseWindow();
        return 0;

    return 0;
}


