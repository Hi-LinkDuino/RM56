#include <stdio.h>
int global_var = 84;

static int stat_var = 84;

void test_static_var()
{
	stat_var++;
}

void test_global_var()
{
	global_var++;
}

int ret_static()
{
	return stat_var;
}

int ret_global()
{
	return global_var;
}
__attribute__((constructor)) void before_main()  
{  
	   printf("before main\n");  
}  
  
__attribute__((destructor)) void after_main()  
{  
	   printf("after main\n");  
}
