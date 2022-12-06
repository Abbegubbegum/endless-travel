#include "common.h"

typedef struct
{
    Vector2 coord;
} player_t;

player_t player;

const int PLAYER_EDGE_OFFSET = 10;

void reset_player(void)
{
    player.coord = city_start_pos_coord;
}

void update_player(void)
{
    city_tile_t current_tile = city_tiles[city_coord2index(player.coord.x, player.coord.y)];

    if (current_tile.tile_type == C_TTYPE_HOUSE)
    {
        generate_house();
        gamestate = GS_HOUSE;
    }

    if (IsKeyPressed(KEY_W) && current_tile.tile->side_rules.top)
    {
        player.coord.y--;
    }
    else if (IsKeyPressed(KEY_S) && current_tile.tile->side_rules.bottom)
    {
        player.coord.y++;
    }
    else if (IsKeyPressed(KEY_A) && current_tile.tile->side_rules.left)
    {
        player.coord.x--;
    }
    else if (IsKeyPressed(KEY_D) && current_tile.tile->side_rules.right)
    {
        player.coord.x++;
    }

    city_tile_t new_tile = city_tiles[city_coord2index(player.coord.x, player.coord.y)];

    if (new_tile.special_tile == C_STYPE_EXIT)
    {
        return_to_map();
    }
}

void draw_player(void)
{
    Vector2 pos = city_coord2screen(player.coord.x, player.coord.y);

    DrawRectangle(pos.x + PLAYER_EDGE_OFFSET, pos.y + PLAYER_EDGE_OFFSET, CITY_TILE_SIZE - PLAYER_EDGE_OFFSET * 2, CITY_TILE_SIZE - PLAYER_EDGE_OFFSET * 2, BLUE);
}