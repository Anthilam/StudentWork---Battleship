/*
	Battleship

	Student Work - UFR-ST - Licence 2

	GUY Timothée , LAURENT Timothée

		- servio.c : main loop, server communication
*/

#include "servio.h"

#define BUFSIZE 256

int main(int argc, char *argv[]) {
  setbuf(stdout, NULL);

  srand(time(NULL)*getpid());

  int start = 0; // Used to trigger the main loop
  char buffer[BUFSIZE]; // Main buffer

  bool foe_grid[10][10]; // Ennemy field
  bool my_grid[10][10]; // Our field

	// Initialization of the grids to false
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			foe_grid[i][j] = false;
			my_grid[i][j] = false;
		}
	}

	/* Generation of polls (X) coordinates to match this pattern

		_|A|B|C|D|E|F|G|H|I|J|
		0|_|_|_|_|_|_|_|_|_|_|
	  1|_|C|_|5|_|_|9|_|D|_|
		2|_|_|_|_|_|_|_|_|_|_|
		3|_|8|_|0|_|_|1|_|6|_|
		4|_|_|_|_|_|_|_|_|_|_|
		5|_|_|_|_|_|_|_|_|_|_|
		6|_|4|_|3|_|_|2|_|A|_|
		7|_|_|_|_|_|_|_|_|_|_|
		8|_|E|_|B|_|_|7|_|F|_|
		9|_|_|_|_|_|_|_|_|_|_|

	  This pattern covers all the grid
  */

  struct coord poll_sites[16];
	bool poll_empty[16];

	for (int i = 0; i < 16; i++) {
		poll_sites[i].x = 0;
		poll_sites[i].y = 0;
		poll_empty[i] = false;
	}

  to_coord(poll_sites+0,"D3",3);
	to_coord(poll_sites+1,"G3",3);
	to_coord(poll_sites+2,"G6",3);
	to_coord(poll_sites+3,"D6",3);
	to_coord(poll_sites+4,"B6",3);
	to_coord(poll_sites+5,"D1",3);
	to_coord(poll_sites+6,"I3",3);
	to_coord(poll_sites+7,"G8",3);
	to_coord(poll_sites+8,"B3",3);
	to_coord(poll_sites+9,"G1",3);
	to_coord(poll_sites+10,"I6",3);
	to_coord(poll_sites+11,"D8",3);
	to_coord(poll_sites+12,"B1",3);
	to_coord(poll_sites+13,"I1",3);
	to_coord(poll_sites+14,"B8",3);
	to_coord(poll_sites+15,"I8",3);

  // Generation of mines and sending to server
  struct coord mines[5];
  for (int i = 0; i < 5; i++) {
    place_mine(mines, i, foe_grid);
    char coord_mine[3];
    to_string(&mines[i], coord_mine, 3);
    printf("%s\n", coord_mine);
  }

  // Generation of ships
  struct ship boats[5];
	int b = 0;
  
	do {
		for (int i = 0; i < 5; i++) {
			b = place_ship(boats, i, my_grid);
			// If place_ship fails
			if (b == 1) {
				// Reset of our grid
				for (int i = 0; i < 10; i++) {
					for (int j = 0; j < 10; j++) {
						my_grid[i][j] = false;
					}
				}
				// Restart of the loop
				break;
			}
		}
	} while (b == 1);

  // Ships sending to server
  for (int i = 0; i < 5; i++) {
    char coord_boat_front[3]; // Front coordinates of the ship
    char coord_boat_back[3];	// Back coordinates of the ship
    to_string(boats[i].front, coord_boat_front, 3);
    to_string(boats[i].back, coord_boat_back, 3);

    fprintf(stderr, "SHIP #%d : %s%s\n", i, coord_boat_front, coord_boat_back);

		printf("%s%s\n", coord_boat_front, coord_boat_back);

    fgets(buffer, BUFSIZE, stdin);
    if (strcmp(buffer, "BOOM\n") != 0) {
      assert(strcmp(buffer, "OK\n") == 0);
    }
  }

  // Loop to receive the "KABOOM" till we got a "START" from the server
  do {
    fgets(buffer, BUFSIZE, stdin);
    if (strcmp(buffer, "START\n") == 0) {
      start = 1;
    }
    else if (strstr(buffer, "KABOOM") == 0) {
      struct coord kaboom;
      to_coord(&kaboom, buffer+7, strlen(buffer));
			// Register the coordinates of the KABOOM
      foe_grid[kaboom.x][kaboom.y] = true;
    }
  } while (start == 0);

  //--MAIN LOOP-----------------------------------------------------------------
  int nextaction = 0; // Set the next action to do
  int countpoll = 0; // Count the polls
  struct coord probe; // Coordinates of a poll
  struct coord shot; // Coordinates of a shot
	bool pollhit = false; // Used to trigger a special poll

  do {
    fprintf(stderr, "-------------------------------------\n");

    //--------------------PLAYER'S ACTIONS--------------------//
		// If we haven't hit a ship, we do polls following the previous pattern
    if (nextaction == 0 && !pollhit) {
			bool b = false;
			for (int i = 0; i < 16; ++i) {
				if (poll_empty[i] == false) {
					b = true;
					break;
				}
			}

			if (b == true) {
				for (int i = 0; i < 16; ++i) {
					poll_empty[i] = false;
				}
			}

			do {
	      countpoll++;
	      if (countpoll > 15) {
	        countpoll = 0;
	      }
			} while (poll_empty[countpoll] == true);

      probe = poll_sites[countpoll];
      poll(&probe);
    }
		// Otherwise we poll right where we shot previously, because we are sure to find the other parts of the ship we had hit
		else if (pollhit == true) {
			poll(&shot);
			pollhit = false;
		}

		// If nextaction is 1, we shot to the coordinates of shot
    if (nextaction == 1) {
      shoot(&shot);
    }

    //--------------------SERVER'S RESPONSE--------------------//
    fgets(buffer, BUFSIZE, stdin);
    fprintf(stderr, "%s", buffer);
    if (strcmp(buffer, "HIT\n") == 0) {
			// Register the coordinates of the shot
      foe_grid[shot.x][shot.y] = true;
			// Next action is a poll where we had shot
      nextaction = 0;
			pollhit = true;
    }
    else if (strcmp(buffer, "MISS\n") == 0) {
      // Register the coordinates of the shot
      foe_grid[probe.x][probe.y] = true;
			// Next action is a poll following the pattern
      nextaction = 0;
    }
    else if (strcmp(buffer, "SHIP\n") == 0) {
      // Reads the coordinates of the ship transmitted by the server
      fgets(buffer, BUFSIZE, stdin);
      fprintf(stderr, "\t%s", buffer);
			// Next action is a shot
      to_coord(&shot, buffer, 3);
      nextaction = 1;
    }
    else if (strcmp(buffer, "EMPTY\n") == 0) {
      // Ajouter le sondage raté dans un tableau
      foe_grid[probe.x][probe.y] = true;
			// Next action is a poll following the pattern
			poll_empty[countpoll] = true;
      nextaction = 0;
    }
		else {
      nextaction = 0;
    }

    //--------------------ENNEMY'S ACTIONS--------------------//
    fgets(buffer, BUFSIZE, stdin);
    fprintf(stderr, "Adversaire: ");
    fprintf(stderr, "%s", buffer);
    if (strcmp(buffer, "KABOOM\n") == 0) {
      fgets(buffer, BUFSIZE, stdin);
      struct coord kaboom;
      to_coord(&kaboom, buffer+7, strlen(buffer));
			// Register the coordinates of the KABOOM
      foe_grid[kaboom.x][kaboom.y] = true;
    }
    else if (strcmp(buffer, "MISSED\n") == 0) {
      fgets(buffer, BUFSIZE, stdin);
			/* Useful to implement move, we could move away the ships from
			the coordinates sent by the server
      struct coord missed;
      to_coord(&missed, buffer+7, strlen(buffer));*/
    }
    else if (strcmp(buffer, "ATTACK\n") == 0) {
      fgets(buffer, BUFSIZE, stdin);
      /* Useful to implement move, we could move away the ships from
			the coordinates sent by the server
			struct coord attack;
      to_coord(&attack, buffer+7, strlen(buffer));*/
    }
  } while (start == 1);

  return EXIT_SUCCESS;
}
