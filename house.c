void generate_house(wfc_tile_t *tile)
{
    for (int y = 0; y < HOUSE_ROWS; y++)
    {
        for (int x = 0; x < HOUSE_COLS; x++)
        {
            if (y == 0 || x == 0 || y == HOUSE_ROWS - 1 || x == HOUSE_COLS - 1)
            {
                house_tiles[house_coord2index(x, y)].tile_type = H_TTYPE_WALL;
                house_tiles[house_coord2index(x, y)].special_tile = H_STYPE_NONE;
            }
            else
            {
                house_tiles[house_coord2index(x, y)].tile_type = H_TTYPE_FLOOR;
                house_tiles[house_coord2index(x, y)].special_tile = H_STYPE_NONE;
            }
        }
    }

    int doorx = 0;
    int doory = 0;

    if (tile->side_rules.top || tile->side_rules.bottom)
    {
        doorx = (rand() % (HOUSE_COLS - 2)) + 1;
        doory = tile->side_rules.top ? 0 : HOUSE_ROWS - 1;

        house_tiles[house_coord2index(doorx, doory)].special_tile = H_STYPE_DOOR;
        house_tiles[house_coord2index(doorx, doory)].tile_type = H_TTYPE_FLOOR;

        house_player_start_pos = (Vector2){
            .x = doorx,
            .y = tile->side_rules.top ? doory + 1 : doory - 1,
        };
    }
    else if (tile->side_rules.left || tile->side_rules.right)
    {
        doory = (rand() % (HOUSE_ROWS - 2)) + 1;
        doorx = tile->side_rules.left ? 0 : HOUSE_COLS - 1;

        house_tiles[house_coord2index(doorx, doory)].special_tile = H_STYPE_DOOR;
        house_tiles[house_coord2index(doorx, doory)].tile_type = H_TTYPE_FLOOR;

        house_player_start_pos = (Vector2){
            .x = tile->side_rules.left ? doorx + 1 : doorx - 1,
            .y = doory,
        };
    }

    int split_dir = rand() % 2;

    if (split_dir == 0)
    {
        int x = house_player_start_pos.x;
        while (house_player_start_pos.x == x)
        {
            x = (rand() % (HOUSE_COLS - 4) + 2);
        }

        int openingy = rand() % (HOUSE_ROWS - 2) + 1;

        for (int y = 0; y < HOUSE_ROWS; y++)
        {
            if (y != openingy)
            {
                house_tiles[house_coord2index(x, y)].tile_type = H_TTYPE_WALL;
            }
        }
    }

    if (split_dir == 1)
    {
        int y = house_player_start_pos.y;
        while (house_player_start_pos.y == y)
        {
            y = (rand() % (HOUSE_ROWS - 4) + 2);
        }

        int openingx = rand() % (HOUSE_COLS - 2) + 1;

        for (int x = 0; x < HOUSE_COLS; x++)
        {
            if (x != openingx)
            {
                house_tiles[house_coord2index(x, y)].tile_type = H_TTYPE_WALL;
            }
        }
    }
}

void draw_house()
{
    for (int y = 0; y < HOUSE_ROWS; y++)
    {
        for (int x = 0; x < HOUSE_COLS; x++)
        {
            house_tile_type_e type = house_tiles[house_coord2index(x, y)].tile_type;
            house_special_tile_e special_type = house_tiles[house_coord2index(x, y)].special_tile;

            Color c = type == H_TTYPE_WALL ? GRAY : BEIGE;

            if (special_type == H_STYPE_DOOR)
            {
                c = BROWN;
            }

            DrawRectangleV(house_coord2screen(x, y), (Vector2){HOUSE_TILE_SIZE, HOUSE_TILE_SIZE}, c);
        }
    }
}
