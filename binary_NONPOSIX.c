
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>

#define PATH "/home/woos/thread-safe-counter"


union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

typedef struct __counter_t {
    int value;
    key_t key;
    int semid;
    struct sembuf s;
    union semun arg;
} counter_t;

unsigned int loop_cnt;
counter_t counter;

void init(counter_t *c, char *arg0) {
    c->value = 0;
    c->key = ftok(PATH, 'z');
    if (c->key < 0) {
        perror(arg0);
        exit(1);
    }
    c->semid = semget(c->key, 1, 0600 | IPC_CREAT);
    if (c->semid < 0) {
        perror(arg0);
        exit(1);
    }

    c->arg.val = 1;
    semctl(c->semid, 0, SETVAL, c->arg);
}

int get(counter_t *c) {
    struct sembuf s; 
    s.sem_num = 0;
    s.sem_op = -1;  // Lock
    s.sem_flg = 0;
    semop(c->semid, &s, 1);

    int rc = c->value;

    s.sem_num = 0;
    s.sem_op = 1; // Unlock
    s.sem_flg = 0;
    semop(c->semid, &s, 1);
    return rc;
}

void increment(counter_t *c) { // increase value
    struct sembuf s;
    s.sem_num = 0;
    s.sem_op = -1;   // Lock
    s.sem_flg = 0;
    semop(c->semid, &s, 1);

    c->value++; // cirtical section
    
    s.sem_num = 0;
    s.sem_op = 1;  // Unlock
    s.sem_flg = 0;
    semop(c->semid, &s, 1);
}

void decrement(counter_t *c) { // decrease value
    struct sembuf s;
    s.sem_num = 0;
    s.sem_op = -1;   // Lock
    s.sem_flg = 0;
    semop(c->semid, &s, 1);

    c->value--; // cirtical section
    
    s.sem_num = 0;
    s.sem_op = 1;  // Unlock
    s.sem_flg = 0;
    semop(c->semid, &s, 1);
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
    loop_cnt = atoi(argv[1]);

    init(&counter, argv[0]);

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