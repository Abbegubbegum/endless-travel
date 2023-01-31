#include "common.h"

typedef struct
{
    city_node_t *n1;
    city_node_t *n2;
} edge_t;

const int HIGHLIGHT_RADIUS = 75;
const int EDGE_BUFFER = 75;

city_node_t _nodes[264];

city_node_list_t city_nodes = {
    .items = _nodes,
    .count = 0,
};

int radius = 150;

edge_t edges[264];
int edge_count = 0;

point_node_list_t delauney_nodes = {0};
bool delauney = false;

Color voronoi_pixels[SCREEN_HEIGHT * SCREEN_WIDTH] = {0};

const Color PALETTE[] = {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    PURPLE,
    ORANGE,
    SKYBLUE,
    VIOLET,
    BROWN,
    PINK,
    MAGENTA,
    MAROON,
    DARKBLUE,
    GOLD,
    LIME,
    DARKGREEN,
    DARKPURPLE,
    BEIGE,
    DARKBROWN,
};

const int PALETTE_COUNT = sizeof(PALETTE) / sizeof(PALETTE[0]);

void add_point_to_list(point_list_t *list, Vector2 point)
{
    list->items[list->count] = point;
    list->count++;
}

void add_node_to_city_node_list(city_node_list_t *list, Vector2 point)
{
    list->items[list->count] = (city_node_t){.pos = point};
    list->count++;
}

void add_edge(city_node_t *n1, city_node_t *n2)
{
    n1->neighbors[n1->neighbor_count] = n2;
    n2->neighbors[n2->neighbor_count] = n1;
    n1->neighbor_count++;
    n2->neighbor_count++;

    edges[edge_count] = (edge_t){
        .n1 = n1,
        .n2 = n2,
    };

    edge_count++;
}

void insert_point_to_grid(int ncols, Vector2 grid[][ncols], int cellsize, Vector2 point)
{
    int x = point.x / cellsize;
    int y = point.y / cellsize;
    grid[y][x] = point;
}

void remove_point_from_list(point_list_t *list, int index)
{
    list->count--;
    list->items[index] = list->items[list->count];
}

void remove_neighbor(city_node_t *to_remove, city_node_t *base_node)
{
    bool found = false;

    for (int i = 0; i < base_node->neighbor_count; i++)
    {
        if (found)
        {
            base_node->neighbors[i - 1] = base_node->neighbors[i];
        }
        else if (base_node->neighbors[i] == to_remove)
        {
            found = true;
        }
    }

    if (found)
    {
        base_node->neighbor_count--;
    }
}

void remove_edge(int index)
{
    remove_neighbor(edges[index].n1, edges[index].n2);
    remove_neighbor(edges[index].n2, edges[index].n1);

    edge_count--;
    edges[index] = edges[edge_count];
}

bool is_point_valid(int nrows, int ncols, Vector2 grid[nrows][ncols], int cellsize, Vector2 p, int r)
{
    if (p.x < EDGE_BUFFER || p.x >= SCREEN_WIDTH - EDGE_BUFFER || p.y < EDGE_BUFFER || p.y >= SCREEN_HEIGHT - EDGE_BUFFER)
    {
        return false;
    }

    int xindex = p.x / cellsize;
    int yindex = p.y / cellsize;

    int y0 = fmax(yindex - 1, 0);
    int y1 = fmin(yindex + 1, nrows);
    int x0 = fmax(xindex - 1, 0);
    int x1 = fmin(xindex + 1, ncols);

    for (int y = y0; y <= y1; y++)
    {
        for (int x = x0; x <= x1; x++)
        {
            if (grid[y][x].x != -1 && grid[y][x].y != -1)
            {
                if (dist(p, (Vector2){.x = grid[y][x].x, .y = grid[y][x].y}) < r)
                {
                    return false;
                }
            }
        }
    }

    return true;
}

void poisson_disk_sampling(int radius, int k, city_node_list_t *list)
{
    int N = 2;

    int cellsize = radius / sqrt(N);

    int grid_cols = ceil(SCREEN_WIDTH / cellsize) + 1;
    int grid_rows = ceil(SCREEN_HEIGHT / cellsize) + 1;

    Vector2 grid[grid_rows][grid_cols];

    Vector2 _active[grid_rows * grid_cols];

    point_list_t active = {
        .items = _active,
        .count = 0,
    };

    Vector2 p0 = (Vector2){
        .x = rand() % (SCREEN_WIDTH - EDGE_BUFFER * 2) + EDGE_BUFFER,
        .y = rand() % (SCREEN_HEIGHT - EDGE_BUFFER * 2) + EDGE_BUFFER,
    };

    for (int y = 0; y < grid_rows; y++)
    {
        for (int x = 0; x < grid_cols; x++)
        {
            grid[y][x] = (Vector2){
                .x = -1,
                .y = -1,
            };
        }
    }

    insert_point_to_grid(grid_cols, grid, cellsize, p0);
    add_node_to_city_node_list(list, p0);
    add_point_to_list(&active, p0);

    while (active.count > 0)
    {
        int index = rand() % active.count;
        Vector2 p = active.items[index];

        bool found = false;

        for (int i = 0; i < k; i++)
        {
            float theta = (rand() % 360) * DEG2RAD;

            int r = (rand() % radius) + radius;

            Vector2 new_p = {
                .x = p.x + r * cos(theta),
                .y = p.y + r * sin(theta),
            };

            if (is_point_valid(grid_rows, grid_cols, grid, cellsize, new_p, radius))
            {
                add_node_to_city_node_list(list, new_p);
                insert_point_to_grid(grid_cols, grid, cellsize, new_p);
                add_point_to_list(&active, new_p);
                found = true;
                break;
            }
        }

        if (!found)
        {
            remove_point_from_list(&active, index);
        }
    }
}

bool edges_has_shared_neighbors(edge_t e1, edge_t e2)
{
    return (e1.n1 == e2.n1 || e1.n1 == e2.n2 || e1.n2 == e2.n1 || e1.n2 == e2.n2);
}

void lazy_edge_generation()
{
    for (int i = 0; i < city_nodes.count - 1; i++)
    {
        for (int j = i + 1; j < city_nodes.count; j++)
        {
            // If they are far away from eachother
            if (!CheckCollisionPointCircle(city_nodes.items[i].pos, city_nodes.items[j].pos, radius * 2 - 10))
            {
                continue;
            }

            add_edge(&city_nodes.items[i], &city_nodes.items[j]);
        }
    }

    // IF TWO EDGES INTERSECT, REMOVE ONE
    for (int i = 0; i < edge_count - 1; i++)
    {
        for (int j = i + 1; j < edge_count; j++)
        {
            if (!edges_has_shared_neighbors(edges[i], edges[j]))
            {
                if (CheckCollisionLines(edges[i].n1->pos, edges[i].n2->pos, edges[j].n1->pos, edges[j].n2->pos, NULL))
                {

                    // printf("EDGE 1: %f, %f -> %f, %f\n", edges[i].p0.x, edges[i].p0.y, edges[i].p1.x, edges[i].p1.y);
                    // printf("EDGE 2: %f, %f -> %f, %f\n", edges[j].p0.x, edges[j].p0.y, edges[j].p1.x, edges[j].p1.y);
                    remove_edge(rand() % 2 == 0 ? i : j);
                }
            }
        }
    }
}

void brute_force_voronoi(void)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            int closest = 0;
            for (int i = 1; i < city_nodes.count; i++)
            {
                // If point[i] is closer than point[closest] then i is closest
                if (sqr_dist(city_nodes.items[i].pos.x, city_nodes.items[i].pos.y, x, y) < sqr_dist(city_nodes.items[closest].pos.x, city_nodes.items[closest].pos.y, x, y))
                {
                    closest = i;
                }
            }
            voronoi_pixels[y * SCREEN_WIDTH + x] = PALETTE[closest % PALETTE_COUNT];
        }
    }
}

void draw_voronoi(void)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            DrawPixel(x, y, voronoi_pixels[y * SCREEN_WIDTH + x]);
        }
    }
}

// Draws the map
void draw_map(void)
{
    draw_voronoi();

    if (delauney)
    {
        for (int i = 0; i < delauney_nodes.count; i++)
        {
            point_node_t node = delauney_nodes.items[i];

            for (int j = 0; j < node.neighbor_count; j++)
            {
                point_node_t neighbor = *node.neighbors[j];
                DrawLineV(node.pos, neighbor.pos, BLACK);
            }
            DrawCircleV(node.pos, 5, BLACK);
        }
    }
    else
    {
        for (int i = 0; i < edge_count; i++)
        {
            DrawLineV(edges[i].n1->pos, edges[i].n2->pos, BLACK);
        }

        for (int i = 0; i < city_nodes.count; i++)
        {
            Color c = city_nodes.items[i].reached ? LIGHTGRAY : BLACK;

            int radius = city_nodes.items[i].reached ? 20 : 5;

            DrawCircleV(city_nodes.items[i].pos, radius, city_nodes.items[i].visited ? WHITE : c);
        }

        // for (int i = 0; i < edge_count; i++)
        // {
        //     DrawLineV(edges[i].p0, edges[i].p1, BLACK);

        //     // DrawLineV(get_point_on_line(-10, mid_point, opposite_k), get_point_on_line(SCREEN_WIDTH + 10, mid_point, opposite_k), GREEN);
        // }
    }

    DrawText(TextFormat("Food: %d", player.food), SCREEN_WIDTH - 250, 20, 64, WHITE);
}

void generate_map(void)
{
    city_nodes.count = 0;
    edge_count = 0;

    // Generate the points using poisson disk sampling distribution algorithm
    poisson_disk_sampling(radius, 30, &city_nodes);
    lazy_edge_generation();

    int lowest_index = 0;
    int lowest_sqr_dist = sqr_dist(0, SCREEN_HEIGHT, city_nodes.items[0].pos.x, city_nodes.items[0].pos.y);

    for (int i = 1; i < city_nodes.count; i++)
    {
        int dist = sqr_dist(city_nodes.items[i].pos.x, city_nodes.items[i].pos.y, 0, SCREEN_HEIGHT);
        if (dist < lowest_sqr_dist)
        {
            lowest_index = i;
            lowest_sqr_dist = dist;
        }
    }

    city_nodes.items[lowest_index].reached = true;

    brute_force_voronoi();

    // BeginDrawing();
    // ClearBackground(WHITE);

    // draw_map();
    // EndDrawing();

    // delauney_nodes = generate_delauney_edges(points);

    // Create the delauney triangles from the points

    // Create the voronoi nodes from the delauney triangles
}

void update_highlighted_city(void)
{
    Vector2 mouse_pos = GetMousePosition();

    int closest_city_index = 0;

    float closest_distance = SCREEN_HEIGHT * SCREEN_WIDTH;

    if (delauney)
    {
        for (int i = 0; i < delauney_nodes.count; i++)
        {
            float current_dist = dist(mouse_pos, delauney_nodes.items[i].pos);

            if (current_dist < closest_distance)
            {
                closest_city_index = i;
                closest_distance = current_dist;
            }
        }

        if (closest_distance < HIGHLIGHT_RADIUS)
        {
        }
    }
    else
    {
        for (int i = 0; i < city_nodes.count; i++)
        {
            float current_dist = dist(mouse_pos, city_nodes.items[i].pos);

            if (dist(mouse_pos, city_nodes.items[i].pos) < closest_distance && city_nodes.items[i].reached && !city_nodes.items[i].visited)
            {
                closest_city_index = i;
                closest_distance = current_dist;
            }
        }

        if (closest_distance < HIGHLIGHT_RADIUS)
        {
            highlighted_city = &city_nodes.items[closest_city_index];
        }
        else
        {
            highlighted_city = NULL;
        }
    }
}

void select_highlighted_city()
{
    highlighted_city->visited = true;

    for (int i = 0; i < highlighted_city->neighbor_count; i++)
    {
        highlighted_city->neighbors[i]->reached = true;
    }

    gamestate = GS_CITY;
    generate_new_city();
}

void return_to_map()
{
    gamestate = GS_MAP;
}

void draw_highlighted_city()
{
    if (highlighted_city != NULL)
    {
        DrawCircleV(highlighted_city->pos, 25, WHITE);
    }
}
