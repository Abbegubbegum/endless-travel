#include "common.h"

int hull_counter = 0;

// OBS: REMEMBER TO FREE THE NODE LIST ITEMS POINTER
point_node_list_t point_list2node_list(point_list_t points)
{
    point_node_list_t list = {
        .count = points.count,
        .items = calloc(points.count, sizeof(point_node_t)),
        .hull_id = hull_counter,
    };

    hull_counter++;

    for (int i = 0; i < points.count; i++)
    {
        list.items[i] = (point_node_t){
            .pos = points.items[i],
            .hull_id = list.hull_id,
        };
    }

    return list;
}

int find_neighbor_index(point_node_t target, point_node_t base)
{
    for (int i = 0; i < base.neighbor_count; i++)
    {
        if (v_eq_v(target.pos, base.neighbors[i]->pos))
        {
            return i;
        }
    }

    return -1;
}

int find_neighbor_index_with_bigger_angle(point_node_t base, float a)
{
    for (int i = 0; i < base.neighbor_count; i++)
    {
        if (angle(base.pos, base.neighbors[i]->pos) >= a)
        {
            return i;
        }
    }

    return base.neighbor_count - 1;
}

// Returns the next ctr_clockwise neighbor to point target from point base
point_node_t *ctr_clockwise(point_node_t end, point_node_t start)
{
    int base_index = find_neighbor_index(end, start);

    int index = base_index;

    point_node_t *node;

    while (true)
    {
        index = (index + 1) % start.neighbor_count;
        node = start.neighbors[index];
        if (index == base_index || start.hull_id == node->hull_id)
        {
            return node;
        }
    }

    return node;
}

void add_neighbor_to_node(point_node_t *to_add, point_node_t *base, bool set_first)
{
    float a = angle(base->pos, to_add->pos);

    int prior_index = find_neighbor_index_with_bigger_angle(*base, a);

    for (int i = base->neighbor_count; i > prior_index + 1; i--)
    {
        base->neighbors[i] = base->neighbors[i - 1];
    }

    base->neighbors[prior_index + 1] = to_add;
    base->neighbor_count++;

    if (base->first == NULL || set_first)
    {
        base->first = to_add;
    }
}

void remove_neighbor_from_node(point_node_t *to_remove, point_node_t *base)
{
    if (base->first == to_remove)
    {
        base->first = ctr_clockwise(*base->first, *base);
    }
    if (base->first == to_remove)
    {
        base->first = NULL;
    }

    bool should_swap = false;

    for (int i = 0; i < base->neighbor_count; i++)
    {
        if (should_swap)
        {
            base->neighbors[i - 1] = base->neighbors[i];
        }
        else if (base->neighbors[i] == to_remove)
        {
            should_swap = true;
        }
    }

    base->neighbor_count--;
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

// Returns the 2D cross product between the two vectors o->p and o->s
// Calculates the cross multiplication, p0.x * p1.y - p0.y * p1.x
float cross(Vector2 s, Vector2 o, Vector2 p)
{
    return ((p.x - o.x) * (s.y - o.y) - (p.y - o.y) * (s.x - o.x));
}

// Returns the next clockwise neighbor to point target from point base
point_node_t *clockwise(point_node_t end, point_node_t start)
{
    int base_index = find_neighbor_index(end, start);

    int index = base_index;

    point_node_t *node;

    while (true)
    {
        index = ((index - 1) + start.neighbor_count) % start.neighbor_count;
        node = start.neighbors[index];
        if (index == base_index || start.hull_id == node->hull_id)
        {
            return node;
        }
    }
}

bool is_point_in_point_list(point_node_t p, point_node_list_t list)
{
    for (int i = 0; i < list.count; i++)
    {
        if (v_eq_v(p.pos, list.items[i].pos))
        {
            return true;
        }
    }

    return false;
}

bool is_right_of(point_node_t target, point_node_t p1, point_node_t p2)
{
    return cross(target.pos, p1.pos, p2.pos) < 0;
}

bool is_left_of(point_node_t target, point_node_t p1, point_node_t p2)
{
    return cross(target.pos, p1.pos, p2.pos) > 0;
}

point_node_list_t get_subarray(point_node_list_t origin, int start, int end)
{
    point_node_list_t list = (point_node_list_t){
        .items = origin.items + start,
        .count = end - start,
        .hull_id = hull_counter,
    };

    hull_counter++;

    for (int i = 0; i < list.count; i++)
    {
        list.items[i].hull_id = list.hull_id;
    }

    return list;
}

void get_tangents(point_node_list_t left, point_node_list_t right, point_node_t **lower, point_node_t **upper)
{
    int max_count = left.count + right.count;

    // Get rightmost node in left
    point_node_t *x = &left.items[left.count - 1];

    // Get leftmost node in right
    point_node_t *y = &right.items[0];

    // Get first node connected to y
    point_node_t *z = y->neighbors[0];

    // Get first node connected to x
    point_node_t *z1 = x->neighbors[0];

    point_node_t *z2 = clockwise(*z1, *x);

    // Walk clockwise around left list and ctr_clockwise around right list until we found the lowest connection x->y
    for (int i = 0; i < max_count; i++)
    {
        if (is_right_of(*z, *x, *y))
        {
            point_node_t *old_z = z;
            z = ctr_clockwise(*y, *z);
            y = old_z;
        }
        else if (is_right_of(*z2, *x, *y))
        {
            point_node_t *old_z2 = z2;
            z2 = clockwise(*x, *z2);
            x = old_z2;
        }
        else
        {
            lower[0] = x;
            lower[1] = y;
            break;
        }
    }

    // Get the rightmost node in left
    x = &left.items[left.count - 1];

    // Get the leftmost node in right
    y = &right.items[0];

    // Get first node connected to y
    z = y->neighbors[0];

    z1 = clockwise(*z, *y);

    // Get first node connected to x
    z2 = x->neighbors[0];

    // Walk ctr_clockwise around left list and clockwise around right list until we found the highest connection x->y
    for (int i = 0; i < max_count; i++)
    {
        if (is_left_of(*z1, *x, *y))
        {
            point_node_t *old_z1 = z1;
            z1 = clockwise(*y, *z1);
            y = old_z1;
        }
        else if (is_left_of(*z2, *x, *y))
        {
            point_node_t *old_z2 = z2;
            z2 = ctr_clockwise(*x, *z2);
            x = old_z2;
        }
        else
        {
            upper[0] = x;
            upper[1] = y;
            break;
        }
    }
}

void join(point_node_t *p1, point_node_t *p2, bool set_first)
{
    add_neighbor_to_node(p2, p1, set_first);
    add_neighbor_to_node(p1, p2, false);
}

void unjoin(point_node_t *p1, point_node_t *p2)
{
    remove_neighbor_from_node(p2, p1);
    remove_neighbor_from_node(p1, p2);
}

// Returns true if the node q is outside the circumcircle of p1->p2->p3
bool outside(point_node_t p1, point_node_t p2, point_node_t p3, point_node_t q)
{
    if (v_eq_v(p1.pos, q.pos) || v_eq_v(p2.pos, q.pos) || v_eq_v(p3.pos, q.pos))
    {
        return true;
    }

    circle_t c = circumcircle(p1.pos, p2.pos, p3.pos);

    float dx = q.pos.x - c.pos.x;
    float dy = q.pos.y - c.pos.y;
    float dsquared = dx * dx + dy * dy;

    return dsquared >= c.rsquared;
}

point_node_list_t add_hull(point_node_list_t left, point_node_list_t right)
{
    for (int i = 0; i < right.count; i++)
    {
        right.items[i].hull_id = left.hull_id;
    }

    return (point_node_list_t){
        .items = left.items,
        .count = left.count + right.count,
        .hull_id = left.hull_id,
    };
}

point_node_list_t merge(point_node_list_t left, point_node_list_t right)
{
    point_node_t *lower[2];
    point_node_t *upper[2];
    get_tangents(left, right, lower, upper);

    point_node_t *lower_left = lower[0];
    point_node_t *lower_right = lower[1];

    point_node_t *upper_left = upper[0];
    point_node_t *upper_right = upper[1];

    point_node_t *l = lower_left;
    point_node_t *r = lower_right;

    point_node_t *l1 = NULL;
    point_node_t *r1 = NULL;
    point_node_t *l2 = NULL;
    point_node_t *r2 = NULL;

    while (!v_eq_v(l->pos, upper_left->pos) || !v_eq_v(r->pos, upper_right->pos))
    {
        bool a = false;
        bool b = false;

        join(l, r, v_eq_v(l->pos, lower_left->pos) && v_eq_v(r->pos, lower_right->pos));

        r1 = clockwise(*l, *r);

        if (is_left_of(*r1, *l, *r))
        {
            r2 = clockwise(*r1, *r);

            while (!outside(*r1, *l, *r, *r2))
            {
                unjoin(r, r1);
                r1 = r2;
                r2 = clockwise(*r1, *r);
            }
        }
        else
        {
            a = true;
        }

        l1 = ctr_clockwise(*r, *l);

        if (is_right_of(*l1, *r, *l))
        {
            l2 = ctr_clockwise(*l1, *l);

            while (!outside(*l, *r, *l1, *l2))
            {
                unjoin(l, l1);
                l1 = l2;
                l2 = ctr_clockwise(*l1, *l);
            }
        }
        else
        {
            b = true;
        }

        if (a)
        {
            l = l1;
        }
        else if (b)
        {
            r = r1;
        }
        else if (outside(*l, *r, *r1, *l1))
        {
            r = r1;
        }
        else
        {
            l = l1;
        }
    }

    join(upper_right, upper_left, true);

    return add_hull(left, right);
}

void triangulate(point_node_list_t *points)
{
    switch (points->count)
    {
    case 2:
        add_neighbor_to_node(&points->items[1], &points->items[0], false);
        add_neighbor_to_node(&points->items[0], &points->items[1], false);
        break;
    case 3:
    {
        point_node_t *p1 = &points->items[0];
        point_node_t *p2 = &points->items[1];
        point_node_t *p3 = &points->items[2];

        float cross_result = cross(p2->pos, p1->pos, p3->pos);

        // Connect the points in counterclockwise direction
        if (cross_result < 0)
        {
            // p2 is right of p1->p3, therefore p2 is bottom of triangle
            add_neighbor_to_node(p2, p1, false);
            add_neighbor_to_node(p3, p2, false);
            add_neighbor_to_node(p1, p3, false);

            add_neighbor_to_node(p2, p3, false);
            add_neighbor_to_node(p1, p2, false);
            add_neighbor_to_node(p3, p1, false);
        }
        else if (cross_result > 0)
        {
            // p2 is left of p1->p3, therefore p2 is top of triangle
            add_neighbor_to_node(p3, p1, false);
            add_neighbor_to_node(p2, p3, false);
            add_neighbor_to_node(p1, p2, false);

            add_neighbor_to_node(p2, p1, false);
            add_neighbor_to_node(p3, p2, false);
            add_neighbor_to_node(p1, p3, false);
        }
        else
        {
            // p2 is on a line between p1 and p3, link left-to-right
            add_neighbor_to_node(p2, p1, false);
            add_neighbor_to_node(p3, p2, false);

            add_neighbor_to_node(p2, p3, false);
            add_neighbor_to_node(p1, p2, false);
        }
    }
    break;

    // > 3
    default:
    {
        int left_count = points->count / 2;

        point_node_list_t left = get_subarray(*points, 0, left_count);
        point_node_list_t right = get_subarray(*points, left_count, points->count);

        triangulate(&left);
        triangulate(&right);

        merge(left, right);
    }
    }
}

// OBS: REMEMBER TO FREE THE RETURNED ITEMS POINTER
point_node_list_t generate_delauney_edges(point_list_t points)
{
    // point_node_list_t nodes = point_list2node_list(points);

    point_node_t n1 = {
        .pos = {
            .x = 10,
            .y = 10,
        },
    };

    point_node_t n2 = {
        .pos = {
            .x = 300,
            .y = 300,
        },
    };

    point_node_t n3 = {
        .pos = {
            .x = 500,
            .y = 100,
        },
    };

    point_node_t n4 = {
        .pos = {
            .x = 600,
            .y = 600,
        },
    };

    point_node_t n5 = {
        .pos = {
            .x = 900,
            .y = 900,
        },
    };

    point_node_t n6 = {
        .pos = {
            .x = 400,
            .y = 900,
        },
    };

    point_node_t *n = calloc(6, sizeof(point_node_t));

    n[0] = n1;
    n[1] = n2;
    n[2] = n3;
    n[3] = n4;
    n[4] = n5;
    n[5] = n6;

    point_node_list_t nodes = {
        .items = n,
        .count = 6,
        .hull_id = 0,
    };

    hull_counter++;

    sort_points(&nodes);

    printf("%f, %f\n", nodes.items[2].pos.x, nodes.items[2].pos.y);

    triangulate(&nodes);

    printf("NODE 0: %f, %f | %d\n", nodes.items[0].pos.x, nodes.items[0].pos.y, nodes.items[0].neighbor_count);
    printf("NODE 1: %f, %f | %d\n", nodes.items[1].pos.x, nodes.items[1].pos.y, nodes.items[0].neighbor_count);

    printf("POINTER TO 1: %p\n", nodes.items[0].neighbors[0]);
    printf("POINTER TO 0: %p\n", nodes.items[1].neighbors[0]);

    return nodes;
}
