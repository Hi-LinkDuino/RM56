/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem module  R0.14a                              /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2020, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:

/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/


#ifndef FF_DEFINED
#define FF_DEFINED	80196	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

#include <dirent.h>
#include "integer.h"	/* Basic integer types */
#include "ffconf.h"		/* FatFs configuration options */
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
#include "virpart.h"
#endif
#ifndef __LITEOS_M__
#include "los_list.h"
#endif
#if FF_DEFINED != FFCONF_DEF
#error Wrong configuration file (ffconf.h).
#endif

/*--------------------------------*/
/* LFN/Directory working buffer   */
/*--------------------------------*/

#if FF_USE_LFN == 0		/* Non-LFN configuration */
#define DEF_NAMBUF
#define INIT_NAMBUF(fs)
#define FREE_NAMBUF()
#define LEAVE_MKFS(res)	return res

#else					/* LFN configurations */
#if FF_MAX_LFN < 12 || FF_MAX_LFN > 255
#error Wrong setting of FF_MAX_LFN
#endif
#if FF_LFN_BUF < FF_SFN_BUF || FF_SFN_BUF < 12
#error Wrong setting of FF_LFN_BUF or FF_SFN_BUF
#endif
#if FF_LFN_UNICODE < 0 || FF_LFN_UNICODE > 3
#error Wrong setting of FF_LFN_UNICODE
#endif
static const BYTE LfnOfs[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};	/* FAT: Offset of LFN characters in the directory entry */

#if FF_USE_LFN == 1		/* LFN enabled with static working buffer */
static WCHAR LfnBuf[FF_MAX_LFN + 1];		/* LFN working buffer */
#define DEF_NAMBUF
#define INIT_NAMBUF(fs)
#define FREE_NAMBUF()
#define LEAVE_MKFS(res)	return res

#elif FF_USE_LFN == 2 	/* LFN enabled with dynamic working buffer on the stack */
#define DEF_NAMBUF		WCHAR lbuf[FF_MAX_LFN+1];	/* LFN working buffer */
#define INIT_NAMBUF(fs)	{ (fs)->lfnbuf = lbuf; }
#define FREE_NAMBUF()
#define LEAVE_MKFS(res)	return res

#elif FF_USE_LFN == 3 	/* LFN enabled with dynamic working buffer on the heap */
#define DEF_NAMBUF		WCHAR *lfn;	/* Pointer to LFN working buffer and directory entry block scratchpad buffer */
#define INIT_NAMBUF(fs)	{ lfn = ff_memalloc((FF_MAX_LFN+1)*2); if (!lfn) LEAVE_FF(fs, FR_NOT_ENOUGH_CORE); (fs)->lfnbuf = lfn; }
#define FREE_NAMBUF()	ff_memfree(lfn)
#define LEAVE_MKFS(res)	{ if (!work) ff_memfree(buf); return res; }
#define MAX_MALLOC	0x8000	/* Must be >=FF_MAX_SS */

#else
#error Wrong setting of FF_USE_LFN

#endif	/* FF_USE_LFN == 1 */
#endif	/* FF_USE_LFN == 0 */


#define NS_NONAME	0x80	/* Not followed */
#define NSFLAG		11		/* Index of the name status byte */

/* Definitions of volume management */

typedef struct {
	BYTE di;	/* Physics disk id */
	BYTE pd;	/* Physical drive number */
	BYTE pt;	/* Partition: 0:Auto detect, 1-4:Forced partition) */
	QWORD ps;	/* Partition start sector */
	QWORD pc;	/* Partition sector count */
} PARTITION;
extern PARTITION VolToPart[];	/* Volume - Partition mapping table */
#define LD2DI(vol) (VolToPart[vol].di)	/* Get physical disk id */
#define LD2PD(vol) (VolToPart[vol].pd)	/* Get physical drive number */

#if FF_MULTI_PARTITION	/* Multiple partition configuration */
#define LD2PT(vol) (VolToPart[vol].pt)	/* Get partition index */
#define LD2PS(vol) (VolToPart[vol].ps)	/* Get partition start sector */
#define LD2PC(vol) (VolToPart[vol].pc)	/* Get partition sector count */

#else							/* Single partition configuration */
#define LD2PT(vol)	0			/* Find first valid partition or in SFD */
#define LD2PS(vol)	0
#define LD2PC(vol)	0

#endif
/* Type of path name strings on FatFs API */


#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
typedef struct FAT_VIRTUAL_PARTITION {
	virpartinfo	virtualinfo;
	BOOL		isparamset;
} FAT_VIR_PART;

#define ISVIRPART(fs)	((fs)->vir_avail == FS_VIRENABLE)
#define ISNORMAL(fs)	((fs)->vir_avail != FS_VIRENABLE)

#define ISPARENT(fs)	((fs)->vir_flag == FS_PARENT)
#define ISCHILD(fs)	((fs)->vir_flag != FS_PARENT)


#define VIRINFO_EXTERNAL	0
#define VIRINFO_BUILDIN		1
#endif



/* Type of path name strings on FatFs API */

#ifndef _INC_TCHAR
#define _INC_TCHAR

#if FF_USE_LFN && FF_LFN_UNICODE == 1 	/* Unicode in UTF-16 encoding */
typedef WCHAR TCHAR;
#define _T(x) L ## x
#define _TEXT(x) L ## x
#elif FF_USE_LFN && FF_LFN_UNICODE == 2	/* Unicode in UTF-8 encoding */
typedef char TCHAR;
#define _T(x) u8 ## x
#define _TEXT(x) u8 ## x
#elif FF_USE_LFN && FF_LFN_UNICODE == 3	/* Unicode in UTF-32 encoding */
typedef DWORD TCHAR;
#define _T(x) U ## x
#define _TEXT(x) U ## x
#elif FF_USE_LFN && (FF_LFN_UNICODE < 0 || FF_LFN_UNICODE > 3)
#error Wrong FF_LFN_UNICODE setting
#else									/* ANSI/OEM code in SBCS/DBCS */
typedef char TCHAR;
#define _T(x) x
#define _TEXT(x) x
#endif
#endif

#if (FF_MAX_SS == FF_MIN_SS)
#define SS(fs)	((UINT)FF_MAX_SS)	/* Fixed sector size */
#else
#define SS(fs)	((fs)->ssize)	/* Variable sector size */
#endif


/* Type of file size variables */
typedef DWORD FSIZE_t;
typedef QWORD LBA_t;

/* DIR offset in fs win */
#define DIR_Name			0		/* Short file name (11-byte) */
#define DIR_Attr			11		/* Attribute (BYTE) */
#define DIR_NTres			12		/* Lower case flag (BYTE) */
#define DIR_CrtTime10		13		/* Created time sub-second (BYTE) */
#define DIR_CrtTime			14		/* Created time (DWORD) */
#define DIR_LstAccDate		18		/* Last accessed date (WORD) */
#define DIR_FstClusHI		20		/* Higher 16-bit of first cluster (WORD) */
#define DIR_ModTime			22		/* Modified time (DWORD) */
#define DIR_FstClusLO		26		/* Lower 16-bit of first cluster (WORD) */
#define DIR_FileSize		28		/* File size (DWORD) */
#define LDIR_Ord			0		/* LFN: LFN order and LLE flag (BYTE) */
#define LDIR_Attr			11		/* LFN: LFN attribute (BYTE) */
#define LDIR_Type			12		/* LFN: Entry type (BYTE) */
#define LDIR_Chksum			13		/* LFN: Checksum of the SFN (BYTE) */
#define LDIR_FstClusLO		26		/* LFN: MBZ field (WORD) */


/* Limits and boundaries */
#define MAX_DIR		0x200000		/* Max size of FAT directory */
#define MAX_FAT12	0xFF5			/* Max FAT12 clusters (differs from specs, but right for real DOS/Windows behavior) */
#define MAX_FAT16	0xFFF5			/* Max FAT16 clusters (differs from specs, but right for real DOS/Windows behavior) */
#define MAX_FAT32	0x0FFFFFF5		/* Max FAT32 clusters (not specified, practical limit) */


/* Timestamp */
#if FF_FS_NORTC == 1
#if FF_NORTC_YEAR < 1980 || FF_NORTC_YEAR > 2107 || FF_NORTC_MON < 1 || FF_NORTC_MON > 12 || FF_NORTC_MDAY < 1 || FF_NORTC_MDAY > 31
#error Invalid FF_FS_NORTC settings
#endif
#define GET_FATTIME()	((DWORD)(FF_NORTC_YEAR - 1980) << 25 | (DWORD)FF_NORTC_MON << 21 | (DWORD)FF_NORTC_MDAY << 16)
#else
#define GET_FATTIME()	get_fattime()
#endif

extern UINT	time_status;

/* Filesystem object structure (FATFS) */

typedef struct {
	BYTE	fs_type;		/* Filesystem type (0:not mounted) */
	BYTE	pdrv;			/* Associated physical drive */
	BYTE	n_fats;			/* Number of FATs (1 or 2) */
	BYTE	wflag;			/* win[] flag (b0:dirty) */
	BYTE	fsi_flag;		/* FSINFO flags (b7:disabled, b0:dirty) */
	WORD	id;				/* Volume mount ID */
	WORD	n_rootdir;		/* Number of root directory entries (FAT12/16) */
	WORD	csize;			/* Cluster size [sectors] */
#if FF_MAX_SS != FF_MIN_SS
	size_t	ssize;			/* Sector size (512, 1024, 2048 or 4096) */
#endif
#if FF_USE_LFN
	WCHAR*	lfnbuf;			/* LFN working buffer */
#endif
#if FF_FS_REENTRANT
	FF_SYNC_t	sobj;		/* Identifier of sync object */
#endif
#if !FF_FS_READONLY
	DWORD	last_clst;		/* Last allocated cluster */
	DWORD	free_clst;		/* Number of free clusters */
#endif
#if FF_FS_RPATH
	DWORD	cdir;			/* Current directory start cluster (0:root) */
#endif
	DWORD	n_fatent;		/* Number of FAT entries, = number of clusters + 2 */
	DWORD	fsize;			/* Sectors per FAT */
	LBA_t	volbase;		/* Volume base sector */
	LBA_t	fatbase;		/* FAT base sector */
	LBA_t	dirbase;		/* Root directory base sector/cluster */
	LBA_t	database;		/* Data base sector */
	LBA_t	winsect;		/* Current sector appearing in the win[] */
	BYTE*	win;			/* Disk access window for Directory, FAT (and file data at tiny cfg) */

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	DWORD	st_clst;
	DWORD	ct_clst;
	BYTE	vir_flag;		/* Flag of Virtual Filesystem Object, b0 : 1 for virtual Fatfs object, 0 for reality Fatfs object */
	BYTE	vir_avail;
	DWORD	vir_amount;
	VOID*	parent_fs;		/* Point to the reality Fatfs object, only available in virtual Fatfs object */
	CHAR	namelabel[_MAX_ENTRYLENGTH + 1]; /* The name label point to the each virtual Fatfs object ,only available in virtual Fatfs obj */
	VOID**	child_fs;		/* Point to the child Fatfs object ,only available in reality Fatfs object */
#endif
#ifndef __LITEOS_M__
	int 	fs_uid;
	int 	fs_gid;
	mode_t 	fs_mode;
#endif
	unsigned short fs_dmask;
	unsigned short fs_fmask;
} FATFS;



/* Object ID and allocation information (FFOBJID) */

typedef struct {
	FATFS*	fs;				/* Pointer to the hosting volume of this object */
	WORD	id;				/* Hosting volume mount ID */
	BYTE	attr;			/* Object attribute */
	BYTE	stat;			/* Object chain status (b1-0: =0:not contiguous, =2:contiguous, =3:fragmented in this session, b2:sub-directory stretched) */
	DWORD	sclust;			/* Object data start cluster (0:no cluster or root directory) */
	FSIZE_t	objsize;		/* Object size (valid when sclust != 0) */
#if FF_FS_LOCK
	UINT	lockid;			/* File lock ID origin from 1 (index of file semaphore table Files[]) */
#endif
} FFOBJID;



/* File object structure (FIL) */

typedef struct {
	FFOBJID	obj;			/* Object identifier (must be the 1st member to detect invalid object pointer) */
	BYTE	flag;			/* File status flags */
	BYTE	err;			/* Abort flag (error code) */
	FSIZE_t	fptr;			/* File read/write pointer (Zeroed on file open) */
	DWORD	clust;			/* Current cluster of fpter (invalid when fptr is 0) */
	LBA_t	sect;			/* Sector number appearing in buf[] (0:invalid) */
#if !FF_FS_READONLY
	LBA_t	dir_sect;		/* Sector number containing the directory entry */
	BYTE*	dir_ptr;		/* Pointer to the directory entry in the win[] */
#endif
#if FF_USE_FASTSEEK
	DWORD*	cltbl;			/* Pointer to the cluster link map table (nulled on open, set by application) */
#endif
#if !FF_FS_TINY
	BYTE*	buf;			/* File private data read/write window */
#endif
#ifndef __LITEOS_M__
	LOS_DL_LIST fp_entry;
#endif
} FIL;



/* Directory object structure (DIR) */

struct __dirstream {
	FFOBJID	obj;			/* Object identifier */
	DWORD	dptr;			/* Current read/write offset */
	DWORD	clust;			/* Current cluster */
	LBA_t	sect;			/* Current sector (0:Read operation has terminated) */
	BYTE*	dir;			/* Pointer to the directory item in the win[] */
	BYTE	fn[12];			/* SFN (in/out) {body[8],ext[3],status[1]} */
#if FF_USE_LFN
	DWORD	blk_ofs;		/* Offset of current entry block being processed (0xFFFFFFFF:Invalid) */
#endif
#if FF_USE_FIND
	const TCHAR* pat;		/* Pointer to the name matching pattern */
#endif
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	BYTE			atrootdir;
#endif
};

/* File information structure (FILINFO) */

typedef struct {
	FSIZE_t	fsize;			/* File size */
	WORD	fdate;			/* Modified date */
	WORD	ftime;			/* Modified time */
	BYTE	fattrib;		/* File attribute */
#if FF_USE_LFN
	TCHAR	altname[FF_SFN_BUF + 1];/* Altenative file name */
	TCHAR	fname[FF_LFN_BUF + 1];	/* Primary file name */
#else
	TCHAR	fname[12 + 1];	/* File name */
#endif
	DWORD	sclst;
#ifndef __LITEOS_M__
	LOS_DL_LIST fp_list;
#endif
} FILINFO;

#ifndef __LITEOS_M__
typedef struct {
	DWORD	clst;
	FSIZE_t	pos;
} FAT_ENTRY;
#endif

/* Format parameter structure (MKFS_PARM) */

typedef struct {
	BYTE fmt;			/* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
	BYTE n_fat;			/* Number of FATs */
	UINT align;			/* Data area alignment (sector) */
	UINT n_root;		/* Number of root directory entries */
	int n_sect;			/* Cluster size (sector) */
} MKFS_PARM;

typedef struct {
	DIR	f_dir;
	FILINFO	fno;
#ifndef __LITEOS_M__
	FAT_ENTRY	fat_entry;
#endif
} DIR_FILE;

#define MAX(a, b, c)	(c = (a > b) ? a : b)
#define MIN(a, b, c)	(c = (a < b) ? a : b)

/* File function return code (FRESULT) */

typedef enum {
	FR_OK = 0,				/* (0) Succeeded */
	FR_DISK_ERR,			/* (1) A hard error occurred in the low level disk I/O layer */
	FR_INT_ERR,				/* (2) Assertion failed */
	FR_NOT_READY,			/* (3) The physical drive cannot work */
	FR_NO_FILE,				/* (4) Could not find the file */
	FR_NO_PATH,				/* (5) Could not find the path */
	FR_INVALID_NAME,		/* (6) The path name format is invalid */
	FR_DENIED,				/* (7) Access denied due to prohibited access or directory full */
	FR_EXIST,				/* (8) Access denied due to prohibited access */
	FR_INVALID_OBJECT,		/* (9) The file/directory object is invalid */
	FR_WRITE_PROTECTED,		/* (10) The physical drive is write protected */
	FR_INVALID_DRIVE,		/* (11) The logical drive number is invalid */
	FR_NOT_ENABLED,			/* (12) The volume has no work area */
	FR_NO_FILESYSTEM,		/* (13) There is no valid FAT volume */
	FR_MKFS_ABORTED,		/* (14) The f_mkfs() aborted due to any problem */
	FR_TIMEOUT,				/* (15) Could not get a grant to access the volume within defined period */
	FR_LOCKED,				/* (16) The operation is rejected according to the file sharing policy */
	FR_NOT_ENOUGH_CORE,		/* (17) LFN working buffer could not be allocated */
	FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > FF_FS_LOCK */
	FR_INVALID_PARAMETER,	/* (19) Given parameter is invalid */
	FR_NO_SPACE_LEFT,		/* (20) No space left */
	FR_NO_DIRENTRY,			/* (21) No directory entry to allocate	*/
	FR_NO_EMPTY_DIR,		/* (22) Directory not empty  */
	FR_IS_DIR,				/* (23) Is a directory	*/
	FR_NO_DIR,				/* (24) Not a directory */
	FR_NO_EPERM,			/* (25) Operation not permitted */
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
	FR_MODIFIED,			/* (26) Virtual Partition Info has been modified by other fictions */
	FR_CHAIN_ERR,			/* (27) File cluster chain has been broken by the other enviornmnet */
	FR_INVAILD_FATFS,		/* (28) FATFS object error */
	FR_NOVIRPART,			/* (29) The external SD card has not been set a virtual partition configure */
	FR_OCCUPIED,			/* (30) The virtual partition has been illegally occupied. */
	FR_NOTCLEAR,			/* (31) The root directory is not clear */
	FR_NOTFIT,				/* (32) The partition does not fit the virtual partition feature */
#endif
} FRESULT;


/*--------------------------------------------------------------*/
/* FatFs module application interface                           */
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
FATFS*  f_getfatfs (int vol);
FRESULT f_scanfat (FATFS* fs);
FRESULT f_boundary (FATFS *fs, DWORD clust);
FRESULT f_disvirfs (FATFS* fs);
FRESULT f_unregvirfs (FATFS* fs);
FRESULT f_regvirfs (FATFS* fs);
FRESULT f_checkvirpart (FATFS* fs, const TCHAR* path, BYTE vol);
FRESULT f_makevirpart (FATFS* fs, const TCHAR* path, BYTE vol);
FRESULT f_getvirfree (const TCHAR* path, DWORD* nclst, DWORD* cclst);
FRESULT f_checkname (const TCHAR* path);
#endif
FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode);				/* Open or create a file */
FRESULT f_close (FIL* fp);											/* Close an open file object */
FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);			/* Read data from the file */
FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	/* Write data to the file */
FRESULT f_lseek (FIL* fp, FSIZE_t ofs);								/* Move file pointer of the file object */
FRESULT f_truncate (FIL* fp, FSIZE_t length);					   /* Truncate the file */
FRESULT f_sync (FIL* fp);											/* Flush cached data of the writing file */
FRESULT f_opendir (DIR* dp, const TCHAR* path);						/* Open a directory */
FRESULT f_closedir (DIR* dp);										/* Close an open directory */
FRESULT f_readdir (DIR* dp, FILINFO* fno);							/* Read a directory item */
FRESULT f_findfirst (DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern);	/* Find first file */
FRESULT f_findnext (DIR* dp, FILINFO* fno);							/* Find next file */
FRESULT f_mkdir (const TCHAR* path);								/* Create a sub directory */
FRESULT f_unlink (const TCHAR* path);								/* Delete an existing file or directory */
FRESULT f_rename (const TCHAR* path_old, const TCHAR* path_new);	/* Rename/Move a file or directory */
FRESULT f_stat (const TCHAR* path, FILINFO* fno);					/* Get file status */
FRESULT f_chmod (const TCHAR* path, BYTE attr, BYTE mask);			/* Change attribute of a file/dir */
FRESULT f_utime (const TCHAR* path, const FILINFO* fno);			/* Change timestamp of a file/dir */
FRESULT f_chdir (const TCHAR* path);								/* Change current directory */
FRESULT f_chdrive (const TCHAR* path);								/* Change current drive */
FRESULT f_getcwd (TCHAR* buff, UINT len);							/* Get current directory */
FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs);	/* Get number of free clusters on the drive */
FRESULT f_getlabel (const TCHAR* path, TCHAR* label, DWORD* vsn);	/* Get volume label */
FRESULT f_setlabel (const TCHAR* label);							/* Set volume label */
FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);	/* Forward data to the stream */
FRESULT f_expand (FIL* fp, FSIZE_t offset, FSIZE_t fsz, int opt);	/* Allocate a contiguous block to the file */
FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt);			/* Mount/Unmount a logical drive */
FRESULT f_mkfs (const TCHAR* path, const MKFS_PARM* opt, void* work, UINT len);	/* Create a FAT volume */
FRESULT f_fdisk (BYTE pdrv, const DWORD szt[], void* work);		/* Divide a physical drive into some partitions */
FRESULT f_setcp (WORD cp);											/* Set current code page */
int f_putc (TCHAR c, FIL* fp);										/* Put a character to the file */
int f_puts (const TCHAR* str, FIL* cp);								/* Put a string to the file */
int f_printf (FIL* fp, const TCHAR* str, ...);						/* Put a formatted string to the file */
TCHAR* f_gets (TCHAR* buff, int len, FIL* fp);						/* Get a string from the file */
void f_clean (const TCHAR* path);									/* clean all opening file and directory */
void f_settimestatus (UINT status);									/* system time flag setting */
FRESULT f_fcheckfat (DIR_FILE* dir_info);							/* check file cluster list */
FRESULT f_getclustinfo (FIL* fp, DWORD* fclust, DWORD* fcount);	/* get the clusters information of the file */
FRESULT f_checkopenlock(int index);
FRESULT sync_fs (FATFS* fs);
FRESULT sync_window(FATFS *fs);
FRESULT move_window (FATFS* fs, QWORD sector);
FRESULT fat_count_free_entries(DWORD *nclst, FATFS *fs);
void get_fileinfo (DIR* dp, FILINFO* fno);
DWORD get_fat (FFOBJID *obj, DWORD clst);
FRESULT put_fat(FATFS *fs, DWORD clst, DWORD val);
FRESULT mount_volume (const TCHAR **path, FATFS **rfs, BYTE mode);
QWORD clst2sect (FATFS* fs, DWORD clst );
DWORD ld_clust(FATFS *fs, const BYTE *dir);
void st_clust(FATFS *fs, BYTE *dir, DWORD cl);
DWORD ld_dword (const BYTE *ptr);
WORD ld_word (const BYTE *ptr);
void st_dword (BYTE *ptr, DWORD val);
void st_word (BYTE *ptr, WORD val);
FRESULT create_name (DIR *dp, const TCHAR **path);
int lock_fs (FATFS *fs);
UINT check_fs(FATFS *fs, QWORD sect);
UINT inc_lock(DIR* dp, int acc);
void unlock_fs (FATFS *fs, FRESULT res);
FRESULT dir_sdi (DIR *dp, DWORD ofs);
FRESULT dir_find(DIR *dp);
FRESULT dir_read(DIR *dp, int vol);
#ifndef __LITEOS_M__
FRESULT dir_read_massive(DIR *dp, int vol);
#endif
FRESULT dir_remove(DIR *dp);
FRESULT dir_next(DIR *dp, int stretch);
DWORD dir_ofs(DIR* dp);
FRESULT dir_register(DIR *dp);
DWORD create_chain (FFOBJID* obj, DWORD clst);
FRESULT remove_chain (FFOBJID* obj, DWORD clst, DWORD pclst);
void mem_set (void* dst, int val, UINT cnt);
void mem_cpy (void* dst, const void* src, UINT cnt);
int fatfs_get_vol (FATFS *fat);
FRESULT set_volumn_label(FATFS *fs, const TCHAR *label);

#define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
#define f_error(fp) ((fp)->err)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->obj.objsize)
#define f_rewind(fp) f_lseek((fp), 0)
#define f_rewinddir(dp) f_readdir((dp), 0)
#define f_rmdir(path) f_unlink(path)
#define f_unmount(path) f_mount(0, path, 0)




/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
#if !FF_FS_READONLY && !FF_FS_NORTC
DWORD get_fattime (void);
#endif

/* LFN support functions */
#if FF_USE_LFN >= 1						/* Code conversion (defined in unicode.c) */
WCHAR ff_oem2uni (WCHAR oem, WORD cp);	/* OEM code to Unicode conversion */
WCHAR ff_uni2oem (DWORD uni, WORD cp);	/* Unicode to OEM code conversion */
DWORD ff_wtoupper (DWORD uni);			/* Unicode upper-case conversion */
#endif
void* ff_memalloc (UINT msize);			/* Allocate memory block */
void ff_memfree (void* mblock);			/* Free memory block */
#ifndef __LITEOS_M__
int ff_strnlen(const void *str, size_t maxlen);
#endif

/* Sync functions */
#if FF_FS_REENTRANT
int ff_cre_syncobj (BYTE vol, FF_SYNC_t* sobj);	/* Create a sync object */
int ff_req_grant (FF_SYNC_t* sobj);		/* Lock sync object */
void ff_rel_grant (FF_SYNC_t* sobj);		/* Unlock sync object */
int ff_del_syncobj (FF_SYNC_t* sobj);	/* Delete a sync object */
#endif




/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access mode and open method flags (3rd argument of f_open) */
#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_OPEN_EXISTING	0x00
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define	FA_OPEN_APPEND		0x30

/* Fast seek controls (2nd argument of f_lseek) */
#define CREATE_LINKMAP	((FSIZE_t)0 - 1)

/* Format options (2nd argument of f_mkfs) */
#define FM_FAT		0x01
#define FM_FAT32	0x02
#define FM_ANY		0x07
#define FM_SFD		0x08

/*System Time Flag */
#define SYSTEM_TIME_ENABLE	0x01
#define SYSTEM_TIME_DISABLE	0x00

/* Filesystem type (FATFS.fs_type) */
#define FS_FAT12	1
#define FS_FAT16	2
#define FS_FAT32	3

/* File attribute bits for directory entry (FILINFO.fattrib) */
#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_LNK	0x80	/* Symbol link */

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
#define FS_PARENT	0x00
#define FS_CHILD	0x01

#define FS_VIRENABLE	0x01
#define FS_VIRDISABLE	0x00

#define PARENTFS(fs)	((FATFS*)((fs)->parent_fs))
#define CHILDFS(fs, i)	((FATFS*)(*((fs)->child_fs + (i))))
#endif

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
/* Global Data Area @ 0x00 - 0x31 (32 Bytes) */
#define VR_ITEMSIZE				0x20
#define VR_GLOBAL				0x00
#define VR_PartitionCnt				0x00	/* Virtual Partition Amount */
#define VR_PartitionFSType			0x01	/* Filesystem Type (1 Byte) */
#define VR_PartitionStSec			0x02	/* Partition Start Sector (4 Bytes) */
#define VR_PartitionCtSec			0x06	/* Partition Sectors Count (4 Bytes) */
#define VR_PartitionClstSz			0x0A	/* Partition Clust Size (2 Bytes) */
#define VR_PartitionCtClst			0x0C	/* Partition Clust Amount (4 Bytes) */
/* Each Virtual Partition Area (32 Bytes) */
/* Start offset at 0x20, Each entry is at 32 Bytes */
#define VR_PARTITION				0x20
#define VR_Available				0x00	/* Vritual Partition Item Flag (1 Bytes) */
#define VR_StartClust				0x01	/* Vritual Partition Start Cluster (4 Bytes) */
#define VR_CountClust				0x05	/* Virtual Partition Cluster Count (4 Bytes) */
#define VR_Entry				0x10	/* Virtual Partition Entry Label (16 Bytes) */

#define VR_VertifyString			0x1FC	/* Vertify String (4 Bytes), Must be "LITE" */
#endif

#ifdef __cplusplus
}
#endif

#endif /* FF_DEFINED */
