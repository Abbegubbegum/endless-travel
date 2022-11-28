const int WIDTH = 1280;
const int HEIGHT = 1280;

const int TILE_SIZE = 64;

const int MAP_COLS = HEIGHT / TILE_SIZE;
const int MAP_ROWS = WIDTH / TILE_SIZE;
const int MAP_SIZE = MAP_COLS * MAP_ROWS;

map_tile_t map_tiles[MAP_SIZE] = {0};

tileset_t road_tileset = {0};
tileset_t house_tileset = {0};

Vector2 start_pos = (Vector2){.x = (int)MAP_COLS / 2, .y = (int)MAP_ROWS / 2};
