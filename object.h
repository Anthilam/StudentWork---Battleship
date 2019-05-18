/*
	Battleship

	Student Work - UFR-ST - Licence 2

	GUY Timothée , LAURENT Timothée

		- object.c : functions library of the project
*/

#ifndef OBJECT_H
#define OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

struct coord {
	int x;
	int y;
};

struct ship {
	struct coord *front;
	struct coord *back;
	size_t size;
};

// Returns the coordinate from a struct coord to a string format
void to_string(const struct coord *self, char *strres, size_t size);

// Returns the coordinate from a string format to a struct coord format
void to_coord(struct coord *self, char* str, size_t size);

// Compares two coordinates ( 0 if equal, < 0 if first < second, > 0 if first > second)
int compare_coord(const struct coord *first, const struct coord *second);

// Checks if a coordinate is valid (x, y >= 0  && x, y < 10)
bool check_coord(const struct coord *self);

// Checks if a ship placement is valid
bool check_ship(const struct ship *self);

// Creates a ship, return 0 if the creation is successful, 1 if not
int place_ship(struct ship *self, int i, bool grid[][10]);

// Checks if a coord belongs to an array
bool coord_is_in(struct coord *checkArray, size_t size, const struct coord *self);

// Moves a ship of 1 case in the given direction (1 or -1);
void move(struct ship *self, int dir);

// Creates a mine in the mines array
void place_mine(struct coord *mines, int i, bool grid[][10]);

// Takes a coordinate and polls there
void poll(const struct coord *self);

// Takes a coordinate and shoots there
void shoot(const struct coord *self);

#endif
