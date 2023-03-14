#include <sys/resource.h>
#include "syscall.h"

int getpriority(int which, id_t who)
{
    return syscall(SYS_getpriority, which, who);
}
