#include "raylib.h"
#include <stdio.h>
#include <chunk.h>
#include <stdlib.h>

typedef enum {
    START_SCREEN, PAUSE_MENU, PLAYING
} States;

void AddCube(Vector3 pos, float size, Mesh mesh) {
    // Half size for easier math (cube centered at pos)
    float hs = size / 2.0f;

    // Define the 8 cube corners
    Vector3 verts[8] = {
        { pos.x - hs, pos.y - hs, pos.z - hs }, // 0: left  bottom back
        { pos.x + hs, pos.y - hs, pos.z - hs }, // 1: right bottom back
        { pos.x + hs, pos.y + hs, pos.z - hs }, // 2: right top    back
        { pos.x - hs, pos.y + hs, pos.z - hs }, // 3: left  top    back
        { pos.x - hs, pos.y - hs, pos.z + hs }, // 4: left  bottom front
        { pos.x + hs, pos.y - hs, pos.z + hs }, // 5: right bottom front
        { pos.x + hs, pos.y + hs, pos.z + hs }, // 6: right top    front
        { pos.x - hs, pos.y + hs, pos.z + hs }  // 7: left  top    front
    };

    // Each face is 2 triangles → indices into verts[]
    int faces[36] = {
        // back
        0,1,2,  0,2,3,
        // front
        4,6,5,  4,7,6,
        // left
        0,3,7,  0,7,4,
        // right
        1,5,6,  1,6,2,
        // bottom
        0,4,5,  0,5,1,
        // top
        3,2,6,  3,6,7
    };

    for (int i = 0; i < 36; i++) {
        mesh.vertices[i*3 + 0] = verts[faces[i]].x;
        mesh.vertices[i*3 + 1] = verts[faces[i]].y;
        mesh.vertices[i*3 + 2] = verts[faces[i]].z;

        // Simple normals (just placeholder: up)
        mesh.normals[i*3 + 0] = 0;
        mesh.normals[i*3 + 1] = 1;
        mesh.normals[i*3 + 2] = 0;

        // Simple texcoords (could be improved)
        mesh.texcoords[i*2 + 0] = (i % 2 == 0) ? 0.0f : 1.0f;
        mesh.texcoords[i*2 + 1] = (i % 3 == 0) ? 0.0f : 1.0f;
    }
}

Model GenMeshTest() {
    Mesh mesh = { 0 };

    mesh.triangleCount = 12;         // 6 faces * 2 triangles
    mesh.vertexCount = 36;           // 12 triangles * 3 vertices

    mesh.vertices = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount*2*sizeof(float));
    mesh.normals  = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));

    // Generate cube at (0,0,0) with size 2
    AddCube((Vector3){0,0,0}, 2.0f, mesh);

    UploadMesh(&mesh, false);
    return LoadModelFromMesh(mesh);
}

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

    // Chunk* chunk = ChunkCreate();
    Model model = GenMeshTest(); 
    while (!WindowShouldClose())
    {
        // Variables: Gamestate ect...
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        if (IsWindowFocused && PLAYING) {
            DisableCursor();
        }

        if (IsKeyDown(KEY_SPACE)) {
            camera.position.y += 0.1;
        }
        if (IsKeyDown(KEY_LEFT_CONTROL)) {
            camera.position.y += -0.1;
        }

        BeginDrawing();

        ClearBackground(DARKBLUE);
        BeginMode3D(camera);
        // ChunkDraw(chunk);
        DrawModel(model, (Vector3){0,0,0}, 1.0f, WHITE);

        EndMode3D();
        DrawFPS(10, 10);
        EndDrawing();
    }

    // free(chunk);
    CloseWindow();
    return 0;
}


