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

Vector2 _points[264];

point_list_t points = {
    .items = _points,
    .count = 0,
};

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

Vector2 get_point_on_line(int x, Vector2 p0, float k)
{
    float m = p0.y - (k * p0.x);

    return (Vector2){
        .x = x,
        .y = k * x + m,
    };
}

void generate_map(void)
{
    // Generate the points using poisson disk sampling distribution algorithm
    poisson_disk_sampling(150, 30, &points);

    // Create the delauney triangles from the points

    // Create the voronoi nodes from the delauney triangles
}

// Draws the voronoi map
void draw_map(void)
{
    for (int i = 0; i < points.count; i++)
    {
        DrawCircleV(points.items[i], 5, BLACK);
    }

    for (int i = 0; i < points.count - 1; i++)
    {
        for (int j = i + 1; j < points.count; j++)
        {

            if (!CheckCollisionPointCircle(points.items[i], points.items[j], 290))
            {
                continue;
            }

            Vector2 mid_point = (Vector2){
                .x = (points.items[i].x + points.items[j].x) / 2,
                .y = (points.items[i].y + points.items[j].y) / 2,
            };

            float k = (points.items[j].y - points.items[i].y) / (points.items[j].x - points.items[i].x);

            float opposite_k = -1 / k;

            // printf("Coord 1: %.0f, %.0f\n", points.items[i].x, points.items[i].y);
            // printf("Coord 2: %.0f, %.0f\n", points.items[j].x, points.items[j].y);

            // printf("Mid point and slope: %.0f, %.0f | %.2f\n", mid_point.x, mid_point.y, k);
            // printf("Opposite: %f\n", opposite_k);

            DrawCircleV(mid_point, 2, RED);

            DrawLineV(get_point_on_line(-10, mid_point, opposite_k), get_point_on_line(SCREEN_WIDTH + 10, mid_point, opposite_k), GREEN);
        }
    }
}

void draw_highlighted_city()
{
    Vector2 mouse_pos = GetMousePosition();

    int closest_city_index = 0;

    for (int i = 0; i < points.count; i++)
    {
        if (dist(mouse_pos, points.items[i]) < dist(mouse_pos, points.items[closest_city_index]))
        {
            closest_city_index = i;
        }
    }

    DrawCircleV(points.items[closest_city_index], 20, GRAY);
}
