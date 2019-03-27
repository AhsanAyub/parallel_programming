# Defining the number of processes as 4
iProcesses?=4

compile:
	mpic++ -o main collective_communication.cpp maa_bcast.cpp

run:
	mpirun -np $(iProcesses) ./main

clean:
	rm main