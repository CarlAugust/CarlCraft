//Render chunks somehow
#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include <chunk.h>
#include <stdio.h>

#define X(i,w) ((i) % (w))
#define Y(i, w, h) (((i) / (w)) % (h))
#define Z(i, w, h) ((i) / ((w) * (h)))
#define I(x,y,z,w,h) ((x) + ((y) * (h)) + ((z) * (h) * (w)))

struct mesh_data {
    float* vertices;
    float* uvs;
    float* norms;
} mesh_data;



Chunk* ChunkCreate(Vector3 position) { 
    // TODO: Ideally should be allocated to some sort of chunk arena, to prevent reallocation
    Chunk* chunk = MemAlloc(sizeof(Chunk) + sizeof(BlockId) * (CHUNK_VOLUME));
    if (!chunk) {
        perror("ERROR ALLOCATING");
        return NULL;
    }
    
    chunk->volume = CHUNK_VOLUME;
    chunk->height = CHUNK_HEIGHT;
    chunk->width = CHUNK_WIDTH;

    // Also has to change to use seed system or something
    int W = CHUNK_WIDTH;
    int H = CHUNK_HEIGHT;
    for (int i = 0; i < chunk->volume; i++) {
        chunk->blocks[i] = rand() % 2;
        // chunk->blocks[i].position.x = (float)(i % W) * BLOCK_SIZE;
        // chunk->blocks[i].position.y = (float)((i / W) % H) * BLOCK_SIZE;
        // chunk->blocks[i].position.z = (float)(i / (H * W)) * BLOCK_SIZE;
    }

    int x, y, z, i2;
    for (int i = 0; i < chunk->volume; i++) {
        x = X(i, CHUNK_WIDTH);
        y = Y(i, CHUNK_WIDTH, CHUNK_HEIGHT);
        z = Z(i, CHUNK_WIDTH, CHUNK_HEIGHT);

        // We figure out what faces should be produces
        // Then we add all the faces to the mesh buffer and its potential uvs
        // Also remember to count the faces
    }

    // Load the mesh... done... or something

    return chunk;
};

void ChunkDraw(Chunk* chunk) {
}