#include "common.h"

Vector2 old_city_coord = {0};

const int PLAYER_EDGE_OFFSET = 10;

void reset_player(void)
{
    player.coord = city_start_pos_coord;
}

void update_player_city(void)
{
    city_tile_t current_tile = city_tiles[city_coord2index(player.coord.x, player.coord.y)];

    if (IsKeyPressed(KEY_ENTER) && current_tile.tile_type == C_TTYPE_HOUSE && !city_tile_is_visited[city_coord2index(player.coord.x, player.coord.y)])
    {
        city_tile_is_visited[city_coord2index(player.coord.x, player.coord.y)] = true;
        generate_house(current_tile.tile);
        gamestate = GS_HOUSE;
        old_city_coord = player.coord;
        player.coord = house_player_start_pos;
        return;
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

void update_player_house(void)
{
    house_tile_t current_tile = house_tiles[house_coord2index(player.coord.x, player.coord.y)];

    Vector2 next_coord = player.coord;

    if (current_tile.special_tile == H_STYPE_DOOR)
    {
        gamestate = GS_CITY;
        player.coord = old_city_coord;
        return;
    }

    if (IsKeyPressed(KEY_W))
    {
        next_coord.y--;
    }
    else if (IsKeyPressed(KEY_S))
    {
        next_coord.y++;
    }
    else if (IsKeyPressed(KEY_A))
    {
        next_coord.x--;
    }
    else if (IsKeyPressed(KEY_D))
    {
        next_coord.x++;
    }

    house_tile_t *next_tile = &house_tiles[house_coord2index(next_coord.x, next_coord.y)];

    bool block_player_movement = false;

    if (next_tile->tile_type == H_TTYPE_WALL) {
        block_player_movement = true;
    }

    if (next_tile->tile_type == H_TTYPE_OBJECT) {
        if (next_tile->special_tile == H_STYPE_CHEST) {
            player.food++;
            next_tile->tile_type = H_TTYPE_FLOOR;
            next_tile->special_tile = H_STYPE_NONE;
        }
    }

    if (!block_player_movement) {
        player.coord = next_coord;
    }
}

void draw_player(void)
{
    Vector2 pos = city_coord2screen(player.coord.x, player.coord.y);
    if (gamestate == GS_HOUSE)
    {
        pos = house_coord2screen(player.coord.x, player.coord.y);
    }

    DrawRectangle(pos.x + PLAYER_EDGE_OFFSET, pos.y + PLAYER_EDGE_OFFSET, CITY_TILE_SIZE - PLAYER_EDGE_OFFSET * 2, CITY_TILE_SIZE - PLAYER_EDGE_OFFSET * 2, BLUE);

    if (gamestate == GS_CITY)
    {
        if (city_tiles[city_coord2index(player.coord.x, player.coord.y)].tile_type == C_TTYPE_HOUSE)
        {
            DrawText(city_tile_is_visited[city_coord2index(player.coord.x, player.coord.y)] ? "Already Visited!" : "Press Enter to go in!", 20, 20, 42, WHITE);
        }
    }
}