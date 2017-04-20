# Define (atleast) the following targets: all, clean
# all must build the executable file named mapreduce.
# clean must remove all object files and executable files created.
# Look at the introduction doc for information on how make a Makefile.
# This make file must build according to the structure described in the
# introduction doc, if it doesn't you will get a ZERO!
CFLAGS = -Wall -Werror -Iinclude
BINS = mapreduce
OBJ = main.o map_reduce.o
BINDIR = ./bin
OBJDIR = ./build

all: $(OBJDIR) $(BINDIR) main.o map_reduce.o mapreduce 

$(OBJDIR):
	mkdir $(OBJDIR)

$(BINDIR):
	mkdir $(BINDIR)

main.o: ./src/main.c
	gcc $(CFLAGS) -c $^ -o $(OBJDIR)/$@

map_reduce.o: ./src/map_reduce.c
	gcc $(CFLAGS) -c $^ -o $(OBJDIR)/$@
	
mapreduce: $(OBJDIR)/main.o $(OBJDIR)/map_reduce.o
	gcc $(CFLAGS) $^ -o $(BINDIR)/$@

clean: 
	rm -rf $(BINDIR) $(OBJDIR) 

