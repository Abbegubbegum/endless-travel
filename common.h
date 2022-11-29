// EXAMPLE
// NOT USED IN CODE

#ifndef __COMMON_H_
#define __COMMON_H_

city_tile_t city_tiles[];

int city_coord2index(int x, int y);

Vector2 city_index2coord(int i);

Vector2 city_coord2screen(int x, int y);

#endif