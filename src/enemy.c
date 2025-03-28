#include "enemy.h"
#include "bullet.h"
#include "utils.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

EnemyData enemies[MAX_ENEMIES] = {0};
unsigned int active_enemy_count = 0;

void enemy_noop(EnemyData *n) {}

// --- POPCORN DRONE ---
#define POPCORN_INITIAL_VELOCITY 250.0f
#define POPCORN_SHOOTING_TIME 0.8f

static void popcorn_drone_process(EnemyData *self, float delta) {
  update_collision_rect(&self->collision_box, self->movement.position);
  self->time_alive += delta;

  if (self->time_alive < POPCORN_SHOOTING_TIME) {
    // Gradually slow down vertical movement
    float slowdown_factor = 1.0f - (self->time_alive / POPCORN_SHOOTING_TIME);
    self->movement.velocity =
        Vector2Scale(VECTOR2DOWN, POPCORN_INITIAL_VELOCITY * slowdown_factor);
    move(&self->movement, delta);
    return;
  }
  if (self->time_alive >= POPCORN_SHOOTING_TIME &&
      self->time_alive < POPCORN_SHOOTING_TIME + delta) {
    self->movement.velocity = (Vector2){0, 0};
    extern PlayerData player;

    Vector2 direction =
        Vector2Subtract(player.movement.position, self->movement.position);
    float angle = atan2f(direction.y, direction.x);

    bullet_fire_one(self->movement.position, angle,
                    (BulletArgs){.acceleration = 0,
                                 .initial_ttl = 5.0f,
                                 .initial_speed = 200.0f,
                                 .max_speed = 150.0f,
                                 .angular_velocity = 0},
                    false);

    self->movement.velocity = (Vector2){.y = -60.0f};

    if (self->config_flags & POPCORN_LEFT) {
      self->movement.velocity.x = -160.0f;
    } else {
      self->movement.velocity.x = 160.0f;
    }
    move(&self->movement, delta);
    return;
  }
  move(&self->movement, delta);
  if (self->time_alive > 5.0f) {
    self->health = 0;
  }
}

static void popcorn_drone_draw(EnemyData *self) {
  DrawCircle(self->movement.position.x, self->movement.position.y, 10.0f,
             PURPLE);
  DrawRectangleLines(self->collision_box.x, self->collision_box.y,
                     self->collision_box.width, self->collision_box.height,
                     RED);
}

static void popcorn_drone_die(EnemyData *self) {
  bullet_fire_ring(self->movement.position, 0,
                   (BulletArgs){.acceleration = 0,
                                .initial_ttl = 5.0f,
                                .initial_speed = 100.0f,
                                .max_speed = 100.0f,
                                .angular_velocity = 0},
                   false, 6, 25.0f);
}

const EnemyData popcorn_drone = {
    .movement = {(Vector2){0, 0}, (Vector2){0, POPCORN_INITIAL_VELOCITY}},
    .health = 20,
    .collision_box = {0, 0, 16.0f, 16.0f},
    .init = &enemy_noop,
    .process = &popcorn_drone_process,
    .die = &popcorn_drone_die,
    .draw = &popcorn_drone_draw,
    .config_flags = 0};

// --- NORMAL ENEMY STUFF ---

void spawn_enemy(EnemyData template, Flags config_flags) {}

void damage_enemy(EnemyData *enemy, int damage) {
  enemy->health -= damage;
  if (enemy->health <= 0) {
    enemy->die(enemy);
  }
}

void process_enemies(float delta) {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].health <= 0)
      continue;
    enemies[i].process(&enemies[i], delta);
  }
}

void draw_enemies() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].health <= 0)
      continue;
    enemies[i].draw(&enemies[i]);
  }
}
