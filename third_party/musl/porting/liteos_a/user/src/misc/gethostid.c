#define _GNU_SOURCE

#include <netdb.h>
#include <string.h>
#include <unistd.h>

#define HOSTID_NUM   16

long gethostid()
{
	int ret;
	struct in_addr in;
	struct hostent *hst = NULL;
	char **p = NULL;
	char host[128] = {0};

	ret = gethostname(host, sizeof(host));
	if (ret || host[0] == '\0') {
		return -1;
	}

	hst = gethostbyname(host);
	if (hst == NULL) {
		return -1;
	}

	p = hst->h_addr_list;
	memcpy(&in.s_addr, *p, sizeof(in.s_addr));

	return (long)((in.s_addr << HOSTID_NUM) | (in.s_addr >> HOSTID_NUM));
}
