#include <stdlib.h>
#include <netinet/ether.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define ETHER_FILE_PATH "/etc/ethers"
#define BUFFER_SIZE     200

struct ether_addr *ether_aton_r (const char *x, struct ether_addr *p_a)
{
	struct ether_addr a;
	char *y;
	for (int ii = 0; ii < 6; ii++) {
		unsigned long int n;
		if (ii != 0) {
			if (x[0] != ':') return 0; /* bad format */
			else x++;
		}
		n = strtoul (x, &y, 16);
		x = y;
		if (n > 0xFF) return 0; /* bad byte */
		a.ether_addr_octet[ii] = n;
	}
	if (x[0] != 0) return 0; /* bad format */
	*p_a = a;
	return p_a;
}

struct ether_addr *ether_aton (const char *x)
{
	static struct ether_addr a;
	return ether_aton_r (x, &a);
}

char *ether_ntoa_r (const struct ether_addr *p_a, char *x) {
	char *y;
	y = x;
	for (int ii = 0; ii < 6; ii++) {
		x += sprintf (x, ii == 0 ? "%.2X" : ":%.2X", p_a->ether_addr_octet[ii]);
	}
	return y;
}

char *ether_ntoa (const struct ether_addr *p_a) {
	static char x[18];
	return ether_ntoa_r (p_a, x);
}

int ether_line(const char *line, struct ether_addr *addr, char *hostname)
{
	char buf[BUFFER_SIZE], *ch, *ptr = NULL;
	if (line[0] == '#') {
		errno = EINVAL;
		return -1;
	}
	strcpy(buf, line);
	ch = strchr(buf, '#');
	if (ch)
		*ch = '\0';
	ch = strtok_r(buf, " \t\n", &ptr);
	ch = strtok_r(NULL, " \t\n", &ptr);
	if (ch == NULL) {
		errno = EINVAL;
		return -1;
	}
	strcpy(hostname, ch);

	unsigned int ret;
	ch = &buf[0];
	for (int i = 0; i < 6; i++) {
		ch = strtok_r(ch, ":", &ptr);
		if (ch == NULL || sscanf(ch, "%x", &ret) != 1) {
			errno = EINVAL;
			return -1;
		}
		addr->ether_addr_octet[i] = (uint8_t)ret;
		ch = NULL;
	}

	return 0;     
}

int ether_ntohost(char *hostname, const struct ether_addr *addr)
{   
	FILE *f = NULL;
	char buf[BUFFER_SIZE + 1];
	char temp_name[BUFFER_SIZE];
	struct ether_addr *temp_addr;
    
	if (!(f = fopen(ETHER_FILE_PATH, "r")))
		return ENOENT;
	temp_addr = (struct ether_addr *)malloc(sizeof(struct ether_addr));
	if (temp_addr == NULL) {
		fclose(f);
		return ENOMEM;
	}
	while (fgets(buf, BUFFER_SIZE, f)) {
		if (ether_line(buf, temp_addr, temp_name) != 0)
			continue;
		if (memcmp(addr, temp_addr, 6) == 0) {
			strcpy(hostname, temp_name);
			free(temp_addr);
			fclose(f);
			return 0; 
		}
	}

	free(temp_addr);
	fclose(f);
	return -1;
}


int ether_hostton(const char *hostname, struct ether_addr *addr)
{
	FILE *f = NULL;
	char buf[BUFFER_SIZE + 1];
	char temp_name[BUFFER_SIZE];
	struct ether_addr *temp_addr;
	
	temp_addr = (struct ether_addr *)malloc(sizeof(struct ether_addr));
	if (temp_addr == NULL)
		return ENOMEM;
	if (!(f = fopen(ETHER_FILE_PATH, "r"))) {
		free(temp_addr);
		return ENOENT;
	}
	while (fgets(buf, BUFFER_SIZE, f)) {
		if (ether_line(buf, temp_addr, temp_name) != 0)
			continue;
		if (strcmp(hostname, temp_name) == 0) {
			memcpy(addr, temp_addr, 6);
			free(temp_addr);
			fclose(f);
			return 0; 
		}
	}
    
	free(temp_addr);
	fclose(f);
	return -1;
}
