#ifndef __NET_SOCKET_H__
#define __NET_SOCKET_H__
#ifndef NUTTX_BUILD
typedef unsigned short sa_family_t;

/*
 *	1003.1g requires sa_family_t and that sa_data is char.
 */

struct sockaddr {
	sa_family_t	sa_family;	/* address family, AF_xxx	*/
	unsigned char	sa_data[14];	/* 14 bytes of protocol address	*/
};
#endif
#endif
