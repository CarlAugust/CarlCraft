#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 256
#define BLOCK_SIZE 3.0f
#define CHUNK_VOLUME CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_WIDTH
#define CHUNK_MAX_VERTS CHUNK_VOLUME * 8

typedef enum BlockId {
    AIR, DIRT, STONE
} BlockId;

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
    BlockId blocks[];
} Chunk;

Chunk* ChunkCreate();
void ChunkDraw(Chunk* chunk);
Model GenMeshTest(Chunk chunk);
