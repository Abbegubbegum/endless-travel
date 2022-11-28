// CONVERSION FUNCTIONS //////////////////////////////////////////////////////////
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

// MAP PLACEMENT FUNCTIONS ///////////////////////////////////////////////////////////////////////////

bool tile_is_placable(tile_t tile, int x, int y)
{
    // CHECK ABOVE TILE
    if (map_tiles[coord2index(x, y - 1)].tile != NULL)
    {
        if (map_tiles[coord2index(x, y - 1)].tile->side_rules.bottom != tile.side_rules.top)
        {
            return false;
        }
    }

    // CHECK BELOW TILE
    if (map_tiles[coord2index(x, y + 1)].tile != NULL)
    {
        if (map_tiles[coord2index(x, y + 1)].tile->side_rules.top != tile.side_rules.bottom)
        {
            return false;
        }
    }

    // CHECK LEFT TILE
    if (map_tiles[coord2index(x - 1, y)].tile != NULL)
    {
        if (map_tiles[coord2index(x - 1, y)].tile->side_rules.right != tile.side_rules.left)
        {
            return false;
        }
    }

    // CHECK RIGHT TILE
    if (map_tiles[coord2index(x + 1, y)].tile != NULL)
    {
        if (map_tiles[coord2index(x + 1, y)].tile->side_rules.left != tile.side_rules.right)
        {
            return false;
        }
    }

    return true;
}

bool tile_is_connected(int x, int y)
{
    // CHECK TOP TILE
    if (map_tiles[coord2index(x, y - 1)].tile != NULL && map_tiles[coord2index(x, y - 1)].tile->side_rules.bottom != 0)
    {
        return true;
    }

    // CHECK BELOW TILE
    if (map_tiles[coord2index(x, y + 1)].tile != NULL && map_tiles[coord2index(x, y + 1)].tile->side_rules.top != 0)
    {
        return true;
    }

    // CHECK LEFT TILE
    if (map_tiles[coord2index(x - 1, y)].tile != NULL && map_tiles[coord2index(x - 1, y)].tile->side_rules.right != 0)
    {
        return true;
    }

    // CHECK RIGHT TILE
    if (map_tiles[coord2index(x + 1, y)].tile != NULL && map_tiles[coord2index(x + 1, y)].tile->side_rules.left != 0)
    {
        return true;
    }

    return false;
}