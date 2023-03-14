#include <sched.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "test.h"

#define TEST(c) ((!c) ? 1 : (t_error(#c" failed: %s" ,strerror(errno)),0))

int main(int argc,char *argv[])
{
    struct sched_param param;
    int maxpri,minpri;
    int sched;
    pid_t pid;
    
    sched = SCHED_FIFO;
    pid = getpid();
    maxpri = sched_get_priority_max(sched);
    minpri = sched_get_priority_min(sched);
    if(maxpri == -1 || minpri == -1) 
    {
        perror("get maxpriority or minpriority failed");
        return -1;
    }
    param.sched_priority = 23;//取值范围:minpri~maxpri
    TEST(sched_setscheduler(pid, sched, &param));
    TEST(!(sched_getscheduler(pid) == sched));
    TEST(sched_getparam(pid, &param));

    return 0;
}

