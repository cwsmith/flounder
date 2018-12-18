CC=gcc
CFLAGS=-std=c99 -O2
LDFLAGS=-lm

flounder: flounder.o refine.o adj_ops.o graph_ops.o graph.o rgraph.o adj.o ints.o space.o vtk.o size.o
	$(CC) $? $(LDFLAGS) -o $@
   
clean:
	rm -f *.o
