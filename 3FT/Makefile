#--------------------------------------------------------------------
# Makefile for Assignment 4, Part 3
# Authors: Ellen Su and Michael Garcia
#--------------------------------------------------------------------

TARGETS = ft

.PRECIOUS: %.o

all: ft

clean:
	rm -f $(TARGETS) *~

clobber: clean
	rm -f node.o ft.o dynarray.o checkerFT.o ft_client.o

dt: dynarray.o node.o checkerFT.o ft.o ft_client.o
	gcc217 -g $^ -o $@

checkerFT.o: checkerFT.c dynarray.h checkerDT.h node.h a4def.h
	gcc217 -g -c $<

dynarray.o: dynarray.c dynarray.h
	gcc217 -g -c $<

ft_client.o: ft_client.c ft.h a4def.h
	gcc217 -g -c $<

ft.o: dt.c dynarray.h ft.h a4def.h node.h checkerDT.h
	gcc217 -g -c $<

node.o: node.c dynarray.h node.h a4def.h checkerDT.h
	gcc217 -g -c $<


