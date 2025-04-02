#ifndef BULLET_H
#define BULLET_H

#include "enemy.h"
#include "utils.h"
#include <raylib.h>

#define MAX_BULLETS 20000ul

extern unsigned long current_bullets;

typedef struct {
  float initial_speed;
  float acceleration;
  float angular_velocity;
  float initial_ttl;
  float max_speed;
} BulletArgs;

typedef struct {
  bool active;
  Movement movement;
  BulletArgs args;
  float ttl;
  bool player;
} Bullet;

void initialise_bullet_pool(void);

void process_bullets(float delta);

void draw_bullets(void);

// Note that the bullet_fire functions expect angles in degrees.

void bullet_fire_one(Vector2 initial_position, float initial_angle,
                     BulletArgs args, bool player_bullet);

void bullet_fire_ring(Vector2 initial_position, float initial_angle,
                      BulletArgs args, bool player_bullet, int bullets_in_ring,
                      float ring_offset);

void bullet_fire_arc(Vector2 initial_position, float initial_angle,
                     BulletArgs args, bool player_bullet, int bullets_in_arc,
                     float arc_offset, float arc_angle);

void bullet_fire_random_sphere(Vector2 initial_position, float initial_angle,
                               BulletArgs args, bool player_bullet,
                               int bullets_in_sphere, float sphere_radius);

void check_bullet_collisions(PlayerData player_data, void (*kill_player)(void));

void cancel_bullets(void);

#endif
