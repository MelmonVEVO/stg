#include "bullet.h"
#include "enemy.h"
#include "utils.h"
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

static const Vector2 BULLET_COLLISION_RECT = (Vector2){2.0f, 2.0f};
static const Vector2 PLAYER_COLLISION_RECT = (Vector2){2.0f, 2.0f};
int current_bullets;

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
  if (!bullet)
    return;
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

void initialise_bullet_pool(void) {
  for (int i = 0; i < MAX_BULLETS; i++) {
    bullet_stack.stack[i] = &bullets[i];
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

void check_bullet_collisions(EnemyData enemies[MAX_ENEMIES], // TODO: unfinished
                             PlayerData player_data,
                             void (*kill_player)(void)) {
  Rectangle player_collision_box = create_centred_rectangle(
      player_data.movement.position.x, player_data.movement.position.y,
      PLAYER_COLLISION_RECT);
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active)
      continue;

    Bullet *bullet = &bullets[i];
    if (bullet->player) {
      for (int j = 0; j < MAX_ENEMIES; j++) {
        if (enemies[j].health <= 0)
          continue;
        if (CheckCollisionRecs(
                create_centred_rectangle(bullet->movement.position.x,
                                         bullet->movement.position.y,
                                         BULLET_COLLISION_RECT),
                enemies[j].collision_box)) {
          damage_enemy(&enemies[j], 3);
          push_bullet(bullet);
          break;
        }
      }
    }
    if (player_data.dead)
      return;
    if (CheckCollisionRecs(create_centred_rectangle(
                               bullet->movement.position
                                   .x, // TODO: optimise collision detection so
                                       // it doesn't do so many calculations.
                               bullet->movement.position.y,
                               BULLET_COLLISION_RECT),
                           player_collision_box)) {
      kill_player();
      push_bullet(bullet);
    }
  }
}
