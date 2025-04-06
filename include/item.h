#ifndef ITEM_H
#define ITEM_H

#include "utils.h"

#define MAX_ITEMS 1000l
#define ITEM_GRAVITY 350.0f

typedef enum { CRYSTAL_BLUE, CRYSTAL_GREEN, SPIRIT_CHIP, EXTRA_LIFE } ItemType;

typedef struct {
  Movement movement;
  ItemType item_type;
  float time_alive;
  bool active;
  bool follow;
} Item;

void make_all_crystals_follow(void);

void spawn_item(ItemType type, Vector2 position);

void process_items(float delta);

void draw_items(void);

void initialise_item_pool(void);

#endif
