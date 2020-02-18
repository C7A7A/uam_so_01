#include <string.h>
#include <stdio.h>
#include <pthread.h>

void *naive_increment();
pthread_mutex_t guard = PTHREAD_MUTEX_INITIALIZER;

int sum = 0;

int main()
{   
    pthread_t mythreads[2];

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
        pthread_mutex_lock(&guard);
        sum++;
        pthread_mutex_unlock(&guard);
    }

    pthread_exit(NULL);
}