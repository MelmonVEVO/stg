#include "bullet.h"
#include "enemy.h"
#include "item.h"
#include "utils.h"
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

static const Vector2 BULLET_COLLISION_RECT = (Vector2){2.0f, 2.0f};
static const Vector2 PLAYER_COLLISION_RECT = (Vector2){2.0f, 2.0f};
static Texture2D bullet_sprite;

static Bullet bullets[MAX_BULLETS] = {0};
static Stack bullet_stack;

unsigned long current_bullets;

extern EnemyData enemies[MAX_ENEMIES];

static Bullet *pop_bullet(void) {
  Bullet *returned_bullet = (Bullet *)pop_stack(&bullet_stack);
  if (!returned_bullet)
    return NULL;
  returned_bullet->active = true;
  current_bullets++;
  return returned_bullet;
}

static void push_bullet(Bullet *bullet) {
  bullet->active = false;
  current_bullets--;
  push_stack(&bullet_stack, bullet);
}

static void fire(Vector2 initial_position, float direction, BulletArgs args,
                 bool player_bullet) {
  Bullet *bullet = pop_bullet();
  if (!bullet)
    return;
  bullet->args = args;
  bullet->movement.position = initial_position;
  bullet->movement.velocity = Vector2Scale(
      (Vector2){cosf(direction), sinf(direction)}, args.initial_speed);
  bullet->ttl = args.initial_ttl;
  bullet->player = player_bullet;
}

static float ring_get_angle_per_bullet(int count_bullets_in_ring) {
  return TAU / (float)count_bullets_in_ring;
}

static float arc_get_angle_per_bullet(int count_bullets_in_arc,
                                      float arc_angle) {
  return arc_angle / ((float)count_bullets_in_arc - 1.0);
}

static float ring_get_bullet_angle(float get_angle_per_bullet, int i,
                                   float rotation) {
  return (get_angle_per_bullet * i) + rotation;
}

static float arc_get_bullet_angle(float angle_per_bullet, int i, float rotation,
                                  float arc_angle) {
  return (angle_per_bullet * (float)i) + rotation - (arc_angle * 0.5f);
}

static Vector2 get_bullet_start_position(Vector2 origin, float offset,
                                         float rotation) {
  return Vector2Add(
      origin, Vector2Scale((Vector2){cosf(rotation), sinf(rotation)}, offset));
}

void initialise_bullet_pool(void) {
  bullet_stack = initialise_stack(MAX_BULLETS, sizeof(Bullet *));
  for (unsigned long i = 0; i < MAX_BULLETS; i++) {
    bullet_stack.items[i] = &bullets[i];
  }
  bullet_stack.top = MAX_BULLETS - 1;
  current_bullets = 0;
  bullet_sprite = LoadTexture("sprites/bullet/directed_bullet_2.png");
}

void process_bullets(float delta) {
  for (unsigned long i = 0; i < MAX_BULLETS; i++) {
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
  for (unsigned long i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active)
      continue;
    Bullet *bullet = &bullets[i];
    float rotation =
        atan2f(bullet->movement.velocity.y, bullet->movement.velocity.x) *
        RAD2DEG;
    Rectangle src = (Rectangle){.width = bullet_sprite.width,
                                .height = bullet_sprite.height};
    Rectangle dest =
        (Rectangle){bullet->movement.position.x, bullet->movement.position.y,
                    bullet_sprite.width, bullet_sprite.height};
    Vector2 origin =
        (Vector2){bullet_sprite.width * 0.5f, bullet_sprite.height * 0.5f};
    DrawTexturePro(bullet_sprite, src, dest, origin, rotation, WHITE);
  }
}

void bullet_fire_one(Vector2 initial_position, float initial_angle,
                     BulletArgs args, bool player_bullet) {
  fire(initial_position, initial_angle, args, player_bullet);
}

void bullet_fire_ring(Vector2 initial_position, float direction,
                      BulletArgs args, bool player_bullet, int bullets_in_ring,
                      float ring_offset) {
  if (bullets_in_ring < 2) {
    log_warning("Tried to fire a ring with less than 2 bullets. Please check "
                "the firing configurations!");
    return;
  }
  float angle_per_bullet = ring_get_angle_per_bullet(bullets_in_ring);
  for (int i = 0; i < bullets_in_ring; i++) {
    float bullet_angle =
        ring_get_bullet_angle(angle_per_bullet, i, DEG2RAD * direction);
    Vector2 start_position =
        get_bullet_start_position(initial_position, ring_offset, bullet_angle);
    fire(start_position, bullet_angle, args, player_bullet);
  }
}

void bullet_fire_arc(Vector2 initial_position, float direction, BulletArgs args,
                     bool player_bullet, int bullets_in_arc, float arc_offset,
                     float arc_angle) {
  if (bullets_in_arc < 2) {
    log_warning("Tried to fire a ring with less than 2 bullets. Please check "
                "the firing configurations!");
    return;
  }
  float angle_per_bullet =
      arc_get_angle_per_bullet(bullets_in_arc, DEG2RAD * arc_angle);
  for (int i = 0; i < bullets_in_arc; i++) {
    float bullet_angle = arc_get_bullet_angle(
        angle_per_bullet, i, DEG2RAD * direction, DEG2RAD * arc_angle);
    Vector2 start_position =
        get_bullet_start_position(initial_position, arc_offset, bullet_angle);
    fire(start_position, bullet_angle, args, player_bullet);
  }
}

void check_bullet_collisions(PlayerData player_data,
                             void (*kill_player)(void)) {
  // PERF: All of this can be optimised later, for now we just need a prototype.
  Rectangle player_collision_box = create_centred_rectangle(
      player_data.movement.position.x, player_data.movement.position.y,
      PLAYER_COLLISION_RECT);
  for (unsigned long bullet_i = 0; bullet_i < MAX_BULLETS; bullet_i++) {
    if (!bullets[bullet_i].active)
      continue;

    Bullet *bullet = &bullets[bullet_i];
    if (bullet->player) {
      for (unsigned long enemy_i = 0; enemy_i < MAX_ENEMIES; enemy_i++) {
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

void cancel_bullets() {
  for (unsigned long i = 0; i < MAX_BULLETS; i++) {
    Bullet *bullet = &bullets[i];
    if (!bullet->active || bullet->player)
      continue;
    spawn_item(CRYSTAL_BLUE, bullet->movement.position);
    push_bullet(bullet);
  }
}
