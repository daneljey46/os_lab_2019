#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1_func(void* arg) {
    printf("Thread 1: Try to lock mutex 1...\n");
    pthread_mutex_lock(&lock1);
    printf("Thread 1: Locked mutex 1. Sleeping...\n");
    
    sleep(1);

    printf("Thread 1: Try to lock mutex 2...\n");
    pthread_mutex_lock(&lock2);
    printf("Thread 1: Locked mutex 2!\n");

    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);
    return NULL;
}

void* thread2_func(void* arg) {
    printf("Thread 2: Try to lock mutex 2...\n");
    pthread_mutex_lock(&lock2);
    printf("Thread 2: Locked mutex 2. Sleeping...\n");

    sleep(1);

    printf("Thread 2: Try to lock mutex 1...\n");
    pthread_mutex_lock(&lock1);
    printf("Thread 2: Locked mutex 1!\n");

    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Main: Finished (this line will never be reached)\n");
    return 0;
}