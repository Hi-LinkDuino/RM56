#include "stdio_impl.h"
#include <unistd.h>

off_t __stdio_seek(FILE *f, off_t off, int whence)
{
    off_t result = 0;
    return _lseek64(f->fd, off>>32, off, &result, whence) ? -1 : result;
}
