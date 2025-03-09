#include "enemy.h"
#include "bullet.h"
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

EnemyData enemies[MAX_ENEMIES] = {0};

void noop(void) {}

// --- POPCORN ---
static void popcorn_process(EnemyData *self, float delta) {
  /* move(&self->movement, delta); */
  self->collision_box.x =
      self->movement.position.x - (self->collision_box.width / 2.0f);
  self->collision_box.y =
      self->movement.position.y - (self->collision_box.height / 2.0f);
  /* self->movement.velocity = */
  /*     Vector2Rotate(self->movement.velocity, PI * 1.5f * delta); */
}

static void popcorn_draw(EnemyData *self) {
  DrawCircle(self->movement.position.x, self->movement.position.y, 10.0f,
             PURPLE);
  DrawRectangleLines(self->collision_box.x, self->collision_box.y,
                     self->collision_box.width, self->collision_box.height,
                     RED);
}

static void popcorn_die(EnemyData *self) {
  bullet_fire_ring(self->movement.position, 0,
                   (BulletArgs){.acceleration = 0,
                                .initial_ttl = 5.0f,
                                .initial_speed = 100.0f,
                                .max_speed = 100.0f,
                                .angular_velocity = 0},
                   false, 6, 25.0f);
}

const EnemyData popcorn = {.movement = {(Vector2){0, 0}, (Vector2){0, 300.0f}},
                           .health = 20,
                           .collision_box = {0, 0, 16.0f, 16.0f},
                           .process = &popcorn_process,
                           .die = &popcorn_die,
                           .draw = &popcorn_draw};

// --- NORMAL ENEMY STUFF ---
void damage_enemy(EnemyData *enemy, int damage) {
  enemy->health -= damage;
  printf("Health: %d\n", enemy->health);
  if (enemy->health <= 0) {
    enemy->die(enemy);
  }
}

void process_enemies(
    float delta) { // TODO: for now just process the first enemy
  enemies[0].process(&enemies[0], delta);
}

void draw_enemies() { // TODO: first enemy processing only~
  if (enemies[0].health <= 0)
    return;
  enemies[0].draw(&enemies[0]);
}
