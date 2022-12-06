#include "common.h"

typedef struct
{
    float a;
    float b;
    float c;
} bisect_t;

typedef struct
{
    Vector2 pos;
    float rsquared;
} circle_t;

Vector2 line_intersection(bisect_t b1, bisect_t b2)
{
    float denom = b1.b * b2.a - b1.a * b2.b;

    if (denom == 0)
    {
        return (Vector2){
            .x = -1,
            .y = -1,
        };
    }

    return (Vector2){
        .x = (b1.b * b2.c - b1.c * b2.b) / denom,
        .y = (b1.c * b2.a - b1.a * b2.c) / denom,
    };
}

bisect_t perpendicular_bisector(Vector2 v1, Vector2 v2)
{
    return (bisect_t){
        .a = v2.x - v1.x,
        .b = v2.y - v1.y,
        .c = ((v2.x * v2.x) - (v1.x * v1.x) + (v2.y * v2.y) - (v1.y * v1.y)) / 2,
    };
}

Vector2 circumcenter(Vector2 v1, Vector2 v2, Vector2 v3)
{
    bisect_t b1 = perpendicular_bisector(v1, v2);
    bisect_t b2 = perpendicular_bisector(v2, v3);

    return line_intersection(b1, b2);
}

circle_t circumcircle(Vector2 v1, Vector2 v2, Vector2 v3)
{
    Vector2 v = circumcenter(v1, v2, v3);

    if (v.x == -1 && v.y == -1)
    {
        return (circle_t){.pos = v};
    }

    float dx = v.x - v1.x;
    float dy = v.y - v1.y;

    float rsqrd = dx * dx + dy * dy;

    return (circle_t){
        .pos = v,
        .rsquared = rsqrd,
    };
}
