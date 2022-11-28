#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "types.c"
#include "globals.c"
#include "utils.c"
#include "map.c"
#include "wfc.c"

// TILE FUNCTIONS //////////////////////////////////////////////////////////
void init_tileset(void)
{
    road_tileset.tiles[0] = (wfc_tile_t){
        .texture = LoadTexture("img/CROSS.png"),
        .side_rules = {1, 1, 1, 1},
    };
    road_tileset.tiles[1] = (wfc_tile_t){
        .texture = LoadTexture("img/H_LINE.png"),
        .side_rules = {
            .left = 1,
            .right = 1,
            .bottom = 0,
            .top = 0,
        },
    };
    road_tileset.tiles[2] = (wfc_tile_t){
        .texture = LoadTexture("img/V_LINE.png"),
        .side_rules = {
            .top = 1,
            .bottom = 1,
            .left = 0,
            .right = 0,
        },
    };
    road_tileset.tiles[3] = (wfc_tile_t){
        .texture = LoadTexture("img/LEFT_T.png"),
        .side_rules = {
            .left = 1,
            .top = 1,
            .bottom = 1,
            .right = 0,
        },
    };

    road_tileset.tiles[4] = (wfc_tile_t){
        .texture = LoadTexture("img/TOP_T.png"),
        .side_rules = {
            .left = 1,
            .top = 1,
            .right = 1,
            .bottom = 0,
        },
    };

    road_tileset.tiles[5] = (wfc_tile_t){
        .texture = LoadTexture("img/TURN_LB.png"),
        .side_rules = {
            .bottom = 1,
            .left = 1,
            .top = 0,
            .right = 0,
        },
    };

    road_tileset.tiles[6] = (wfc_tile_t){
        .texture = LoadTexture("img/TURN_TR.png"),
        .side_rules = {
            .top = 1,
            .right = 1,
            .left = 0,
            .bottom = 0,
        },
    };

    road_tileset.tiles[7] = (wfc_tile_t){
        .texture = LoadTexture("img/TURN_RB.png"),
        .side_rules = {
            .bottom = 1,
            .right = 1,
            .top = 0,
            .left = 0,
        },
    };

    road_tileset.tiles[8] = (wfc_tile_t){
        .texture = LoadTexture("img/BOTTOM_T.png"),
        .side_rules = {
            .bottom = 1,
            .left = 1,
            .right = 1,
            .top = 0,
        },
    };

    road_tileset.tiles[9] = (wfc_tile_t){
        .texture = LoadTexture("img/RIGHT_T.png"),
        .side_rules = {
            .bottom = 1,
            .right = 1,
            .top = 1,
            .left = 0,
        },
    };

    road_tileset.tiles[10] = (wfc_tile_t){
        .texture = LoadTexture("img/TURN_TL.png"),
        .side_rules = {
            .top = 1,
            .left = 1,
            .bottom = 0,
            .right = 0,
        },
    };

    road_tileset.tile_count = 11;

    house_tileset.tiles[0] = (wfc_tile_t){
        .texture = LoadTexture("img/L_STOP.png"),
        .side_rules = {
            .left = 1,
            .right = 0,
            .bottom = 0,
            .top = 0,
        },
    };

    house_tileset.tiles[1] = (wfc_tile_t){
        .texture = LoadTexture("img/R_STOP.png"),
        .side_rules = {
            .right = 1,
            .left = 0,
            .bottom = 0,
            .top = 0,
        },
    };

    house_tileset.tiles[2] = (wfc_tile_t){
        .texture = LoadTexture("img/B_STOP.png"),
        .side_rules = {
            .bottom = 1,
            .left = 0,
            .right = 0,
            .top = 0,
        },
    };

    house_tileset.tiles[3] = (wfc_tile_t){
        .texture = LoadTexture("img/T_STOP.png"),
        .side_rules = {
            .top = 1,
            .bottom = 0,
            .left = 0,
            .right = 0,
        },
    };

    house_tileset.tile_count = 4;

    // road_tileset.tiles[11] = (wfc_tile_t){
    //     .texture = LoadTexture("img/H_B2G.png"),
    //     .side_rules = {
    //         .left = 1,
    //         .right = 2,
    //         .top = 0,
    //         .bottom = 0,
    //     },
    // };

    // road_tileset.tiles[12] = (wfc_tile_t){
    //     .texture = LoadTexture("img/L_STOP_G.png"),
    //     .side_rules = {
    //         .left = 2,
    //         .top = 0,
    //         .bottom = 0,
    //         .right = 0,
    //     },
    // };
}

void draw_tiles(void)
{
    for (int y = 0; y < CITY_ROWS; y++)
    {
        for (int x = 0; x < CITY_COLS; x++)
        {
            // IF IT EXISTS
            if (city_tiles[coord2index(x, y)].tile != NULL)
            {
                Vector2 coord = coord2screen(x, y);

                if (city_tiles[coord2index(x, y)].special_tile == C_STYPE_NONE)
                {
                    DrawTexture(city_tiles[coord2index(x, y)].tile->texture, coord.x, coord.y, WHITE);
                }
                else
                {
                    if (city_tiles[coord2index(x, y)].special_tile == C_STYPE_START)
                    {
                        DrawTexture(city_tiles[coord2index(x, y)].tile->texture, coord.x, coord.y, GREEN);
                    }
                    else if (city_tiles[coord2index(x, y)].special_tile == C_STYPE_EXIT)
                    {
                        DrawTexture(city_tiles[coord2index(x, y)].tile->texture, coord.x, coord.y, RED);
                    }
                    else if (city_tiles[coord2index(x, y)].special_tile == C_STYPE_SHOP)
                    {
                        DrawTexture(city_tiles[coord2index(x, y)].tile->texture, coord.x, coord.y, YELLOW);
                    }
                }
            }
        }
    }
}

// MAIN FUNCTION //////////////////////////////////////////////////////////

int main(void)
{
    srand(time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "WFC");
    SetTargetFPS(60);

    init_tileset();

    generate_map();

    while (!WindowShouldClose())
    {
        switch (gamestate)
        {
        case GS_MAP:
        {
            // MAP UPDATE //////////////////////////////////////////////

            // MAP DRAW //////////////////////////////////////////////
            BeginDrawing();
            ClearBackground(WHITE);

            draw_map();
            EndDrawing();
        }
        break;
        case GS_CITY:
        {
            // CITY UPDATE //////////////////////////////////////////////

            if (IsKeyPressed(KEY_SPACE))
            {
                generate_new_city();
            }

            // CITY DRAW //////////////////////////////////////////////
            BeginDrawing();
            ClearBackground(BEIGE);
            DrawFPS(0, 0);
            draw_tiles();
            EndDrawing();
        }
        break;
        default:
        {
            BeginDrawing();
            ClearBackground(WHITE);
            EndDrawing();
        }
        break;
        }
    }

    CloseWindow();

    return 0;
}