#include "utils.h"
#include <raymath.h>
#include <stdio.h>

Vector2 accelerate(Vector2 vector, float acceleration, float delta) {
  if (Vector2Length(vector) == 0.0f)
    return vector;
  return Vector2Add(
      vector, Vector2Scale(Vector2Normalize(vector), acceleration * delta));
}

void move(Movement *movement, float delta) {
  movement->position.x += movement->velocity.x * delta;
  movement->position.y += movement->velocity.y * delta;
}

void update_collision_rect(Rectangle *collision_rect, Vector2 at) {
  collision_rect->x = at.x - (collision_rect->width / 2.0f);
  collision_rect->y = at.y - (collision_rect->height / 2.0f);
}

Rectangle create_centred_rectangle(int x, int y, Vector2 sizes) {
  return (Rectangle){x - (sizes.x / 2), y - (sizes.y / 2), sizes.x, sizes.y};
}

Vector2 rectangle_centre(Rectangle rectangle) {
  return (Vector2){rectangle.x + (rectangle.width / 2.0f),
                   rectangle.y + (rectangle.height / 2.0f)};
}

void log_warning(char *s) {
  fprintf(stderr, TERM_WARNING "[WARNING] " TERM_NORMAL "%s", s);
}

void log_error(char *s) {
  fprintf(stderr, TERM_ERROR "[ERROR] " TERM_NORMAL "%s", s);
}
