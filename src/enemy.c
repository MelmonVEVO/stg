#include "enemy.h"
#include "bullet.h"

void noop(void) {}

// --- POPCORN ---
static void popcorn_process(EnemyData *self, float delta) {
  move(&self->movement, delta);
}

static void popcorn_die(EnemyData *self) {
  bullet_fire_ring(self->movement.position, 0,
                   (BulletArgs){.acceleration = 0,
                                .initial_ttl = 5.0f,
                                .initial_speed = 200.0f,
                                .max_speed = 200.0f,
                                .angular_velocity = 0},
                   false, 6, 5.0f);
}

const EnemyData popcorn = {.movement = {(Vector2){0, 0}, (Vector2){0, 300.0f}},
                           .health = 20,
                           .collision_box = {0, 0, 16.0f, 16.0f},
                           .process = &popcorn_process};

// --- NORMAL ENEMY STUFF ---
void damage_enemy(EnemyData *enemy, int damage) { enemy->health -= damage; }
