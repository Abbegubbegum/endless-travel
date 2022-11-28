const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 1280;

gamestate_e gamestate = GS_MAP;

const int CITY_TILE_SIZE = 64;

const int CITY_COLS = SCREEN_HEIGHT / CITY_TILE_SIZE;
const int CITY_ROWS = SCREEN_WIDTH / CITY_TILE_SIZE;
const int CITY_SIZE = CITY_COLS * CITY_ROWS;

city_tile_t city_tiles[CITY_SIZE] = {0};

wfc_tileset_t road_tileset = {0};
wfc_tileset_t house_tileset = {0};

Vector2 city_start_pos = (Vector2){.x = (int)CITY_COLS / 2, .y = (int)CITY_ROWS / 2};
