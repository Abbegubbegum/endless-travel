#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef struct
{
    int left;
    int right;
    int top;
    int bottom;
} side_rules_t;

typedef struct
{
    Texture2D texture;
    side_rules_t side_rules;
} tile_t;

typedef struct
{
    tile_t tiles[16];
    int tile_count;
} tileset_t;

typedef struct
{
    bool options[16];
    int count;
} entropy_t;

const int WIDTH = 1280;
const int HEIGHT = 1280;

const int TILE_SIZE = 64;

const int MAP_COLS = HEIGHT / TILE_SIZE;
const int MAP_ROWS = WIDTH / TILE_SIZE;
const int MAP_SIZE = MAP_COLS * MAP_ROWS;

tile_t *map_tiles[MAP_SIZE] = {0};

tileset_t tileset = {0};

bool algorithm_finished = false;

int coord2index(int x, int y)
{
    return y * MAP_COLS + x;
}

Vector2 index2coord(int i)
{
    return (Vector2){
        .x = i % MAP_COLS,
        .y = i / MAP_COLS};
}

Vector2 coord2screen(int x, int y)
{
    return (Vector2){
        .x = x * TILE_SIZE,
        .y = y * TILE_SIZE,
    };
}

void init_tileset(void)
{
    tileset.tiles[0] = (tile_t){
        .texture = LoadTexture("img/CROSS.png"),
        .side_rules = {1, 1, 1, 1},
    };
    tileset.tiles[1] = (tile_t){
        .texture = LoadTexture("img/H_LINE.png"),
        .side_rules = {
            .left = 1,
            .right = 1,
            .bottom = 0,
            .top = 0,
        },
    };
    tileset.tiles[2] = (tile_t){
        .texture = LoadTexture("img/L_STOP.png"),
        .side_rules = {
            .left = 1,
            .right = 0,
            .bottom = 0,
            .top = 0,
        },
    };
    tileset.tiles[3] = (tile_t){
        .texture = LoadTexture("img/V_LINE.png"),
        .side_rules = {
            .top = 1,
            .bottom = 1,
            .left = 0,
            .right = 0,
        },
    };
    tileset.tiles[4] = (tile_t){
        .texture = LoadTexture("img/LEFT_T.png"),
        .side_rules = {
            .left = 1,
            .top = 1,
            .bottom = 1,
            .right = 0,
        },
    };

    tileset.tiles[5] = (tile_t){
        .texture = LoadTexture("img/TOP_T.png"),
        .side_rules = {
            .left = 1,
            .top = 1,
            .right = 1,
            .bottom = 0,
        },
    };

    tileset.tiles[6] = (tile_t){
        .texture = LoadTexture("img/TURN_LB.png"),
        .side_rules = {
            .bottom = 1,
            .left = 1,
            .top = 0,
            .right = 0,
        },
    };

    tileset.tiles[7] = (tile_t){
        .texture = LoadTexture("img/TURN_TR.png"),
        .side_rules = {
            .top = 1,
            .right = 1,
            .left = 0,
            .bottom = 0,
        },
    };

    tileset.tiles[8] = (tile_t){
        .texture = LoadTexture("img/R_STOP.png"),
        .side_rules = {
            .right = 1,
            .left = 0,
            .bottom = 0,
            .top = 0,
        },
    };

    tileset.tiles[9] = (tile_t){
        .texture = LoadTexture("img/B_STOP.png"),
        .side_rules = {
            .bottom = 1,
            .left = 0,
            .right = 0,
            .top = 0,
        },
    };

    tileset.tiles[10] = (tile_t){
        .texture = LoadTexture("img/TURN_RB.png"),
        .side_rules = {
            .bottom = 1,
            .right = 1,
            .top = 0,
            .left = 0,
        },
    };

    tileset.tile_count = 11;
}

void draw_tiles(void)
{
    for (int y = 0; y < MAP_ROWS; y++)
    {
        for (int x = 0; x < MAP_COLS; x++)
        {
            // IF IT EXISTS
            if (map_tiles[coord2index(x, y)] != NULL)
            {
                Vector2 coord = coord2screen(x, y);
                DrawTexture(map_tiles[coord2index(x, y)]->texture, coord.x, coord.y, WHITE);
            }
        }
    }
}

bool tile_is_placable(tile_t tile, int x, int y)
{
    // CHECK ABOVE TILE
    if (map_tiles[coord2index(x, y - 1)] != NULL)
    {
        if (map_tiles[coord2index(x, y - 1)]->side_rules.bottom != tile.side_rules.top)
        {
            return false;
        }
    }

    // CHECK BELOW TILE
    if (map_tiles[coord2index(x, y + 1)] != NULL)
    {
        if (map_tiles[coord2index(x, y + 1)]->side_rules.top != tile.side_rules.bottom)
        {
            return false;
        }
    }

    // CHECK LEFT TILE
    if (map_tiles[coord2index(x - 1, y)] != NULL)
    {
        if (map_tiles[coord2index(x - 1, y)]->side_rules.right != tile.side_rules.left)
        {
            return false;
        }
    }

    // CHECK RIGHT TILE
    if (map_tiles[coord2index(x + 1, y)] != NULL)
    {
        if (map_tiles[coord2index(x + 1, y)]->side_rules.left != tile.side_rules.right)
        {
            return false;
        }
    }

    return true;
}

entropy_t calc_entropy(x, y)
{
    entropy_t entropy = {0};
    for (int i = 0; i < tileset.tile_count; i++)
    {
        if (tile_is_placable(tileset.tiles[i], x, y))
        {
            entropy.count++;
            entropy.options[i] = true;
        }
    }
    return entropy;
}

void place_next_tile()
{
    entropy_t entropy_map[MAP_SIZE] = {0};

    int lowest_entropy = __INT32_MAX__;
    Vector2 lowest_entropy_pos = {0};

    for (int y = 1; y < MAP_ROWS - 1; y++)
    {
        for (int x = 1; x < MAP_COLS - 1; x++)
        {
            // IF IT IS NOT ALREADY COLLAPSED
            if (map_tiles[coord2index(x, y)] == NULL)
            {
                entropy_map[coord2index(x, y)] = calc_entropy(x, y);
                if (entropy_map[coord2index(x, y)].count < lowest_entropy && entropy_map[coord2index(x, y)].count != 0)
                {
                    lowest_entropy = entropy_map[coord2index(x, y)].count;
                    lowest_entropy_pos = (Vector2){.x = x, .y = y};
                }
            }
        }
    }

    if (lowest_entropy == __INT32_MAX__)
    {
        algorithm_finished = true;
        return;
    }

    printf("%d | %0.f, %0.f\n", lowest_entropy, lowest_entropy_pos.x, lowest_entropy_pos.y);

    entropy_t entropy_to_collapse = entropy_map[coord2index(lowest_entropy_pos.x, lowest_entropy_pos.y)];

    int tile_index = rand() % entropy_to_collapse.count;

    for (int i = 0; i < tileset.tile_count; i++)
    {
        if (entropy_to_collapse.options[i])
        {
            if (tile_index == 0)
            {
                map_tiles[coord2index(lowest_entropy_pos.x, lowest_entropy_pos.y)] = &tileset.tiles[i];
                break;
            }
            else
            {
                tile_index--;
            }
        }
    }
}

int main(void)
{
    srand(time(NULL));

    InitWindow(WIDTH, HEIGHT, "WFC");
    SetTargetFPS(60);

    init_tileset();

    map_tiles[coord2index(4, 4)] = &tileset.tiles[0];

    clock_t start;
    clock_t end;

    while (!WindowShouldClose())
    {
        // UPDATE ------------------------------
        if (!algorithm_finished)
        {
            start = clock();
            while (!algorithm_finished)
            {
                place_next_tile();
            }
            end = clock();
            printf("%f\n", ((double)end - start) / 10000000);
        }

        // DRAW --------------------------------
        BeginDrawing();
        ClearBackground(BEIGE);
        // DrawFPS(0, 0);
        draw_tiles();
        EndDrawing();
    }

    return 0;
}