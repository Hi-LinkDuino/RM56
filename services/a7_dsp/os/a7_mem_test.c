#ifdef RTOS
#include "cmsis_os.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_trace.h"

/* ------------  memmory test ---------------- */
#define MEM_SIZE 100
#define BIG_MEM_SIZE 10
#define MAX_COUNT 1000
static void test_mem(void)
{
    char *mem_list[MEM_SIZE];
    memset(mem_list, 0, sizeof(char *) * MEM_SIZE);

    char *realloc_mem_list[MEM_SIZE];
    memset(realloc_mem_list, 0, sizeof(char *) * MEM_SIZE);

    for (int i = 0; i < MEM_SIZE; i++)
    {
        int r = 1 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));
        //printf("small_size =%d\r\n",r);
        mem_list[i] = (char *)malloc(r);
        if (!mem_list[i])
        {
            TRACE(0, "%s malloc failed i = %d size is %d", __func__, i, r);
        }

        int r2 = 100 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));
        //printf("small_size =%d\r\n",r);
        realloc_mem_list[i] = (char *)realloc(mem_list[i], r2);
        if (realloc_mem_list[i] == NULL)
        {
            free(mem_list[i]);
            TRACE(0, "%s realloc failed i = %d realloc size is %d", __func__, i, r2);
        }
        mem_list[i] = realloc_mem_list[i];
    }
    for (int i = 0; i < MEM_SIZE; i++)
    {
        char *p= mem_list[i];
        if (p!=NULL)
        {
            //printf("free small_mem i=%d\r\n",i);
            free(p);
            mem_list[i]=NULL;
        }
    }
}

int bes_realloc_cross_test(void)
{
    // TRACE(0, "Enter %s %d\r\n", __func__, __LINE__);
#ifdef osCMSIS_FreeRTOS
    int _freeRTOS_heap_size = xPortGetFreeHeapSize();
#endif /* osCMSIS_FreeRTOS */
    char *mem_list[MEM_SIZE];
    memset(mem_list, 0, sizeof(char *) * MEM_SIZE);

    char *realloc_mem_list[MEM_SIZE];
    memset(realloc_mem_list, 0, sizeof(char *) * MEM_SIZE);

    uint32_t n = 0;
    char *mem_big[BIG_MEM_SIZE];
    memset(mem_big, 0, sizeof(char *) * BIG_MEM_SIZE);

    char *realloc_mem_big[BIG_MEM_SIZE];
    memset(realloc_mem_big, 0, sizeof(char *) * BIG_MEM_SIZE);
    while(1)
    {
        n++;
        //printf("count=%d\r\n",n);
        srand(n);

        for (int i = 0; i < MEM_SIZE; i++)
        {
            int r = 1 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));  //rand 1-100 size
            // TRACE(0, "small rand size =%d\r\n",r);
            mem_list[i] = (char *)malloc(r);   //malloc rand size 1byte-100bye heap
            if (!mem_list[i])
            {
                TRACE(0, "%s %d malloc failed, i = %d size is %d", __func__, __LINE__, i, r);
            }

            int r2 = 100 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));  //rand 100-200 size
            // TRACE(0, "realloc small rand size =%d\r\n",r2);
            realloc_mem_list[i] = (char *)realloc(mem_list[i], r2);
            if (realloc_mem_list[i] == NULL)
            {
                TRACE(0, "%s %d realloc failed, i = %d size is %d", __func__, __LINE__, i, r2);
                free(mem_list[i]);
            }
            mem_list[i] = realloc_mem_list[i];
        }

        for (int i = 0; i < BIG_MEM_SIZE; i++)
        {
            char *p = mem_big[i];
            if (p != NULL)
            {
                //printf("free big_mem i=%d\r\n",i);
                free(p);                      //free 10byte-1000bye rand size heap
                mem_big[i] = NULL;
            }
        }

        test_mem();
        test_mem();
        if (n <= MAX_COUNT){
            for (int i = 0; i < BIG_MEM_SIZE; i++)
            {
                int r = 1 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));   //rand 1-100 size
                // TRACE(0, "big rand size =%d\r\n",r);
                mem_big[i] = (char *)malloc(r * 10);   //malloc rand size 10byte-1000bye
                if (!mem_big[i])
                {
                    TRACE(0, "%s %d malloc failed, i = %d size is %d", __func__, __LINE__, i, r);
                }

                int r2 = 100 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));   //rand 100-200 size
                // TRACE(0, "realloc big rand size =%d\r\n",r2);
                realloc_mem_big[i] = (char *)realloc(mem_big[i], r2 * 10);
                // TRACE(0, "realloc_mem_big[i] = %p mem_big[i] = %p", realloc_mem_big[i], mem_big[i]);
                if (realloc_mem_big[i] == NULL)
                {
                    TRACE(0, "%s %d realloc failed, i = %d size is %d", __func__, __LINE__, i, r2);
                    free(mem_big[i]);
                }
                mem_big[i] = realloc_mem_big[i];
            }
        }
        for (int i = 0; i < MEM_SIZE; i++)
        {
            char *p = mem_list[i];
            if (p != NULL)
            {
                //printf("free small_mem i=%d\r\n",i);
                free(p);               //free 1byte-100bye rand size heap
                mem_list[i] = NULL;
            }
        }
        if (n > MAX_COUNT){
            //os_heap_stat_print();
            break;
        }
        osDelay(5);
    }

#ifdef osCMSIS_FreeRTOS
    TRACE(0, "%s freeRTOS free heap, start size:%d, now size:%d, leakage:%d\n", __func__, _freeRTOS_heap_size, xPortGetFreeHeapSize(),
                    (xPortGetFreeHeapSize() < _freeRTOS_heap_size)?(_freeRTOS_heap_size - xPortGetFreeHeapSize()):0);
#endif //osCMSIS_FreeRTOS
    // TRACE(0, "Leave %s %d\r\n",__func__, __LINE__);
    return 0;
}

#define REALLOC_NULL_NUM 100
int bes_realloc_null_test(void)
{
#ifdef osCMSIS_FreeRTOS
    int _freeRTOS_heap_size = xPortGetFreeHeapSize();
#endif /* osCMSIS_FreeRTOS */

    // TRACE(0, "Enter %s %d", __func__, __LINE__);
    char *malloc_null_list[REALLOC_NULL_NUM];
    memset(malloc_null_list, 0, sizeof(char *) * REALLOC_NULL_NUM);

    char *realloc_null_list[REALLOC_NULL_NUM];
    memset(realloc_null_list, 0, sizeof(char *) * REALLOC_NULL_NUM);
    for (int i = 0; i < REALLOC_NULL_NUM; i++)
    {
        int r = 1 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));
        // TRACE(0, "rand number r = %d", r);
        malloc_null_list[i] = (char *)malloc(r * 10);
        if (!malloc_null_list[i])
        {
            TRACE(0, "%s malloc failed", __func__);
        }
        realloc_null_list[i] = (char *)realloc(malloc_null_list[i], 0);
        if (realloc_null_list[i])
        {
            TRACE(0, "%s realloc NULL failed", __func__);
        }
    }

    // TRACE(0, "Leave %s %d", __func__, __LINE__);
#ifdef osCMSIS_FreeRTOS
    TRACE(0, "%s freeRTOS free heap, start size:%d, now size:%d, leakage:%d\n",__func__, _freeRTOS_heap_size, xPortGetFreeHeapSize(),
                    (xPortGetFreeHeapSize() < _freeRTOS_heap_size)?(_freeRTOS_heap_size - xPortGetFreeHeapSize()):0);
#endif //osCMSIS_FreeRTOS

    return 0;
}

#define MALLOC_NUM  100
int bes_malloc_test(void)
{
#ifdef osCMSIS_FreeRTOS
    int _freeRTOS_heap_size = xPortGetFreeHeapSize();
#endif /* osCMSIS_FreeRTOS */

    char *malloc_list[MALLOC_NUM];
    memset(malloc_list, 0, sizeof(char *) * MALLOC_NUM);

    for (int i = 0; i < MALLOC_NUM; i++)
    {
        int random_size = 1 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));
        // TRACE(0, "malloc random_size = %d", random_size);
        malloc_list[i] = (char *)malloc(random_size);
        if (!malloc_list[i])
        {
            TRACE(-0, "%s malloc failed i = %d random_size = %d", __func__, i, random_size);
        }
    }

    for (int i = 0; i < MALLOC_NUM; i++)
    {
        if (malloc_list[i])
        {
            free(malloc_list[i]);
        }
    }

#ifdef osCMSIS_FreeRTOS
    TRACE(0, "%s freeRTOS free heap, start size:%d, now size:%d, leakage:%d\n", __func__, _freeRTOS_heap_size, xPortGetFreeHeapSize(),
                    (xPortGetFreeHeapSize() < _freeRTOS_heap_size)?(_freeRTOS_heap_size - xPortGetFreeHeapSize()):0);
#endif //osCMSIS_FreeRTOS

    return 0;
}

#define MALLOC_BIG_NUM  100
int bes_malloc_big_test(void)
{
#ifdef osCMSIS_FreeRTOS
    int _freeRTOS_heap_size = xPortGetFreeHeapSize();
#endif /* osCMSIS_FreeRTOS */

    char *malloc_list[MALLOC_BIG_NUM];
    memset(malloc_list, 0, sizeof(char *) * MALLOC_BIG_NUM);
    for (int i = 0; i < MALLOC_BIG_NUM; i++)
    {
        int random_size = 1000 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));
        // TRACE(0, "malloc random_size = %d", random_size);
        malloc_list[i] = (char *)malloc(random_size);
        if (!malloc_list[i])
        {
            TRACE(0, "%s malloc failed i = %d random_size = %d", __func__, i, random_size);
        }
    }

    for (int i = 0; i < MALLOC_BIG_NUM; i++)
    {
        if (malloc_list[i])
        {
            free(malloc_list[i]);
        }
    }
    #define MALLOC_BIG_LEN  1024*1024*2
    char *pchar = (char *)malloc(MALLOC_BIG_LEN);
    if(pchar == NULL){
        TRACE(0, "==== now malloc big data：%u FAILED!!!",MALLOC_BIG_LEN);
        return -1;
    }
    free(pchar);

#ifdef osCMSIS_FreeRTOS
    TRACE(0, "%s freeRTOS free heap, start size:%d, now size:%d, leakage:%d\n", __func__, _freeRTOS_heap_size, xPortGetFreeHeapSize(),
                    (xPortGetFreeHeapSize() < _freeRTOS_heap_size)?(_freeRTOS_heap_size - xPortGetFreeHeapSize()):0);
#endif //osCMSIS_FreeRTOS

    return 0;
}

#define REALLOC_NUM  100
int bes_realloc_test(void)
{
#ifdef osCMSIS_FreeRTOS
    int _freeRTOS_heap_size = xPortGetFreeHeapSize();
#endif /* osCMSIS_FreeRTOS */

    char *mem_list[REALLOC_NUM];
    memset(mem_list, 0, sizeof(char *) * REALLOC_NUM);

    char *realloc_list[REALLOC_NUM];
    memset(realloc_list, 0, sizeof(char *) * REALLOC_NUM);

    for (int i = 0; i < REALLOC_NUM; i++)
    {
        int malloc_random_size = 1 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));
        mem_list[i] = (char *)malloc(malloc_random_size);
        if (!mem_list[i])
        {
            TRACE(0, "%s malloc failed i = %d, malloc size %d", __func__,  i, malloc_random_size);
        }
        int realloc_random_size = 100 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));
        realloc_list[i] = (char *)realloc(mem_list[i], realloc_random_size);
        if (!realloc_list[i])
        {
            TRACE(0, "%s realloc failed i = %d, realloc size %d", __func__, i, realloc_random_size);
            free(mem_list[i]);
        }
        mem_list[i] = realloc_list[i];
    }

    for (int i = 0; i < REALLOC_NUM; i++)
    {
        char *ptr = mem_list[i];
        if (ptr)
        {
            free(ptr);
        }
    }

#ifdef osCMSIS_FreeRTOS
    TRACE(0, "%s freeRTOS free heap, start size:%d, now size:%d, leakage:%d\n", __func__, _freeRTOS_heap_size, xPortGetFreeHeapSize(),
                    (xPortGetFreeHeapSize() < _freeRTOS_heap_size)?(_freeRTOS_heap_size - xPortGetFreeHeapSize()):0);
#endif //osCMSIS_FreeRTOS

    return 0;
}

#define CALLOC_NUM   100

int bes_calloc_test(void)
{
#ifdef osCMSIS_FreeRTOS
    int _freeRTOS_heap_size = xPortGetFreeHeapSize();
#endif /* osCMSIS_FreeRTOS */

    char *calloc_list[REALLOC_NUM];
    memset(calloc_list, 0, sizeof(char *) * REALLOC_NUM);
    for (int i = 0; i < CALLOC_NUM; i++)
    {
        int calloc_random_size = 1 + (int)(100.0 * rand() / ((float)RAND_MAX + 1.0));
        calloc_list[i] = (char *)calloc(1, calloc_random_size);
        if (!calloc_list[i])
        {
            TRACE(0, "%s calloc failed i = %d calloc size = %d", __func__, i, calloc_random_size);
        }
    }

    for (int i = 0; i < CALLOC_NUM; i++)
    {
        char *ptr = calloc_list[i];
        if (ptr)
        {
            free(ptr);
        }
    }
    #define CALLOC_BIG_LEN  1024*1024*2
    char *pchar = (char *)calloc(1,CALLOC_BIG_LEN);
    if(pchar == NULL){
        TRACE(0, "==== now calloc big data：%u FAILED!!!",CALLOC_BIG_LEN);
        return -1;
    }
    free(pchar);
    
    
#ifdef osCMSIS_FreeRTOS
    TRACE(0, "%s freeRTOS free heap, start size:%d, now size:%d, leakage:%d\n", __func__, _freeRTOS_heap_size, xPortGetFreeHeapSize(),
                    (xPortGetFreeHeapSize() < _freeRTOS_heap_size)?(_freeRTOS_heap_size - xPortGetFreeHeapSize()):0);
#endif //osCMSIS_FreeRTOS
    return 0;
}

void a7_heap_test(void)
{
    osDelay(10);
    TRACE(0, "\n\n%s() start ----------------------------->\n",__func__);
    bes_realloc_cross_test();
    bes_realloc_null_test();
    bes_malloc_test();
    bes_malloc_big_test();
    bes_realloc_test();
    bes_calloc_test();
    TRACE(0, "\n\n<----------------------------------%s() done\n",__func__);
}

//-----------------------------
