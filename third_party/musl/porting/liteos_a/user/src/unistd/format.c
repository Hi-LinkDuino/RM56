#include <unistd.h>
#include <fcntl.h>
#include "syscall.h"

int format(const char *dev, int sectors, int option)
{
	return syscall(SYS_format, dev, sectors, option);
}
