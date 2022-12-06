#include "common.h"

typedef enum
{
    GS_NONE,
    GS_MAP,
    GS_CITY,
    GS_HOUSE,
} gamestate_e;

typedef enum
{
    C_TTYPE_NONE,
    C_TTYPE_ROAD,
    C_TTYPE_HOUSE,
} city_tile_type_e;

typedef enum
{
    C_STYPE_NONE,
    C_STYPE_START,
    C_STYPE_EXIT,
    C_STYPE_SHOP,
} city_special_tile_e;

typedef enum
{
    H_TTYPE_NONE,
    H_TTYPE_FLOOR,
    H_TTYPE_WALL,
    H_TTYPE_OBJECT,
} house_tile_type_e;

typedef enum
{
    H_STYPE_NONE,
    H_STYPE_DOOR,
    H_STYPE_CHEST,
} house_special_tile_e;

typedef struct
{
    Vector2 *items;
    int count;
} point_list_t;

typedef struct city_node_t
{
    Vector2 pos;
    struct city_node_t *neighbors[16];
    int neighbor_count;
    bool reached;
    bool visited;
} city_node_t;

typedef struct
{
    city_node_t *items;
    int count;
} city_node_list_t;

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
} wfc_tile_t;

typedef struct
{
    wfc_tile_t tiles[16];
    int tile_count;
} wfc_tileset_t;

typedef struct
{
    wfc_tile_t *tile;
    city_tile_type_e tile_type;
    city_special_tile_e special_tile;
} city_tile_t;

typedef struct
{
    house_tile_type_e tile_type;
    house_special_tile_e special_tile;
} house_tile_t;