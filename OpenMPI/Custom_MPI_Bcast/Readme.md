# Project Title
Comparison and Implementation of Custom MPI_Bcast 

### Prerequisites

- OpenMPI Library
- GNU Library

### Execution

A step by step series of examples that tell you how to get a development env running

Step 1. Transfer the cpp file and Makeup in HPC cluster
Step 2. command: make coompile
Step 3. command: make run
Step 4. command: make clearn

Example:
>> hpcshell --ntasks-per-node=4
>> make compile
<Output>
Matrix Size: 2000, # of Processes: 2, Time: 41.77
>> make run
<Output>
Matrix Size: 2000, # of Processes: 2, Time: 41.77
>> make clean
<Output>
Matrix Size: 2000, # of Processes: 2, Time: 41.77