#include "utils.h"

Vector2 accelerate(Vector2 vector, float acceleration, float delta) {
  return Vector2Add(
      vector, Vector2Scale(Vector2Normalize(vector), acceleration * delta));
}

void move(Movement *movement, float delta) {
  movement->position.x += movement->velocity.x * delta;
  movement->position.y += movement->velocity.y * delta;
}

Rectangle create_centred_rectangle(int x, int y, Vector2 sizes) {
  return (Rectangle){x - (sizes.x / 2), y - (sizes.y / 2), sizes.x, sizes.y};
}
