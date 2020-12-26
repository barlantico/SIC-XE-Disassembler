###########################################################
# Makefile Assignment 2
# Brian Arlantico, cssc3010, 821125494
# Nicholas Dollick, cssc3046, 815970305
# CS530 Fall 2020
# Assignment 2, Disassembler for XE computer
# makefile
###########################################################
PROGRAM = main
CC = g++
CFLAGS = -g -std=c99

dis: ${PROGRAM}.o disassembler.o searchdir.o structs.o
	${CC} ${PROGRAM}.o disassembler.o searchdir.o structs.o -o dis

${PROGRAM}.o: ${PROGRAM}.cpp functions.hpp
	${CC} -c ${PROGRAM}.cpp

disassembler.o: disassembler.cpp functions.hpp
	${CC} -c disassembler.cpp

searchdir.o: searchdir.cpp functions.hpp
	${CC} -c searchdir.cpp

structs.o: structs.cpp functions.hpp
	${CC} -c structs.cpp

clean:
	rm -f *.o dis