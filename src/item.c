#include "item.h"
#include "utils.h"
#include <raylib.h>
#include <raymath.h>

#define CRYSTAL_BASE_SCORE 80u
#define CRYSTAL_UPGRADE_REQUIREMENT 200u
#define COLLECT_CIRCLE_RADIUS 30.0f

static Item items[MAX_ITEMS] = {0};
static Stack item_stack;
static unsigned int green_crystals_collected = 0;

long items_in_field = 0;

// TEMP
Sound collect_sound;
Texture2D blue_crystal_sprite;
Texture2D green_crystal_sprite;

extern unsigned long score;
extern PlayerData player;

static void return_item(Item *item) {
  item->active = false;
  items_in_field--;
  push_stack(&item_stack, item);
}

static Item *get_new_item(void) { return (Item *)pop_stack(&item_stack); }

static unsigned int get_crystal_level() {
  return green_crystals_collected / CRYSTAL_UPGRADE_REQUIREMENT;
}

static void add_crystal_score(void) {
  score += CRYSTAL_BASE_SCORE << get_crystal_level();
}

static void pickup_crystal_blue(void) { add_crystal_score(); }

static void pickup_crystal_green(void) {
  green_crystals_collected =
      Clamp(++green_crystals_collected, 0,
            (float)((CRYSTAL_UPGRADE_REQUIREMENT * 4u) + 50u));
  add_crystal_score();
}

static void pickup_spirit_chip(void) {}

static void pickup_extra_life(void) {}

static void collect_item(Item *item) {
  if (!CheckCollisionPointCircle(item->movement.position,
                                 player.movement.position,
                                 COLLECT_CIRCLE_RADIUS))
    return;
  PlaySound(collect_sound);
  switch (item->item_type) {
  case CRYSTAL_BLUE:
    pickup_crystal_blue();
    break;
  case CRYSTAL_GREEN:
    pickup_crystal_green();
    break;
  case SPIRIT_CHIP:
    pickup_spirit_chip();
    break;
  case EXTRA_LIFE:
    pickup_extra_life();
    break;
  }
  return_item(item);
}

void make_all_crystals_follow(void) {
  for (long i = 0; i < MAX_ITEMS; i++) {
    if (!items[i].active || !(items[i].item_type == CRYSTAL_GREEN) ||
        !(items[i].item_type == CRYSTAL_BLUE)) {
    }
    continue;
    items[i].follow = true;
  }
}

static void process_following_item(Item *self, float delta) {
  self->movement.velocity = Vector2Scale(
      Vector2Subtract(player.movement.position, self->movement.position),
      160.0f);
}

static void process_green_crystal(Item *self, float delta) {
  self->movement.velocity.x = 0;
  if (self->time_alive > 1.5f) {
    self->movement.velocity.y = -170.0f;
    return;
  }
  self->movement.velocity.y =
      Lerp(self->movement.velocity.y, 10.0f, 4.0f * delta);
}

static void process_other_item(Item *self, float delta) {
  self->movement.velocity.y += ITEM_GRAVITY * delta;
}

static void despawn_if_oob(Item *item) {
  if (item->movement.position.y > VIEWPORT_HEIGHT + 20.0f ||
      item->movement.position.y < -40.0f ||
      item->movement.position.x > VIEWPORT_WIDTH + 20.0f ||
      item->movement.position.x < -20.0f)
    return_item(item);
}

void process_items(float delta) {
  Item *item;
  for (long i = 0; i < MAX_ITEMS; i++) {
    item = &items[i];
    if (!item->active)
      continue;
    if (item->follow) {
      process_following_item(item, delta);
    } else if (item->item_type == CRYSTAL_GREEN) {
      process_green_crystal(item, delta);
    } else {
      process_other_item(item, delta);
    }
    move(&item->movement, delta);
    despawn_if_oob(item);
    collect_item(item);
    item->time_alive += delta;
  }
}

static void spawn_green_crystal(Item *crystal) {
  crystal->movement.velocity = Vector2Scale(VECTOR2DOWN, 200.0f);
  crystal->item_type = CRYSTAL_GREEN;
}

static void spawn_blue_crystal(Item *crystal) {
  float rangle = (random_float() * (PI / 6)) - (PI / 12);
  crystal->movement.velocity =
      Vector2Rotate(Vector2Scale(VECTOR2UP, 150.0f), rangle);
  crystal->item_type = CRYSTAL_BLUE;
}

void spawn_item(ItemType type, Vector2 position) {
  Item *item = get_new_item();
  if (!item)
    return;
  items_in_field++;
  item->item_type = type;
  item->movement.position = position;
  item->active = true;
  item->time_alive = 0;
  item->follow = false;
  switch (type) {
  case CRYSTAL_BLUE:
    spawn_blue_crystal(item);
    break;
  case CRYSTAL_GREEN:
    spawn_green_crystal(item);
    break;
  case SPIRIT_CHIP:
  case EXTRA_LIFE:
    break;
  }
}

static void draw_green_crystal(Vector2 position) {
  DrawTexture(green_crystal_sprite,
              (int)position.x - (green_crystal_sprite.width / 2),
              (int)position.y - (green_crystal_sprite.height / 2), WHITE);
}

static void draw_blue_crystal(Vector2 position) {
  DrawTexture(blue_crystal_sprite,
              (int)position.x - (blue_crystal_sprite.width / 2),
              (int)position.y - (blue_crystal_sprite.height / 2), WHITE);
}

void draw_items(void) {
  for (long i = 0; i < MAX_ITEMS; i++) {
    if (!items[i].active)
      continue;
    Vector2 position = items[i].movement.position;
    switch (items[i].item_type) {
    case CRYSTAL_GREEN:
      draw_green_crystal(position);
      break;
    case CRYSTAL_BLUE:
      draw_blue_crystal(position);
      break;
    default:
      break;
    }
  }
}

void initialise_item_pool(void) {
  item_stack = initialise_stack(MAX_ITEMS, sizeof(Item *));
  for (unsigned long i = 0; i < MAX_ITEMS; i++) {
    item_stack.items[i] = &items[i];
  }
  item_stack.top = MAX_ITEMS - 1;

  // TEMP
  collect_sound = LoadSound("sfx/unowned_sfx_please_remove/getze.wav");
  blue_crystal_sprite = LoadTexture("sprites/crystals/b_crystal2.png");
  green_crystal_sprite = LoadTexture("sprites/crystals/g_crystal4.png");
  SetSoundVolume(collect_sound, 0.4f);
}
