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

    if (tile->side_rules.top || tile->side_rules.bottom)
    {
        int x = (rand() % (HOUSE_COLS - 2)) + 1;
        int y = tile->side_rules.top ? 0 : HOUSE_ROWS - 1;

        house_tiles[house_coord2index(x, y)].special_tile = H_STYPE_DOOR;
        house_tiles[house_coord2index(x, y)].tile_type = H_TTYPE_FLOOR;

        house_player_start_pos = (Vector2){
            .x = x,
            .y = tile->side_rules.top ? y + 1 : y - 1,
        };
    }
    else if (tile->side_rules.left || tile->side_rules.right)
    {
        int y = (rand() % (HOUSE_ROWS - 2)) + 1;
        int x = tile->side_rules.left ? 0 : HOUSE_COLS - 1;

        house_tiles[house_coord2index(x, y)].special_tile = H_STYPE_DOOR;
        house_tiles[house_coord2index(x, y)].tile_type = H_TTYPE_FLOOR;

        house_player_start_pos = (Vector2){
            .x = tile->side_rules.left ? x + 1 : x - 1,
            .y = y,
        };
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
