OBJ_FILES = bridge_utils.o str_utils.o constraint.o contract_in.o \
	    scoring.o generation.o results_proc.o
main: main.o $(OBJ_FILES)
	g++ -O3 -flto -mtune=generic -fopenmp main.o $(OBJ_FILES) -L. -ldds -o main

%.o:	%.cpp
	g++ -O3 -flto -mtune=generic -fopenmp -c $< -o $*.o

