//Render chunks somehow

#include <chunk.h>

#define X(i) ((i) % (CHUNK_WIDTH))
#define Y(i) (((i) / (CHUNK_WIDTH)) % (CHUNK_HEIGHT))
#define Z(i) ((i) / ((CHUNK_WIDTH) * (CHUNK_HEIGHT)))
#define I(x,y,z) ((x) + ((y) * (CHUNK_WIDTH)) + ((z) * (CHUNK_HEIGHT) * (CHUNK_WIDTH)))
#define WORLDGEN_SEED 1001

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
    { 5, 7, 4,  5, 6, 7 }, // Z + 
    { 4, 3, 0,  4, 7, 3 }, // X -
    { 1, 6, 5,  1, 2, 6 }, // X +
    { 1, 4, 0,  1, 5, 4 }, // Y -
    { 3, 6, 2,  3, 7, 6 } // Y +
};

static const Vector2 faceTexcoords[6] = {
    {0.0f, 1.0f }, // Bottom-left
    { 1.0f, 0.0f }, // Top-right
    { 1.0f, 1.0f }, // Bottom-right
    {0.0f, 1.0f }, // Bottom-left
    { 0.0f, 0.0f },  // Top-left
    { 1.0f, 0.0f}, // Top-right
};

static Vector3 cubeNormals[6] = {
    {0.0f, 0.0f, 1.0f}, // Front
    {1.0f, 0.0f, 0.0f}, // Right
    {0.0f, 0.0f, -1.0f}, // Back
    {-1.0f, 0.0f, 0.0f}, // Left
    {0.0f, -1.0f, 0.0f}, // Bottom
    {0.0f, 1.0f, 0.0f}, // Top
};

struct mesh_data {
    float* vertices;
    float* uvs;
    float* norms;
} mesh_data;

// @r: relative position, @v: vertices
void addVert(float* vertices, Vector3 r, Vector3 v, int i) {
    vertices[i*3 + 0] = r.x + v.x;
    vertices[i*3 + 1] = r.y + v.y;
    vertices[i*3 + 2] = r.z + v.z;
}

void addTexcoords(float* texcoords, Vector2 uv, int i) {
	texcoords[i * 2 + 0] = uv.x;
	texcoords[i * 2 + 1] = uv.y;
}

void addNormals(float* normals, Vector3 n, int i) {
	normals[i * 3 + 0] = n.x;
	normals[i * 3 + 1] = n.y;
	normals[i * 3 + 2] = n.z;
}

void ChunkGeneration(Chunk* chunk) {
    float height;
    // Make all empty
    for (int i = 0; i < CHUNK_VOLUME; i++) {
        chunk->blocks[i] = AIR;
    }

    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_WIDTH; z++) {
            height = PerlinNoise2d((Vector2) { x + chunk->position.x / BLOCK_SIZE, z + chunk->position.z / BLOCK_SIZE }, 12, 0.5, WORLDGEN_SEED) * CHUNK_HEIGHT;
            for (int y = 0; y < height - 1; y++) {
                if (height - y < 5) {    
                    chunk->blocks[I(x, y, z)] = DIRT;
                } else {
                    chunk->blocks[I(x, y, z)] = STONE;
                }
            }
            chunk->blocks[I(x, (int)(height - 1), z)] = GRASS;
        }
    }
}

void ChunkMeshGen(Chunk* chunk ) {
    Mesh mesh = { 0 };
    if (prepMeshBuffers() == -1) {
        return;
    }

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
    float tileWidth = 1.0f / BLOCK_TEXTURE_ATLAS_ROW_COUNT;
    float tileHeight = 1.0f / 3; 

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
                addVert(verteciesBuffer, rel, verts[vi], currentVert);
                addNormals(normalsBuffer, cubeNormals[f], currentVert);

                float uOffset = (chunk->blocks[i] - 1) * tileWidth;
                float vOffset = 0.0f;

                if (f == 5) {
                    vOffset = 2 * tileHeight;
                } else if (f == 4) {
                    vOffset = 1 * tileHeight;
                } else {
                    vOffset = 0 * tileHeight;
                }

                Vector2 uv = faceTexcoords[k];
                uv.x = uOffset + uv.x * tileWidth;
                uv.y = vOffset + uv.y * tileHeight;

                addTexcoords(texcoordsBuffer, uv, currentVert);
                currentVert++;
            }
        }
    }

    mesh.vertexCount = currentVert;
    mesh.vertices = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));
    mesh.texcoords = (float *)MemAlloc(mesh.vertexCount*2*sizeof(float));
    mesh.normals = (float *)MemAlloc(mesh.vertexCount*3*sizeof(float));    
    memcpy(mesh.vertices, verteciesBuffer, mesh.vertexCount*3*sizeof(float));
    memcpy(mesh.texcoords, texcoordsBuffer, mesh.vertexCount*2*sizeof(float));
    memcpy(mesh.normals, normalsBuffer, mesh.vertexCount*3*sizeof(float));

    UploadMesh(&mesh, false);
    chunk->mesh = mesh;
}

Chunk* ChunkCreate(Arena* arena, Vector3 position) { 

    Chunk* chunk = Arena_alloc(arena, sizeof(Chunk) + sizeof(BlockId) * (CHUNK_VOLUME));
    if (!chunk) {
        perror("ERROR ALLOCATING");
        return NULL;
    }
    
    chunk->position = position;
    chunk->volume = CHUNK_VOLUME;
    chunk->height = CHUNK_HEIGHT;
    chunk->width = CHUNK_WIDTH;

    ChunkGeneration(chunk);
    ChunkMeshGen(chunk);
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

ChunkManager* ChunkManagerCreate(Arena* arena, int renderDistance, Texture atlas) {
    int chunkCount = renderDistance * renderDistance; // Its going to be rendered by area despite using radius for calculations
    ChunkManager* manager = Arena_alloc(arena, sizeof(ChunkManager));
    if (manager == NULL) {
        return NULL;
    }

    manager->count = chunkCount;
    manager->chunks = Arena_alloc(arena, chunkCount * sizeof(Chunk));

    for (int i = 0; i < chunkCount; i++) {
        manager->chunks[i] = ChunkCreate(arena, (Vector3){(i % renderDistance) * CHUNK_WIDTH * BLOCK_SIZE - CHUNK_WIDTH * BLOCK_SIZE * renderDistance / 2, 0, ((int)(i / renderDistance)) * CHUNK_WIDTH * BLOCK_SIZE - CHUNK_WIDTH * BLOCK_SIZE * renderDistance / 2});
        if (manager->chunks[i] == NULL) {
            return NULL;
        }
		Mesh mesh = manager->chunks[i]->mesh;
        manager->chunks[i]->model = LoadModelFromMesh(manager->chunks[i]->mesh);
        manager->chunks[i]->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = atlas;
    }

    return manager;
}



// This is techincally a rather inefficent solution, though i dont know what else to do
static char* blockToString(BlockId id) {
    switch(id) {
        case DIRT:
            return "dirt";
        case GRASS:
            return "grass";
        case STONE:
            return "stone";
        default:
            return "";
    }
}

Texture BlockLoadTexturePackAtlas() {
    Image atlas = GenImageColor(BLOCK_TEXTURE_WIDTH * BLOCK_TEXTURE_ATLAS_ROW_COUNT, BLOCK_TEXTURE_HEIGHT, BLANK);
    BlockId block = DIRT;
    char* blockName;
    Image blockImg;

    while(block != BLOCKID_ENUM_SIZE) {
        blockName = blockToString(block);
        blockImg = LoadImage(TextFormat("resources/%satlas.png", blockName));

        if(blockImg.height == 0 && blockImg.width == 0) {
            printf("[ENGINE]: DID NOT FIND TEXTURE FOR %s, ERROR!!!\n", blockName);
            blockImg = GenImageChecked(BLOCK_TEXTURE_WIDTH, BLOCK_TEXTURE_HEIGHT, BLOCK_TEXTURE_WIDTH / 2, BLOCK_TEXTURE_HEIGHT / 2, PURPLE, BLACK);
        }

        ImageDraw(&atlas, blockImg, (Rectangle){0,0,8,24}, (Rectangle){BLOCK_TEXTURE_WIDTH * (block - 1), 0, BLOCK_TEXTURE_WIDTH, BLOCK_TEXTURE_HEIGHT}, WHITE);
        UnloadImage(blockImg);
        block++;
    }

    Texture tatlas = LoadTextureFromImage(atlas);
    UnloadImage(atlas);
    return tatlas;
}