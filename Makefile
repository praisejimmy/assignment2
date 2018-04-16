CC = gcc
CFLAGS = -Wall -ansi -pedantic -g
MAIN = fw
OBJS = fw.o
all : $(MAIN)

$(MAIN) : $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)

fw.o : fw.c fw.h
	$(CC) $(CFLAGS) -c fw.c

handin : $(MAIN) 
	handin getaylor-grader 357hw2-01 fw.c fw.h Makefile

clean :
	rm *.o $(MAIN) core
