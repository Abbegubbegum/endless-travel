typedef struct point_node_t
{
    Vector2 pos;
    point_node_t *neighbors[16];
    int neighbor_count;
} point_node_t;

typedef struct
{
    point_node_t *items;
    int count;
} point_node_list_t;

typedef struct
{

} hull_t;
// OBS: REMEMBER TO FREE THE NODE LIST ITEMS POINTER
point_node_list_t point_list2node_list(point_list_t points)
{
    point_node_list_t list = {
        .count = points.count,
        .items = calloc(points.count, sizeof(point_node_t)),
    };

    for (int i = 0; i < points.count; i++)
    {
        list.items[i] = (point_node_t){
            .pos = points.items[i],
        };
    }

    return list;
}

void add_neighbor_to_node(point_node_t *to_add, point_node_t *p0)
{
    p0->neighbors[p0->neighbor_count] = to_add;
    p0->neighbor_count++;
}

void sort_points(point_node_list_t *points)
{
    // BUBBLE SORT

    bool has_swapped = true;
    while (has_swapped)
    {
        has_swapped = false;

        for (int i = 0; i < points->count - 1; i++)
        {
            if (points->items[i].pos.x > points->items[i + 1].pos.x)
            {
                point_node_t p = points->items[i];
                points->items[i] = points->items[i + 1];
                points->items[i + 1] = p;
                has_swapped = true;
            }
            else if (points->items[i].pos.x == points->items[i + 1].pos.x && points->items[i].pos.y < points->items[i + 1].pos.y)
            {
                point_node_t p = points->items[i];
                points->items[i] = points->items[i + 1];
                points->items[i + 1] = p;
                has_swapped = true;
            }
        }
    }
}

point_node_list_t get_subarray(point_node_list_t origin, int start, int end)
{
    return (point_node_list_t){
        .items = origin.items + start,
        .count = end - start,
    };
}

hull_t merge(hull_t hull1, hull_t hull2)
{
}

hull_t triangulate(point_node_list_t points)
{
    switch (points.count)
    {
    case 0:
        // return (hull_t) { [] }
    case 1:
        // return (hull_t) { points }
        break;
    case 2:
        add_neighbor_to_node(&points.items[1], &points.items[0]);
        add_neighbor_to_node(&points.items[0], &points.items[1]);
        // return (hull_t) { points }
        break;

    // > 3
    default:
        int left_count = points.count / 2;
        merge(triangulate(get_subarray(points, 0, left_count)), triangulate(get_subarray(points, left_count, points.count)));
        break;
    }
}

void generate_delauney_edges(point_list_t points)
{
    point_node_list_t nodes = point_list2node_list(points);

    sort_points(&nodes);

    triangulate(nodes);

    free(nodes.items);
}
