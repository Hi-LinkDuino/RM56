#define _BSD_SOURCE
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<errno.h>

#define HOST_FILE_PATH "/etc/hosts"
#define NET_FILE_PATH  "/etc/networks"
#define BUFFER_SIZE    200
#define INET_ADDR_LEN  4
#define INET6_ADDR_LEN 16
#define MAX_ALIAS_NO   10
#define MAX_ADDR_NO    10
#define MAX_NAME_LEN   20
#define MAX_ALIAS_LEN  20

struct hostdata {
	FILE *hostf;
	int stayopen;
	struct hostent he;
	char buf[BUFFER_SIZE];
	char *addr[MAX_ADDR_NO + 1];
	char addr_list[MAX_ADDR_NO + 1][INET6_ADDR_LEN + 1];
	char *aliases[MAX_ALIAS_NO + 1];
};

struct netdata {
	FILE *netf;
	int stayopen;
	struct netent ne;
	char buf[BUFFER_SIZE];
	char *aliases[MAX_ALIAS_NO + 1];
};

static struct hostdata *hd = NULL;
static struct netdata  *nd = NULL;

static int gethostent_resolv()
{
	char *ch, *ptr = NULL;
	if (hd->buf[0] == '#')
		return -1;

	ch = strchr(hd->buf, '#');
	if (ch) 
		*ch = '\0';
	ch = strtok_r(hd->buf, " \t\n", &ptr);
	if (!ch)
		return -1;

	if (inet_pton(AF_INET, ch, hd->addr_list[0]) == 1) {
		hd->he.h_addrtype = AF_INET;
		hd->he.h_length = INET_ADDR_LEN;
		hd->addr_list[0][INET_ADDR_LEN] = '\0';
	} else if (inet_pton(AF_INET6, ch, hd->addr_list[0]) == 1) {
		hd->he.h_addrtype = AF_INET6;
		hd->he.h_length = INET6_ADDR_LEN;
		hd->addr_list[0][INET6_ADDR_LEN] = '\0';
	} else {
		return -1;
	}

	hd->addr[0] = hd->addr_list[0];
	hd->addr[1] = NULL;
	hd->he.h_addr_list = hd->addr;

	ch = strtok_r(NULL, " \t\n", &ptr);
	if (!ch)
		return -1;
	hd->he.h_name = ch;

	int i = 0;
	while (i < MAX_ALIAS_NO) {
		ch = strtok_r(NULL, " \t\n", &ptr);
		hd->aliases[i++] = ch;
	}

	hd->aliases[MAX_ALIAS_NO] = NULL;
	hd->he.h_aliases = hd->aliases;
	return 0;
}

struct hostent *gethostent(void)
{
	if (hd == NULL) {
		if ((hd = malloc(sizeof(struct hostdata))) == NULL) {
			h_errno = NO_RECOVERY;
			return NULL;
		}
		hd->hostf = NULL;
		hd->stayopen = 0;
	}
	if (!hd->hostf && !(hd->hostf = fopen(HOST_FILE_PATH, "r"))) {
		h_errno = NO_RECOVERY;
		free(hd);
		hd =NULL;
		return NULL;
	}
	do {
		if (!fgets(hd->buf, BUFFER_SIZE, hd->hostf)) {
			h_errno = HOST_NOT_FOUND;
			return NULL;
		}
	} while (gethostent_resolv());

	return &(hd->he);
}

void sethostent(int stayopen)
{
	if (hd == NULL) {
		if ((hd = malloc(sizeof(struct hostdata))) == NULL) {
			h_errno = NO_RECOVERY;
			return;
		}
		hd->hostf = NULL;
	}
	if (hd->hostf == NULL)
		hd->hostf = fopen(NET_FILE_PATH, "r");
	else
		rewind(hd->hostf);
	hd->stayopen = stayopen;
}

void endhostent(void)
{
	if (hd == NULL)
		return;
	if (hd->stayopen == 0) {
		if (hd->hostf != NULL)
			fclose(hd->hostf);
		free(hd);
		hd = NULL;
	}
}

static int getnetent_resolv()
{
	if (nd->buf[0] == '#')
		return -1;
	char *ch, *ptr = NULL;
	ch = strchr(nd->buf, '#');
	if (ch) 
		*ch = '\0';
	ch = strtok_r(nd->buf, " \t\n", &ptr);
	if (!ch)
		return -1;
	nd->ne.n_name = ch;
	nd->ne.n_addrtype = AF_INET;

	ch = strtok_r(NULL, " \t\n", &ptr);
	if (!ch) 
		return -1;
	nd->ne.n_net = inet_network(ch);	

	int i = 0;
	while ((ch = strtok_r(NULL, " \t\n", &ptr)) != NULL && i < MAX_ALIAS_NO) {
		nd->aliases[i++] = ch;
	}
	nd->aliases[i] = NULL;	
	nd->ne.n_aliases = nd->aliases;
	return 0;
}

struct netent *getnetent(void)
{
	if (nd == NULL) {
		if ((nd = malloc(sizeof(struct netdata))) == NULL) {
			errno = ENOMEM;
			return NULL;
		}
		nd->netf = NULL;
		nd->stayopen = 0;
	}
	if (nd->netf == NULL && (nd->netf = fopen(NET_FILE_PATH, "r")) == NULL) {
		errno = ENOENT;
		free(nd);
		nd = NULL;
		return NULL;
	}
	while (1) { 
		if (fgets(nd->buf, BUFFER_SIZE, nd->netf) == NULL)
			break;
		if (getnetent_resolv() == 0)
			return &(nd->ne);
	}

	return NULL;
}

void setnetent(int stayopen)
{
	if (nd == NULL) {
		if ((nd = malloc(sizeof(struct netdata))) == NULL) {
			errno = ENOMEM;
			return;
		}
		nd->netf = NULL;
	}
	if (nd->netf == NULL)
		nd->netf = fopen(NET_FILE_PATH, "r");
	else
		rewind(nd->netf);
	nd->stayopen = stayopen;
}

void endnetent(void)
{
	if (nd == NULL)
		return;
	if (!nd->stayopen) {
		if (nd->netf != NULL)
			fclose(nd->netf);
		free(nd);
		nd = NULL;
	}
}