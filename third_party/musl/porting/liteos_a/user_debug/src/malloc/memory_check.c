#define _GNU_SOURCE
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <debug.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>
#include "libc.h"
#include "atomic.h"
#include "pthread_impl.h"
#include "malloc_impl.h"

#if defined(__GNUC__) && defined(__PIC__)
#define inline inline __attribute__((always_inline))
#endif

bool g_enable_check = false;
int g_recycle_num;
size_t g_recycle_size;
int g_mem_lock[2];
static struct chunk recycle_list;
static struct heap_block block_list;

static struct {
	struct stat_bin bins[PTHREAD_NUM_MAX];
	struct stat_bin free_list;
	size_t p_total_size;
	size_t peak_size;
	char *f_path;
	char f_path_buf[PATH_MAX];
	int fd;
	bool verbose;
} mem_stat;

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

static inline void lock_stat_bin(int tid)
{
	lock(mem_stat.bins[tid].lock);
	if (!mem_stat.bins[tid].head.next)
		mem_stat.bins[tid].head.next = mem_stat.bins[tid].head.prev = &mem_stat.bins[tid].head;
}

static inline void unlock_stat_bin(int tid)
{
	unlock(mem_stat.bins[tid].lock);
}

static void insert_free_list(struct node *node)
{
	struct list *list = NULL;

	list = mem_stat.free_list.head.prev;
	node->list.prev = list;
	node->list.next = list->next;
	list->next = &node->list;
	node->list.next->prev = &node->list;
}

static int try_delete_node(int tid, void *ptr)
{
	struct list *list = NULL;
	struct node *node = NULL;

	lock_stat_bin(tid);
	for (list = mem_stat.bins[tid].head.next; list != &mem_stat.bins[tid].head; list = list->next) {
		node = (struct node *)((uintptr_t)list - (uint32_t)&((struct node *)0)->list);
		if (node->ptr != ptr) {
			continue;
		}
		list->prev->next = list->next;
		list->next->prev = list->prev;
		mem_stat.bins[tid].t_total_size -= node->size;
		insert_free_list(node);
		mem_stat.p_total_size -= node->size;
		unlock_stat_bin(tid);
		return 0;
	}
	unlock_stat_bin(tid);
	return -1;
}

int delete_node(void *ptr)
{
	int tid = ((struct pthread *)pthread_self())->tid;
	int status, i;

	lock(g_mem_lock);
	status = try_delete_node(tid, ptr);
	if (status == 0) {
		unlock(g_mem_lock);
		return 0;
	}

	for (i = 0; i < PTHREAD_NUM_MAX; ++i) {
		if (i == tid) {
			continue;
		}
		status = try_delete_node(i, ptr);
		if (status == 0) {
			unlock(g_mem_lock);
			return 0;
		}
	}
	unlock(g_mem_lock);
	return -1;
}

static struct node *expand_mem(void)
{
	struct node *ptr = NULL;
	struct node *node = NULL;
	size_t node_len = sizeof(struct node);
	int n_node = PAGE_SIZE / node_len;
	int i;

	ptr = __mmap(0, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ptr == MAP_FAILED) {
		printf("%s %d, map failed, err: %s\n", __func__, __LINE__, strerror(errno));
		return NULL;
	}

	for (i = 1; i < n_node; ++i) {
		node = (struct node *)((uintptr_t)ptr + i * node_len);
		insert_free_list(node);
	}

	return ptr;
}

static struct node *alloc_node(void)
{
	struct list *list = NULL;
	struct node *node = NULL;
	int ret;

	if (!mem_stat.free_list.head.next) {
		mem_stat.free_list.head.next = mem_stat.free_list.head.prev = &mem_stat.free_list.head;
	}

	for (list = mem_stat.free_list.head.next; list != &mem_stat.free_list.head; list = list->next) {
		node = (struct node *)((uintptr_t)list - (uint32_t)&((struct node *)0)->list);
		list->prev->next = list->next;
		list->next->prev = list->prev;
		return node;
	}

	return expand_mem();
}

static struct node *create_node(int tid, void *ptr, size_t size)
{
	pid_t pid = getpid();
	struct node *node = NULL;
	void *lr[BACKTRACE_DEPTH_MAX + BACKTRACE_OFFSET + 1] = { 0 };
	int nptr;

	node = alloc_node();
	if (node == NULL) {
		return NULL;
	}
	mem_stat.p_total_size += size;
	mem_stat.peak_size = (mem_stat.peak_size < mem_stat.p_total_size) ? mem_stat.p_total_size : mem_stat.peak_size;
	node->tid = tid;
	node->pid = pid;
	node->ptr = ptr;
	node->size = size;
	nptr = backtrace(lr, BACKTRACE_DEPTH_MAX + BACKTRACE_OFFSET + 1);
	memcpy(node->lr, lr + BACKTRACE_OFFSET + 1, BACKTRACE_DEPTH_MAX * sizeof(void *));
	return node;
}

void insert_node(void *ptr, size_t size)
{
	int tid = ((struct pthread *)pthread_self())->tid;
	struct list *list = NULL;
	struct node *node = NULL;

	lock(g_mem_lock);
	node = create_node(tid, ptr, size);
	if (node == NULL) {
		unlock(g_mem_lock);
		return;
	}

	lock_stat_bin(tid);
	mem_stat.bins[tid].t_total_size += size;
	list = mem_stat.bins[tid].head.prev;
	node->list.prev = list;
	node->list.next = list->next;
	list->next = &node->list;
	node->list.next->prev = &node->list;
	unlock_stat_bin(tid);
	unlock(g_mem_lock);
}

static void file_path_init(void)
{
	char *pos = NULL;
	int len;

	if (!mem_stat.f_path) {
		return;
	}

	pos = strrchr(mem_stat.f_path, '/');
	if (pos) {
		len = pos - mem_stat.f_path + 1;
		strncpy(mem_stat.f_path_buf, mem_stat.f_path, PATH_MAX - 1);
		snprintf(mem_stat.f_path_buf + len, PATH_MAX - len, "pid(%d)_%s", getpid(), pos + 1);
	} else {
		snprintf(mem_stat.f_path_buf, PATH_MAX, "pid(%d)_%s", getpid(), mem_stat.f_path);
	}
}

static bool get_file(void)
{
	if (!g_enable_check) {
		printf("You should call mem_check_init(char *) or use command line parameters, "
			"--mwatch or --mrecord <full path>, to call mem_check_init(char *) "
			"when executing your program.\n");
		return false;
	}

	if (mem_stat.verbose) {
		return true;
	}

	file_path_init();
	if (!access(mem_stat.f_path_buf, 0)) {
		return true;
	}
	mem_stat.fd = open(mem_stat.f_path_buf, O_RDWR | O_CREAT);
	if (mem_stat.fd < 0) {
		printf("err: %s create failed, memory info will output from serial port!\n", mem_stat.f_path_buf);
		mem_stat.verbose = true;
	}
	return true;
}

static int get_backtrace_info(void **buffer, int nptr, int fd, bool verbose, bool checkpoint)
{
	int i, ret;
	char str_buf[ITEM_BUFFER_SIZE];
	Dl_info info = { 0 };
	bool checkpoint_head = false;
	int checkpoint_trace_num = 0;
	bool symbol_found;

	for (i = 0; i < nptr; ++i) {
		symbol_found = true;
		dladdr((void *)buffer[i], &info);
		if ((info.dli_fname == NULL) || (info.dli_fname[0] == '\0')) {
			symbol_found = false;
		}

		if (checkpoint && !checkpoint_head) {
			checkpoint_head = true;
			if (verbose) {
				printf("    [Check point]:\n");
			} else {
				snprintf(str_buf, ITEM_BUFFER_SIZE, "    [Check point]:\n");
				ret = write(fd, str_buf, strlen(str_buf));
				if (ret != strlen(str_buf)) {
					goto err;
				}
			}
		}
		if (verbose) {
			symbol_found ?
			printf("\t#%02d: <%s+%#x>[%#x] -> %s\n", i, info.dli_sname, (uintptr_t)buffer[i] -
				(uintptr_t)info.dli_saddr, (uintptr_t)buffer[i] - (uintptr_t)info.dli_fbase, info.dli_fname) :
			printf("\t#%02d: %#x\n", i, buffer[i]);
		} else {
			symbol_found ?
			snprintf(str_buf, ITEM_BUFFER_SIZE, "\t#%02d: <%s+%#x>[%#x] -> %s\n", i, info.dli_sname,
				(uintptr_t)buffer[i] - (uintptr_t)info.dli_saddr, (uintptr_t)buffer[i] - (uintptr_t)info.dli_fbase,
				info.dli_fname) :
			snprintf(str_buf, ITEM_BUFFER_SIZE, "\t#%02d: %#x\n", i, buffer[i]);
			ret = write(fd, str_buf, strlen(str_buf));
			if (ret != strlen(str_buf)) {
				goto err;
			}
		}
		if (checkpoint) {
			checkpoint_trace_num++;
			if (checkpoint_trace_num == CHECK_POINT_TRACE_MAX) {
				break;
			}
		}
	}
	return 0;
err:
	printf("Write failed, err: %s\n", strerror(errno));
	return ret;
}

static int print_integrity_info(struct node *node)
{
	int ret;
	char buffer[ITEM_BUFFER_SIZE];
	char *str = "The possible attacker was allocated from:";

	if (mem_stat.verbose) {
		printf("\n==PID:%d== Memory integrity information:\n", getpid());
		printf("    [TID:%d PID:%d allocated addr: %#x, size: %#x] %s\n", node->tid, node->pid, node->ptr, node->size,
			str);
	} else {
		snprintf(buffer, ITEM_BUFFER_SIZE, "\n==PID:%d== Memory integrity information:\n", getpid());
		ret = write(mem_stat.fd, buffer, strlen(buffer));
		if (ret != strlen(buffer)) {
			goto err;
		}
		snprintf(buffer, ITEM_BUFFER_SIZE, "    [TID:%d PID:%d allocated addr: %#x, size: %#x] %s\n", node->tid, node->pid,
			node->ptr, node->size, str);
		ret = write(mem_stat.fd, buffer, strlen(buffer));
		if (ret != strlen(buffer)) {
			goto err;
		}
	}
	return 0;
err:
	printf("Write failed, err: %s\n", strerror(errno));
	return ret;
}

static int check_mem_integrity(int tid, void *ptr)
{
	struct list *list = NULL;
	struct node *node = NULL;
	int nptr = 0;

	lock_stat_bin(tid);
	for (list = mem_stat.bins[tid].head.next; list != &mem_stat.bins[tid].head; list = list->next) {
		node = (struct node *)((uintptr_t)list - (uint32_t)&((struct node *)0)->list);
		if (node->ptr != ptr) {
			continue;
		}
		if (print_integrity_info(node) != 0) {
			unlock_stat_bin(tid);
			printf("Memory integrity check failed!\n");
			return -1;
		}
		while (node->lr[nptr] != NULL) {
			++nptr;
			if (nptr == BACKTRACE_DEPTH_MAX) {
				break;
			}
		}
		if ((nptr == 0) || (get_backtrace_info(node->lr, nptr, mem_stat.fd, mem_stat.verbose, false) != 0)) {
			unlock_stat_bin(tid);
			printf("get backtrace failed!\n");
			return -1;
		}
		if (!mem_stat.verbose) {
			printf("Memory integrity information saved in %s\n", mem_stat.f_path_buf);
		}
		unlock_stat_bin(tid);
		return 0;
	}
	unlock_stat_bin(tid);
	return 1;
}

static void get_integrity_info(void *ptr)
{
	int i, status;
	int tid = ((struct pthread *)pthread_self())->tid;

	status = check_mem_integrity(tid, ptr);
	if (status != 1) {
		return;
	}

	for (i = 0; i < PTHREAD_NUM_MAX; ++i) {
		if (i == tid) {
			continue;
		}
		status = check_mem_integrity(i, ptr);
		if (status != 1) {
			return;
		}
	}
}

bool is_invalid(struct chunk *self)
{
	uint32_t checksum;
	checksum = CHUNK_SIZE(self) ^ CHUNK_PSIZE(self) ^ NODE_MAGIC;
	if (checksum != self->checksum) {
		return true;
	} else {
		return false;
	}
}

void calculate_checksum(struct chunk *cur, struct chunk *next)
{
	if (cur) {
		cur->checksum = CHUNK_SIZE(cur) ^ CHUNK_PSIZE(cur) ^ NODE_MAGIC;
	}

	if (next) {
		next->checksum = CHUNK_SIZE(next) ^ CHUNK_PSIZE(next) ^ NODE_MAGIC;
	}
}

void check_heap_integrity(void)
{
	struct chunk *cur = NULL;
	struct chunk *next = NULL;
	struct heap_block *block = NULL;

	if (!block_list.next) {
		return;
	}

	lock(g_mem_lock);
	if (!get_file()) {
		unlock(g_mem_lock);
		return;
	}
	block = block_list.next;
	while (block != &block_list) {
		cur = BLOCK_TO_CHUNK(block);
		do {
			next = NEXT_CHUNK(cur);
			if (is_invalid(next)) {
				get_integrity_info(CHUNK_TO_MEM(cur));
				unlock(g_mem_lock);
				a_crash();
			}
			cur = next;
		} while (CHUNK_SIZE(next));
		block = block->next;
	}
	unlock(g_mem_lock);
	printf("\nCheck heap integrity ok!\n");
}

void check_chunk_integrity(struct chunk *cur)
{
	struct chunk *next = NULL;

	if (is_invalid(cur)) {
		check_heap_integrity();
	}

	lock(g_mem_lock);
	next = NEXT_CHUNK(cur);
	if ((CHUNK_SIZE(next)) && is_invalid(next)) {
		get_integrity_info(CHUNK_TO_MEM(cur));
		unlock(g_mem_lock);
		a_crash();
	}
	unlock(g_mem_lock);
}

void insert_block_list(struct chunk *self)
{
	struct heap_block *block = CHUNK_TO_BLOCK(self);
	struct heap_block *cur = NULL;

	if (!block_list.next) {
		block_list.next = block_list.prev = &block_list;
	}

	cur = block_list.prev;
	block->next = cur->next;
	block->prev = cur;
	cur->next = block;
	block_list.prev = block;
}

void get_free_trace(void *ptr)
{
	void *lr[BACKTRACE_DEPTH_MAX + BACKTRACE_OFFSET] = { 0 };
	int tid = ((struct pthread *)pthread_self())->tid;
	char buffer[ITEM_BUFFER_SIZE];
	int nptr, ret;

	lock(g_mem_lock);
	if (!get_file()) {
		unlock(g_mem_lock);
		return;
	}
	if (mem_stat.verbose) {
		printf("\n==PID:%d== double free\n", getpid());
		printf("    [TID:%d freed addr: %#x]:\n", tid, ptr);
	} else {
		snprintf(buffer, ITEM_BUFFER_SIZE, "\n==PID:%d== double free\n", getpid());
		ret = write(mem_stat.fd, buffer, strlen(buffer));
		if (ret != strlen(buffer)) {
			goto err;
		}
		snprintf(buffer, ITEM_BUFFER_SIZE, "    [TID:%d freed addr: %#x]:\n", tid, ptr);
		ret = write(mem_stat.fd, buffer, strlen(buffer));
		if (ret != strlen(buffer)) {
			goto err;
		}
	}

	nptr = backtrace(lr, BACKTRACE_DEPTH_MAX + BACKTRACE_OFFSET);
	if (get_backtrace_info(lr + BACKTRACE_OFFSET, nptr - BACKTRACE_OFFSET, mem_stat.fd, mem_stat.verbose, false) != 0) {
		printf("Trace failed\n");
	}

	unlock(g_mem_lock);
	return;
err:
	printf("Write failed, err: %s\n", strerror(errno));
	unlock(g_mem_lock);
	return;
}

void watch_mem(void)
{
	int tid, ret;
	char buffer[ITEM_BUFFER_SIZE];
	void *lr[BACKTRACE_DEPTH_MAX + BACKTRACE_OFFSET] = { 0 };
	pid_t pid = getpid();
	int nptr;

	lock(g_mem_lock);
	if (!get_file()) {
		unlock(g_mem_lock);
		return;
	}
	if (mem_stat.verbose) {
		printf("\n==PID:%d== Heap memory statistics(bytes):\n", pid);
	} else {
		snprintf(buffer, ITEM_BUFFER_SIZE, "\n==PID:%d== Heap memory statistics(bytes):\n", pid);
		ret = write(mem_stat.fd, buffer, strlen(buffer));
		if (ret != strlen(buffer)) {
			goto err2;
		}
	}
	nptr = backtrace(lr, BACKTRACE_DEPTH_MAX + BACKTRACE_OFFSET);
	if (get_backtrace_info(lr + BACKTRACE_OFFSET, nptr - BACKTRACE_OFFSET, mem_stat.fd, mem_stat.verbose, true) != 0) {
		printf("Check failed\n");
		unlock(g_mem_lock);
		return;
	}
	for (tid = 0; tid < PTHREAD_NUM_MAX; ++tid) {
		lock_stat_bin(tid);
		if (mem_stat.bins[tid].t_total_size == 0) {
			unlock_stat_bin(tid);
			continue;
		}
		if (mem_stat.verbose) {
			printf("\n    [TID: %d, Used: %#x]", tid, mem_stat.bins[tid].t_total_size);
		} else {
			snprintf(buffer, ITEM_BUFFER_SIZE, "\n    [TID: %d, Used: %#x]", tid, mem_stat.bins[tid].t_total_size);
			ret = write(mem_stat.fd, buffer, strlen(buffer));
			if (ret != strlen(buffer)) {
				goto err1;
			}
		}
		unlock_stat_bin(tid);
	}
	if (mem_stat.verbose) {
		printf("\n\n==PID:%d== Total heap: %#x byte(s), Peak: %#x byte(s)\n", pid,
				mem_stat.p_total_size, mem_stat.peak_size);
	} else {
		snprintf(buffer, ITEM_BUFFER_SIZE, "\n\n==PID:%d== Total heap: %#x byte(s), Peak: %#x byte(s)\n", pid,
				 mem_stat.p_total_size, mem_stat.peak_size);
		ret = write(mem_stat.fd, buffer, strlen(buffer));
		if (ret != strlen(buffer)) {
			goto err2;
		}
	}
	if (!mem_stat.verbose) {
		printf("Memory statistics information saved in %s\n", mem_stat.f_path_buf);
	}
	unlock(g_mem_lock);
	return;
err1:
	unlock_stat_bin(tid);
err2:
	printf("Write failed, err: %s\n", strerror(errno));
	unlock(g_mem_lock);
}

static int get_node_info(struct node *node, int fd, bool verbose, bool mem_leak_exist)
{
	char buffer[ITEM_BUFFER_SIZE];
	void *lr[BACKTRACE_DEPTH_MAX + BACKTRACE_OFFSET] = { 0 };
	int nptr, ret;

	if (!mem_leak_exist) {
		if (verbose) {
			printf("\n==PID:%d== Detected memory leak(s):\n", getpid());
		} else {
			snprintf(buffer, ITEM_BUFFER_SIZE, "\n==PID:%d== Detected memory leak(s):\n", getpid());
			ret = write(fd, buffer, strlen(buffer));
			if (ret != strlen(buffer)) {
				goto err;
			}
		}
		nptr = backtrace(lr, BACKTRACE_DEPTH_MAX + BACKTRACE_OFFSET);
		if (get_backtrace_info(lr + BACKTRACE_OFFSET, nptr - BACKTRACE_OFFSET, mem_stat.fd, mem_stat.verbose, true) != 0) {
			printf("Check failed\n");
			goto err;
		}
	}

	if (verbose) {
		printf("\n    [TID:%d Leak:%#x byte(s)] Allocated from:\n", node->tid, node->size);
	} else {
		snprintf(buffer, ITEM_BUFFER_SIZE, "\n    [TID:%d Leak:%#x byte(s)] Allocated from:\n", node->tid, node->size);
		ret = write(fd, buffer, strlen(buffer));
		if (ret != strlen(buffer)) {
			goto err;
		}
	}
	return 0;
err:
	printf("Write failed, err: %s\n", strerror(errno));
	return ret;

}

static void print_summary_info(size_t leak_size, size_t allocs, int fd, bool verbose, bool mem_leak_exist)
{
	char buffer[ITEM_BUFFER_SIZE];
	int ret;

	if (!mem_leak_exist) {
		if (verbose) {
			printf("\nNo memory leak!\n");
			return;
		} else {
			snprintf(buffer, ITEM_BUFFER_SIZE, "\nNo memory leak!\n");
			ret = write(fd, buffer, strlen(buffer));
			if (ret != strlen(buffer)) {
				printf("Write failed, err: %s\n", strerror(errno));
			}
			return;
		}
	}

	if (verbose) {
		printf("\n==PID:%d== SUMMARY: %#x byte(s) leaked in %d allocation(s).\n", getpid(), leak_size, allocs);
	} else {
		snprintf(buffer, ITEM_BUFFER_SIZE, "\n==PID:%d== SUMMARY: %#x byte(s) leaked in %d allocation(s).\n", getpid(),
			leak_size, allocs);
		ret = write(fd, buffer, strlen(buffer));
		if (ret != strlen(buffer)) {
			printf("Write failed, err: %s\n", strerror(errno));
		}
	}
}

void check_leak(void)
{
	struct list *list = NULL;
	struct node *node = NULL;
	int tid, nptr;
	size_t leak_size = 0;
	size_t allocs = 0;
	bool mem_leak_exist = false;
	pid_t pid = getpid();

	lock(g_mem_lock);
	if (!get_file()) {
		unlock(g_mem_lock);
		return;
	}
	for (tid = 0; tid < PTHREAD_NUM_MAX; ++tid) {
		lock_stat_bin(tid);
		for (list = mem_stat.bins[tid].head.next; list != &mem_stat.bins[tid].head; list = list->next) {
			node = (struct node *)((uintptr_t)list - (uint32_t)&((struct node *)0)->list);
			if (node->pid != pid) {
				continue;
			}
			if (get_node_info(node, mem_stat.fd, mem_stat.verbose, mem_leak_exist) != 0) {
				unlock_stat_bin(tid);
				unlock(g_mem_lock);
				printf("Check failed\n");
				return;
			}
			++allocs;
			leak_size += node->size;
			mem_leak_exist = true;
			nptr = 0;
			while (node->lr[nptr] != NULL) {
				++nptr;
				if (nptr == BACKTRACE_DEPTH_MAX) {
					break;
				}
			}
			if (nptr == 0) {
				continue;
			}
			if (get_backtrace_info(node->lr, nptr, mem_stat.fd, mem_stat.verbose, false) != 0) {
				unlock_stat_bin(tid);
				unlock(g_mem_lock);
				printf("Check failed\n");
				return;
			}
		}
		unlock_stat_bin(tid);
	}
	print_summary_info(leak_size, allocs, mem_stat.fd, mem_stat.verbose, mem_leak_exist);
	if (!mem_stat.verbose) {
		printf("Leak check information saved in %s\n", mem_stat.f_path_buf);
	}
	unlock(g_mem_lock);
}

void mem_check_init(char *f_path)
{
	signal(35, watch_mem);
	signal(36, check_leak);
	signal(37, check_heap_integrity);
	g_enable_check = true;
	mem_stat.fd = -1;
	const char *string = "memory info will print to serial port!";

	if (!f_path) {
		goto out;
	}

	if (strlen(f_path) > (PATH_MAX - PREFIX_PLACE_HOLDER - 1)) {
		printf("file name: %s is too long, %s\n", f_path, string);
		goto out;
	}
	mem_stat.f_path = f_path;
	file_path_init();
	mem_stat.fd = open(mem_stat.f_path_buf, O_RDWR | O_CREAT | O_EXCL);
	if (mem_stat.fd < 0) {
		switch (errno) {
		case EEXIST:
			printf("file: %s is exist, %s\n", mem_stat.f_path_buf, string);
			goto out;
		default:
			printf("path: %s create failed, %s\n", mem_stat.f_path_buf, string);
			goto out;
		}
	}
	mem_stat.verbose = false;
	return;

out:
	mem_stat.verbose = true;
}

void mem_check_deinit()
{
	if (mem_stat.fd > 0) {
		close(mem_stat.fd);
	}
}

void parse_argv(int argc, char *argv[])
{

	if (argc <= 1) {
		return;
	}

	if (!strcmp(argv[argc - 1], "--mwatch")) {
		mem_check_init(NULL);
	} else if ((argc > 2) && (!strcmp(argv[argc - 2], "--mrecord"))) {
		mem_check_init(argv[argc - 1]);
	} else if (!strcmp(argv[argc - 1], "--mrecord")) {
		printf("usage: --mrecord filepath\n");
	}
}

void insert_free_tail(struct chunk *self)
{
	volatile struct chunk *cur = NULL;
	lock(g_mem_lock);
	if (!recycle_list.next) {
		recycle_list.next = recycle_list.prev = &recycle_list;
	}
	cur = recycle_list.prev;
	self->next = cur->next;
	self->prev = cur;
	cur->next = self;
	recycle_list.prev = self;
	memset(CHUNK_TO_MEM(self), FREE_MAGIC, CHUNK_SIZE(self) - OVERHEAD);
	++g_recycle_num;
	g_recycle_size += CHUNK_SIZE(self);
	unlock(g_mem_lock);
}

struct chunk *get_free_head(void)
{
	struct chunk *cur = NULL;
	lock(g_mem_lock);
	cur = recycle_list.next;
	if ((cur == NULL) || (cur == &recycle_list)) {
		unlock(g_mem_lock);
		return NULL;
	}
	recycle_list.next = cur->next;
	cur->next->prev = cur->prev;
	--g_recycle_num;
	g_recycle_size -= CHUNK_SIZE(cur);
	unlock(g_mem_lock);
	return cur;
}

void clean_recycle_list(bool clean_all)
{
	struct chunk *self = NULL;
	self = get_free_head();
	while (self) {
		__bin_chunk(self);
		if ((!clean_all) && (g_recycle_size < RECYCLE_SIZE_MAX)) {
			break;
		}
		self = get_free_head();
	}
}

