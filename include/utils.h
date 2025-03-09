#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>
#include <raymath.h>

#define VECTOR2UP (Vector2){0, -1.0f}
#define VECTOR2DOWN (Vector2){0, 1.0f}
#define VECTOR2LEFT (Vector2){-1.0f, 0}
#define VECTOR2RIGHT (Vector2){1.0f, 0}
#define TAU (PI * 2)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define VIEWPORT_WIDTH 400.0f
#define VIEWPORT_HEIGHT 300.0f
#define MAX_ENEMIES 100

typedef struct {
  Vector2 position;
  Vector2 velocity;
} Movement;

typedef struct {
  Movement movement;
  bool dead;
} PlayerData;

typedef short Flags;

Vector2 accelerate(Vector2 vector, float acceleration, float delta);

void move(Movement *movement, float delta);

Rectangle create_centred_rectangle(int x, int y, Vector2 sizes);

#endif
