#include "common.h"

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
    GS_NONE,
    GS_MAP,
    GS_CITY,
} gamestate_e;

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
    city_node_t *n1;
    city_node_t *n2;
} edge_t;

typedef struct
{
    Vector2 pos;
    float rsquared;
} circle_t;

typedef struct point_node_t
{
    Vector2 pos;
    struct point_node_t *neighbors[16];
    struct point_node_t *first;
    int neighbor_count;
    int hull_id;
} point_node_t;

typedef struct
{
    point_node_t *items;
    int count;
    int hull_id;
} point_node_list_t;

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