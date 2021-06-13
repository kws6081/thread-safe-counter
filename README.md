# thread-safe-counter
Computer Architecture and Real-Time Operating System
Project#2

## Development Environment
Ubuntu 18.04

## How to Run aShell
1. Download
```
git clone https://github.com/kws6081/thread-safe-counter
cd /thread-safe-counter
```

2. Complie C codes
```
gcc -o posix binary\_POSIX.c -lpthread && gcc -o nonposix binary\_NONPOSIX.c -lpthread && gcc -o tscounter tscounter.c -lpthread
```

3. Run
```
time ./posix 1000000
time ./nonposix 1000000
time ./tscounter 1000000
```

## Semaphore
A semaphore is an object with an interger value that we can manipulate with two routines. In a POSIX standard lioke executable file posix, these routines are sem\_wait() and sem\_post().	
The second argument 0 in sem\_init(&c->m, 0, 1) indicates the semaphore is shared between all the threads in the process, and initializes the semaphore value to 1 by its third argument.	
sem\_wait(&c->m) decreases the value of semaphore by one and waits if value of semaphore is negative.
sem\_post(&c->m) increases the value of semaphore by one and wakes up one thread if there are one or more threads waiting.	
These semaphore routines can be implemented by multiple threads and semaphore manages critical sections by calling sem\_wait(&c->m) and sem\_post(&c->m).
sem\_wait(&c->m) returns right away when the value of semaphore is or higher and suspends action of the caller to wait for the subsequent post.	If there are multiple sem\_wait(&c->m) calls by threads, a waiting queue will be formed to line them up.	
sem\_post(&c->m) simply increments the value of a semaphore and if there is a thread that needs to be woken up, sem\_post(&c->m) wakes one of those up.


Executable file posix and nonposix contains binary semaphore.	
In posix, if a thread(p1) calls sem\_wait(&c->m), the value of a semaphore decreases. Since the initial value is set to 1, the result becomes 0. And because the value is not negative, thread doesn't wait and continues, which leads to critical section.	
If another thread(p2) tries to interrupt the thread, it will have to pass through its own sem\_wait(&c->m). This will make the value -1 and the thread(p2) will sleep.	
Then the working thread finishes the work in critical section and implement sem\_post(&c->m), which makes the value of -1 back to 0.	
sem\_post(&c->m) from thread p1 wakes up thread p2, and p2 becomes ready state and turns into running state(because the value is 0, which is 0 and above). Thread p2 finishes its task and its sem\_post(&c->m) will make the value from 0 to 1, which enables the next turn.
As we can see, race condition will not occur, resulting a fair concurrency by making the same result as lock.

## posix vs tscounter, nonposix vs tscounter 
The screencapture images shows the result of posix(semaphore according to POSIX standard), nonposix(based on sem1.c), and tscounter(given code).	
2nd argument on the command line will be the loop count in the code.	
The result of 100,000 loop counts shows that posix takes 1.4 ~ 1.5 times longer than tscounter and nonposix takes 10.6 ~ 12.4 times longer than tscounter.
The result of 1,000,000 loop counts shows that posix takes 1.7 ~ 2.1 times longer than tscounter and nonposix takes 18.3 ~ 20.3 times longer than tscounter.	

posix takes about 1.5 ~ 2 longer than tscounter because posix has to calculate the values of the semaphore as well, and tscounter doesn't have such a value to calculate. A simple adding calculation is consist of LD, ADD, and ST commands. LD loads a data from memory to register. ADD calculates data inside the register, and ST moves data from register to memory. This might not make a significant difference if this whole LD, ADD, and ST process is implemented several times, but it can make a big difference if it occurs 0.1M and 1M times.	
This becomes the reason for the difference between nonposix and tscounter too. nonposix code has more calculations to do than posix code, because posix code calculates the value of a semaphore by its library function and nonposix code does it by declaring variables that are needed to form a semaphore.
