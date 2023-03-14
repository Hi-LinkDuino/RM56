#define _BSD_SOURCE
#include<netdb.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define SERV_FILE_PATH  "/etc/services"
#define BUFFER_SIZE     200
#define MAX_PORT        65535
#define MAX_ALIAS_NO    10

struct servdata
{
	FILE *servf;
	int stayopen;
	struct servent se;
	char buf[BUFFER_SIZE];
	char *alias_list[MAX_ALIAS_NO + 1];
};
static struct servdata *sd = NULL;

static int servent_resolv()
{
	if (sd->buf[0] == '#')
		return -1;
	char *ch, *temp, *ptr = NULL;
	ch = strchr(sd->buf, '#');
	if (ch)
		*ch = '\0';
	ch = strtok_r(sd->buf, " \t\n", &ptr);
	temp = strtok_r(NULL, " \t\n", &ptr);
	if (!ch || !temp)
		return -1;
		
	sd->se.s_name = ch;
	ch = strchr(temp, '/');
	if (ch == NULL)
		return -1;
	*ch++ = '\0'; 
	sd->se.s_port = atoi(temp);
	if (sd->se.s_port < 1 || sd->se.s_port > MAX_PORT)
		return -1;
	sd->se.s_port = htons(sd->se.s_port);
	if (strcmp(ch, "tcp") != 0 && strcmp(ch, "udp") != 0)
		return -1;
	sd->se.s_proto = ch;

	int i = 0;
	while (i < MAX_ALIAS_NO) {
		ch = strtok_r(NULL, " \t\n", &ptr);
		sd->alias_list[i++] = ch;
	}
	sd->alias_list[MAX_ALIAS_NO] = NULL;
	sd->se.s_aliases = sd->alias_list;
	return 0;
}

struct servent *getservent(void)
{
	if (sd == NULL) {
		if ((sd = malloc(sizeof(struct servdata))) == NULL) {
			errno = ENOMEM;
			return NULL;
		}
		sd->servf = NULL;
		sd->stayopen = 0;
	}
	if (!sd->servf && !(sd->servf = fopen(SERV_FILE_PATH, "r"))) {
		errno = ENOENT;
		free(sd);
		sd = NULL;
		return NULL;
	}
	do {
		if (!fgets(sd->buf, BUFFER_SIZE - 1, sd->servf)) {
			errno = EINVAL;
			return NULL;
		}
	} while (servent_resolv());
	return &(sd->se);
}

void endservent(void)
{
	if (sd == NULL)
		return;
	if (sd->stayopen == 0) {
		if (sd->servf != NULL)
			fclose(sd->servf);
		free(sd);
		sd = NULL;
	}
}

void setservent(int stayopen)
{
	if (sd == NULL) {
		if ((sd = malloc(sizeof(struct servdata))) == NULL) {
			errno = ENOMEM;
			return;
		}
		sd->servf = NULL;
	}
	if (!sd->servf)
		sd->servf = fopen(SERV_FILE_PATH, "r");
	else
		rewind(sd->servf);
	sd->stayopen = stayopen;    
}