#include "enemy.h"
#include "bullet.h"
#include "utils.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

EnemyData enemies[MAX_ENEMIES] = {0};
unsigned int active_enemy_count = 0;

void ef_noop(EnemyData *n) {}

// --- POPCORN ---
#define POPCORN_LEFT 0b00000001

static void popcorn_drone_process(EnemyData *self, float delta) {
  update_collision_rect(&self->collision_box, self->movement.position);
  move(&self->movement, delta);
  self->timer += delta;
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
    .movement = {(Vector2){0, 0}, (Vector2){0, 300.0f}},
    .health = 20,
    .collision_box = {0, 0, 16.0f, 16.0f},
    .init = &ef_noop,
    .process = &popcorn_drone_process,
    .die = &popcorn_drone_die,
    .draw = &popcorn_drone_draw,
    .config_flags = 0};

// --- NORMAL ENEMY STUFF ---

void spawn_enemy(EnemyData template, Flags config_flags) {}

void damage_enemy(EnemyData *enemy, int damage) {
  enemy->health -= damage;
  printf("Health: %d\n", enemy->health);
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
