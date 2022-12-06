#include "common.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 1280;

gamestate_e gamestate = GS_MAP;

const int CITY_TILE_SIZE = 64;

const int CITY_COLS = SCREEN_HEIGHT / CITY_TILE_SIZE;
const int CITY_ROWS = SCREEN_WIDTH / CITY_TILE_SIZE;
const int CITY_SIZE = CITY_COLS * CITY_ROWS;

city_tile_t city_tiles[CITY_SIZE] = {0};

Vector2 city_start_pos_coord = (Vector2){.x = (int)CITY_COLS / 2, .y = (int)CITY_ROWS / 2};

city_node_t *highlighted_city = NULL;

const int HOUSE_TILE_SIZE = 64;

const int HOUSE_COLS = 16;
const int HOUSE_ROWS = 16;
const int HOUSE_SIZE = HOUSE_COLS * HOUSE_ROWS;

house_tile_t house_tiles[HOUSE_SIZE] = {0};

const int HOUSE_X_EDGE_OFFSET = (SCREEN_WIDTH - (HOUSE_TILE_SIZE * HOUSE_COLS)) / 2;
const int HOUSE_Y_EDGE_OFFSET = (SCREEN_HEIGHT - (HOUSE_TILE_SIZE * HOUSE_ROWS)) / 2;