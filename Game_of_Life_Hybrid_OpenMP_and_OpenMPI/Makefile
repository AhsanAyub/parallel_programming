# Defining the number of processes as 2
iProcesses?=2

# Defining the number of threads as 2
iThreads?=2

# Defining the number of Generations as 2
iGenerations?=2

compile:
	mpic++ -fopenmp -o hybrid game_of_life_hybrid.cpp

run:
	mpirun -np $(iProcesses) ./hybrid 10000by10000_0.txt $(iThreads) $(iGenerations) output.txt

clean:
	rm hybrid
