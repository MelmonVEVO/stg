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

typedef struct {
  Vector2 position;
  Vector2 velocity;
} Movement;

inline Vector2 accelerate(Vector2 vector, float acceleration, float delta) {
  if (Vector2Length(vector) == 0)
    return vector;
  return Vector2Add(
      vector, Vector2Scale(Vector2Normalize(vector), acceleration * delta));
}

#endif
