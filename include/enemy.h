#ifndef ENEMY_H
#define ENEMY_H

#include "utils.h"
#include <raylib.h>

typedef struct EnemyData EnemyData;

struct EnemyData {
  Movement movement;
  Rectangle collision_box;
  void (*process)(EnemyData *self, float delta);
  void (*draw)(EnemyData *self);
  void (*die)(EnemyData *self);
  int health;
  float timer;
};

extern const EnemyData popcorn;
extern EnemyData enemies[MAX_ENEMIES];

void damage_enemy(EnemyData *enemy, int damage);

void process_enemies(float delta);

void draw_enemies();

#endif
