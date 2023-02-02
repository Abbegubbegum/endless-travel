#include "common.h"

typedef struct
{
    wfc_tile_t *road_options[16];
    int road_count;
    wfc_tile_t *house_options[16];
    int house_count;
    int count;
} entropy_t;

typedef struct
{
    int index;
    int path_length;
} frontier_data_t;

wfc_tileset_t road_tileset = {0};
wfc_tileset_t house_tileset = {0};

bool city_is_generated = false;

const int ROAD_PERCENT_BASE = 50;
const int ROAD_PERCENT_CHANGE_RATE = 30;
int road_percent = ROAD_PERCENT_BASE;

const int MIN_HOUSES = 4;
const int MAX_HOUSES = 10;
int house_count = 0;

// BFS VARIABLES
frontier_data_t frontier[CITY_SIZE] = {0};
int frontier_count = 0;
int reached[CITY_SIZE] = {0};
int reached_count = 0;

// BFS FUNCTIONS //////////////////////////////////////////////////////////
void add_tile_to_frontier(int tile_index, int path_length)
{
    frontier[frontier_count] = (frontier_data_t){
        .index = tile_index,
        .path_length = path_length,
    };
    frontier_count++;
}

frontier_data_t get_tile_from_frontier(void)
{
    frontier_data_t return_val = frontier[0];

    for (int i = 0; i < frontier_count - 1; i++)
    {
        frontier[i] = frontier[i + 1];
    }

    frontier_count--;

    return return_val;
}

void add_tile_to_reached(int tile_index)
{
    for (int i = 0; i < reached_count; i++)
    {
        if (reached[i] == tile_index)
        {
            return;
        }
    }

    reached[reached_count] = tile_index;
    reached_count++;
}

bool tile_is_reached(int tile_index)
{
    for (int i = 0; i < reached_count; i++)
    {
        if (reached[i] == tile_index)
        {
            return true;
        }
    }

    return false;
}

frontier_data_t find_farthest_away_tile(int x, int y)
{
    frontier_count = 0;
    reached_count = 0;

    int source_index = city_coord2index(x, y);
    frontier_data_t farthest_away = {0};

    add_tile_to_frontier(source_index, 0);
    add_tile_to_reached(source_index);

    while (frontier_count > 0)
    {
        frontier_data_t current_frontier_tile = get_tile_from_frontier();
        Vector2 current_position = city_index2coord(current_frontier_tile.index);

        side_rules_t current_tile_connections = city_tiles[current_frontier_tile.index].tile->side_rules;

        // CHECK TOP
        if (current_tile_connections.top)
        {
            if (!tile_is_reached(city_coord2index(current_position.x, current_position.y - 1)))
            {
                add_tile_to_frontier(city_coord2index(current_position.x, current_position.y - 1), current_frontier_tile.path_length + 1);
                add_tile_to_reached(city_coord2index(current_position.x, current_position.y - 1));
            }
        }

        // CHECK BOTTOM
        if (current_tile_connections.bottom)
        {
            if (!tile_is_reached(city_coord2index(current_position.x, current_position.y + 1)))
            {
                add_tile_to_frontier(city_coord2index(current_position.x, current_position.y + 1), current_frontier_tile.path_length + 1);
                add_tile_to_reached(city_coord2index(current_position.x, current_position.y + 1));
            }
        }

        // CHECK LEFT
        if (current_tile_connections.left)
        {
            if (!tile_is_reached(city_coord2index(current_position.x - 1, current_position.y)))
            {
                add_tile_to_frontier(city_coord2index(current_position.x - 1, current_position.y), current_frontier_tile.path_length + 1);
                add_tile_to_reached(city_coord2index(current_position.x - 1, current_position.y));
            }
        }

        // CHECK RIGHT
        if (current_tile_connections.right)
        {
            if (!tile_is_reached(city_coord2index(current_position.x + 1, current_position.y)))
            {
                add_tile_to_frontier(city_coord2index(current_position.x + 1, current_position.y), current_frontier_tile.path_length + 1);
                add_tile_to_reached(city_coord2index(current_position.x + 1, current_position.y));
            }
        }

        if (current_frontier_tile.path_length > farthest_away.path_length)
        {
            farthest_away = current_frontier_tile;
        }
    }

    return farthest_away;
}

void place_road_tile(entropy_t entropy, Vector2 pos)
{
    int tile_index = rand() % entropy.road_count;

    city_tiles[city_coord2index(pos.x, pos.y)] = (city_tile_t){
        .tile = entropy.road_options[tile_index],
        .tile_type = C_TTYPE_ROAD,
    };

    road_percent -= ROAD_PERCENT_CHANGE_RATE;
}

void place_house_tile(entropy_t entropy, Vector2 pos)
{
    int tile_index = rand() % entropy.house_count;

    city_tiles[city_coord2index(pos.x, pos.y)] = (city_tile_t){
        .tile = entropy.house_options[tile_index],
        .tile_type = C_TTYPE_HOUSE,
    };
    house_count++;

    road_percent += ROAD_PERCENT_CHANGE_RATE;
}

// MAP GENERATION FUNCTIONS ////////////////////////////////////////////
entropy_t calc_entropy(int x, int y)
{
    entropy_t entropy = {0};

    for (int i = 0; i < road_tileset.tile_count; i++)
    {
        if (city_tile_is_placable(road_tileset.tiles[i], x, y))
        {
            entropy.road_options[entropy.road_count] = &road_tileset.tiles[i];
            entropy.road_count++;
            entropy.count++;
        }
    }

    for (int i = 0; i < house_tileset.tile_count; i++)
    {
        if (city_tile_is_placable(house_tileset.tiles[i], x, y))
        {
            entropy.house_options[entropy.house_count] = &house_tileset.tiles[i];
            entropy.house_count++;
            entropy.count++;
        }
    }

    return entropy;
}

void place_next_connected_tile(void)
{
    entropy_t entropy_to_collapse = {
        .count = __INT32_MAX__,
    };
    Vector2 lowest_entropy_pos = {0};

    for (int y = 1; y < CITY_ROWS - 1; y++)
    {
        for (int x = 1; x < CITY_COLS - 1; x++)
        {
            // IF IT IS NOT ALREADY COLLAPSED AND IS CONNECTED TO THE MAIN PATH
            if (city_tiles[city_coord2index(x, y)].tile == NULL && city_tile_is_connected(x, y))
            {
                entropy_t entropy = calc_entropy(x, y);
                if (entropy.count < entropy_to_collapse.count && entropy.count != 0)
                {
                    entropy_to_collapse = entropy;
                    lowest_entropy_pos = (Vector2){.x = x, .y = y};
                }
            }
        }
    }

    if (entropy_to_collapse.count == __INT32_MAX__)
    {
        city_is_generated = true;
        return;
    }

    // printf("%d | %0.f, %0.f\n", entropy_to_collapse.count, lowest_entropy_pos.x, lowest_entropy_pos.y);
    // printf("road percent: %d%% \n", road_percent);

    if (entropy_to_collapse.house_count == 0)
    {
        place_road_tile(entropy_to_collapse, lowest_entropy_pos);
    }
    else if (entropy_to_collapse.road_count == 0)
    {
        place_house_tile(entropy_to_collapse, lowest_entropy_pos);
    }
    else
    {
        if ((rand() % 100) < road_percent)
        {
            place_road_tile(entropy_to_collapse, lowest_entropy_pos);
        }
        else
        {
            place_house_tile(entropy_to_collapse, lowest_entropy_pos);
        }
    }

    // road_percent -= ROAD_PERCENT_CHANGE_RATE;
}

void generate_new_city()
{
    // clock_t start = clock();
    city_is_generated = false;
    road_percent = ROAD_PERCENT_BASE;
    house_count = 0;

    for (int i = 0; i < CITY_SIZE; i++)
    {
        city_tiles[i] = (city_tile_t){0};
        city_tile_is_visited[i] = false;
    }

    city_tiles[city_coord2index(city_start_pos_coord.x, city_start_pos_coord.y)] = (city_tile_t){
        .tile = &road_tileset.tiles[0],
        .tile_type = C_TTYPE_ROAD,
        .special_tile = C_STYPE_START,
    };

    while (!city_is_generated)
    {
        place_next_connected_tile();
    }

    if (house_count < MIN_HOUSES || house_count > MAX_HOUSES)
    {
        generate_new_city();
        return;
    }

    frontier_data_t exit = find_farthest_away_tile(city_start_pos_coord.x, city_start_pos_coord.y);

    // printf("%d | %d\n", exit.path_length, exit.index);

    city_tiles[exit.index].special_tile = C_STYPE_EXIT;

    // clock_t end = clock();

    // printf("TIME: %f\n", ((double)end - start) / CLOCKS_PER_SEC);
}

void draw_city(void)
{
    for (int y = 0; y < CITY_ROWS; y++)
    {
        for (int x = 0; x < CITY_COLS; x++)
        {
            // IF IT EXISTS
            if (city_tiles[city_coord2index(x, y)].tile != NULL)
            {
                Vector2 coord = city_coord2screen(x, y);

                if (city_tiles[city_coord2index(x, y)].special_tile == C_STYPE_NONE)
                {
                    DrawTexture(city_tiles[city_coord2index(x, y)].tile->texture, coord.x, coord.y, WHITE);
                }
                else
                {
                    if (city_tiles[city_coord2index(x, y)].special_tile == C_STYPE_START)
                    {
                        DrawTexture(city_tiles[city_coord2index(x, y)].tile->texture, coord.x, coord.y, GREEN);
                    }
                    else if (city_tiles[city_coord2index(x, y)].special_tile == C_STYPE_EXIT)
                    {
                        DrawTexture(city_tiles[city_coord2index(x, y)].tile->texture, coord.x, coord.y, RED);
                    }
                    else if (city_tiles[city_coord2index(x, y)].special_tile == C_STYPE_SHOP)
                    {
                        DrawTexture(city_tiles[city_coord2index(x, y)].tile->texture, coord.x, coord.y, YELLOW);
                    }
                }
            }
        }
    }

    DrawText(TextFormat("Food: %d", player.food), SCREEN_WIDTH - 250, 20, 64, WHITE);
}