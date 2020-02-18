#include <sys/types.h>
#include <bsd/md5.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

char *to_crack = "fedcc6ca728c91e15fb3bf409e423f30";

void *divided_md5calc_1();
void *divided_md5calc_2();
void compute_md5(char *str, char *md5string);

int main()
{
    printf("TO CRACK: %s\n", to_crack);

    pthread_t thread1, thread2;

    pthread_create(&thread1, NULL, divided_md5calc_1, NULL);
    pthread_create(&thread2, NULL, divided_md5calc_2, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}

void *divided_md5calc_1()
{
    int i;
    for (i = 0; i <= 5000000; i++)
    {
        char pin[8];
        sprintf(pin, "%07d", i);

        char md5string[33];
        compute_md5(pin, md5string);

        if (strcmp(to_crack, md5string) == 0)
        {
            printf("GEN HASH: %s\nPIN: %s\n", md5string, pin);
            break;
        }
    }

    pthread_exit(NULL);
}

void *divided_md5calc_2()
{
    int i;
    for (i = 5000001; i <= 9999999; i++)
    {
        char pin[8];
        sprintf(pin, "%07d", i);

        char md5string[33];
        compute_md5(pin, md5string);

        if (strcmp(to_crack, md5string) == 0)
        {
            printf("GEN HASH: %s\nPIN: %s\n", md5string, pin);
            break;
        }
    }

    pthread_exit(NULL);
}

void compute_md5(char *str, char *md5string)
{
    unsigned char digest[16];

    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, str, strlen(str));
    MD5Final(digest, &ctx);

    for(int i = 0; i < 16; ++i)
        sprintf(&md5string[i*2], "%02x", (unsigned int)digest[i]);
}