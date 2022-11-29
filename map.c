/*

typedef struct circle_event_t;

typedef struct voronoi_seg_t
{
    Vector2 start;
    Vector2 end;
    bool done;
} voronoi_seg_t;

typedef struct voronoi_arc_t
{
    Vector2 p;
    voronoi_arc_t *prev;
    voronoi_arc_t *next;
    circle_event_t *event;
    voronoi_seg_t *s0;
    voronoi_seg_t *s1;
} voronoi_arc_t;

typedef struct
{
    int x;
    Vector2 p;
    voronoi_arc_t *a;
    bool valid;
} circle_event_t;

circle_event_t *events[64];
int events_count = 0;

voronoi_arc_t *root = {0};

// Sort the points array by x in ascending order
void sort_points(void)
{
    bool has_swapped = true;

    // BUBBLE SORT
    while (has_swapped)
    {
        has_swapped = false;
        for (int i = 0; i < points_count - 1; i++)
        {
            if (points[i].x > points[i + 1].x)
            {
                Vector2 point = points[i];
                points[i] = points[i + 1];
                points[i + 1] = point;
                has_swapped = true;
            }
        }
    }
}

// Sorts the event array by x in ascending order
void sort_events(void)
{
    bool has_swapped = true;

    // BUBBLE SORT
    while (has_swapped)
    {
        has_swapped = false;
        for (int i = 0; i < events_count - 1; i++)
        {
            if (events[i]->x > events[i + 1]->x)
            {
                circle_event_t *event = events[i];
                events[i] = events[i + 1];
                events[i + 1] = event;
                has_swapped = true;
            }
        }
    }
}

// Fetches and removes the first point in the array
Vector2 pop_point_from_array(void)
{
    Vector2 p = points[0];
    points_count--;
    for (int i = 0; i < points_count; i++)
    {
        points[i] = points[i + 1];
    }

    return p;
}

void front_insert(Vector2 p)
{
    // If this is the first point, allocate a new arc
    if (!root)
    {
        root = calloc(1, sizeof(voronoi_arc_t));
        root->p = p;
        return;
    }

    // Find the current arc at height p.y (if there are any)
    for (voronoi_arc_t *i = root; i; i = i->next)
    {
        Vector2 z, zz;
    }
}

void process_point(void)
{
    Vector2 point = pop_point_from_array();

    // Add a new arc to the parabolic front(?)
    front_insert(point);
}

*/

#include <math.h>

const int POINT_EDGE_MARGIN = 100;

const int GRIDSIZE = 10;
const float JITTER = 0.5;

const float GRIDX_TO_SCREEN = SCREEN_WIDTH / GRIDSIZE;
const float GRIDY_TO_SCREEN = SCREEN_HEIGHT / GRIDSIZE;

Vector2 _points[8464];

point_list_t points = {
    .items = _points,
    .count = 0,
};

// Gets a random point with a bit of margin from the edges of the screen
Vector2 get_random_point(void)
{
    return (Vector2){
        .x = (rand() % (SCREEN_WIDTH - POINT_EDGE_MARGIN * 2)) + POINT_EDGE_MARGIN,
        .y = (rand() % (SCREEN_HEIGHT - POINT_EDGE_MARGIN * 2)) + POINT_EDGE_MARGIN,
    };
}

void add_point_to_list(point_list_t *list, Vector2 point)
{
    list->items[list->count] = point;
    list->count++;
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

float dist(Vector2 p0, Vector2 p1)
{
    float x = p0.x - p1.x;
    float y = p0.y - p1.y;

    return sqrt(x * x + y * y);
}

bool is_point_valid(int nrows, int ncols, Vector2 grid[nrows][ncols], int cellsize, Vector2 p, int r)
{
    if (p.x < 0 || p.x >= SCREEN_WIDTH || p.y < 0 || p.y >= SCREEN_HEIGHT)
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

void poisson_disk_sampling(int radius, int k, point_list_t *points)
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
        .x = rand() % SCREEN_WIDTH,
        .y = rand() % SCREEN_HEIGHT,
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
    add_point_to_list(points, p0);
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
                add_point_to_list(points, new_p);
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

void generate_map(void)
{
    // // Spawns random points on the map
    // for (int y = 0; y < GRIDSIZE; y++)
    // {
    //     for (int x = 0; x < GRIDSIZE; x++)
    //     {
    //         add_point_to_list(&points, (Vector2){
    //                                        .x = (x + (JITTER * (((rand() % 2000) - 1000.0) / 1000.0))) * GRIDX_TO_SCREEN,
    //                                        .y = (y + (JITTER * (((rand() % 2000) - 1000.0) / 1000.0))) * GRIDY_TO_SCREEN,
    //                                    });
    //     }
    // }

    poisson_disk_sampling(150, 30, &points);

    // for (int i = 0; i < points_count - 1; i++)
    // {
    //     for (int j = i + 1; j < points_count; j++)
    //     {
    //         Vector2 mid_point = (Vector2){
    //             .x = (points[i].x + points[j].x) / 2,
    //             .y = (points[i].y + points[j].y) / 2,
    //         };

    //         float k = (points[j].y - points[i].y) / (points[j].x - points[i].x);

    //         float opposite_k = -1 / k;

    //         printf("Coord 1: %.0f, %.0f\n", points[i].x, points[i].y);
    //         printf("Coord 2: %.0f, %.0f\n", points[j].x, points[j].y);

    //         printf("Mid point and slope: %.0f, %.0f | %.2f\n", mid_point.x, mid_point.y, k);
    //         printf("Opposite: %f\n", opposite_k);
    //     }
    // }
}

// Draws the voronoi map
void draw_map(void)
{
    for (int i = 0; i < points.count; i++)
    {
        DrawCircleV(points.items[i], 3, BLACK);
    }
}
