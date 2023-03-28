/* libc/sys/linux/sys/dirent.h - Directory entry as returned by readdir */

/* Written 2000 by Werner Almesberger */


#ifndef _SYS_DIRENT_H
#define _SYS_DIRENT_H
#define __FILE_defined

#include <sys/types.h>
#include <bits/dirent.h>

#define MAXNAMLEN 255		/* sizeof(struct dirent.d_name)-1 */


typedef struct __dirstream DIR;

// #if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WHT 14
// #define IFTODT(x) ((x)>>12 & 017)
// #define DTTOIF(x) ((x)<<12)
// #endif



DIR *opendir(const char *);

struct dirent *readdir(DIR *);

void rewinddir(DIR *);

int closedir(DIR *);

uint32_t telldir (DIR *);
void seekdir (DIR *, off_t loc);
int mount_flash(void);
int unmount_flash(void);



#endif /* _SYS_DIRENT_H */
