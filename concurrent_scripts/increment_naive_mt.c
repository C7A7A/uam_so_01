#include <string.h>
#include <stdio.h>
#include <pthread.h>

void *naive_increment();
sem_t my_sem;

int sum = 0;

int main()
{   
    pthread_t mythreads[2];
    sem_init_(&my_sem, 0, 1);

    int i;
    for (i = 0; i < 2; i++)
    {
        pthread_create(&mythreads[i], NULL, naive_increment, NULL);
    }

    for (i = 0; i < 2; i++)
    {
        pthread_join(mythreads[i], NULL);
    }

    printf("%d\n", sum);

    return 0;
}

void *naive_increment()
{
    int i;
    for (i = 0; i < 50000; i++)
    {
        sem_wait(&my_sem);
        sum++;
        sem_post(&my_sem);
    }

    pthread_exit(NULL);
}