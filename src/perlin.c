// Generating perlin noise

#include <raylib.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

unsigned int hash(int x, int y, int seed) {
	unsigned int h = x * 374761393 + y * 668265263 + seed * 69069;
	h = (h ^ (h >> 13)) * 1274126177;
	return h ^ (h >> 16);
}

float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float dot2d(Vector2 v1, Vector2 v2) {
    return v1.x * v2.x + v1.y * v2.y;
}


Vector2 randomGradient2D(int x, int y, int seed) {
	unsigned int h = hash(x, y, seed);
    float angle = (h % 360) / (PI / 180.0f);
    return (Vector2){cosf(angle), sinf(angle)};
}

float perlinNoiseSample2d(Vector2 p, int seed) {

    const double xf = fmodf(p.x, 1.0f);
    const double yf = fmodf(p.y, 1.0f);

    Vector2 p1 = {xf, yf};
    Vector2 p2 = {xf - 1, yf};
    Vector2 p3 = {xf - 1, yf - 1};
    Vector2 p4 = {xf, yf - 1};

    // Random gradient Vectors
    Vector2 r1 = randomGradient2D(xf, yf, seed);
    Vector2 r2 = randomGradient2D(xf + 1, xf, seed);
    Vector2 r3 = randomGradient2D(xf + 1, yf + 1, seed);
    Vector2 r4 = randomGradient2D(xf, yf + 1, seed);

    float u = fade(xf);
    float v = fade(yf);

    float l1 = lerp(dot2d(p1, r1), dot2d(p2, r2), u);
    float l2 = lerp(dot2d(p3, r3), dot2d(p4, r4), u);
    return lerp(l1, l2, v);
}

/* Returns a float 0 - 1, used for surface generation
    @p: positional vector; p.x = block.position.x, p.y = block.position.z NOTE: z maps to y
*/
float PerlinNoise2d(Vector2 p, int octaves, float persistence, int seed) {
    float total = 0.0f;
    float amplitude = 1.0f;
    float maxAmplitude = 0.0f;
    float frequency = 1.0f;

    for (int i = 0; i < octaves; i++)
	{
		float noise = perlinNoiseSample2d((Vector2){p.x * frequency * 0.02f, p.y * frequency * 0.02f}, seed);
		total += noise * amplitude;
		maxAmplitude += amplitude;
		amplitude *= persistence;
		frequency *= 2.0f;
	}

    // Return clamped to [0, 1]
	return ((total / maxAmplitude) + 1.0f) * 0.5f;
}
