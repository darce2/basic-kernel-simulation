#******************************************************************************/#
#      file ame:  makefile
#
#   description:  makefile for project 2 simulator
#
#        author:  D'Arcy, Arlen	
#      login id:  FA_15_CPS356_21
#      
#         class:  CPS 356
#    instructor:  Perugini
#    assignment:  project 2
#
#      assigned:  September 29, 2015
#           due:  October 29, 2015
#
#******************************************************************************/

all: a.out

a.out: Main.o
	g++ Main.o

main.o: Main.cpp utils.h globals.h
	g++ -c Main.cpp utils.h globals.h

clean:
	rm *.o a.out 

