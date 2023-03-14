#include <netdb.h>
#include <string.h>

struct netent *getnetbyaddr(uint32_t net, int addrtype)
{	
	struct netent *ne = NULL;
	setnetent(1);
	while (1) {
		ne = getnetent();
		if (!ne)
			break;
		if (ne->n_net == net && ne->n_addrtype == addrtype) {
			setnetent(0);
			endnetent();
			return ne;
		}
	}
	setnetent(0);
	endnetent();
	return NULL;
}

struct netent *getnetbyname(const char *netname)
{
	struct netent *ne = NULL;
	setnetent(1);
	while (1) {
		ne = getnetent();
		if (!ne)
			break;
		if (strcmp(ne->n_name, netname) == 0) {
			setnetent(0);
			endnetent();
			return ne;
		}
	}
	setnetent(0);
	endnetent();
	return NULL;
}

