#include <string.h>
#include <stdio.h>
#include <pthread.h>

void *hello(void *id);

int main()
{   
    pthread_t mythread_a, mythread_b, mythread_c;
    char names[3][7] = {"proc_A", "proc_B", "proc_C"};

    pthread_create(&mythread_a, NULL, hello, (void *) &names[0]);
    pthread_create(&mythread_b, NULL, hello, (void *) &names[1]);
    pthread_create(&mythread_c, NULL, hello, (void *) &names[2]);

    pthread_join(mythread_a, NULL);
    pthread_join(mythread_b, NULL);
    pthread_join(mythread_c, NULL);

    return 0;
}

void *hello(void *id)
{
    printf("Hi! I am a process!\n");
    printf("My ID is: %s\n", (char *)id);
    printf("Bye!\n");

    pthread_exit(NULL);
}