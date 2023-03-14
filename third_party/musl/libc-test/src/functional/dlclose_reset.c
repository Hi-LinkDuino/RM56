#include <dlfcn.h>
#include <stdio.h>
#include "test.h"


typedef int (* TEST_S_VAR)();  
typedef int (* TEST_G_VAR)();
typedef int (* INSP_S_VAR)();  
typedef int (* INSP_G_VAR)();  


int main(int argc, char *argv[])
{
	char buf[512];
	void *h = NULL;
	void *g = NULL;
	TEST_S_VAR test_s_var = NULL;
	TEST_G_VAR test_g_var = NULL;
	INSP_S_VAR insp_s_var = NULL;  
	INSP_G_VAR insp_g_var = NULL;  
	
	int s_var = 0;
	int g_var = 0;

	if(!t_pathrel(buf, sizeof buf, argv[0], "libdlclose_reset_dso.so")) {
		t_error("failed to obtain relative path to libdlclose_reset_dso.so\n");
		return 1;
	}

	h = dlopen(buf, RTLD_LAZY|RTLD_LOCAL);
	if(!h)
		t_error("dlopen %s failed: %s\n", buf, dlerror());

	test_s_var = (TEST_S_VAR)dlsym( h, "test_static_var");
	if (!test_s_var)
		t_error("dlsym test_static_var failed: %s\n", dlerror());

	test_g_var = (TEST_G_VAR)dlsym( h, "test_global_var");
	    if (!test_g_var)
					t_error("dlsyn test_global_var failed: %s\n", dlerror());

	test_s_var();
	test_g_var();

	if(dlclose( h))
		t_error("dlclose failed: %s\n", dlerror());

	g = dlopen(buf, RTLD_LAZY|RTLD_LOCAL);
	if(!g)
        t_error("dlopen %s failed: %s\n", buf, dlerror());
	
	insp_s_var = (INSP_S_VAR)dlsym( g, "ret_static");
	if (!insp_s_var)
		t_error("dlsym ret_static failed: %s\n", dlerror());

	insp_g_var = (INSP_G_VAR)dlsym( g, "ret_global");
	if (!insp_g_var)
		t_error("dlsyn global_var failed: %s\n", dlerror());
	
	s_var = insp_s_var();
	g_var = insp_g_var();

	if(s_var != 84)
		t_error("Static variable reset failed: want s_var=84 got s_var=%d\n", s_var);

	if(g_var != 84)
		t_error("Global variable reset failed: want g_var=84 got g_var=%d\n", g_var);

	if(dlclose( g))
		t_error("dlclose failed: %s\n", dlerror());

}
