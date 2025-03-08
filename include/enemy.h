#ifndef ENEMY_H
#define ENEMY_H

#include "utils.h"
#include <raylib.h>

typedef struct EnemyData EnemyData;

extern const EnemyData popcorn;

struct EnemyData {
  Movement movement;
  Rectangle collision_box;
  void (*process)(EnemyData *self, float delta);
  void (*die)(EnemyData *self);
  // void (*init)(EnemyData *self);
  int health;
  float timer;
};

void damage_enemy(EnemyData *enemy, int damage);

#endif
