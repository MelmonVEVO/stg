#include "bullet.h"
#include "enemy.h"
#include "item.h"
#include "utils.h"
#include <float.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>

#define PLAYER_SPEED 200.0f
#define PLAYER_FIRE_TIME 0.06f
#define PLAYER_RECOVERY_TIME 5.0f

PlayerData player = {0};
unsigned long score = 0;
float tension = 0;

static const BulletArgs PLAYER_BULLET_ARGS =
    (BulletArgs){.max_speed = FLT_MAX,
                 .initial_speed = 1000.0f,
                 .initial_ttl = 1.0f,
                 .acceleration = 0.0f,
                 .angular_velocity = 0.0f};

static float fire_time = 0;
static Font mgsinker;
static Sound music;

extern long current_bullets;
extern EnemyData enemies[MAX_ENEMIES];

static void kill_player(void) {
  printf("Player was hit!\n");
  player.recovery_time = PLAYER_RECOVERY_TIME;
}

void move_player(float delta) {
  player.movement.velocity.x = 0, player.movement.velocity.y = 0;

  // Player movement.
  if (IsKeyDown(KEY_LEFT))
    player.movement.velocity.x -= 1.0f;
  if (IsKeyDown(KEY_RIGHT))
    player.movement.velocity.x += 1.0f;
  if (IsKeyDown(KEY_UP))
    player.movement.velocity.y -= 1.0f;
  if (IsKeyDown(KEY_DOWN))
    player.movement.velocity.y += 1.0f;

  player.movement.velocity.x *= delta * PLAYER_SPEED;
  player.movement.velocity.y *= delta * PLAYER_SPEED;

  player.movement.position.x =
      Clamp(player.movement.position.x + player.movement.velocity.x, 10.0f,
            VIEWPORT_WIDTH - 10.0f);
  player.movement.position.y =
      Clamp(player.movement.position.y + player.movement.velocity.y, 10.0f,
            VIEWPORT_HEIGHT - 10.0f);
}

void _process(float delta) {
  process_items(delta);
  process_bullets(delta);
  check_bullet_collisions(&player, &kill_player);
  process_enemies(delta);
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
  /* draw_background(); */
  if (player.recovery_time <= 0.0) {
    DrawCircle(player.movement.position.x, player.movement.position.y, 9.0f,
               GREEN);
  }
  draw_items();
  draw_enemies();
  draw_bullets();
#ifdef DEBUG
  DrawTextEx(mgsinker, TextFormat("bullets: %lu", current_bullets),
             (Vector2){10.0f, 10.0f}, (float)mgsinker.baseSize, 1, WHITE);
  extern long items_in_field;
  DrawTextEx(mgsinker, TextFormat("items: %lu", items_in_field),
             (Vector2){10.0f, 30.0f}, (float)mgsinker.baseSize, 1, WHITE);
  DrawTextEx(mgsinker, TextFormat("score: %lu", score), (Vector2){10.0f, 50.0f},
             (float)mgsinker.baseSize, 1, WHITE);
#endif
  EndTextureMode();

  // Draw the viewport to the screen.
  BeginDrawing();
  ClearBackground(BLACK);
  DrawTexturePro(
      target.texture,
      (Rectangle){0, 0, target.texture.width, -target.texture.height},
      (Rectangle){(GetScreenWidth() - VIEWPORT_WIDTH * scale) * 0.5f,
                  (GetScreenHeight() - VIEWPORT_HEIGHT * scale) * 0.5f,
                  VIEWPORT_WIDTH * scale, VIEWPORT_HEIGHT * scale},
      (Vector2){0, 0}, 0, WHITE);

  EndDrawing();
}

int main(void) {
  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
  InitWindow((int)VIEWPORT_WIDTH, (int)VIEWPORT_HEIGHT, "STG");
  SetWindowMinSize((int)VIEWPORT_WIDTH, (int)VIEWPORT_HEIGHT);
  SetTargetFPS(120);
  /* SetExitKey(0); */
  InitAudioDevice();
  HideCursor();
  SetRandomSeed(GetTime());
  ChangeDirectory("resources");
  initialise_bullet_pool();
  initialise_item_pool();
  mgsinker = LoadFontEx("MGSinker.ttf", 8, 0, 255);
  music = LoadSound("music/TR_02.mp3");
  SetSoundVolume(music, 0.5f);
  /* PlaySound(music); */

  RenderTexture2D screen_target =
      LoadRenderTexture(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
  SetTextureFilter(screen_target.texture, TEXTURE_FILTER_POINT);

  float delta;
  float stage_time = 0;
  player.movement.position.x = VIEWPORT_WIDTH / 2;
  player.movement.position.y = VIEWPORT_HEIGHT - 30.0f;

  enemies[0] = twist_drone;
  enemies[0].movement.position.x = VIEWPORT_WIDTH / 3;
  enemies[0].movement.position.y = 0;
  enemies[0].config_flags = 0;
  enemies[0].init(&enemies[0]);

  enemies[1] = twist_drone;
  enemies[1].movement.position.x = VIEWPORT_WIDTH * 2 / 3;
  enemies[1].movement.position.y = 0;
  enemies[1].config_flags = 1;
  enemies[1].init(&enemies[1]);

  while (!WindowShouldClose()) {
    delta = GetFrameTime();
    _input(delta);
    spawn_item(CRYSTAL_BLUE,
               (Vector2){VIEWPORT_WIDTH / 2.0f, VIEWPORT_HEIGHT / 2.0f});
    _process(delta);
    _draw(screen_target);
    stage_time += delta;
  }

  UnloadFont(mgsinker);
  CloseAudioDevice();
  CloseWindow();

  return 0;
}
