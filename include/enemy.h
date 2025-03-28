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
  /* char *enemy_name; */
  int score_value;
  /* float tension_value */
};

extern const EnemyData popcorn_drone;
extern EnemyData enemies[MAX_ENEMIES];

void damage_enemy(EnemyData *enemy, int damage);

void process_enemies(float delta);

void draw_enemies();

#endif
