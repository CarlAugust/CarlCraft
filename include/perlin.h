#pragma once
#include <raylib.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>

float PerlinNoise2d(Vector2 p, int octaves, float persistence, int seed);
