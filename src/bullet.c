#include "bullet.h"
#include "utils.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

static const Rectangle BULLET_RECT = (Rectangle){0, 0, 2.0f, 2.0f};

int current_bullets = 0;

typedef struct {
  int top;
  Bullet *stack[MAX_BULLETS];
} BulletStack;

static Bullet bullets[MAX_BULLETS] = {0};
static BulletStack bullet_stack = (BulletStack){.top = MAX_BULLETS - 1};

static Bullet *pop_bullet() {
  if (bullet_stack.top == -1)
    return NULL;
  Bullet *returned_bullet = bullet_stack.stack[bullet_stack.top--];
  returned_bullet->active = true;
  current_bullets++;
  return returned_bullet;
}

static void push_bullet(Bullet *bullet) {
  bullet->active = false;
  bullet_stack.stack[++bullet_stack.top] = bullet;
  current_bullets--;
}

static void fire(Vector2 initial_position, float initial_angle, BulletArgs args,
                 bool player_bullet) {
  Bullet *bullet = pop_bullet();
  bullet->args = args;
  bullet->movement.position = initial_position;
  bullet->movement.velocity = Vector2Scale(
      Vector2Rotate(VECTOR2UP, DEG2RAD * initial_angle), args.initial_speed);
  bullet->ttl = args.initial_ttl;
  bullet->player = player_bullet;
}

/* static float get_angle_per_bullet(int count_bullets_in_ring) { */
/*   return TAU / (float)count_bullets_in_ring; */
/* } */

/* static float get_bullet_angle(float get_angle_per_bullet, int i, */
/*                               float rotation) { */
/*   return (get_angle_per_bullet * i) + rotation; */
/* } */

void initialise_bullet_pool() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    bullet_stack.stack[i] = &bullets[i];
  }
}

void process_bullets(float delta) {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active)
      continue;
    Bullet *bullet = &bullets[i];
    Movement *bullet_mov = &bullets[i].movement;
    bullet_mov->position = Vector2Add(
        bullet_mov->position, Vector2Scale(bullet_mov->velocity, delta));

    bullet_mov->velocity =
        accelerate(bullet_mov->velocity, bullet->args.acceleration, delta);

    bullet->ttl -= delta;
    if (bullet->ttl <= 0)
      push_bullet(bullet);
  }
}

void draw_bullets(void) {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active)
      continue;
    DrawCircle(bullets[i].movement.position.x, bullets[i].movement.position.y,
               5.0f, WHITE);
  }
}

void bullet_fire_one(Vector2 initial_position, float initial_angle,
                     BulletArgs args, bool player_bullet) {
  fire(initial_position, initial_angle, args, player_bullet);
}
