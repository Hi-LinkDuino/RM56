#ifndef MALLOC_IMPL_H
#define MALLOC_IMPL_H

#include <sys/mman.h>
#include <stdbool.h>

#define BACKTRACE_DEPTH_MAX      5          /* The max depth of backtrace */
#define BACKTRACE_START_OFFSET   0          /* The start offset of backtrace */
#define SECONDARY_CALL_OFFSET    2          /* The backtrace offset for secondary call backtrace() */
#define BACKTRACE_OFFSET         (BACKTRACE_START_OFFSET + SECONDARY_CALL_OFFSET)
#define PREFIX_PLACE_HOLDER      10         /* Reserve positions for file name prefix "pid()_" */
#define PTHREAD_NUM_MAX          128        /* Same as number of task of kernel */
#define NODE_MAGIC               0xFCFCFCFC /* Magic number for node of chunk */
#define FREE_MAGIC               0xFE       /* Magic number for filling freed heap memory not recycled to heap pool */
#define RECYCLE_MAX              128        /* The queue size for free() to recycle */
#define RECYCLE_SIZE_MAX         0x300000   /* The max sum size of freed chunk for recycle list */
#define ITEM_BUFFER_SIZE         256        /* The buffer max size for one item of memory debug info */
#define CHECK_POINT_TRACE_MAX    2          /* The trace max for check point */

hidden void *__expand_heap(size_t *);

hidden void __malloc_donate(char *, char *);

hidden void *__memalign(size_t, size_t);

struct chunk {
	unsigned int checksum;
	size_t psize, csize;
	struct chunk *next, *prev;
};

struct bin {
	unsigned int checksum;
	volatile int lock[2];
	struct chunk *head;
	struct chunk *tail;
};

struct heap_block {
	struct heap_block *next;
	struct heap_block *prev;
};

struct list {
	struct list *prev;
	struct list *next;
};

struct node {
	short tid, pid;
	void *ptr;
	size_t size;
	void *lr[BACKTRACE_DEPTH_MAX];
	struct list list;
};

struct stat_bin {
	volatile int lock[2];
	struct list head;
	size_t t_total_size;
};

#define ROUNDUP(a, b) (((a) + ((b) - 1)) & ~((b) - 1))
#define SIZE_ALIGN ROUNDUP(sizeof(struct chunk), 0x10)
#define SIZE_MASK (-SIZE_ALIGN)
#define OVERHEAD (sizeof(struct chunk))
#define BLOCK_HEAD (sizeof(struct heap_block) + OVERHEAD)
#define CHUNK_BLOCK_OFFSET (sizeof(struct heap_block))
#define CHUNK_TO_BLOCK(c) (struct heap_block *)((char *)(c) - CHUNK_BLOCK_OFFSET)
#define BLOCK_TO_CHUNK(p) (struct chunk *)((char *)(p) + CHUNK_BLOCK_OFFSET)
#define MMAP_THRESHOLD (0x1c00*(4*sizeof(size_t)))
#define DONTCARE SIZE_ALIGN
#define RECLAIM 163840

#define CHUNK_SIZE(c) ((c)->csize & -2)
#define CHUNK_PSIZE(c) ((c)->psize & -2)
#define PREV_CHUNK(c) ((struct chunk *)((char *)(c) - CHUNK_PSIZE(c)))
#define NEXT_CHUNK(c) ((struct chunk *)((char *)(c) + CHUNK_SIZE(c)))
#define MEM_TO_CHUNK(p) (struct chunk *)((char *)(p) - OVERHEAD)
#define CHUNK_TO_MEM(c) (void *)((char *)(c) + OVERHEAD)
#define BIN_TO_CHUNK(i) (&mal.bins[i].checksum)

#define C_INUSE  ((size_t)1)

#define IS_MMAPPED(c) !((c)->csize & (C_INUSE))

hidden void __bin_chunk(struct chunk *);

hidden extern int __malloc_replaced;

hidden extern bool g_enable_check;
hidden extern int g_recycle_num;
hidden extern size_t g_recycle_size;
hidden extern int g_mem_lock[];
hidden extern void insert_node(void *ptr, size_t size);
hidden extern int delete_node(void *ptr);
hidden extern void insert_block_list(struct chunk *ptr);
hidden extern void insert_free_tail(struct chunk *self);
hidden extern struct chunk *get_free_head(void);
hidden extern void get_free_trace(void *ptr);
hidden extern void clean_recycle_list(bool clean_all);
hidden extern void check_chunk_integrity(struct chunk *cur);
hidden extern void calculate_checksum(struct chunk *cur, struct chunk *next);

#endif
