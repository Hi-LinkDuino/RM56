#include <limits.h>
#include <stdio.h>
#include <stdatomic.h>
#include <memory.h>
#include "test.h"
#include "musl_preinit_common.h"

#define TEST(c, ...) \
	( (c) || (t_error(#c " failed: " __VA_ARGS__),0) )

static bool client_hook_flag;

bool get_hook_flag()
{
	return client_hook_flag;
}

bool set_hook_flag(bool flag)
{
	bool before_flag = client_hook_flag;
	client_hook_flag = flag;
	return before_flag;
}

void install_hook_function()
{
	memset(function_of_shared_lib, 0, sizeof(function_of_shared_lib[0]) * LAST_FUNCTION);
	function_of_shared_lib[GET_HOOK_FLAG_FUNCTION] = get_hook_flag;
	function_of_shared_lib[SET_HOOK_FLAG_FUNCTION] = set_hook_flag;
}

int main(int argc, char **argv)
{
	printf("Installing client's functions...");
	install_hook_function();
	printf("\nInstallation Completes\n");

	printf("Start 7 scenes testing...\n");

	printf("Case 1: client shared library has not been loaded.\n");
	atomic_store_explicit(&ohos_malloc_hook_shared_liibrary, (volatile long long)0, memory_order_seq_cst);
	bool hook_flag = __get_hook_flag();
	TEST(hook_flag == false, "hook_flag shoud be false\n");
	volatile const struct MallocDispatchType* malloc_dispatch_table = get_current_dispatch_table();
	TEST(malloc_dispatch_table == NULL, "malloc_dispatch_table shoud be NULL\n");

	printf("Case 2: client shared library is loading, no uninstall signal raising, but the functions hasn't been loaded, using the temporary malloc table.\n");
	atomic_store_explicit(&ohos_malloc_hook_shared_liibrary, (volatile long long)-1, memory_order_seq_cst);
	atomic_store_explicit(&__musl_libc_globals.current_dispatch_table, (volatile const long long)0x95919591, memory_order_seq_cst);
	atomic_store_explicit(&__hook_enable_hook_flag, (volatile bool)true, memory_order_seq_cst);
	hook_flag = __get_hook_flag();
	TEST(hook_flag == true, "hook_flag shoud be true\n");
	malloc_dispatch_table = get_current_dispatch_table();
	TEST(malloc_dispatch_table != NULL, "malloc_dispatch_table shoud be not NULL\n");

	printf("Case 3: client shared library is loading, uninstall signal raising, but the functions hasn't been loaded, using the temporary malloc table.\n");
	atomic_store_explicit(&ohos_malloc_hook_shared_liibrary, (volatile long long)-1, memory_order_seq_cst);
	atomic_store_explicit(&__musl_libc_globals.current_dispatch_table, (volatile const long long)0x95919591, memory_order_seq_cst);
	atomic_store_explicit(&__hook_enable_hook_flag, (volatile bool)false, memory_order_seq_cst);
	hook_flag = __get_hook_flag();
	TEST(hook_flag == true, "hook_flag shoud be true\n");
	malloc_dispatch_table = get_current_dispatch_table();
	TEST(malloc_dispatch_table == NULL, "malloc_dispatch_table shoud be NULL\n");

	printf("Case 4: client shared library is loaded, no uninstall signal raising, outside client malloc.\n");
	atomic_store_explicit(&ohos_malloc_hook_shared_liibrary, (volatile long long)0x10319090, memory_order_seq_cst);
	atomic_store_explicit(&__hook_enable_hook_flag, (volatile bool)true, memory_order_seq_cst);
	set_hook_flag(true);
	hook_flag = __get_hook_flag();
	TEST(hook_flag == true, "hook_flag shoud be true\n");
	malloc_dispatch_table = get_current_dispatch_table();
	TEST(malloc_dispatch_table != NULL, "malloc_dispatch_table shoud be not NULL\n");

	printf("Case 5: client shared library is loaded, no uninstall signal raising, inside client malloc.\n");
	atomic_store_explicit(&ohos_malloc_hook_shared_liibrary, (volatile long long)0x10319090, memory_order_seq_cst);
	atomic_store_explicit(&__hook_enable_hook_flag, (volatile bool)true, memory_order_seq_cst);
	set_hook_flag(false);
	hook_flag = __get_hook_flag();
	TEST(hook_flag == false, "hook_flag shoud be false\n");
	malloc_dispatch_table = get_current_dispatch_table();
	TEST(malloc_dispatch_table == NULL, "malloc_dispatch_table shoud be NULL\n");

	printf("Case 6: client shared library is loaded, uninstall signal raising, outside client malloc.\n");
	atomic_store_explicit(&ohos_malloc_hook_shared_liibrary, (volatile long long)0x10319090, memory_order_seq_cst);
	atomic_store_explicit(&__hook_enable_hook_flag, (volatile bool)false, memory_order_seq_cst);
	set_hook_flag(true);
	hook_flag = __get_hook_flag();
	TEST(hook_flag == true, "hook_flag shoud be true\n");
	malloc_dispatch_table = get_current_dispatch_table();
	TEST(malloc_dispatch_table == NULL, "malloc_dispatch_table shoud be NULL\n");

	printf("Case 7: client shared library is loaded, uninstall signal raising, inside client malloc.\n");
	atomic_store_explicit(&ohos_malloc_hook_shared_liibrary, (volatile long long)0x10319090, memory_order_seq_cst);
	atomic_store_explicit(&__hook_enable_hook_flag, (volatile bool)false, memory_order_seq_cst);
	set_hook_flag(false);
	hook_flag = __get_hook_flag();
	TEST(hook_flag == false, "hook_flag shoud be false\n");
	malloc_dispatch_table = get_current_dispatch_table();
	TEST(malloc_dispatch_table == NULL, "malloc_dispatch_table shoud be NULL\n");
	TEST(malloc_dispatch_table == NULL, "malloc_dispatch_table shoud be NULL\n");

	if (t_status == 0) {
		printf("All test cases passed!\n");
	} else {
		printf("***There are some test cases not passed.\n");
	}
	return t_status;
}
