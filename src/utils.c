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

Rectangle create_centred_rectangle(float x, float y, Vector2 sizes) {
  return (Rectangle){x - (sizes.x * 0.5f), y - (sizes.y * 0.5f), sizes.x,
                     sizes.y};
}

Vector2 rectangle_centre(Rectangle rectangle) {
  return (Vector2){rectangle.x + (rectangle.width / 2.0f),
                   rectangle.y + (rectangle.height / 2.0f)};
}

Stack initialise_stack(size_t capacity, size_t element_sizeof) {
  Stack new = {.capacity = capacity, .top = 0};
  new.items = malloc(element_sizeof * capacity);
  return new;
}

void free_stack(Stack *stack) { free(stack->items); }

void *pop_stack(Stack *stack) {
  if (stack->top == -1UL) {
    log_warning("Stack empty, skipping.");
    return NULL;
  }
  void *returned_item = stack->items[stack->top--];
  return returned_item;
}

void push_stack(Stack *stack, void *item) {
  if (stack->top >= stack->capacity - 1) {
    log_error("Tried to push to a full stack. Your code cocked up somewhere. "
              "Have fun debugging!");
    return;
  }
  stack->items[++stack->top] = item;
}

void log_warning(char *s) {
  fprintf(stderr, TERM_WARNING "[WARNING] " TERM_NORMAL "%s", s);
}

void log_error(char *s) {
  fprintf(stderr, TERM_ERROR "[ERROR] " TERM_NORMAL "%s", s);
}
