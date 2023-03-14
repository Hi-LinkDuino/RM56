#define _GNU_SOURCE
#include <stdint.h>
#include <malloc.h>
#include <dlfcn.h>
#include <string.h>
#include <stdio.h>
#include <debug.h>
#include <unwind.h>

#define TRACE_MAX_DEPTH 16
#define STRING_MAX_LEN  256
#define TRACE_IGNORE    1

struct unwind_state_t {
	_Unwind_Word **cur;
	_Unwind_Word **end;
};

static _Unwind_Reason_Code unwind_callback(struct _Unwind_Context *context, void *arg)
{
	struct unwind_state_t *state = (struct unwind_state_t *)arg;
	_Unwind_Word pc = _Unwind_GetIP(context);
	if (pc != 0) {
		if (state->cur != state->end) {
			*state->cur = (_Unwind_Word *)pc;
		} else {
			return _URC_END_OF_STACK;
		}
	}
	++state->cur;

	return _URC_NO_REASON;
}

int backtrace(void **buffer, int size)
{
	struct unwind_state_t state;

	if ((buffer == NULL) || (size <= 0)) {
		return 0;
	}

	state.cur = (_Unwind_Word **)buffer;
	state.end = (_Unwind_Word **)(buffer + size);
	_Unwind_Backtrace(&unwind_callback, &state);

	return (void **)state.cur - buffer;
}

char **backtrace_symbols(void *const *buffer, int size)
{
	int i;
	char **str_location = NULL;
	char *str_buffer = NULL;
	char *str_base = NULL;
	char **string = NULL;
	Dl_info info = { 0 };
	size_t total_size = 0;

	if ((buffer == NULL) || (size <= 0)) {
		return NULL;
	}
	str_location = (char **)malloc(size * sizeof(char *));
	if (str_location == NULL) {
		return NULL;
	}
	memset(str_location, 0, size * sizeof(char *));
	for (i = 0; i < size; ++i) {
		dladdr((void *)buffer[i], &info);
		if ((info.dli_fname == NULL) || (info.dli_fname[0] == '\0')) {
			break;
		}
		str_buffer = (char *)malloc(STRING_MAX_LEN * sizeof(char));
		if (str_buffer == NULL) {
			goto err;
		}

		snprintf(str_buffer, STRING_MAX_LEN, "    #%02d: <%s+%#x>[%#x] -> %s\n", i, info.dli_sname,
			(uintptr_t)buffer[i] - (uintptr_t)info.dli_saddr,
			(uintptr_t)buffer[i] - (uintptr_t)info.dli_fbase, info.dli_fname);
		str_location[i] = str_buffer;
		total_size += strlen(str_buffer) + 1;
	}
	string = (char **)malloc(total_size + (size * sizeof(char *)));
	if (string == NULL) {
		goto err;
	}
	memset(string, 0, total_size + (size * sizeof(char *)));
	str_base = (char *)(string + size);
	for (i = 0; i < size; ++i) {
		if (str_location[i] == NULL) {
			break;
		}
		strcpy(str_base, str_location[i]);
		string[i] = str_base;
		str_base += strlen(string[i]) + 1;
		free(str_location[i]);
	}
	free(str_location);
	return string;

err:
	for (i = 0; i < size; ++i) {
		if (str_location[i]) {
			free(str_location[i]);
		}
	}
	free(str_location);
	return NULL;
}

static void get_backtrace_addr(void *const *buffer, int nptrs)
{
	for (int i = 1; i < nptrs; ++i) {
		printf("    #%02d: %#x\n", i, buffer[i]);
	}
}

void print_trace()
{
	int nptrs, i;
	void *buffer[TRACE_MAX_DEPTH];
	char **strings = NULL;

	nptrs = backtrace(buffer, TRACE_MAX_DEPTH);
	printf("\nBacktrace() returned %d addresses\n", nptrs - TRACE_IGNORE);
	strings = backtrace_symbols(buffer, nptrs);
	if (strings == NULL) {
		printf("backtrace_symbols() err!\n");
		return;
	}
	for (i = 1; i < nptrs; ++i) {
		if ((i == 1) && (strings[i] == NULL)) {
			get_backtrace_addr(buffer, nptrs);
			break;
		}
		printf("%s", strings[i]);
	}
	free(strings);
}

