#include "bullet.h"
#include "enemy.h"
#include "utils.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

static const Vector2 BULLET_COLLISION_RECT = (Vector2){2.0f, 2.0f};
static const Vector2 PLAYER_COLLISION_RECT = (Vector2){2.0f, 2.0f};
unsigned int current_bullets;

typedef struct {
  int top;
  Bullet *stack[MAX_BULLETS];
} BulletStack;

static Bullet bullets[MAX_BULLETS] = {0};
static BulletStack available_bullet_stack =
    (BulletStack){.top = MAX_BULLETS - 1};

extern EnemyData enemies[MAX_ENEMIES];

static Bullet *pop_bullet() {
  if (available_bullet_stack.top == -1)
    return NULL;
  Bullet *returned_bullet =
      available_bullet_stack.stack[available_bullet_stack.top--];
  returned_bullet->active = true;
  current_bullets++;
  return returned_bullet;
}

static void push_bullet(Bullet *bullet) {
  bullet->active = false;
  available_bullet_stack.stack[++available_bullet_stack.top] = bullet;
  current_bullets--;
}

static void fire(Vector2 initial_position, float initial_angle, BulletArgs args,
                 bool player_bullet) {
  Bullet *bullet = pop_bullet();
  if (!bullet)
    return;
  bullet->args = args;
  bullet->movement.position = initial_position;
  bullet->movement.velocity = Vector2Scale(
      (Vector2){cosf(initial_angle), sinf(initial_angle)}, args.initial_speed);
  bullet->ttl = args.initial_ttl;
  bullet->player = player_bullet;
}

static float get_angle_per_bullet(int count_bullets_in_ring) {
  return TAU / (float)count_bullets_in_ring;
}

static float get_bullet_angle(float get_angle_per_bullet, int i,
                              float rotation) {
  return (get_angle_per_bullet * i) + rotation;
}

static Vector2 get_bullet_start_position(Vector2 origin, float offset,
                                         float rotation) {
  return Vector2Add(
      origin, Vector2Scale((Vector2){cosf(rotation), sinf(rotation)}, offset));
}

void initialise_bullet_pool(void) {
  for (int i = 0; i < MAX_BULLETS; i++) {
    available_bullet_stack.stack[i] = &bullets[i];
  }
  current_bullets = 0;
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

void bullet_fire_ring(Vector2 initial_position, float initial_angle,
                      BulletArgs args, bool player_bullet, int bullets_in_ring,
                      float ring_offset) {
  if (bullets_in_ring < 2) {
    fprintf(stderr, "Tried to fire a ring with count less than 2. Please check "
                    "the firing configurations!");
    return;
  }
  float angle_per_bullet = get_angle_per_bullet(bullets_in_ring);
  for (int i = 0; i < bullets_in_ring; i++) {
    float bullet_angle = get_bullet_angle(angle_per_bullet, i, initial_angle);

    fire(get_bullet_start_position(initial_position, ring_offset, bullet_angle),
         bullet_angle, args, player_bullet);
  }
}

void check_bullet_collisions(PlayerData player_data,
                             void (*kill_player)(void)) {
  // PERF: All of this can be optimised later, for now we just need a prototype.
  Rectangle player_collision_box = create_centred_rectangle(
      player_data.movement.position.x, player_data.movement.position.y,
      PLAYER_COLLISION_RECT);
  for (int bullet_i = 0; bullet_i < MAX_BULLETS; bullet_i++) {
    if (!bullets[bullet_i].active)
      continue;

    Bullet *bullet = &bullets[bullet_i];
    if (bullet->player) {
      for (int enemy_i = 0; enemy_i < MAX_ENEMIES; enemy_i++) {
        if (enemies[enemy_i].health <= 0)
          continue;
        if (CheckCollisionRecs(
                create_centred_rectangle(bullet->movement.position.x,
                                         bullet->movement.position.y,
                                         BULLET_COLLISION_RECT),
                enemies[enemy_i].collision_box)) {
          damage_enemy(&enemies[enemy_i], 3);
          push_bullet(bullet);
          break;
        }
      }
    }
    if (player_data.recovery_time > 0.0f)
      return;
    if (CheckCollisionRecs(create_centred_rectangle(bullet->movement.position.x,
                                                    bullet->movement.position.y,
                                                    BULLET_COLLISION_RECT),
                           player_collision_box)) {
      kill_player();
      push_bullet(bullet);
    }
  }
}
