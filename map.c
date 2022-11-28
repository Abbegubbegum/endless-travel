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

const int POINT_EDGE_MARGIN = 100;

Vector2 points[64];
int points_count = 0;

circle_event_t *events[64];
int events_count = 0;

voronoi_arc_t *root = {0};

// Gets a random point with a bit of margin from the edges of the screen
Vector2 get_random_point(void)
{
    return (Vector2){
        .x = (rand() % (SCREEN_WIDTH - POINT_EDGE_MARGIN * 2)) + POINT_EDGE_MARGIN,
        .y = (rand() % (SCREEN_HEIGHT - POINT_EDGE_MARGIN * 2)) + POINT_EDGE_MARGIN,
    };
}

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

void generate_map(void)
{
    // Spawns random points on the map
    for (int i = 0; i < 50; i++)
    {
        points[i] = get_random_point();
        points_count++;
        printf("%.0f, %.0f\n", points[i].x, points[i].y);
    }

    // Sort the arrays
    sort_points();
    sort_events();

    // While there are points left to process
    while (points_count > 0)
    {
        // If there is a circle event that has higher priority than the next point, process it
        if (events_count > 0 && events[0]->x <= points[0].x)
        {
            process_event();
        }
        // Else process the next point
        else
        {
            process_point();
        }
    }

    // Process the remaining circle events
    while (events_count > 0)
    {
        process_event();
    }

    // Clean up dangling edges
    finish_edges();
}

// Draws the voronoi map
void draw_map(void)
{
    for (int i = 0; i < points_count; i++)
    {
        DrawCircleV(points[i], 3, BLACK);
    }
}
