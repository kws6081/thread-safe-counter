#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h> // Code based on POSIX standard

typedef struct __counter_t {
    int value; // count the number of loop
    sem_t m; // initialize
} counter_t;

unsigned int loop_cnt;
counter_t counter;

void init(counter_t *c) {
    c->value = 0; // initialize value
    sem_init(&c->m, 0, 1); // binary semaphore; initial value = 1
}

void increment(counter_t *c) { // increase value
    sem_wait(&c->m); // P(); wait for signal
    c->value++; // cirtical section
    sem_post(&c->m); // V(); send signal
}

void decrement(counter_t *c) { // decrease value
    sem_wait(&c->m); // P(); wait for signal
    c->value--; // cirtical section
    sem_post(&c->m); // V(); send signal
}

int get(counter_t *c) { // get value
    sem_wait(&c->m); // P(); wait for signal
    int rc = c->value; // cirtical section
    sem_post(&c->m); // V(); send signal
    return rc;
}

void *mythread(void *arg)
{
    char *letter = arg;
    int i;

    printf("%s: begin\n", letter);
    for (i = 0; i < loop_cnt; i++) {
        increment(&counter);
    }
    printf("%s: done\n", letter);
    return NULL;
}
                                                                             
int main(int argc, char *argv[])
{
    loop_cnt = atoi(argv[1]);  // use second command argument(first index) as loop
    init(&counter); // name the semaphore "counter"

    pthread_t p1, p2;
    printf("main: begin [counter = %d]\n", get(&counter));
    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");
    // join waits for the threads to finish
    pthread_join(p1, NULL); 
    pthread_join(p2, NULL); 
    printf("main: done [counter: %d] [should be: %d]\n", get(&counter), loop_cnt * 2);
    return 0;
}