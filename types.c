typedef enum
{
    TTYPE_NONE,
    TTYPE_ROAD,
    TTYPE_HOUSE,
} tile_type_e;

typedef enum
{
    STYPE_NONE,
    STYPE_START,
    STYPE_EXIT,
    STYPE_SHOP,
} special_tile_e;

typedef struct
{
    int left;
    int right;
    int top;
    int bottom;
} side_rules_t;

typedef struct
{
    Texture2D texture;
    side_rules_t side_rules;
} tile_t;

typedef struct
{
    tile_t tiles[16];
    int tile_count;
} tileset_t;

typedef struct
{
    tile_t *tile;
    tile_type_e tile_type;
    special_tile_e special_tile;
} map_tile_t;