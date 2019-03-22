CC = gcc
CFLAGS = -std=c99 -Wall
LFLAGS = -lpthread
OBJECTS = Project2.o prod_cons_mt.o
EXECS = Project2

all: $(EXECS)

$(EXECS) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECS) $(OBJECTS) $(LFLAGS)

clean:
	-rm $(OBJECTS) $(EXECS)