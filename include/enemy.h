#ifndef ENEMY_H
#define ENEMY_H

#include "utils.h"
#include <raylib.h>

#define MAX_ENEMIES 255
#define POPCORN_LEFT 0b00000001

typedef struct EnemyData EnemyData;

struct EnemyData {
  Movement movement;
  Rectangle collision_box;
  void (*init)(EnemyData *self);
  void (*process)(EnemyData *self, float delta);
  void (*draw)(EnemyData *self);
  void (*die)(EnemyData *self);
  int health;
  float time_alive;
  Flags config_flags;
  char *name;
  int score_value;
  /* float tension_value */
};

extern EnemyData enemies[MAX_ENEMIES];

extern const EnemyData popcorn_drone;
extern const EnemyData twist_drone;

void spawn_enemy(EnemyData enemy, Flags config_flags, Vector2 initial_position);

void damage_enemy(EnemyData *enemy, int damage);

void process_enemies(float delta);

void draw_enemies();

#endif
