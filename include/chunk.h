#pragma once

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 64
#define BLOCK_SIZE 3.0f
#define CHUNK_VOLUME CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH
#define CHUNK_MAX_VERTS CHUNK_VOLUME * 8
#define WORLDGEN_SEED 23173

#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <arena.h>
#include <perlin.h>
#include <threads.h>
#include <time.h>

typedef enum BlockId {
    AIR,
    DIRT, GRASS, STONE,


    BLOCKID_ENUM_SIZE // end of enum
} BlockId;
#define BLOCK_TEXTURE_WIDTH 8
#define BLOCK_TEXTURE_HEIGHT 24
#define BLOCK_TEXTURE_ATLAS_ROW_COUNT (BLOCK_TEXTURE_HEIGHT / BLOCK_TEXTURE_WIDTH)

/*
    @position: The lowest left left position of a block. So for a chunch spanning x: 0-32, y 0-64, z 0-32,
    the position would be 0, 0, 0
*/
typedef struct Chunk {
    Vector3 position;
    int width;
    int height;
    int volume;
    Mesh mesh;
    Model model;
    BlockId* blocks;
} Chunk;

// TODO: 
/*
Instead of using a queue i simply use a stack for simplicity sake as to define chunk gen order i have to 
define while looping through anyways. 
Then the main game loop will loop through all chunks then notify through a cnd_t that 
chunks are ready to be loaded which wakes up the threads that take the top element of the stack
boom!!
Then the threads generate the chunks.

*/

typedef struct ChunksUnusedStack {
    mtx_t lock;
    int current;
    int capacity;
    int bucket[];
} ChunksUnusedStack;

// This is used by the threads
int ChunksUnusedStackTop(ChunksUnusedStack* stack);

// This is used by the main game loop
int ChunksUnusedStackAdd(ChunksUnusedStack* stack, int idx);
ChunksUnusedStack* ChunksUnusedStackCreate(Arena* arena, int size);


typedef struct ChunkManager {
    int count;
    int workerCount;
    thrd_t* workerPool;
    volatile running;
    ChunksUnusedStack* stack;
    Chunk** chunks;
} ChunkManager;

#define CHUNK_RENDER_DISTACE 4

Chunk* ChunkCreate(Arena* arena, Vector3 position);
void ChunkDraw(Chunk* chunk);
ChunkManager* ChunkManagerCreate(Arena* arena, int renderDistance, Texture atlas);
void ChunkWorkerLoop(void* voidedManager);

void FreeMeshBuffers();
Texture BlockLoadTexturePackAtlas();