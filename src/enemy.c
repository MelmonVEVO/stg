#include "enemy.h"
#include "bullet.h"
#include "utils.h"
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

EnemyData enemies[MAX_ENEMIES] = {0};
Stack available_enemy_stack;

long active_enemy_count = 0;

extern unsigned long score;

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
    .movement = {.velocity = (Vector2){0, POPCORN_INITIAL_VELOCITY}},
    .health = 20,
    .collision_box = {0, 0, 16.0f, 16.0f},
    .init = &enemy_noop,
    .process = &popcorn_drone_process,
    .die = &popcorn_drone_die,
    .draw = &popcorn_drone_draw,
    .score_value = 120,
    .name = "Unnamed AQ-56"};

// --- TWIST DRONE ---
#define TWIST_ACCELERATION -322.2f

static void twist_drone_process(EnemyData *self, float delta) {
  update_collision_rect(&self->collision_box, self->movement.position);
  self->time_alive += delta;

  self->movement.velocity.y += TWIST_ACCELERATION * delta;
  move(&self->movement, delta);
  return;
}

static void twist_drone_draw(EnemyData *self) {
  DrawCircle(self->movement.position.x, self->movement.position.y, 10.0f,
             YELLOW);
  DrawRectangleLines(self->collision_box.x, self->collision_box.y,
                     self->collision_box.width, self->collision_box.height,
                     RED);
}

static void twist_drone_die(EnemyData *self) {
  extern PlayerData player;
  Vector2 direction =
      Vector2Subtract(player.movement.position, self->movement.position);
  float angle = atan2f(direction.y, direction.x) * RAD2DEG;
  bullet_fire_arc(self->movement.position, angle,
                  (BulletArgs){
                      .initial_ttl = 5.0f,
                      .initial_speed = 255.0f,
                      .max_speed = 255.0f,
                  },
                  false, 3, 5.0f, 20.0f);
}

const EnemyData twist_drone = {.movement = {.velocity = (Vector2){0, 300.0f}},
                               .health = 20,
                               .collision_box = {0, 0, 16.0f, 16.0f},
                               .init = &enemy_noop,
                               .process = &twist_drone_process,
                               .die = &twist_drone_die,
                               .draw = &twist_drone_draw,
                               .score_value = 120,
                               .name = "KR-88 Twist"};

// --- HOVER DRONE ---

static Texture2D hover_drone_texture;

static void hover_drone_process(EnemyData *self, float delta) {}

static void hover_drone_draw(EnemyData *self) {}

const EnemyData hover_drone = {.health = 20000,
                               .collision_box = {0, 0, 24.0f, 16.0f},
                               .init = &enemy_noop,
                               .process = &hover_drone_process,
                               .die = &enemy_noop,
                               .draw = &hover_drone_draw,
                               .score_value = 120,
                               .name = "LT-50 Phalanx"};

// --- NORMAL ENEMY STUFF ---

void spawn_enemy(EnemyData template, Flags config_flags,
                 Vector2 initial_position) {
  EnemyData *enemy = (EnemyData *)pop_stack(&available_enemy_stack);
  if (!enemy)
    return;
  *enemy = template;
  enemy->config_flags = config_flags;
  enemy->movement.position = initial_position;
  enemy->health = template.health;
  enemy->init(enemy);
  active_enemy_count++;
}

void return_enemy(EnemyData *enemy) {
  enemy->health = 0;
  push_stack(&available_enemy_stack, enemy);
  active_enemy_count--;
}

void damage_enemy(EnemyData *enemy, int damage) {
  enemy->health -= damage;
  if (enemy->health <= 0) {
    enemy->die(enemy);
    score += enemy->score_value;
    return_enemy(enemy);
  }
}

void process_enemies(float delta) {
  for (long i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].health <= 0)
      continue;
    enemies[i].process(&enemies[i], delta);
  }
}

void draw_enemies(void) {
  for (long i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].health <= 0)
      continue;
    enemies[i].draw(&enemies[i]);
  }
}
