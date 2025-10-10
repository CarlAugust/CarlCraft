//Render chunks somehow
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include <chunk.h>
#include <stdio.h>
#include <string.h>
#include <arena.h>
#include <perlin.h>

#define X(i) ((i) % (CHUNK_WIDTH))
#define Y(i) (((i) / (CHUNK_WIDTH)) % (CHUNK_HEIGHT))
#define Z(i) ((i) / ((CHUNK_WIDTH) * (CHUNK_HEIGHT)))
#define I(x,y,z) ((x) + ((y) * (CHUNK_WIDTH)) + ((z) * (CHUNK_HEIGHT) * (CHUNK_WIDTH)))

static float* verteciesBuffer = NULL;
static float* texcoordsBuffer = NULL;
static float* normalsBuffer = NULL;

int prepMeshBuffers() {
    int maxVertecies = CHUNK_VOLUME * 36;
    if (verteciesBuffer == NULL) {
        verteciesBuffer = MemAlloc(maxVertecies * 3 * sizeof(float));
        if (!verteciesBuffer) return -1;
    }
    if (texcoordsBuffer == NULL) {
        texcoordsBuffer = MemAlloc(maxVertecies * 2 * sizeof(float));
        if (!texcoordsBuffer) return -1;
    }
    if (normalsBuffer == NULL) {
        normalsBuffer = MemAlloc(maxVertecies * 3 * sizeof(float));
        if (!normalsBuffer) return -1;
    }
    return 0;
}

void FreeMeshBuffers() {
    MemFree(verteciesBuffer);
    MemFree(texcoordsBuffer);
    MemFree(normalsBuffer);
}

static const int FACE_INDICES[6][6] = {
    // { 0, 1, 2,  0, 2, 3 }, // Z -
    { 0, 2, 1,  0, 3, 2 }, // Z -
    { 4, 5, 6,  4, 6, 7 }, // Z + 
    { 0, 7, 3,  0, 4, 7 }, // X -
    { 1, 6, 5,  1, 2, 6 }, // X +
    { 0, 5, 4,  0, 1, 5 }, // Y -
    { 3, 6, 2,  3, 7, 6 } // Y +
};

struct mesh_data {
    float* vertices;
    float* uvs;
    float* norms;
} mesh_data;

// @r: relative position, @v: vertices
static int AddVert(float* vertices, Vector3 r, Vector3 v, int i) {
    vertices[i*3 + 0] = r.x + v.x;
    vertices[i*3 + 1] = r.y + v.y;
    vertices[i*3 + 2] = r.z + v.z;
    return i + 1;
}

Chunk* ChunkCreate(Arena* arena, Vector3 position) { 
    if (prepMeshBuffers() == -1) {
        // TODO: DO SOME ERROR ACTION
        return NULL;
    }

    // TODO: Ideally should be allocated to some sort of chunk arena, to prevent reallocation
    Chunk* chunk = Arena_alloc(arena, sizeof(Chunk) + sizeof(BlockId) * (CHUNK_VOLUME));
    if (!chunk) {
        perror("ERROR ALLOCATING");
        return NULL;
    }
    
    chunk->position = position;
    chunk->volume = CHUNK_VOLUME;
    chunk->height = CHUNK_HEIGHT;
    chunk->width = CHUNK_WIDTH;

    // Also has to change to use seed system or something
    int W = CHUNK_WIDTH;
    int H = CHUNK_HEIGHT;
    float height;

    // Make all empty
    for (int i = 0; i < CHUNK_VOLUME; i++) {
        chunk->blocks[i] = AIR;
    }

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_WIDTH; z++) {
            float zheight = PerlinNoise2d((Vector2) { x + chunk->position.x, z + chunk->position.z }, 12, 0.7, 1010);
            height = zheight * CHUNK_HEIGHT;
            printf("Z %f;;;; H %f\n", zheight, height);
            for (int y = 0; y < height; y++) {
                chunk->blocks[I(x, y, z)] = DIRT;
            }
        }
    }

    Mesh mesh = { 0 };

    // TODO: IS there a better number to use here then max amount of vertexes?
    mesh.vertexCount = CHUNK_VOLUME * 36;

    Vector3 verts[8] = {
        (Vector3){0,0,0},
        (Vector3){BLOCK_SIZE, 0, 0},
        (Vector3){BLOCK_SIZE, BLOCK_SIZE, 0},
        (Vector3){0, BLOCK_SIZE, 0},
        (Vector3){0,0, BLOCK_SIZE},
        (Vector3){BLOCK_SIZE, 0, BLOCK_SIZE},
        (Vector3){BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE},
        (Vector3){0, BLOCK_SIZE, BLOCK_SIZE}
    };     


    int x, y, z, currentVert = 0;
    for (int i = 0; i < chunk->volume; i++) {
        if (chunk->blocks[i] == AIR) {
            continue;
        }

        x = X(i);
        y = Y(i);
        z = Z(i);
        Vector3 rel = {x * BLOCK_SIZE, y * BLOCK_SIZE, z * BLOCK_SIZE};
        int index = I(x, y, z + 1);
        bool back  = (z == 0)                     || (chunk->blocks[I(x,y,z-1)] == AIR); // z-
        bool front = (z == CHUNK_WIDTH - 1)       || (chunk->blocks[I(x,y,z+1)] == AIR); // z+
        bool left  = (x == 0)                     || (chunk->blocks[I(x-1,y,z)] == AIR); // x-
        bool right = (x == CHUNK_WIDTH - 1)       || (chunk->blocks[I(x+1,y,z)] == AIR); // x+
        bool bottom= (y == 0)                     || (chunk->blocks[I(x,y-1,z)] == AIR); // y-
        bool top   = (y == CHUNK_HEIGHT - 1)      || (chunk->blocks[I(x,y+1,z)] == AIR); // y+

        bool facingWall[6] = {back, front, left, right, bottom, top};

        for (int f = 0; f < 6; f++) {
            if (!facingWall[f]) {
                continue;
            }

            for (int k = 0; k < 6; k++) {
                int vi = FACE_INDICES[f][k];
                currentVert = AddVert(verteciesBuffer, rel, verts[vi], currentVert);
            }
        }
    }

    mesh.vertexCount = currentVert;
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount*2*sizeof(float));
    mesh.normals  = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));    
    memcpy(mesh.vertices, verteciesBuffer, mesh.vertexCount*3*sizeof(float));

    // TODO: THERE IS PROBOBLY WORK TO BE DONE EHRE
    UploadMesh(&mesh, false);
    chunk->mesh = mesh;

    return chunk;
};

void ChunkDraw(Chunk* chunk) {
    for (int i = 0; i < chunk->volume; i++) {

        if (chunk->blocks[i] == AIR) continue;
        int x = X(i);
        int y = Y(i);
        int z = Z(i);

        DrawCubeWires((Vector3){x * BLOCK_SIZE + BLOCK_SIZE / 2, y * BLOCK_SIZE + BLOCK_SIZE / 2 , z * BLOCK_SIZE + BLOCK_SIZE / 2}, BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, BLACK);
    }
}