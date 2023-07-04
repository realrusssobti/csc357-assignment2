CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g
MAIN = main
OBJS = main.o
all : $(MAIN)
#
#$(MAIN) : $(OBJS)
#	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

$(MAIN) : $(OBJS)
	$(CC) $(CFLAGS) -o fs_simulator $(OBJS)

clean :
	rm *.o $(MAIN) core
