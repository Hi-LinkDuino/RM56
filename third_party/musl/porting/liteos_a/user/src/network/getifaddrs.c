#define _GNU_SOURCE
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <syscall.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include "netlink.h"
#include<stdio.h>

#define IFADDRS_HASH_SIZE 64
#define MAX_IF_NO 10

/* getifaddrs() reports hardware addresses with PF_PACKET that implies
 * struct sockaddr_ll.  But e.g. Infiniband socket address length is
 * longer than sockaddr_ll.ssl_addr[8] can hold. Use this hack struct
 * to extend ssl_addr - callers should be able to still use it. */
struct sockaddr_ll_hack {
	unsigned short sll_family, sll_protocol;
	int sll_ifindex;
	unsigned short sll_hatype;
	unsigned char sll_pkttype, sll_halen;
	unsigned char sll_addr[24];
};

union sockany {
	struct sockaddr sa;
	struct sockaddr_ll_hack ll;
	struct sockaddr_in v4;
	struct sockaddr_in6 v6;
};

struct ifaddrs_storage {
	struct ifaddrs ifa;
	struct ifaddrs_storage *hash_next;
	union sockany addr, netmask, ifu;
	unsigned int index;
	char name[IFNAMSIZ+1];
};

struct ifaddrs_ctx {
	struct ifaddrs_storage *first;
	struct ifaddrs_storage *last;
	struct ifaddrs_storage *hash[IFADDRS_HASH_SIZE];
};

void freeifaddrs(struct ifaddrs *ifp)
{
	struct ifaddrs *tmp = NULL;
	while (ifp) {
		if (!ifp->ifa_name)
			free(ifp);
		if (!ifp->ifa_addr)
			free(ifp->ifa_addr);
		if (!ifp->ifa_netmask)
			free(ifp->ifa_netmask);
		tmp = ifp->ifa_next;
		free(ifp);
		ifp = tmp;
	}
}

static void copy_addr(struct sockaddr **r, int af, union sockany *sa, void *addr, size_t addrlen, int ifindex)
{
	uint8_t *dst;
	int len;

	switch (af) {
	case AF_INET:
		dst = (uint8_t*) &sa->v4.sin_addr;
		len = 4;
		break;
	case AF_INET6:
		dst = (uint8_t*) &sa->v6.sin6_addr;
		len = 16;
		if (IN6_IS_ADDR_LINKLOCAL(addr) || IN6_IS_ADDR_MC_LINKLOCAL(addr))
			sa->v6.sin6_scope_id = ifindex;
		break;
	default:
		return;
	}
	if (addrlen < len) return;
	sa->sa.sa_family = af;
	memcpy(dst, addr, len);
	*r = &sa->sa;
}

static void gen_netmask(struct sockaddr **r, int af, union sockany *sa, int prefixlen)
{
	uint8_t addr[16] = {0};
	int i;

	if (prefixlen > 8*sizeof(addr)) prefixlen = 8*sizeof(addr);
	i = prefixlen / 8;
	memset(addr, 0xff, i);
	if (i < sizeof(addr)) addr[i++] = 0xff << (8 - (prefixlen % 8));
	copy_addr(r, af, sa, addr, sizeof(addr), 0);
}

static void copy_lladdr(struct sockaddr **r, union sockany *sa, void *addr, size_t addrlen, int ifindex, unsigned short hatype)
{
	if (addrlen > sizeof(sa->ll.sll_addr)) return;
	sa->ll.sll_family = AF_PACKET;
	sa->ll.sll_ifindex = ifindex;
	sa->ll.sll_hatype = hatype;
	sa->ll.sll_halen = addrlen;
	memcpy(sa->ll.sll_addr, addr, addrlen);
	*r = &sa->sa;
}

static int netlink_msg_to_ifaddr(void *pctx, struct nlmsghdr *h)
{
	struct ifaddrs_ctx *ctx = pctx;
	struct ifaddrs_storage *ifs, *ifs0;
	struct ifinfomsg *ifi = NLMSG_DATA(h);
	struct ifaddrmsg *ifa = NLMSG_DATA(h);
	struct rtattr *rta;
	int stats_len = 0;

	if (h->nlmsg_type == RTM_NEWLINK) {
		for (rta = NLMSG_RTA(h, sizeof(*ifi)); NLMSG_RTAOK(rta, h); rta = RTA_NEXT(rta)) {
			if (rta->rta_type != IFLA_STATS) continue;
			stats_len = RTA_DATALEN(rta);
			break;
		}
	} else {
		for (ifs0 = ctx->hash[ifa->ifa_index % IFADDRS_HASH_SIZE]; ifs0; ifs0 = ifs0->hash_next)
			if (ifs0->index == ifa->ifa_index)
				break;
		if (!ifs0) return 0;
	}

	ifs = calloc(1, sizeof(struct ifaddrs_storage) + stats_len);
	if (ifs == 0) return -1;

	if (h->nlmsg_type == RTM_NEWLINK) {
		ifs->index = ifi->ifi_index;
		ifs->ifa.ifa_flags = ifi->ifi_flags;

		for (rta = NLMSG_RTA(h, sizeof(*ifi)); NLMSG_RTAOK(rta, h); rta = RTA_NEXT(rta)) {
			switch (rta->rta_type) {
			case IFLA_IFNAME:
				if (RTA_DATALEN(rta) < sizeof(ifs->name)) {
					memcpy(ifs->name, RTA_DATA(rta), RTA_DATALEN(rta));
					ifs->ifa.ifa_name = ifs->name;
				}
				break;
			case IFLA_ADDRESS:
				copy_lladdr(&ifs->ifa.ifa_addr, &ifs->addr, RTA_DATA(rta), RTA_DATALEN(rta), ifi->ifi_index, ifi->ifi_type);
				break;
			case IFLA_BROADCAST:
				copy_lladdr(&ifs->ifa.ifa_broadaddr, &ifs->ifu, RTA_DATA(rta), RTA_DATALEN(rta), ifi->ifi_index, ifi->ifi_type);
				break;
			case IFLA_STATS:
				ifs->ifa.ifa_data = (void*)(ifs+1);
				memcpy(ifs->ifa.ifa_data, RTA_DATA(rta), RTA_DATALEN(rta));
				break;
			}
		}
		if (ifs->ifa.ifa_name) {
			unsigned int bucket = ifs->index % IFADDRS_HASH_SIZE;
			ifs->hash_next = ctx->hash[bucket];
			ctx->hash[bucket] = ifs;
		}
	} else {
		ifs->ifa.ifa_name = ifs0->ifa.ifa_name;
		ifs->ifa.ifa_flags = ifs0->ifa.ifa_flags;
		for (rta = NLMSG_RTA(h, sizeof(*ifa)); NLMSG_RTAOK(rta, h); rta = RTA_NEXT(rta)) {
			switch (rta->rta_type) {
			case IFA_ADDRESS:
				/* If ifa_addr is already set we, received an IFA_LOCAL before
				 * so treat this as destination address */
				if (ifs->ifa.ifa_addr)
					copy_addr(&ifs->ifa.ifa_dstaddr, ifa->ifa_family, &ifs->ifu, RTA_DATA(rta), RTA_DATALEN(rta), ifa->ifa_index);
				else
					copy_addr(&ifs->ifa.ifa_addr, ifa->ifa_family, &ifs->addr, RTA_DATA(rta), RTA_DATALEN(rta), ifa->ifa_index);
				break;
			case IFA_BROADCAST:
				copy_addr(&ifs->ifa.ifa_broadaddr, ifa->ifa_family, &ifs->ifu, RTA_DATA(rta), RTA_DATALEN(rta), ifa->ifa_index);
				break;
			case IFA_LOCAL:
				/* If ifa_addr is set and we get IFA_LOCAL, assume we have
				 * a point-to-point network. Move address to correct field. */
				if (ifs->ifa.ifa_addr) {
					ifs->ifu = ifs->addr;
					ifs->ifa.ifa_dstaddr = &ifs->ifu.sa;
					memset(&ifs->addr, 0, sizeof(ifs->addr));
				}
				copy_addr(&ifs->ifa.ifa_addr, ifa->ifa_family, &ifs->addr, RTA_DATA(rta), RTA_DATALEN(rta), ifa->ifa_index);
				break;
			case IFA_LABEL:
				if (RTA_DATALEN(rta) < sizeof(ifs->name)) {
					memcpy(ifs->name, RTA_DATA(rta), RTA_DATALEN(rta));
					ifs->ifa.ifa_name = ifs->name;
				}
				break;
			}
		}
		if (ifs->ifa.ifa_addr)
			gen_netmask(&ifs->ifa.ifa_netmask, ifa->ifa_family, &ifs->netmask, ifa->ifa_prefixlen);
	}

	if (ifs->ifa.ifa_name) {
		if (!ctx->first) ctx->first = ifs;
		if (ctx->last) ctx->last->ifa.ifa_next = &ifs->ifa;
		ctx->last = ifs;
	} else {
		free(ifs);
	}
	return 0;
}


static struct ifaddrs* ifaddrs_init()
{
	struct ifaddrs *ifa = NULL;
	ifa = malloc(sizeof(struct ifaddrs));
	if (!ifa)
		return NULL;
	ifa->ifa_name = malloc(sizeof(char) *(IF_NAMESIZE + 1));
	ifa->ifa_addr = malloc(sizeof(struct sockaddr));
	ifa->ifa_netmask = malloc(sizeof(struct sockaddr));
	ifa->ifa_next = NULL;
	if (!ifa->ifa_name || !ifa->ifa_addr || !ifa->ifa_netmask) {
		free(ifa->ifa_name);
		free(ifa->ifa_addr);
		free(ifa->ifa_netmask);
		free(ifa);
		return NULL;
	}
	return ifa;
}

int getifaddrs(struct ifaddrs **ifap)
{
	if (ifap == NULL) {
		errno = EINVAL;
		return -1;
	}
	
	int fd, ifno, ret;
	struct ifreq ifr[MAX_IF_NO];
	struct ifconf ifconfig;
	struct ifaddrs *ifstart = NULL;

	ifconfig.ifc_buf = ifr;
	ifconfig.ifc_len = sizeof(ifr);
	if ((fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0)) < 0) 
		return -1;
	if (ioctl(fd, SIOCGIFCONF, &ifconfig) < 0)
		goto error;
	if (ifconfig.ifc_len % sizeof(struct ifreq)) {
		errno = EINVAL;
		goto error;
	}
	ifno = ifconfig.ifc_len / sizeof(struct ifreq);

	if (!(ifstart = ifaddrs_init())) {
		errno = ENOMEM;
		goto error;
	}
	struct ifaddrs *ifa = ifstart;

	for (int i = 0; i < ifno; i++) {
		memcpy(ifa->ifa_name, ifr[i].ifr_name, IF_NAMESIZE);
		ifa->ifa_name[IF_NAMESIZE] = '\0';
		memcpy(ifa->ifa_addr, &ifr[i].ifr_addr, sizeof(struct sockaddr));
		if (ioctl(fd, SIOCGIFNETMASK, &ifr[i]) < 0)
			goto error;
		memcpy(ifa->ifa_netmask, &ifr[i].ifr_netmask, sizeof(struct sockaddr));

		if (ioctl(fd, SIOCGIFFLAGS, &ifr[i]) < 0) 
			goto error;
		ifa->ifa_flags = ifr[i].ifr_flags;

		if (i < ifno - 1) {
			ifa->ifa_next = ifaddrs_init();
			if (!ifa->ifa_next)	{
				errno = ENOMEM;
				goto error;
			}
			ifa = ifa->ifa_next;
		}
	}

	*ifap = ifstart;
	return 0;

error:	
	freeifaddrs(ifstart);
	__syscall(SYS_close, fd);
	return -1;
}

