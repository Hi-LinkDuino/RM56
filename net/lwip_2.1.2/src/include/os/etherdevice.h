/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  NET  is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Definitions for the Ethernet handlers.
 *
 * Version:	@(#)eth.h	1.0.4	05/13/93
 *
 * Authors:	Ross Biro
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *
 *		Relocated to include/linux where it belongs by Alan Cox
 *							<gw4pts@gw4pts.ampr.org>
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 */
#ifndef _LINUX_ETHERDEVICE_H
#define _LINUX_ETHERDEVICE_H

#include "if_ether.h"
#include "unaligned.h"
#include "netdevice.h"
#include "security/random.h"

#define IP_KEEPALIVE_MAX_LEN  128
#define NUM_IP_FRAMES         8
#define TCP_PROTO             6
#define UDP_PROTO             17

struct ip_header {
	/* version / header length */
	uint8_t _v_hl;
	/* type of service */
	uint8_t _tos;
	/* total length */
	uint16_t _len;
	/* identification */
	uint16_t _id;
	/* fragment offset field */
	uint16_t _offset;
	/* time to live */
	uint8_t _ttl;
	/* protocol*/
	uint8_t _proto;
	/* checksum */
	uint16_t _chksum;
	/* source and destination IP addresses */
	uint32_t src;
	uint32_t dest;
} ;

struct tcp_header {
	uint16_t src;
	uint16_t dest;
	uint32_t seqno;
	uint32_t ackno;
	uint16_t _hdrlen_rsvd_flags;
	uint16_t wnd;
	uint16_t chksum;
	uint16_t urgp;
};

struct udp_header {
	u16_t src;
	u16_t dest;
	u16_t len;
	u16_t chksum;
};

struct ip_alive_info {
	uint8_t idx_used;
	uint8_t proto;/* 0 for udp and 1 for tcp; */
	uint16_t dest_port;
	uint16_t len;
	uint16_t next_seqno;
	uint8_t payload[IP_KEEPALIVE_MAX_LEN];
	uint8_t dest_mac[6];
};

struct ip_alive_cfg {
	struct ip_header iphd;
	struct tcp_header tcphd;
	struct udp_header udphd;
	struct ip_alive_info bd;
};

extern void ether_setup(struct net_device *dev);
extern __be16 eth_type_trans(struct sk_buff *skb, struct net_device *dev);

/**
 * is_zero_ether_addr - Determine if give Ethernet address is all zeros.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is all zeroes.
 */
static inline int is_zero_ether_addr(const u8 *addr)
{
	return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}


/**
 * is_multicast_ether_addr - Determine if the Ethernet address is a multicast.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is a multicast address.
 * By definition the broadcast address is also a multicast address.
 */
static inline bool is_multicast_ether_addr(const u8 *addr)
{
	return 0x01 & addr[0];
}

/**
 * is_local_ether_addr - Determine if the Ethernet address is locally-assigned one (IEEE 802).
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is a local address.
 */
static inline bool is_local_ether_addr(const u8 *addr)
{
	return 0x02 & addr[0];
}

/**
 * net_is_broadcast_ether_addr - Determine if the Ethernet address is broadcast
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is the broadcast address.
 *
 * Please note: addr must be aligned to u16.
 */
static inline bool is_broadcast_ether_addr(const u8 *addr)
{
	return (*(const u16 *)(addr + 0) &
		*(const u16 *)(addr + 2) &
		*(const u16 *)(addr + 4)) == 0xffff;
}

/**
 * is_unicast_ether_addr - Determine if the Ethernet address is unicast
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is a unicast address.
 */
static inline bool is_unicast_ether_addr(const u8 *addr)
{
	return !is_multicast_ether_addr(addr);
}

/**
 * is_valid_ether_addr - Determine if the given Ethernet address is valid
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Check that the Ethernet address (MAC) is not 00:00:00:00:00:00, is not
 * a multicast address, and is not FF:FF:FF:FF:FF:FF.
 *
 * Return true if the address is valid.
 *
 * Please note: addr must be aligned to u16.
 */
static inline bool is_valid_ether_addr(const u8 *addr)
{
	/* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
	 * explicitly check for it here. */
	return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}

/**
 * eth_random_addr - Generate software assigned random Ethernet address
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Generate a random Ethernet address (MAC) that is not multicast
 * and has the local assigned bit set.
 */
static inline void eth_random_addr(u8 *addr)
{
	get_random_bytes(addr, ETH_ALEN);
	addr[0] &= 0xfe;	/* clear multicast bit */
	addr[0] |= 0x02;	/* set local assignment bit (IEEE802) */
}

#define random_ether_addr(addr) eth_random_addr(addr)

/**
 * eth_broadcast_addr - Assign broadcast address
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Assign the broadcast address to the given address array.
 */
static inline void eth_broadcast_addr(u8 *addr)
{
	memset(addr, 0xff, ETH_ALEN);
}

/**
 * eth_zero_addr - Assign zero address
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Assign the zero address to the given address array.
 */
static inline void eth_zero_addr(u8 *addr)
{
	memset(addr, 0x00, ETH_ALEN);
}

/**
 * eth_hw_addr_random - Generate software assigned random Ethernet and
 * set device flag
 * @dev: pointer to net_device structure
 *
 * Generate a random Ethernet address (MAC) to be used by a net device
 * and set addr_assign_type so the state can be read by sysfs and be
 * used by userspace.
 */
static inline void eth_hw_addr_random(struct net_device *dev)
{
	eth_random_addr(&dev->dev_addr[0]);
}


/**
 * compare_ether_addr - Compare two Ethernet addresses
 * @addr1: Pointer to a six-byte array containing the Ethernet address
 * @addr2: Pointer other six-byte array containing the Ethernet address
 *
 * Compare two ethernet addresses, returns 0 if equal, non-zero otherwise.
 * Unlike memcmp(), it doesn't return a value suitable for sorting.
 */
static inline unsigned compare_ether_addr(const u8 *addr1, const u8 *addr2)
{
	return memcmp(addr1, addr2, ETH_ALEN);
}


/**
 * ether_addr_copy - Copy an Ethernet address
 * @dst: Pointer to a six-byte array Ethernet address destination
 * @src: Pointer to a six-byte array Ethernet address source
 *
 * Please note: dst & src must both be aligned to u16.
 */
static inline void ether_addr_copy(u8 *dst, const u8 *src)
{
	memcpy(dst, src, ETH_ALEN);
}

/**
 * eth_hw_addr_inherit - Copy dev_addr from another net_device
 * @dst: pointer to net_device to copy dev_addr to
 * @src: pointer to net_device to copy dev_addr from
 *
 * Copy the Ethernet address from one net_device to another along with
 * the address attributes (addr_assign_type).
 */
static inline void eth_hw_addr_inherit(struct net_device *dst,
				       struct net_device *src)
{
	ether_addr_copy(&dst->dev_addr[0], &src->dev_addr[0]);
}

/**
 * ether_addr_equal - Compare two Ethernet addresses
 * @addr1: Pointer to a six-byte array containing the Ethernet address
 * @addr2: Pointer other six-byte array containing the Ethernet address
 *
 * Compare two Ethernet addresses, returns true if equal
 *
 * Please note: addr1 & addr2 must both be aligned to u16.
 */
static inline bool ether_addr_equal(const u8 *addr1, const u8 *addr2)
{
	return (memcmp(addr1, addr2, ETH_ALEN) == 0);
}

/**
 * ether_addr_equal_64bits - Compare two Ethernet addresses
 * @addr1: Pointer to an array of 8 bytes
 * @addr2: Pointer to an other array of 8 bytes
 *
 * Compare two Ethernet addresses, returns true if equal, false otherwise.
 *
 * The function doesn't need any conditional branches and possibly uses
 * word memory accesses on CPU allowing cheap unaligned memory reads.
 * arrays = { byte1, byte2, byte3, byte4, byte5, byte6, pad1, pad2 }
 *
 * Please note that alignment of addr1 & addr2 are only guaranteed to be 16 bits.
 */

static inline bool ether_addr_equal_64bits(const u8 addr1[6 + 2],
					   const u8 addr2[6 + 2])
{
	return ether_addr_equal(addr1, addr2);
}

/**
 * compare_ether_addr_64bits - Compare two Ethernet addresses
 * @addr1: Pointer to an array of 8 bytes
 * @addr2: Pointer to an other array of 8 bytes
 *
 * Compare two ethernet addresses, returns 0 if equal, non-zero otherwise.
 * Unlike memcmp(), it doesn't return a value suitable for sorting.
 * The function doesn't need any conditional branches and possibly uses
 * word memory accesses on CPU allowing cheap unaligned memory reads.
 * arrays = { byte1, byte2, byte3, byte4, byte6, byte7, pad1, pad2}
 *
 * Please note that alignment of addr1 & addr2 is only guaranted to be 16 bits.
 */

static inline unsigned compare_ether_addr_64bits(const u8 addr1[6 + 2],
						 const u8 addr2[6 + 2])
{
	return compare_ether_addr(addr1, addr2);
}

/**
 * ether_addr_equal_unaligned - Compare two not u16 aligned Ethernet addresses
 * @addr1: Pointer to a six-byte array containing the Ethernet address
 * @addr2: Pointer other six-byte array containing the Ethernet address
 *
 * Compare two Ethernet addresses, returns true if equal
 *
 * Please note: Use only when any Ethernet address may not be u16 aligned.
 */
static inline bool ether_addr_equal_unaligned(const u8 *addr1, const u8 *addr2)
{
	return memcmp(addr1, addr2, ETH_ALEN) == 0;
}

#endif	/* _LINUX_ETHERDEVICE_H */
