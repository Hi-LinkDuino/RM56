#define _DIRENT_HAVE_D_RECLEN
#define _DIRENT_HAVE_D_OFF
#define _DIRENT_HAVE_D_TYPE

#if 0
struct dirent {
	ino_t d_ino;
	off_t d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[256];
};
#else //littlefs
struct dirent {
    unsigned int d_ino;
    unsigned int d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};
#endif
