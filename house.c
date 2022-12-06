void generate_house()
{
    for (int y = 0; y < HOUSE_ROWS; y++)
    {
        for (int x = 0; x < HOUSE_COLS; x++)
        {
            if (y == 0 || x == 0 || y == HOUSE_ROWS - 1 || x == HOUSE_COLS - 1)
            {
                house_tiles[house_coord2index(x, y)].tile_type = H_TTYPE_WALL;
            }
            else
            {
                house_tiles[house_coord2index(x, y)].tile_type = H_TTYPE_FLOOR;
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
            DrawRectangleV(house_coord2screen(x, y), (Vector2){HOUSE_TILE_SIZE, HOUSE_TILE_SIZE}, house_tiles[house_coord2index(x, y)].tile_type == H_TTYPE_WALL ? GRAY : BEIGE);
        }
    }
}
