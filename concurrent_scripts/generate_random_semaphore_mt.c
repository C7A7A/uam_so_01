#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

void *random1();
void *random2();
void *print_sum();

int num1 = -10000, num2 = -10000;
sem_t my_sem;

int main(){
    srand(time(NULL));
    pthread_t thread1, thread2, thread3;
    sem_init(&my_sem, 0, 0);

    pthread_create(&thread1, NULL, random1, NULL);
    pthread_create(&thread2, NULL, random2, NULL);
    pthread_create(&thread3, NULL, print_sum, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);

    return 0;
}

void *random1(){
    num1 = rand()%100;
    sem_post(&my_sem);
    pthread_exit(NULL);
}

void *random2(){
    num2 = rand()%100;
    sem_post(&my_sem); 
    pthread_exit(NULL);
}

void *print_sum(){
    sem_wait(&my_sem);
    sem_wait(&my_sem);
    printf("Num1: %d Num2: %d %d\n", num1, num2, num1 + num2);

    pthread_exit(NULL);
}