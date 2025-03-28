#include "bullet.h"
#include "enemy.h"
#include "utils.h"
#include <float.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

#define PLAYER_SPEED 200.0f
#define PLAYER_FIRE_TIME 0.06f
#define PLAYER_RECOVERY_TIME 5.0f

static const BulletArgs PLAYER_BULLET_ARGS =
    (BulletArgs){.max_speed = FLT_MAX,
                 .initial_speed = 800.0f,
                 .initial_ttl = 2.0f,
                 .acceleration = 0.0f,
                 .angular_velocity = 0.0f};

static PlayerData player = {0};
static float fire_time = 0;
static float recovery_time = 0.0;
static Font mgsinker;

extern unsigned int current_bullets;
extern EnemyData enemies[MAX_ENEMIES];

static void kill_player(void) {
  printf("Player was hit!\n");
  player.recovery_time = PLAYER_RECOVERY_TIME;
}

void move_player(float delta) {
  player.movement.velocity.x = 0, player.movement.velocity.y = 0;

  // Player movement.
  if (IsKeyDown(KEY_LEFT))
    player.movement.velocity.x -= 1.0f * PLAYER_SPEED;
  if (IsKeyDown(KEY_RIGHT))
    player.movement.velocity.x += 1.0f * PLAYER_SPEED;
  if (IsKeyDown(KEY_UP))
    player.movement.velocity.y -= 1.0f * PLAYER_SPEED;
  if (IsKeyDown(KEY_DOWN))
    player.movement.velocity.y += 1.0f * PLAYER_SPEED;

  player.movement.velocity.x *= delta;
  player.movement.velocity.y *= delta;

  player.movement.position.x =
      Clamp(player.movement.position.x + player.movement.velocity.x, 20.0f,
            VIEWPORT_WIDTH - 20.0f);
  player.movement.position.y =
      Clamp(player.movement.position.y + player.movement.velocity.y, 20.0f,
            VIEWPORT_HEIGHT - 20.0f);
}

void _process(float delta) {
  process_enemies(delta);
  process_bullets(delta);
  check_bullet_collisions(player, &kill_player);
  player.recovery_time = MAX(player.recovery_time - delta, 0.0f);
}

void _input(float delta) {
  if (player.recovery_time > 0.0f)
    return;
  move_player(delta);
  fire_time -= delta;
  if (IsKeyDown(KEY_Z) && fire_time <= 0) {
    fire_time = PLAYER_FIRE_TIME;
    bullet_fire_one((Vector2){player.movement.position.x - 10.0f,
                              player.movement.position.y},
                    1.5f * PI, PLAYER_BULLET_ARGS, true);
    bullet_fire_one((Vector2){player.movement.position.x + 10.0f,
                              player.movement.position.y},
                    1.5f * PI, PLAYER_BULLET_ARGS, true);
    bullet_fire_one((Vector2){player.movement.position.x + 20.0f,
                              player.movement.position.y + 9.0f},
                    (1.5f * PI) + (PI / 16.0f), PLAYER_BULLET_ARGS, true);
    bullet_fire_one((Vector2){player.movement.position.x - 20.0f,
                              player.movement.position.y + 9.0f},
                    (1.5f * PI) - (PI / 16.0f), PLAYER_BULLET_ARGS, true);
  }
}

void _draw(RenderTexture2D target) {
  float scale = MIN((float)GetScreenWidth() / VIEWPORT_WIDTH,
                    (float)GetScreenHeight() / VIEWPORT_HEIGHT);

  BeginTextureMode(target);
  ClearBackground(BLACK);
  if (player.recovery_time <= 0.0) {
    DrawCircle(player.movement.position.x, player.movement.position.y, 9.0f,
               GREEN);
  }
#ifdef DEBUG
  DrawTextEx(mgsinker, TextFormat("Bullets: %d", current_bullets),
             (Vector2){10.0f, 10.0f}, 16, 0, WHITE);
  DrawTextEx(mgsinker, TextFormat("FPS: %d", GetFPS()),
             (Vector2){10.0f, 300.0f}, 16, 0, WHITE);
#endif
  draw_bullets();
  draw_enemies();
  EndTextureMode();

  BeginDrawing();
  ClearBackground(BLACK);
  DrawTexturePro(
      target.texture,
      (Rectangle){0.0f, 0.0f, target.texture.width,
                  (float)-target.texture.height},
      (Rectangle){(GetScreenWidth() - VIEWPORT_WIDTH * scale) * 0.5f,
                  (GetScreenHeight() - VIEWPORT_HEIGHT * scale) * 0.5f,
                  VIEWPORT_WIDTH * scale, VIEWPORT_HEIGHT * scale},
      (Vector2){0, 0}, 0.0f, WHITE);

  EndDrawing();
}

int main(void) {
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
  ChangeDirectory("resources");
  mgsinker = LoadFontEx("MGSinker.ttf", 16, 0, 255);

  InitWindow((int)VIEWPORT_WIDTH, (int)VIEWPORT_HEIGHT, "STG");
  SetWindowMinSize((int)VIEWPORT_WIDTH, (int)VIEWPORT_HEIGHT);
  SetTargetFPS(60);
  HideCursor();

  RenderTexture2D target = LoadRenderTexture(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
  SetTextureFilter(target.texture,
                   TEXTURE_FILTER_BILINEAR); // bilinear for now...

  float delta;
  initialise_bullet_pool();
  player.movement.position.x = VIEWPORT_WIDTH / 2;
  player.movement.position.y = VIEWPORT_HEIGHT - 30.0f;
  enemies[0] = popcorn_drone;
  enemies[0].movement.position.x = VIEWPORT_WIDTH / 2;
  enemies[0].movement.position.y = VIEWPORT_HEIGHT / 4;
  while (!WindowShouldClose()) {
    delta = GetFrameTime();
    _input(delta);
    _process(delta);
    _draw(target);
  }

  UnloadFont(mgsinker);
  CloseWindow();

  return 0;
}
