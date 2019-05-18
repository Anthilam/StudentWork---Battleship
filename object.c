/*
	Battleship

	Student Work - UFR-ST - Licence 2

	GUY Timothée , LAURENT Timothée

		- object.c : functions library of the project
*/

#include "object.h"

// Returns the coordinate from a struct coord to a string format
void to_string(const struct coord *self, char *strres, size_t size) {
	assert(size >= 3);

	strres[0] = 'A' + self->x;
	strres[1] = '0' + self->y;
	strres[2] = '\0';
}

// Returns the coordinate from a string format to a struct coord format
void to_coord(struct coord *self, char* str, size_t size) {
	assert(size >= 3);

	self->x = str[0] - 'A';
	self->y = str[1] - '0';
}

// Takes a coordinate and polls there
void poll(const struct coord *self) {
	char pos[3];
	to_string(self, pos, 3);
	printf("POLL\n");
	printf("%s\n", pos);

	fprintf(stderr, "POLL %s\n", pos);
}

// Takes a coordinate and shoots there
void shoot(const struct coord *self) {
	char pos[3];
	to_string(self, pos, 3);
	printf("SHOOT\n");
	printf("%s\n", pos);
	fprintf(stderr, "SHOOT %s\n", pos);
}

// Checks if a coordinate is valid (x, y >= 0  && x, y < 10)
bool check_coord(const struct coord *self) {
	assert(self);

	return (self->x >= 0 && self->x < 10 && self->y >= 0 && self->y < 10);
}

// Checks if a coord belongs to an array
bool coord_is_in(struct coord *checkArray, size_t size, const struct coord *self) {
	assert(checkArray);
	assert(self);

	for (int i = 0; i < (int)size; i++) {
		if (compare_coord(&checkArray[i], self) == 0) {
			return true;
		}
	}

	return false;
}

// Checks if a ship placement is valid
bool check_ship(const struct ship *self) {
	assert(self);

	if (!check_coord(self->front) || !check_coord(self->back)) {
		return false;
	}

	if (self->front->x != self->back->x && self->front->y != self->back->y) {
		return false;
	}

	if (self->front->x == self->back->x && abs(self->front->y - self->back->y) != (int)self->size) {
		return false;
	}

	if (self->front->y == self->back->y && abs(self->front->x - self->back->x) != (int)self->size) {
		return false;
	}

	return true;
}

// Moves a ship of 1 case in the given direction (1 or -1);
void move(struct ship *self, int dir) {
	assert(self);

	if (self->front->x == self->back->x) {
		self->front->x += dir;
		self->back->x += dir;

		if (!check_coord(self->front) || !check_coord(self->back)) {
			self->front->x -= dir;
			self->back->x -= dir;
		}
	}

	if (self->front->y == self->back->y) {
		self->front->y += dir;
		self->back->y += dir;

		if (!check_coord(self->front) || !check_coord(self->back)) {
			self->front->y -= dir;
			self->back->y -= dir;
		}
	}
}

// Compares two coordinates ( 0 if equal, < 0 if first < second, > 0 if first > second)
int compare_coord(const struct coord *first, const struct coord *second) {
	if (first->y != second->y) {
		return (first->y - second->y);
	}

	if (first->x != second->x) {
		return (first->x - second->x);
	}

	return 0;
}

// Creates a mine in the mines array
void place_mine(struct coord *mines, int i, bool grid[][10]) {
	assert (i >= 0);
	assert (i < 5);

	/*
		Each mine has a specific spawning area that is determined by the switch below
		Representation of the spawning area of the mines:

		_|_|_|0|0|0|0|_|_|_|
		_|_|_|0|0|0|0|_|_|_|
		_|_|_|0|0|0|0|_|_|_|
		1|1|1|2|2|2|2|3|3|3|
		1|1|1|2|2|2|2|3|3|3|
		1|1|1|2|2|2|2|3|3|3|
		1|1|1|2|2|2|2|3|3|3|
		_|_|_|4|4|4|4|_|_|_|
		_|_|_|4|4|4|4|_|_|_|
		_|_|_|4|4|4|4|_|_|_|

		Note: two mines can't spawn next to each other
	*/

	int zoneX = 0;
	int zoneY = 0;
	int moduloX = 0;
	int moduloY = 0;

	switch (i) {
		//Mine 0: Top mine
		case 0:
			zoneX = 3; zoneY = 0;
			moduloX = 4; moduloY = 3;
			break;
		//Mine 1: Left mine
		case 1:
			zoneX = 0; zoneY = 3;
			moduloX = 3; moduloY = 4;
			break;
		//Mine 2: Center mine
		case 2:
			zoneX = 3; zoneY = 3;
			moduloX = 4; moduloY = 4;
			break;
		//Mine 3: Right mine
		case 3:
			zoneX = 7; zoneY = 3;
			moduloX = 3; moduloY = 4;
			break;
		//Mine 4: Bottom mine
		case 4:
			zoneX = 3; zoneY = 7;
			moduloX = 4; moduloY = 3;
			break;
		default:
			fprintf(stderr, "Error while placing mine");
	}

	// While the coordinates are considered as taken (already a mine there or in the surrounding area)
	do {
		mines[i].x = rand()%moduloX + zoneX;
		mines[i].y = rand()%moduloY + zoneY;
	} while(grid[mines[i].x][mines[i].y]);

	// double loop for all the surrounding cases around a mine
	for (int m = -1; m <= 1; m++) {
		for (int n = -1; n <= 1; n++) {
			struct coord new;
			new.x = mines[i].x+m;
			new.y = mines[i].y+n;
			grid[new.x][new.y] = true;
		}
	}
}

// Creates a ship, return 0 if the creation is successful, 1 if not
int place_ship(struct ship *self, int i, bool grid[][10]) {
	assert (i >= 0);
	assert (i < 5);

	int size = 0; // Size of the ship
	int dir = 0; // Direction of the ship
	int cretry = 0; // Counter of retry
	bool retry = false;

	// Determines the size of the ship based on the index that is sent to the function
	switch (i) {
		case 0: size = 5;break;
		case 1:	size = 4;break;
		case 2:	size = 3;break;
		case 3:	size = 3;break;
		case 4:	size = 2;break;
	}

	// Allocates memory for the ship's coordinates
	self[i].front = malloc(sizeof(struct coord));
	self[i].back = malloc(sizeof(struct coord));

	do {
		// If after 100 attempts of retry no position has been found, 1 is returned
		cretry++;
		if (cretry > 100) {
			return 1;
		}

		retry = false; // Resets retry
		dir = rand()%2; // 0 = horizontal direction, 1 = vertical direction

		// New coordinates of the ship
		struct coord new;

		// If the direction is horizontal
		if (dir == 0) {
			// Selects random coordinates for the front of the ship
			new.x = rand()%(10-size);
			new.y = rand()%10;

			// Checks if the ships coordinates are already occupied
			// Retry is set to true if it's the case
			for (int m = new.x; m < new.x + size; m++) {
				if (grid[m][new.y]) {
					retry = true;
				}
			}

			// If the ship placement is correct
			if (!retry) {
				// Sets the ship coordinates to its new values
				self[i].front->x = new.x;
				self[i].front->y = self[i].back->y = new.y;
				self[i].back->x = new.x + size-1;

				// Sets the positions of the ship and its surrounding to true in the boolean grid
				for (int m = new.x-2; m < new.x + size+1; m++) {
					for (int n = new.y-3; n < new.y+3; n++) {
						struct coord newbool;
						newbool.x = m;
						newbool.y = n;

						if (check_coord(&newbool)) {
							grid[newbool.x][newbool.y] = true;
						}
					}
				}
			}
		}
		// Same for vertical direction
		else {
			new.x = rand()%10;
			new.y = rand()%(10-size);

			for (int m = new.y; m < new.y + size; m++) {
				if (grid[new.x][m]) {
					retry = true;
				}
			}

			if (!retry) {
				self[i].front->y = new.y;
				self[i].front->x = self[i].back->x = new.x;
				self[i].back->y = new.y + size-1;
				
				for (int m = new.y-2; m < new.y + size+1; m++) {
					for (int n = new.x-2; n < new.x+2; n++) {
						struct coord newbool;
						newbool.x = n;
						newbool.y = m;

						if (check_coord(&newbool)) {
							grid[newbool.x][newbool.y] = true;
						}
					}
				}
			}
		}
	} while(retry);

	// Sets the size of the ship
	self[i].size = size;

	return 0;
}
