#pragma once

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 32
#define BLOCK_SIZE 3.0f
#define CHUNK_VOLUME CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH
#define CHUNK_MAX_VERTS CHUNK_VOLUME * 8

#include <stdlib.h>
#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <arena.h>
#include <perlin.h>

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
    BlockId blocks[];
} Chunk;


typedef struct ChunkManager {
    int count;
    Chunk** chunks;
} ChunkManager;

#define CHUNK_RENDER_DISTACE 8

Chunk* ChunkCreate(Arena* arena, Vector3 position);
void ChunkDraw(Chunk* chunk);
ChunkManager* ChunkManagerCreate(Arena* arena, int renderDistance, Texture atlas);
void FreeMeshBuffers();
Texture BlockLoadTexturePackAtlas();