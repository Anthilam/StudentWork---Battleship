#!/bin/bash

# Battleship
#
# Student Work - UFR-ST - Licence 2
#
# GUY Timothée , LAURENT Timothée

TOTAL=0;
ME=0;

while [ $TOTAL -lt 100 ];
do
    TOTAL=$((TOTAL+1));

    ./navy-server Charles-de-Gaulle navy-not-dumb 2>&1 | grep ": Charles-de-Gaulle" > /dev/null;

    if [ $? -eq 0 ];
    then
        ME=$((ME+1));
    fi
done

echo "Player 1 won $ME times of $TOTAL rounds."
echo "Player 2 won $((TOTAL-ME)) times of $TOTAL rounds."
echo "Logs can be seen in player0.log and player1.log files.";