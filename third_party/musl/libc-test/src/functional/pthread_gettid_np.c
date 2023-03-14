#include <pthread.h>
#include <string.h>
#include "test.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define TEST(c, ...) ((c) ? 1 : (t_error(#c" failed: " __VA_ARGS__),0))

static pthread_mutex_t mutex ;

void* pthread_test(void* arg)
{
    pthread_mutex_lock(&mutex);
    *((pid_t *)arg) = gettid();
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_mutex_init(&mutex, NULL);
    TEST(gettid() == pthread_gettid_np(pthread_self()),"pthread_gettid_np() is failed\n");

    pthread_mutex_lock(&mutex);
    pid_t tid;
    pthread_t t;
    pthread_create(&t,NULL,pthread_test,&tid);
    pid_t recv_result = pthread_gettid_np(t);
    TEST(0 == pthread_join(t,NULL),"pthread_join is failed\n");

    pthread_mutex_unlock(&mutex);
    TEST(tid == recv_result,"the tid of pthread or tid of pthread_gettid_np() is wrong\n");
    return 0;
}