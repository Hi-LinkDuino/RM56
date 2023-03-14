#ifndef __MBEDTLS_SOCKETS_PORT_H__
#define __MBEDTLS_SOCKETS_PORT_H__

#include "lwip/opt.h"

#if LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */

#include <stddef.h> /* for size_t */

#include "lwip/ip_addr.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"


#ifdef __cplusplus
extern "C" {
#endif

#if LWIP_COMPAT_SOCKETS != 2
#include "lwip/sockets.h"
#else
#define accept(a,b,c)         lwip_accept(a,b,(socklen_t *)(c))
#define bind(a,b,c)           lwip_bind(a,b,c)
#define shutdown(a,b)         lwip_shutdown(a,b)
#define closesocket(s)        lwip_close(s)
#define connect(a,b,c)        lwip_connect(a,b,c)
#define getsockname(a,b,c)    lwip_getsockname(a,b,(socklen_t *)(c))
#define getpeername(a,b,c)    lwip_getpeername(a,b,c)
#define setsockopt(a,b,c,d,e) lwip_setsockopt(a,b,c,d,e)
#define getsockopt(a,b,c,d,e) lwip_getsockopt(a,b,c,d,(socklen_t *)(e))
#define listen(a,b)           lwip_listen(a,b)
#define recv(a,b,c,d)         lwip_recv(a,b,c,d)
#define recvfrom(a,b,c,d,e,f) lwip_recvfrom(a,b,c,d,e,(socklen_t *)(f))
#define send(a,b,c,d)         lwip_send(a,b,c,d)
#define sendto(a,b,c,d,e,f)   lwip_sendto(a,b,c,d,e,f)
#define socket(a,b,c)         lwip_socket(a,b,c)
#define select(a,b,c,d,e)     lwip_select(a,b,c,d,e)
#define ioctlsocket(a,b,c)    lwip_ioctl(a,b,c)
#define read(a,b,c)           lwip_read(a,b,c)
#define write(a,b,c)          lwip_write(a,b,c)
#define close(s)              lwip_close(s)
#endif

#define __socklen_t_defined 1
#if !defined(sa_family_t) && !defined(SA_FAMILY_T_DEFINED)
typedef u8_t sa_family_t;
#endif

#if !defined(in_port_t) && !defined(IN_PORT_T_DEFINED)
typedef u16_t in_port_t;
#endif
/*
struct in6_addr {
  union {
    u32_t u32_addr[4];
    u8_t  u8_addr[16];
  } un;
#define s6_addr  un.u8_addr
};
*/
#ifndef AI_PASSIVE
#define AI_PASSIVE      0x01
#endif

#ifndef LWIP_COMPAT_SOCKETS
#define gethostbyname(name)   lwip_gethostbyname(name)
#endif
/*
struct sockaddr_storage {
  u8_t        s2_len;
  sa_family_t ss_family;
  char        s2_data1[2];
  u32_t       s2_data2[3];
};
*/
#if !LWIP_IPV6_SUPPORT
struct sockaddr_in6 {
  u8_t            sin6_len;      /* length of this structure    */
  sa_family_t     sin6_family;   /* AF_INET6                    */
  in_port_t       sin6_port;     /* Transport layer port #      */
  u32_t           sin6_flowinfo; /* IPv6 flow information       */
  struct in6_addr sin6_addr;     /* IPv6 address                */
  u32_t           sin6_scope_id; /* Set of interfaces for scope */
};
#endif

#ifdef __cplusplus
}
#endif

#endif /* LWIP_SOCKET */

#endif /* __MBEDTLS_SOCKETS_PORT_H__ */

