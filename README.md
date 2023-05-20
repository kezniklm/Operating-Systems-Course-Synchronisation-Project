# Operating Systems Course - Synchronisation Project

## Implementation of Building H2O Molecules with Semaphores
Evaluation: 12/15 points

## Assignment
[Assignment link](https://github.com/kezniklm/IOS_project2/blob/main/Assignment.pdf)

## Description
This is a C implementation of a concurrent program that simulates the building of H2O molecules using semaphores. The program utilizes the concept of synchronization and coordination between threads to ensure the correct combination of hydrogen (H) and oxygen (O) atoms. This implementation is inspired by The Little Book of Semaphores from Allen B. Downey.

## Compilation
To compile the program, use the following command:
```bash
$ make
```

## Usage
```bash
$ ./proj2 NO NH TI TB
```
where 
* NO is number of oxygen
* NH is number of hydrogen
* TI: Maximum time in milliseconds that an oxygen/hydrogen atom waits after its creation before joining the molecule creation queue. 0<=TI<=1000
* TB: Maximum time in milliseconds required to create one molecule. 0<=TB<=1000

## Example
```bash
$ ./proj2 3 5 100 100
```
```bash
1: H 1: started
2: H 3: started
3: O 1: started
4: O 1: going to queue
5: H 2: started
6: H 2: going to queue
7: H 1: going to queue
8: O 3: started
9: O 3: going to queue
10: H 5: started
11: H 4: started
12: O 2: started
13: H 1: creating molecule 1
14: H 4: going to queue
15: O 1: creating molecule 1
16: H 2: creating molecule 1
17: H 5: going to queue
18: H 2: molecule 1 created
19: H 1: molecule 1 created
20: O 1: molecule 1 created
21: H 3: going to queue
22: O 3: creating molecule 2
23: O 2: going to queue
24: H 4: creating molecule 2
25: H 5: creating molecule 2
26: O 3: molecule 2 created
27: H 4: molecule 2 created
28: H 5: molecule 2 created
29: H 3: not enough O or H
30: O 2: not enough H
```
