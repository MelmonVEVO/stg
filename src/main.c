#include "bullet.h"
#include "resource_dir.h"
#include "utils.h"
#include <float.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>

#define VIEWPORT_WIDTH 400.0f
#define VIEWPORT_HEIGHT 300.0f
#define PLAYER_SPEED 200.0f
#define MAX_ENEMIES 100
#define PLAYER_FIRE_TIME 0.06f

typedef struct {
  Movement movement;
  bool dead;
} PlayerData;

typedef struct {
  Movement movement;
  Rectangle collision_box;
  int health;
} EnemyData;

/* typedef struct { */
/*   EnemyData *enemies; */
/*   size_t count; */
/*   size_t capacity; */
/* } Enemies; */

static const BulletArgs PLAYER_BULLET_ARGS =
    (BulletArgs){.max_speed = FLT_MAX,
                 .initial_speed = 600.0f,
                 .initial_ttl = 2.0f,
                 .acceleration = 0.0f,
                 .angular_velocity = 0.0f};

static PlayerData player = {0};
static EnemyData enemies[MAX_ENEMIES] = {0};
static float fire_time = 0;

extern int current_bullets;

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

void _process(float delta) { process_bullets(delta); }

void _input(float delta) {
  move_player(delta);
  fire_time -= delta;
  if (IsKeyDown(KEY_Z) && fire_time <= 0) {
    fire_time = PLAYER_FIRE_TIME;
    bullet_fire_one((Vector2){player.movement.position.x - 10.0f,
                              player.movement.position.y},
                    0.0f, PLAYER_BULLET_ARGS, true);
    bullet_fire_one((Vector2){player.movement.position.x + 10.0f,
                              player.movement.position.y},
                    0.0f, PLAYER_BULLET_ARGS, true);
    bullet_fire_one((Vector2){player.movement.position.x + 20.0f,
                              player.movement.position.y + 9.0f},
                    15.0f, PLAYER_BULLET_ARGS, true);
    bullet_fire_one((Vector2){player.movement.position.x - 20.0f,
                              player.movement.position.y + 9.0f},
                    -15.0f, PLAYER_BULLET_ARGS, true);
  }
}

void _draw(RenderTexture2D target) {
  float scale = MIN((float)GetScreenWidth() / VIEWPORT_WIDTH,
                    (float)GetScreenHeight() / VIEWPORT_HEIGHT);

  BeginTextureMode(target);
  ClearBackground(BLACK);
  DrawCircle(player.movement.position.x, player.movement.position.y, 9.0f,
             GREEN);
  DrawText(TextFormat("Player Position (%02.02f, %02.02f)",
                      player.movement.position.x, player.movement.position.y),
           20, 20, 10, WHITE);
  DrawText(TextFormat("Bullets: %d", current_bullets), 20, 50, 10, WHITE);
  DrawText(TextFormat("FPS: %d", GetFPS()), 260, 20, 10, WHITE);
  draw_bullets();
  EndTextureMode();

  BeginDrawing();
  ClearBackground(BLACK);
  DrawTexturePro( // idk what this is doing lmao
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
  SearchAndSetResourceDir("resources");

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
  while (!WindowShouldClose()) {
    delta = GetFrameTime();
    _input(delta);
    _process(delta);
    _draw(target);
  }

  CloseWindow();
  return 0;
}
