#include "common.h"

int hull_counter = 0;

// OBS: REMEMBER TO FREE THE NODE LIST ITEMS POINTER
// Creates a new hull from a point list and assigns the hull ids
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

// Find the index of the neighbor of base matching *target*'s position, returns -1 if not found
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

/**
 * @brief Finds the index of the neighbor of base which has a bigger angle, relative to base, than a
 *
 * @param base The node in which to search for the neighbor in
 * @param a The angle that should be compared with
 * @return The index of the neighbor that has a bigger angle, otherwise the last index as none has a bigger angle
 */
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

/**
 * @brief Returns the next node connected to the base_node, starting with the start_node and moving clockwise around the base node, which is also part of the same hull as start_node
 *
 * @param start_node The node to start the search at
 * @param base_node The node which neighbors should be searched
 * @return A pointer to the resulting node
 */
point_node_t *ctr_clockwise(point_node_t start_node, point_node_t base_node)
{
    int start_index = find_neighbor_index(start_node, base_node);

    int index = start_index;

    point_node_t *node;

    while (true)
    {
        index = (index + 1) % base_node.neighbor_count;
        node = base_node.neighbors[index];
        if (index == start_index || start_node.hull_id == node->hull_id)
        {
            return node;
        }
    }

    return node;
}

/**
 * @brief Adds the given node to the base nodes list of neighbors and places it in the right spot, according to the angle relative to the base node
 *
 * @param node_to_add The node to add
 * @param base_node The node that will receive the other node
 * @param should_set_first Sets if the new node should be set as the first node connected on the base node
 */
void add_neighbor_to_node(point_node_t *node_to_add, point_node_t *base_node, bool should_set_first)
{
    float a = angle(base_node->pos, node_to_add->pos);

    int prior_index = find_neighbor_index_with_bigger_angle(*base_node, a);

    for (int i = base_node->neighbor_count; i > prior_index + 1; i--)
    {
        base_node->neighbors[i] = base_node->neighbors[i - 1];
    }

    base_node->neighbors[prior_index + 1] = node_to_add;
    base_node->neighbor_count++;

    if (base_node->first == NULL || should_set_first)
    {
        base_node->first = node_to_add;
    }
}

/**
 * @brief Removes the given node from the base nodes list of neighbors, and changes the base nodes first node accordingly
 *
 * @param node_to_remove The node to remove
 * @param base_node The node that will the other node will be removed from
 */
void remove_neighbor_from_node(point_node_t *node_to_remove, point_node_t *base_node)
{
    if (base_node->first == node_to_remove)
    {
        base_node->first = ctr_clockwise(*base_node->first, *base_node);
    }
    if (base_node->first == node_to_remove)
    {
        base_node->first = NULL;
    }

    bool should_swap = false;

    for (int i = base_node->neighbor_count; i < 0; i++)
    {
        if (should_swap)
        {
            base_node->neighbors[i - 1] = base_node->neighbors[i];
        }
        else if (base_node->neighbors[i] == node_to_remove)
        {
            should_swap = true;
        }
    }

    base_node->neighbor_count--;
}

/**
 * @brief Sorts the given list of nodes by their positions x value in ascending order, i.e from left to right
 *
 * @param points The list of nodes to be sorted
 */
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
double cross(Vector2 s, Vector2 o, Vector2 p)
{
    // return ((p.x - o.x) * (s.y - o.y) - (p.y - o.y) * (s.x - o.x));
    return ((p.x - o.x) * (s.y - o.y)) - ((p.y - o.y) * (s.x - o.x));
}

/**
 * @brief Returns the next node connected to the base_node, starting with the start_node and moving clockwise around the base node, which is also part of the same hull as start_node
 *
 * @param start_node The node to start the search at
 * @param base_node The node which neighbors should be searched
 * @return A pointer to the resulting node
 */
point_node_t *clockwise(point_node_t start_node, point_node_t base_node)
{
    int start_index = find_neighbor_index(start_node, base_node);

    int index = start_index;

    point_node_t *node;

    while (true)
    {
        index = ((index - 1) + base_node.neighbor_count) % base_node.neighbor_count;
        node = base_node.neighbors[index];
        if (index == start_index || base_node.hull_id == node->hull_id)
        {
            return node;
        }
    }
}

/**
 * @brief Checks if the given node is to the right of the vector p1->p2
 *
 * @param target The node to check
 * @param p1 The source point of the vector
 * @param p2 The end point of the vector
 * @return true if the node is to the right of the vector p1->p2, and false otherwise
 */
bool is_right_of(point_node_t target, point_node_t p1, point_node_t p2)
{
    return cross(target.pos, p1.pos, p2.pos) < 0;
}

/**
 * @brief Checks if the given node is to the left of the vector p1->p2
 *
 * @param target The node to check
 * @param p1 The source point of the vector
 * @param p2 The end point of the vector
 * @return true if the node is to the left of the vector p1->p2, and false otherwise
 */
bool is_left_of(point_node_t target, point_node_t p1, point_node_t p2)
{
    return cross(target.pos, p1.pos, p2.pos) > 0;
}

/**
 * @brief Creates a new node list from the original node list, inheriting from the indices given. Also creates a new hull and assigns all nodes to that hull id
 *
 * @param origin The list that the new list will inherit from
 * @param start The start index from where the new list will begin
 * @param end The end index from where the new list will stop
 * @return The newly created node list
 */
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

/**
 * @brief Finds the two most lower nodes of both lists, and the two most upper nodes of both lists
 *
 * @param left_node_list The leftmost node list to search
 * @param right_node_list The rightmost node list to search
 * @param lower_pair The array that will contain the pair of the two most lower nodes of the two lists
 * @param upper_pair The array that will contain the pair of the two most upper nodes of the two lists
 */
void get_limit_nodes(point_node_list_t left_node_list, point_node_list_t right_node_list, point_node_t **lower_pair, point_node_t **upper_pair)
{
    int max_count = left_node_list.count + right_node_list.count;

    // Get rightmost node in left_node_list
    point_node_t *x = &left_node_list.items[left_node_list.count - 1];

    // Get leftmost node in right_node_list
    point_node_t *y = &right_node_list.items[0];

    // Get first node connected to y
    point_node_t *z = y->first;

    // Get first node connected to x
    point_node_t *z1 = x->first;

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
            lower_pair[0] = x;
            lower_pair[1] = y;
            break;
        }
    }

    // Get the rightmost node in left_node_list
    x = &left_node_list.items[left_node_list.count - 1];

    // Get the leftmost node in right_node_list
    y = &right_node_list.items[0];

    // Get first node connected to y
    z = y->first;

    z1 = clockwise(*z, *y);

    // Get first node connected to x
    z2 = x->first;

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
            upper_pair[0] = x;
            upper_pair[1] = y;
            break;
        }
    }
}

/**
 * @brief Joins the two given nodes together, adding them both to eachother as neighbors
 *
 * @param p1 The first node that will be joined
 * @param p2 The second node that will be joined
 * @param set_first Sets if node p1 should have their first connected node be set to the newly added node
 */
void join(point_node_t *p1, point_node_t *p2, bool set_first)
{
    add_neighbor_to_node(p2, p1, set_first);
    add_neighbor_to_node(p1, p2, false);
}

// Unjoins the two given nodes together, removing them from eachother as neighbors
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

/**
 * @brief Joins the two node lists into one, with the left hulls id becoming the new id
 *
 * @param left The leftmost hull that will be joined by the rightmost hull
 * @param right The rightmost hull that will be joined into the left most hull
 * @return A new node list that will contain both the left and right hulls
 */
point_node_list_t add_hulls_together(point_node_list_t left, point_node_list_t right)
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

/**
 * @brief Merges two hulls together, making connections inbetween their nodes
 *
 * @param left The leftmost hull that will be merged
 * @param right The rightmost hull that will be merged
 * @return A new hull that will be a merge between the other two hulls
 */
point_node_list_t merge(point_node_list_t left, point_node_list_t right)
{
    point_node_t *lower[2];
    point_node_t *upper[2];
    get_limit_nodes(left, right, lower, upper);

    // The lowest nodes in each list, left and right
    point_node_t *lower_left = lower[0];
    point_node_t *lower_right = lower[1];

    // The highest nodes in each list, left and right
    point_node_t *upper_left = upper[0];
    point_node_t *upper_right = upper[1];

    // THE NODES THAT SHOULD BE CONNECTED TOGETHER
    // Sets the nodes that should connect to start at the lowest nodes
    point_node_t *left_current = lower_left;
    point_node_t *right_current = lower_right;

    // Other nodes spots
    point_node_t *first_ctr_clockwise_left = NULL;
    point_node_t *first_clockwise_right = NULL;
    point_node_t *second_ctr_clockwise_left = NULL;
    point_node_t *second_clockwise_right = NULL;

    // While the nodes that are/will be connected are not the highest node pair
    while (!v_eq_v(left_current->pos, upper_left->pos) || !v_eq_v(right_current->pos, upper_right->pos))
    {
        bool a = false;
        bool b = false;

        // Join the two nodes to be connected
        // If they are the lowest node pair, sets the left_current's first node to right_current
        join(left_current, right_current, v_eq_v(left_current->pos, lower_left->pos) && v_eq_v(right_current->pos, lower_right->pos));

        // Get the next node connected to the right node, that is clockwise of the left node,
        first_clockwise_right = clockwise(*left_current, *right_current);

        // If the new node is to the left of the current node pair
        if (is_left_of(*first_clockwise_right, *left_current, *right_current))
        {
            // Gets the next node connected to the right node, that is clockwise of the first clockwise
            second_clockwise_right = clockwise(*first_clockwise_right, *right_current);

            // While the second clockwise node is inside the circumcircle of the current node pair and the first clockwise node
            // Keep moving clockwise while unjoining the "leftmost" clockwise node with the current right node
            while (!outside(*first_clockwise_right, *left_current, *right_current, *second_clockwise_right))
            {
                unjoin(right_current, first_clockwise_right);
                first_clockwise_right = second_clockwise_right;
                second_clockwise_right = clockwise(*first_clockwise_right, *right_current);
            }
        }
        // If the next clockwise node on the right hull is to the right of the current node pair, it is invalid
        else
        {
            // Sets that the left counter clockwise node thing should be the node to be connected to next
            a = true;
        }

        // DOES THE SAME PROCEDURE WITH LEFT BUT REVERSED
        // CTR_CLOCKWISE INSTEAD OF CLOCKWISE AND TO THE RIGHT OF INSTEAD OF LEFT OF

        // Get the next node connected to the left node, that is counter_clockwise of the right node
        first_ctr_clockwise_left = ctr_clockwise(*right_current, *left_current);

        // If the next counter clockwise node is to the right of the current node pair
        if (is_right_of(*first_ctr_clockwise_left, *right_current, *left_current))
        {
            // Gets the next counter clockwise node from the first counter clockwise node, connected to the left node,
            second_ctr_clockwise_left = ctr_clockwise(*first_ctr_clockwise_left, *left_current);

            // While the second counter clockwise node is inside the circumcircle of the current node pair and the first coumter clockwise node
            // Keep moving counter clockwise while unjoining the "rightmost" counter clockwise node with the current left node
            while (!outside(*left_current, *right_current, *first_ctr_clockwise_left, *second_ctr_clockwise_left))
            {
                unjoin(left_current, first_ctr_clockwise_left);
                first_ctr_clockwise_left = second_ctr_clockwise_left;
                second_ctr_clockwise_left = ctr_clockwise(*first_ctr_clockwise_left, *left_current);
            }
        }
        // If the next counter clockwise node on the left hull is to the left of the current node pair, it is invalid
        else
        {
            // Sets that the right clockwise node should be the node to be connected to next
            b = true;
        }

        // If the first clockwise node on the right hull was to the right of the current node pair, connect the left side before the right
        if (a)
        {
            left_current = first_ctr_clockwise_left;
        }
        // If the first counter clockwise node on the left hull was to the left of the current node pair, connect the right side before the left
        else if (b)
        {
            right_current = first_clockwise_right;
        }
        // If the left choice is outside the circumcircle of the current node pair and the right choice, connect the right side before the left
        else if (outside(*left_current, *right_current, *first_clockwise_right, *first_ctr_clockwise_left))
        {

            right_current = first_clockwise_right;
        }
        // If the left choice is inside the circumcircle of the current node pair and the right choice, connect the left side before the right
        else
        {
            left_current = first_ctr_clockwise_left;
        }
    }

    // When finished, connect last two nodes which are also the two most upper nodes
    join(upper_right, upper_left, true);

    // Combine the two hulls into one
    return add_hulls_together(left, right);
}

/**
 * @brief Connectes a unconnected list of points into a hull with delauney triangles
 *
 * @param points A list of unconnected points to be connected
 */
void triangulate(point_node_list_t *points)
{
    switch (points->count)
    {
    // If there are two points,
    case 2:
        join(&points->items[0], &points->items[1], false);
        break;
    case 3:
    {
        point_node_t *p1 = &points->items[0];
        point_node_t *p2 = &points->items[1];
        point_node_t *p3 = &points->items[2];

        double cross_result = cross(p2->pos, p1->pos, p3->pos);

        // Connect the points in counterclockwise direction
        if (cross_result > 0)
        {
            // p2 is right of p1->p3, therefore p2 is bottom of triangle
            add_neighbor_to_node(p2, p1, false);
            add_neighbor_to_node(p3, p2, false);
            add_neighbor_to_node(p1, p3, false);

            add_neighbor_to_node(p2, p3, false);
            add_neighbor_to_node(p1, p2, false);
            add_neighbor_to_node(p3, p1, false);
        }
        else if (cross_result < 0)
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

        // Split the node list into two halves on the x axis
        point_node_list_t left = get_subarray(*points, 0, left_count);
        point_node_list_t right = get_subarray(*points, left_count, points->count);

        // Triangulate each halve seperately
        triangulate(&left);
        triangulate(&right);

        // Set the node list to the merge of the two triangulated halves
        *points = merge(left, right);
    }
    }
}

// OBS: REMEMBER TO FREE THE RETURNED ITEMS POINTER
point_node_list_t generate_delauney_edges(point_list_t points)
{
    // point_node_list_t nodes = point_list2node_list(points);

    point_node_t n1 = {
        .pos = {
            .x = 300,
            .y = 100,
        },
    };

    point_node_t n2 = {
        .pos = {
            .x = 100,
            .y = 500,
        },
    };

    point_node_t n3 = {
        .pos = {
            .x = 500,
            .y = 1000,
        },
    };

    point_node_t n4 = {
        .pos = {
            .x = 600,
            .y = 100,
        },
    };

    point_node_t n5 = {
        .pos = {
            .x = 1000,
            .y = 300,
        },
    };

    point_node_t *n = calloc(5, sizeof(point_node_t));

    n[0] = n1;
    n[1] = n2;
    n[2] = n3;
    n[3] = n4;
    n[4] = n5;

    point_node_list_t nodes = {
        .items = n,
        .count = 5,
        .hull_id = 0,
    };

    double a = cross((Vector2){.x = 700, .y = 600}, (Vector2){500, 500}, (Vector2){1000, 500});

    double b = cross((Vector2){.x = 700, .y = 400}, (Vector2){500, 500}, (Vector2){1000, 500});

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
