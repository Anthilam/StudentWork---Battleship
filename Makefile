# Battleship
#
# Student Work - UFR-ST - Licence 2
#
# GUY Timothée , LAURENT Timothée

CFLAGS=-c -Wall -std=c99 -g -o
MATH=-lm

all: Charles-de-Gaulle clean

Charles-de-Gaulle: servio.o object.o
	gcc -g -Wall -std=c99 -o Charles-de-Gaulle $^

servio.o: servio.c object.h
	gcc $(CFLAGS) servio.o $<

object.o: object.c object.h
	gcc $(CFLAGS) object.o object.c

clean:
	rm -f *.o

mrproper: clean
	rm -f Charles-de-Gaulle