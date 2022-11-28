typedef struct
{
    tile_t *road_options[16];
    int road_count;
    tile_t *house_options[16];
    int house_count;
    int count;
} entropy_t;

typedef struct
{
    int index;
    int path_length;
} frontier_data_t;

bool algorithm_finished = false;

const int ROAD_PERCENT_BASE = 50;
const int ROAD_PERCENT_CHANGE_RATE = 30;
int road_percent = ROAD_PERCENT_BASE;

const int MIN_HOUSES = 4;
const int MAX_HOUSES = 10;
int house_count = 0;

// BFS VARIABLES
frontier_data_t frontier[MAP_SIZE] = {0};
int frontier_count = 0;
int reached[MAP_SIZE] = {0};
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

    int source_index = coord2index(x, y);
    frontier_data_t farthest_away = {0};

    add_tile_to_frontier(source_index, 0);
    add_tile_to_reached(source_index);

    while (frontier_count > 0)
    {
        frontier_data_t current_frontier_tile = get_tile_from_frontier();
        Vector2 current_position = index2coord(current_frontier_tile.index);

        side_rules_t current_tile_connections = map_tiles[current_frontier_tile.index].tile->side_rules;

        // CHECK TOP
        if (current_tile_connections.top)
        {
            if (!tile_is_reached(coord2index(current_position.x, current_position.y - 1)))
            {
                add_tile_to_frontier(coord2index(current_position.x, current_position.y - 1), current_frontier_tile.path_length + 1);
                add_tile_to_reached(coord2index(current_position.x, current_position.y - 1));
            }
        }

        // CHECK BOTTOM
        if (current_tile_connections.bottom)
        {
            if (!tile_is_reached(coord2index(current_position.x, current_position.y + 1)))
            {
                add_tile_to_frontier(coord2index(current_position.x, current_position.y + 1), current_frontier_tile.path_length + 1);
                add_tile_to_reached(coord2index(current_position.x, current_position.y + 1));
            }
        }

        // CHECK LEFT
        if (current_tile_connections.left)
        {
            if (!tile_is_reached(coord2index(current_position.x - 1, current_position.y)))
            {
                add_tile_to_frontier(coord2index(current_position.x - 1, current_position.y), current_frontier_tile.path_length + 1);
                add_tile_to_reached(coord2index(current_position.x - 1, current_position.y));
            }
        }

        // CHECK RIGHT
        if (current_tile_connections.right)
        {
            if (!tile_is_reached(coord2index(current_position.x + 1, current_position.y)))
            {
                add_tile_to_frontier(coord2index(current_position.x + 1, current_position.y), current_frontier_tile.path_length + 1);
                add_tile_to_reached(coord2index(current_position.x + 1, current_position.y));
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

    map_tiles[coord2index(pos.x, pos.y)] = (map_tile_t){
        .tile = entropy.road_options[tile_index],
        .tile_type = TTYPE_ROAD,
    };

    road_percent -= ROAD_PERCENT_CHANGE_RATE;
}

void place_house_tile(entropy_t entropy, Vector2 pos)
{
    int tile_index = rand() % entropy.house_count;

    map_tiles[coord2index(pos.x, pos.y)] = (map_tile_t){
        .tile = entropy.house_options[tile_index],
        .tile_type = TTYPE_HOUSE,
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
        if (tile_is_placable(road_tileset.tiles[i], x, y))
        {
            entropy.road_options[entropy.road_count] = &road_tileset.tiles[i];
            entropy.road_count++;
            entropy.count++;
        }
    }

    for (int i = 0; i < house_tileset.tile_count; i++)
    {
        if (tile_is_placable(house_tileset.tiles[i], x, y))
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

    for (int y = 1; y < MAP_ROWS - 1; y++)
    {
        for (int x = 1; x < MAP_COLS - 1; x++)
        {
            // IF IT IS NOT ALREADY COLLAPSED AND IS CONNECTED TO THE MAIN PATH
            if (map_tiles[coord2index(x, y)].tile == NULL && tile_is_connected(x, y))
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
        algorithm_finished = true;
        return;
    }

    printf("%d | %0.f, %0.f\n", entropy_to_collapse.count, lowest_entropy_pos.x, lowest_entropy_pos.y);
    printf("road percent: %d%% \n", road_percent);

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

void generate_new_map()
{
    clock_t start = clock();
    algorithm_finished = false;
    road_percent = ROAD_PERCENT_BASE;
    house_count = 0;

    for (int i = 0; i < MAP_SIZE; i++)
    {
        map_tiles[i] = (map_tile_t){0};
    }

    map_tiles[coord2index(start_pos.x, start_pos.y)] = (map_tile_t){
        .tile = &road_tileset.tiles[0],
        .tile_type = TTYPE_ROAD,
        .special_tile = STYPE_START,
    };

    while (!algorithm_finished)
    {
        place_next_connected_tile();
    }

    if (house_count < MIN_HOUSES || house_count > MAX_HOUSES)
    {
        generate_new_map();
        return;
    }

    frontier_data_t exit = find_farthest_away_tile(start_pos.x, start_pos.y);

    printf("%d | %d\n", exit.path_length, exit.index);

    map_tiles[exit.index].special_tile = STYPE_EXIT;

    clock_t end = clock();

    printf("TIME: %f\n", ((double)end - start) / CLOCKS_PER_SEC);
}

/* DEPRECATED
void place_next_tile(void)
{
    entropy_t entropy_map[MAP_SIZE] = {0};

    int lowest_entropy = __INT32_MAX__;
    Vector2 lowest_entropy_pos = {0};

    for (int y = 1; y < MAP_ROWS - 1; y++)
    {
        for (int x = 1; x < MAP_COLS - 1; x++)
        {
            // IF IT IS NOT ALREADY COLLAPSED
            if (map_tiles[coord2index(x, y)].tile == NULL)
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

    for (int i = 0; i < road_tileset.tile_count; i++)
    {
        if (entropy_to_collapse.road_options[i])
        {
            if (tile_index == 0)
            {
                map_tiles[coord2index(lowest_entropy_pos.x, lowest_entropy_pos.y)] = &road_tileset.tiles[i];
                break;
            }
            else
            {
                tile_index--;
            }
        }
    }
}
*/