#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include "libc.h"
#include "pthread_impl.h"
#include "malloc_impl.h"

static inline void lock(volatile int *lk)
{
	if (libc.threads_minus_1)
		while(a_swap(lk, 1)) __wait(lk, lk+1, 1, 1);
}

static inline void unlock(volatile int *lk)
{
	if (lk[0]) {
		a_store(lk, 0);
		if (lk[1]) __wake(lk, 1, 1);
	}
}

void *__memalign(size_t align, size_t len)
{
	unsigned char *mem, *new;

	if ((align & -align) != align) {
		errno = EINVAL;
		return 0;
	}

	if (len > SIZE_MAX - align || __malloc_replaced) {
		errno = ENOMEM;
		return 0;
	}

	if (align <= SIZE_ALIGN)
		return malloc(len);

	if (!(mem = malloc(len + align-1)))
		return 0;

	new = (void *)((uintptr_t)mem + align-1 & -align);
	if (new == mem) return mem;

	struct chunk *c = MEM_TO_CHUNK(mem);
	struct chunk *n = MEM_TO_CHUNK(new);

	if (g_enable_check) {
		int status = delete_node(mem);
		if (status != 0) {
			get_free_trace(mem);
			a_crash();
		}
	}

	if (IS_MMAPPED(c)) {
		/* Apply difference between aligned and original
		 * address to the "extra" field of mmapped chunk. */
		n->psize = c->psize + (new-mem);
		n->csize = c->csize - (new-mem);
		if (g_enable_check) {
			insert_node(CHUNK_TO_MEM(n), CHUNK_SIZE(n));
		}
		return new;
	}

	struct chunk *t = NEXT_CHUNK(c);

	/* Split the allocated chunk into two chunks. The aligned part
	 * that will be used has the size in its footer reduced by the
	 * difference between the aligned and original addresses, and
	 * the resulting size copied to its header. A new header and
	 * footer are written for the split-off part to be freed. */
	lock(g_mem_lock);
	n->psize = c->csize = C_INUSE | (new-mem);
	n->csize = t->psize -= new-mem;
	calculate_checksum(c, n);
	calculate_checksum(NULL, t);
	unlock(g_mem_lock);
	if (g_enable_check) {
		insert_node(CHUNK_TO_MEM(n), CHUNK_SIZE(n));
	}

	__bin_chunk(c);
	return new;
}

weak_alias(__memalign, memalign);
