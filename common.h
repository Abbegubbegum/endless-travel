// EXAMPLE
// NOT USED IN CODE

#ifndef __COMMON_H_
#define __COMMON_H_

map_tile_t map_tiles[];

int coord2index(int x, int y);

Vector2 index2coord(int i);

Vector2 coord2screen(int x, int y);

#endif