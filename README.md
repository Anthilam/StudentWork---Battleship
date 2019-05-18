# StudentWork - Battleship

An automatic battleship program.

## Game rules

Each player has a 10x10 area where he can put ships.
Columns are labeled from *A* to *J* and lines from *0* to *9*. Coordinates are identified by the column and the line, for exemple *D7*.

At the beginning of the game, each player puts 5 **mines** in the opponent area. The mines doesn't move and are hidden to the opponent player.

Then, each player puts 5 **ships** in their areas.  Ships can't overlap. If a ship is put on a mine, it blows up immediately.

When the game starts, at each turn, a player can :
    - launch a missile to specific coordinates > the player knows if its a hit;
    - probe specific coordinates > the player will know if there's a ship at these coordinates or around these coordinates;
    - step a specific ship in its base direction (a ship cannot turn), if the ship hits a mine, it blows up.

## Build

Build the program with `make`.

Clean object files using `make clean`.

Clean object and binary files using `make mrproper`.

## Launch

Launch the program using the `fights.sh` script who plays 100 games and print the score for each players (players are hardcoded in the script).

Logs of each player can be found in the `player0.log` and `player1.log` files.

You can clean the logs by using the `clean-logs.sh` script.