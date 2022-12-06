#include "common.h"
#include "geometry.c"
#include "delauney.c"
#include "city.c"
#include "map.c"
#include "house.c"
#include "player.c"

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

// MAIN FUNCTION //////////////////////////////////////////////////////////

int main(void)
{
    srand(time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Oregon Trail");
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
            if (IsKeyPressed(KEY_SPACE))
            {
                generate_map();
            }

            update_highlighted_city();

            if (IsMouseButtonPressed(0) && highlighted_city != NULL)
            {
                select_highlighted_city();
                reset_player();
            }

            // MAP DRAW //////////////////////////////////////////////
            BeginDrawing();
            ClearBackground(WHITE);

            draw_map();

            draw_highlighted_city();
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

            update_player_city();

            // CITY DRAW //////////////////////////////////////////////
            BeginDrawing();
            ClearBackground(BEIGE);
            draw_city();
            draw_player();
            EndDrawing();
        }
        break;
        case GS_HOUSE:
        {
            // HOUSE UPDATE //////////////////////////////////////////////
            update_player_house();

            // HOUSE DRAW //////////////////////////////////////////
            BeginDrawing();
            ClearBackground(BLACK);
            draw_house();
            draw_player();
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

    free(delauney_nodes.items);

    return 0;
}