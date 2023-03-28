/*
 *	Definitions for the 'struct sk_buff' memory handlers.
 *
 *	Authors:
 *		Alan Cox, <gw4pts@gw4pts.ampr.org>
 *		Florian La Roche, <rzsfl@rz.uni-sb.de>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#ifndef _LINUX_SKBUFF_H
#define _LINUX_SKBUFF_H

#include "net_defs.h"
#include "net_atomic.h"
#include "net_mutex.h"
#include "net_memory.h"
#include "net_debug.h"
#include "s_memory.h"
#define RX_FLOWCTRL               (1)

#define BES_HEAD_SIZE             (64)

#define SKB_HEAD_RESERVE          (88)
#define SKB_TAIL_RESERVE          (18)
#define SKB_TX_DATA_SIZE          (1536)
#define TX_MEM_HIGH_THROSHOLD     ((SKB_TX_DATA_SIZE + SKB_HEAD_RESERVE + SKB_TAIL_RESERVE) * 16)
#define TX_MEM_LOW_THROSHOLD      (24*1024)

#if(SKB_SRAM == 2)
#define RX_MEM_THROSHOLD (11000)
#elif(SKB_SRAM == 1)
#define RX_MEM_THROSHOLD (58000)
#else
#define RX_MEM_THROSHOLD (16*1024)
#endif

#if(SKB_SRAM == 2)
#define RX_FLOWCTRL_WATERLINE_HIGH (10500)
#define RX_FLOWCTRL_WATERLINE_LOW  (5500)
#elif(SKB_SRAM == 1)
#define RX_FLOWCTRL_WATERLINE_HIGH (0xffffff)
#define RX_FLOWCTRL_WATERLINE_LOW  (0xffffff)
#else
#define RX_FLOWCTRL_WATERLINE_HIGH (0xffffff)
#define RX_FLOWCTRL_WATERLINE_LOW  (0xffffff)
#endif

/* Don't change this without changing skb_csum_unnecessary! */
#define CHECKSUM_NONE 0
#define CHECKSUM_UNNECESSARY 1
#define CHECKSUM_COMPLETE 2
#define CHECKSUM_PARTIAL 3

#define SKB_DATA_ALIGN(X)	(((X) + (4 - 1)) & \
				 ~(4 - 1))

#define SKB_WITH_OVERHEAD(X)	\
	((X) - SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))

struct page {
	unsigned long flags;		/* Atomic flags, some possibly*/
	atomic32_t _count;		/* Usage count, see below. */
	char  *addr;
};

#define IPSKB_FORWARDED		1
#define IPSKB_XFRM_TUNNEL_SIZE	2
#define IPSKB_XFRM_TRANSFORMED	4
#define IPSKB_FRAG_COMPLETE	8
#define IPSKB_REROUTED		16

#define SO_EE_ORIGIN_NONE	0
#define SO_EE_ORIGIN_LOCAL	1
#define SO_EE_ORIGIN_ICMP	2
#define SO_EE_ORIGIN_ICMP6	3
#define SO_EE_ORIGIN_TIMESTAMPING 4

struct scatterlist {
	unsigned long	page_link;
	unsigned int	offset;
	unsigned int	length;
	//dma_addr_t	dma_address;
};

#define sg_dma_address(sg)	((sg)->dma_address)

#ifdef CONFIG_NEED_SG_DMA_LENGTH
#define sg_dma_len(sg)		((sg)->dma_length)
#else
#define sg_dma_len(sg)		((sg)->length)
#endif


#if SKB_SRAM
#define kmem_cache_free(x,y) s_kfree(y)
#else
#define kmem_cache_free(x,y) kfree(y)
#endif

#define NUMA_NO_NODE 1

#define NET_SKB_PAD	   32

#define SKB_DST_NOREF	1UL
#define SKB_DST_PTRMASK	~(SKB_DST_NOREF)

/* We divide dataref into two halves.  The higher 16 bits hold references
 * to the payload part of skb->data.  The lower 16 bits hold references to
 * the entire skb->data.  A clone of a headerless skb holds the length of
 * the header in skb->hdr_len.
 *
 * All users must obey the rule that the skb->data reference count must be
 * greater than or equal to the payload reference count.
 *
 * Holding a reference to the payload part means that the user does not
 * care about modifications to the header part of skb->data.
 */
#define SKB_DATAREF_SHIFT 16
#define SKB_DATAREF_MASK ((1 << SKB_DATAREF_SHIFT) - 1)

/* Sock flags */
enum sock_flags {
	SOCK_DEAD,
	SOCK_DONE,
	SOCK_URGINLINE,
	SOCK_KEEPOPEN,
	SOCK_LINGER,
	SOCK_DESTROY,
	SOCK_BROADCAST,
	SOCK_TIMESTAMP,
	SOCK_ZAPPED,
	SOCK_USE_WRITE_QUEUE, /* whether to call sk->sk_write_space in sock_wfree */
	SOCK_DBG, /* %SO_DEBUG setting */
	SOCK_RCVTSTAMP, /* %SO_TIMESTAMP setting */
	SOCK_RCVTSTAMPNS, /* %SO_TIMESTAMPNS setting */
	SOCK_LOCALROUTE, /* route locally only, %SO_DONTROUTE setting */
	SOCK_QUEUE_SHRUNK, /* write queue has been shrunk recently */
	SOCK_TIMESTAMPING_TX_HARDWARE,  /* %SOF_TIMESTAMPING_TX_HARDWARE */
	SOCK_TIMESTAMPING_TX_SOFTWARE,  /* %SOF_TIMESTAMPING_TX_SOFTWARE */
	SOCK_TIMESTAMPING_RX_HARDWARE,  /* %SOF_TIMESTAMPING_RX_HARDWARE */
	SOCK_TIMESTAMPING_RX_SOFTWARE,  /* %SOF_TIMESTAMPING_RX_SOFTWARE */
	SOCK_TIMESTAMPING_SOFTWARE,     /* %SOF_TIMESTAMPING_SOFTWARE */
	SOCK_TIMESTAMPING_RAW_HARDWARE, /* %SOF_TIMESTAMPING_RAW_HARDWARE */
	SOCK_TIMESTAMPING_SYS_HARDWARE, /* %SOF_TIMESTAMPING_SYS_HARDWARE */
	SOCK_FASYNC, /* fasync() active */
	SOCK_RXQ_OVFL,
};

struct sock_extended_err {
	__u32	ee_errno;
	__u8	ee_origin;
	__u8	ee_type;
	__u8	ee_code;
	__u8	ee_pad;
	__u32   ee_info;
	__u32   ee_data;
};
struct sock_exterr_skb {
	/*
	union {
		struct inet_skb_parm	h4;

	} header;
	*/
	struct sock_extended_err	ee;
	u16				addr_offset;
	__be16				port;
};

#define SKB_EXT_ERR(skb) ((struct sock_exterr_skb *) ((skb)->cb))

#define atomic_inc(v)		atomic_add(1, v)
#define atomic_dec(v)		atomic_sub(1, v)

#define atomic_inc_and_test(v)	(atomic_add_return(1, v) == 0)
#define atomic_dec_and_test(v)	(atomic_sub_return(1, v) == 0)
#define atomic_inc_return(v)    (atomic_add_return(1, v))
#define atomic_dec_return(v)    (atomic_sub_return(1, v))
#define atomic_sub_and_test(i, v) (atomic_sub_return(i, v) == 0)

struct skb_shared_info {
	atomic32_t	    dataref;
};

struct sk_buff_head {
	/* These two members must be first. */
	struct sk_buff	*next;
	struct sk_buff	*prev;

	__u32		qlen;
	spinlock_t	lock;
};

struct sk_buff;

#define MAX_SKB_FRAGS 4UL		//16

union ktime {
	s32	tv64;
	/*
	struct {
		s32	nsec, sec;
	} tv;
	*/
};

typedef union ktime ktime_t;

struct skb_shared_hwtstamps {
	ktime_t	hwtstamp;
	ktime_t	syststamp;
};

/* Definitions for tx_flags in struct skb_shared_info */
enum {
	/* generate hardware time stamp */
	SKBTX_HW_TSTAMP = 1 << 0,

	/* generate software time stamp */
	SKBTX_SW_TSTAMP = 1 << 1,

	/* device driver is going to provide hardware time stamp */
	SKBTX_IN_PROGRESS = 1 << 2,

	/* ensure the originating sk reference is available on driver level */
	SKBTX_DRV_NEEDS_SK_REF = 1 << 3,
};

enum {
	SKB_FCLONE_UNAVAILABLE,
	SKB_FCLONE_ORIG,
	SKB_FCLONE_CLONE,
};

enum {
	SKB_GSO_TCPV4 = 1 << 0,
	SKB_GSO_UDP = 1 << 1,

	/* This indicates the skb is from an untrusted source. */
	SKB_GSO_DODGY = 1 << 2,

	/* This indicates the tcp segment has CWR set. */
	SKB_GSO_TCP_ECN = 1 << 3,

	SKB_GSO_TCPV6 = 1 << 4,

	SKB_GSO_FCOE = 1 << 5,
};

typedef unsigned char *sk_buff_data_t;

//#if defined(CONFIG_NF_DEFRAG_IPV4) || defined(CONFIG_NF_DEFRAG_IPV4_MODULE) || defined(CONFIG_NF_DEFRAG_IPV6) || defined(CONFIG_NF_DEFRAG_IPV6_MODULE)
//#define NET_SKBUFF_NF_DEFRAG_NEEDED 1
//#endif


struct sk_buff {
	/* These two members must be first. */
	struct sk_buff		*next;
	struct sk_buff		*prev;
	struct net_device	*dev;

	/*
	 * This is the control buffer. It is free to use for every
	 * layer. Please put your private variables there. If you
	 * want to keep them across layers you have to do a skb_clone()
	 * first. This is owned by whoever has the skb queued ATM.
	 */
	char			cb[48]  __attribute__((__aligned__(8)));

	unsigned int		len;
	unsigned int		data_len;
	__u16			mac_len,
				hdr_len;

	__u32			priority;

	__u8			local_df:1,
				cloned:1,
				ip_summed:2,
				nohdr:1,
				nfctinfo:3;
	__u8			pkt_type:3,
				fclone:2,
				ipvs_property:1,
				peeked:1,
				nf_trace:1;

	__be16			protocol;

	void			(*destructor)(struct sk_buff *skb);
	__u16			queue_mapping;

	__u8			ooo_okay:1,
#ifdef  KEY_FRAME_SW_RETRY
				sw_retry_cnt:4,
#endif
					bes_flag:3;

	sk_buff_data_t		transport_header;
	sk_buff_data_t		network_header;
	sk_buff_data_t		mac_header;
	/* These elements must be at the end, see alloc_skb() for details.  */
	sk_buff_data_t		tail;
	sk_buff_data_t		end;
	unsigned char		*head,
				        *data;
	unsigned int		truesize;
	atomic32_t		users;

	sk_buff_data_t      payload;
	unsigned int        payload_len;
	//uint8 used_sram;
};

#define kmap_skb_frag(X) ((X)->page->addr)

#define kunmap_skb_frag(x)

extern uint32 g_rx_flowctrl;
extern uint32 g_flowctrl_turnback;
static inline uint32 get_rx_flowctrl(void)
{
	return g_rx_flowctrl;
}

static inline uint32 get_rx_flowctrl_turnback(void)
{
	return g_flowctrl_turnback;
}

static inline void set_rx_flowctrl_turnback(uint32 value)
{
	g_flowctrl_turnback = value;
	return;
}


static inline unsigned char *skb_network_header(const struct sk_buff *skb)
{
	return skb->network_header;
}

static inline int skb_network_offset(const struct sk_buff *skb)
{
	return (int)skb_network_header(skb) - (int)skb->data;
}

static inline void skb_reset_network_header(struct sk_buff *skb)
{
	skb->network_header = skb->data;
}

static inline void skb_set_network_header(struct sk_buff *skb, const int offset)
{
	skb->network_header = skb->data + offset;
}

#define SKB_DST_NOREF	1UL
#define SKB_DST_PTRMASK	~(SKB_DST_NOREF)

struct dst_entry {

	struct dst_entry	*child;
	struct net_device       *dev;

	unsigned long		_metrics;
	unsigned long		expires;
	struct dst_entry	*path;
	struct neighbour	*neighbour;

	void			*__pad1;

	int			(*input)(struct sk_buff*);
	int			(*output)(struct sk_buff*);

	short			error;
	short			obsolete;
	unsigned short		header_len;	/* more space at head required */
	unsigned short		trailer_len;	/* space to reserve at tail */

	__u32			__pad2;
	/*
	 * __refcnt wants to be on a different cache line from
	 * input/output/ops or performance tanks badly
	 */
	atomic32_t		__refcnt;	/* client references	*/
	int			__use;
	unsigned long		lastuse;
	int			flags;
#define DST_HOST		0x0001
#define DST_NOXFRM		0x0002
#define DST_NOPOLICY		0x0004
#define DST_NOHASH		0x0008
#define DST_NOCACHE		0x0010
#define DST_NOCOUNT		0x0020

	struct dst_entry	*next;

};


struct rtable {
	struct dst_entry	dst;

	/* Lookup key. */
	__be32			rt_key_dst;
	__be32			rt_key_src;

	int			rt_genid;
	unsigned		rt_flags;
	__u16			rt_type;
	__u8			rt_key_tos;

	__be32			rt_dst;	/* Path destination	*/
	__be32			rt_src;	/* Path source		*/
	int			rt_route_iif;
	int			rt_iif;
	int			rt_oif;
	__u32			rt_mark;

	/* Info on neighbour */
	__be32			rt_gateway;

	/* Miscellaneous cached information */
	__be32			rt_spec_dst; /* RFC1122 specific destination */
	u32			rt_peer_genid;

};

extern void kfree_skb(struct sk_buff *skb);
extern void consume_skb(struct sk_buff *skb);
extern void	       __kfree_skb(struct sk_buff *skb);

extern struct sk_buff *__alloc_skb(unsigned int size, gfp_t gfp_mask, int fclone, int node);

extern void	       __kfree_skb_sram(struct sk_buff *skb);

extern struct sk_buff *__alloc_skb_sram(unsigned int size, gfp_t gfp_mask, int fclone, int node);
extern struct sk_buff *__alloc_skb_bes(unsigned int size, gfp_t gfp_mask, int fclone, int node);

#if(SKB_SRAM == 2)
#define alloc_skb_bes(size, priority) __alloc_skb_bes(size, priority, 0, NUMA_NO_NODE)
#define alloc_skb(size, priority) __alloc_skb(size, priority, 0, NUMA_NO_NODE)
#define free_skb(skb) __kfree_skb(skb);
#elif SKB_SRAM
#define alloc_skb(size, priority) __alloc_skb_sram(size, priority, 0, NUMA_NO_NODE)
#define free_skb(skb) __kfree_skb_sram(skb);
#else
#define alloc_skb(size, priority) __alloc_skb(size, priority, 0, NUMA_NO_NODE)
#define free_skb(skb) __kfree_skb(skb);
#endif

#define  alloc_skb_fclone(size, priority)  ((struct sk_buff *)__alloc_skb(size, priority, 1, NUMA_NO_NODE))

uint32_t get_global_skb_alloc(void);
extern struct sk_buff *skb_morph(struct sk_buff *dst, struct sk_buff *src);
extern struct sk_buff *skb_clone(struct sk_buff *skb,
				 gfp_t priority);
extern struct sk_buff *skb_copy(const struct sk_buff *skb,
				gfp_t priority);
extern struct sk_buff *skb_copy_expand(const struct sk_buff *skb,
				int newheadroom, int newtailroom,
				gfp_t gfp_mask);
extern int	       skb_cow_data(struct sk_buff *skb, int tailbits,
				    struct sk_buff **trailer);
extern int	       skb_pad(struct sk_buff *skb, int pad);
#define dev_kfree_skb(a)	consume_skb(a)

#define dev_kfree_skb_any   dev_kfree_skb
#define dev_kfree_skb_irq(a) dev_kfree_skb(a)

extern void skb_recover(struct sk_buff *skb);
extern struct sk_buff *dev_alloc_skb(unsigned int length);

extern struct sk_buff *__dev_alloc_skb(unsigned int length,
					      gfp_t gfp_mask);
void skb_check(void *buff);

struct skb_seq_state {
	__u32		lower_offset;
	__u32		upper_offset;
	__u32		frag_idx;
	__u32		stepped_offset;
	struct sk_buff	*root_skb;
	struct sk_buff	*cur_skb;
	__u8		*frag_data;
};

extern __u32 __skb_get_rxhash(struct sk_buff *skb);

extern unsigned char *skb_push(struct sk_buff *skb, unsigned int len);


#define skb_end_pointer(SKB) (SKB->end)
#define skb_shinfo(SKB)	((struct skb_shared_info *)(skb_end_pointer(SKB)))

/**
 *	skb_queue_empty - check if a queue is empty
 *	@list: queue head
 *
 *	Returns true if the queue is empty, false otherwise.
 */
#define skb_queue_empty(LIST)	((LIST)->next == (struct sk_buff *)LIST)

/**
 *	skb_queue_is_last - check if skb is the last entry in the queue
 *	@list: queue head
 *	@skb: buffer
 *
 *	Returns true if @skb is the last buffer on the list.
 */
#define skb_queue_is_last(LIST,  SKB)  ((SKB)->next == (struct sk_buff *)LIST)


/**
 *	skb_queue_is_first - check if skb is the first entry in the queue
 *	@list: queue head
 *	@skb: buffer
 *
 *	Returns true if @skb is the first buffer on the list.
 */
#define skb_queue_is_first(LIST, SKB)	 ((SKB)->prev == (struct sk_buff *)LIST)

/**
 *	skb_queue_next - return the next packet in the queue
 *	@list: queue head
 *	@skb: current buffer
 *
 *	Return the next packet in @list after @skb.  It is only valid to
 *	call this if skb_queue_is_last() evaluates to false.
 */
#define skb_queue_next(LIST, SKB) )  ((SKB)->next)

/**
 *	skb_queue_prev - return the prev packet in the queue
 *	@list: queue head
 *	@skb: current buffer
 *
 *	Return the prev packet in @list before @skb.  It is only valid to
 *	call this if skb_queue_is_first() evaluates to false.
 */

#define skb_queue_prev(LIST, SKB) )  ((SKB)->prev)

/**
 *	skb_get - reference buffer
 *	@skb: buffer to reference
 *
 *	Makes another reference to a socket buffer and returns a pointer
 *	to the buffer.
 */
#define skb_get(SKB)  (SKB)

/*
 * If users == 1, we are the only owner and are can avoid redundant
 * atomic change.
 */

/**
 *	skb_cloned - is the buffer a clone
 *	@skb: buffer to check
 *
 *	Returns true if the buffer was generated with skb_clone() and is
 *	one of multiple shared copies of the buffer. Cloned buffers are
 *	shared data so must not be written to under normal circumstances.
 */
#define skb_cloned(SKB)	((SKB)->cloned &&(atomic_read(&skb_shinfo(SKB)->dataref) & SKB_DATAREF_MASK) != 1)

/**
 *	skb_shared - is the buffer shared
 *	@skb: buffer to check
 *
 *	Returns true if more than one person has a reference to this
 *	buffer.
 */
static inline int skb_shared(const struct sk_buff *skb)
{
	return atomic_read((volatile atomic32_t *)&skb->users) != 1;
}

static inline int skb_linearize(struct sk_buff *skb)
{
	return 0;
}
/**
 *	skb_queue_len	- get queue length
 *	@list_: list to measure
 *
 *	Return the length of an &sk_buff queue.
 */
static inline __u32 skb_queue_len(const struct sk_buff_head *list_)
{
	return list_->qlen;
}

/**
 *	__skb_queue_head_init - initialize non-spinlock portions of sk_buff_head
 *	@list: queue to initialize
 *
 *	This initializes only the list and queue length aspects of
 *	an sk_buff_head object.  This allows to initialize the list
 *	aspects of an sk_buff_head without reinitializing things like
 *	the spinlock.  It can also be used for on-stack sk_buff_head
 *	objects where the spinlock is known to not be used.
 */
static inline void __skb_queue_head_init(struct sk_buff_head *list)
{
	list->prev = list->next = (struct sk_buff *)list;
	list->qlen = 0;
}

/*
 * This function creates a split out lock class for each invocation;
 * this is needed for now since a whole lot of users of the skb-queue
 * infrastructure in drivers have different locking usage (in hardirq)
 * than the networking core (in softirq only). In the long run either the
 * network layer or drivers should need annotation to consolidate the
 * main types of usage into 3 classes.
 */
static inline void skb_queue_head_init(struct sk_buff_head *list)
{
	spin_lock_init(&list->lock);
	__skb_queue_head_init(list);
}


static inline void skb_queue_head_init_class(struct sk_buff_head *list)
{
	skb_queue_head_init(list);
}

static inline void __skb_insert(struct sk_buff *newsk,
				struct sk_buff *prev, struct sk_buff *next,
				struct sk_buff_head *list)
{
	newsk->next = next;
	newsk->prev = prev;
	next->prev  = prev->next = newsk;
	list->qlen++;
}

static inline void __skb_queue_before(struct sk_buff_head *list,
				      struct sk_buff *next,
				      struct sk_buff *newsk)
{
	__skb_insert(newsk, next->prev, next, list);
}

static inline void __skb_queue_tail(struct sk_buff_head *list,
				   struct sk_buff *newsk)
{
	__skb_queue_before(list, (struct sk_buff *)list, newsk);
}

/**
 *	__skb_queue_tail - queue a buffer at the list tail
 *	@list: list to use
 *	@newsk: buffer to queue
 *
 *	Queue a buffer at the end of a list. This function takes no locks
 *	and you must therefore hold required locks before calling it.
 *
 *	A buffer cannot be placed on two lists at the same time.
 */
static inline void skb_queue_tail(struct sk_buff_head *list, struct sk_buff *newsk)
{
	unsigned long flags;

	spin_lock_irqsave(&list->lock, &flags);
	__skb_queue_tail(list, newsk);
	spin_unlock_irqrestore(&list->lock, flags);
}

/**
 *	__skb_queue_after - queue a buffer at the list head
 *	@list: list to use
 *	@prev: place after this buffer
 *	@newsk: buffer to queue
 *
 *	Queue a buffer int the middle of a list. This function takes no locks
 *	and you must therefore hold required locks before calling it.
 *
 *	A buffer cannot be placed on two lists at the same time.
 */
static inline void __skb_queue_after(struct sk_buff_head *list,
				     struct sk_buff *prev,
				     struct sk_buff *newsk)
{
	__skb_insert(newsk, prev, prev->next, list);
}


static inline void __skb_queue_head(struct sk_buff_head *list,
				    struct sk_buff *newsk)
{
	__skb_queue_after(list, (struct sk_buff *)list, newsk);
}

/**
 *	skb_orphan - orphan a buffer
 *	@skb: buffer to orphan
 *
 *	If a buffer currently has an owner then we call the owner's
 *	destructor function and make the @skb unowned. The buffer continues
 *	to exist but is no longer charged to its former owner.
 */
static inline void skb_orphan(struct sk_buff *skb)
{
	if (skb->destructor)
		skb->destructor(skb);
	skb->destructor = NULL;
}

/**
 *	skb_peek - peek at the head of an &sk_buff_head
 *	@list_: list to peek at
 *
 *	Peek an &sk_buff. Unlike most other operations you _MUST_
 *	be careful with this one. A peek leaves the buffer on the
 *	list and someone else may run off with it. You must hold
 *	the appropriate locks or have a private queue to do this.
 *
 *	Returns %NULL for an empty list or a pointer to the head element.
 *	The reference count is not incremented and the reference is therefore
 *	volatile. Use with caution.
 */
static inline struct sk_buff *skb_peek(const struct sk_buff_head *list_)
{
	struct sk_buff *list = ((const struct sk_buff *)list_)->next;
	if (list == (struct sk_buff *)list_)
		list = NULL;
	return list;
}

static inline void __skb_unlink(struct sk_buff *skb, struct sk_buff_head *list)
{
	struct sk_buff *next, *prev;

	list->qlen--;
	next	   = skb->next;
	prev	   = skb->prev;
	skb->next  = skb->prev = NULL;
	next->prev = prev;
	prev->next = next;
}

static inline struct sk_buff *__skb_dequeue(struct sk_buff_head *list)
{
	struct sk_buff *skb = skb_peek(list);
	if (skb)
		__skb_unlink(skb, list);
	return skb;
}

/**
 *	__skb_dequeue - remove from the head of the queue
 *	@list: list to dequeue from
 *
 *	Remove the head of the list. This function does not take any locks
 *	so must be used with appropriate locks held only. The head item is
 *	returned or %NULL if the list is empty.
 */
extern struct sk_buff *skb_dequeue(struct sk_buff_head *list);

extern void skb_queue_purge(struct sk_buff_head *list);
static inline void __skb_queue_purge(struct sk_buff_head *list)
{
	struct sk_buff *skb;
	while ((skb = __skb_dequeue(list)) != NULL)
		kfree_skb(skb);
}

static inline int skb_is_nonlinear(const struct sk_buff *skb)
{
	return skb->data_len;
}

static inline unsigned int skb_headlen(const struct sk_buff *skb)
{
	return skb->len - skb->data_len;
}

static inline unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
	return skb->tail;
}

static inline void skb_reset_tail_pointer(struct sk_buff *skb)
{
	skb->tail = skb->data;
}

static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	skb->tail = skb->data + offset;
}

/*
 *	Add data to an sk_buff
 */
extern unsigned char *skb_put(struct sk_buff *skb, unsigned int len);
static inline unsigned char *__skb_put(struct sk_buff *skb, unsigned int len)
{
	unsigned char *tmp = skb_tail_pointer(skb);

	skb->tail += len;
	skb->len  += len;
	return tmp;
}

static inline void *skb_put_data(struct sk_buff *skb, const void *data,
				 unsigned int len)
{
	void *tmp = skb_put(skb, len);

	memcpy(tmp, data, len);

	return tmp;
}

static inline unsigned char *__skb_push(struct sk_buff *skb, unsigned int len)
{
	skb->data -= len;
	skb->len  += len;
	return skb->data;
}

static inline unsigned char *__skb_pull(struct sk_buff *skb, unsigned int len)
{
	skb->len -= len;

	return skb->data += len;
}

static inline unsigned char *skb_pull_inline(struct sk_buff *skb, unsigned int len)
{
	return unlikely(len > skb->len) ? NULL : __skb_pull(skb, len);
}

static inline unsigned char *skb_pull(struct sk_buff *skb, unsigned int len)
{
	return skb_pull_inline(skb, len);
}

/**
 *	skb_headroom - bytes at buffer head
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the head of an &sk_buff.
 */
static inline unsigned int skb_headroom(const struct sk_buff *skb)
{
	return skb->data - skb->head;
}

/**
 *	skb_tailroom - bytes at buffer end
 *	@skb: buffer to check
 *
 *	Return the number of bytes of free space at the tail of an sk_buff
 */
static inline int skb_tailroom(const struct sk_buff *skb)
{
	return skb_is_nonlinear(skb) ? 0 : skb->end - skb->tail;
}

/**
 *	skb_reserve - adjust headroom
 *	@skb: buffer to alter
 *	@len: bytes to move
 *
 *	Increase the headroom of an empty &sk_buff by reducing the tail
 *	room. This is only allowed for an empty buffer.
 */
static inline void skb_reserve(struct sk_buff *skb, int len)
{
	skb->data += len;
	skb->tail += len;
}

static inline void skb_reset_mac_len(struct sk_buff *skb)
{
	skb->mac_len = skb->network_header - skb->mac_header;
}

static inline unsigned char *skb_transport_header(const struct sk_buff *skb)
{
	return skb->transport_header;
}

static inline void skb_reset_transport_header(struct sk_buff *skb)
{
	skb->transport_header = skb->data;
}

static inline void skb_set_transport_header(struct sk_buff *skb,
					    const int offset)
{
	skb->transport_header = skb->data + offset;
}

static inline unsigned char *skb_mac_header(const struct sk_buff *skb)
{
	return skb->mac_header;
}

static inline int skb_mac_header_was_set(const struct sk_buff *skb)
{
	return skb->mac_header != NULL;
}

static inline void skb_reset_mac_header(struct sk_buff *skb)
{
	skb->mac_header = skb->data;
}

static inline void skb_set_mac_header(struct sk_buff *skb, const int offset)
{
	skb->mac_header = skb->data + offset;
}

static inline int pskb_may_pull(struct sk_buff *skb, unsigned int len)
{
	if (len > skb_headlen(skb)) {
		int i = 0;
		printk(KERN_DEBUG, "pskb_may_pull len = %d, skblen = %d, skbdatalen = %d\n", len, skb->len, skb->data_len);
		for (i = 0; i < skb->len; i++)
			printk(KERN_DEBUG, "0x%02X", skb->data[i]);
		printk(KERN_DEBUG, "\n");
		return 0;
	} else
		return 1;
}

extern struct sk_buff *dev_alloc_skb(unsigned int length);

#define skb_queue_walk(queue, skb) \
for (skb = (queue)->next;					\
skb != (struct sk_buff *)(queue);				\
skb = skb->next)

#define skb_queue_walk_safe(queue, skb, tmp)					\
for (skb = (queue)->next, tmp = skb->next;			\
skb != (struct sk_buff *)(queue);				\
skb = tmp, tmp = skb->next)

#define skb_queue_walk_from(queue, skb)						\
for (; skb != (struct sk_buff *)(queue);			\
skb = skb->next)

#define skb_queue_walk_from_safe(queue, skb, tmp)				\
for (tmp = skb->next;						\
skb != (struct sk_buff *)(queue);				\
skb = tmp, tmp = skb->next)

#define skb_queue_reverse_walk(queue, skb) \
for (skb = (queue)->prev;					\
skb != (struct sk_buff *)(queue);				\
skb = skb->prev)

#define skb_queue_reverse_walk_safe(queue, skb, tmp)				\
for (skb = (queue)->prev, tmp = skb->prev;			\
skb != (struct sk_buff *)(queue);				\
skb = tmp, tmp = skb->prev)

#define skb_queue_reverse_walk_from_safe(queue, skb, tmp)			\
for (tmp = skb->prev;						\
skb != (struct sk_buff *)(queue);				\
skb = tmp, tmp = skb->prev)

extern int pskb_expand_head(struct sk_buff *skb, int nhead, int ntail,
		     gfp_t gfp_mask);
void skb_trim(struct sk_buff *skb, unsigned int len);

static inline void __skb_trim(struct sk_buff *skb, unsigned int len)
{
	if (unlikely(skb_is_nonlinear(skb))) {
		WARN_ON(1);
		return;
	}
	skb->len = len;
	skb_set_tail_pointer(skb, len);
}

static inline int __pskb_trim(struct sk_buff *skb, unsigned int len)
{
	NET_ASSERT (skb->data_len == 0);
	__skb_trim(skb, len);
	return 0;
}

static inline int pskb_trim(struct sk_buff *skb, unsigned int len)
{
	return (len < skb->len) ? __pskb_trim(skb, len) : 0;
}

struct sk_buff *__netdev_alloc_skb(struct net_device *dev, unsigned int length,
	gfp_t gfp_mask);

void skb_reserve(struct sk_buff *skb, int len);
int skb_copy_bits(const struct sk_buff *skb, int offset, void *to, int len);
static inline struct ethhdr *skb_eth_hdr(const struct sk_buff *skb)
{
	return (struct ethhdr *)skb_mac_header(skb);
}

static inline void skb_set_queue_mapping(struct sk_buff *skb, u16 queue_mapping)
{
	skb->queue_mapping = queue_mapping;
}

static inline u16 skb_get_queue_mapping(const struct sk_buff *skb)
{
	return skb->queue_mapping;
}

static inline void skb_copy_queue_mapping(struct sk_buff *to, const struct sk_buff *from)
{
	to->queue_mapping = from->queue_mapping;
}

static inline void skb_record_rx_queue(struct sk_buff *skb, u16 rx_queue)
{
	skb->queue_mapping = rx_queue + 1;
}

static inline u16 skb_get_rx_queue(const struct sk_buff *skb)
{
	return skb->queue_mapping - 1;
}

static inline bool skb_rx_queue_recorded(const struct sk_buff *skb)
{
	return skb->queue_mapping != 0;
}

static inline void __skb_queue_splice(const struct sk_buff_head *list,
				      struct sk_buff *prev,
				      struct sk_buff *next)
{
	struct sk_buff *first = list->next;
	struct sk_buff *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 *	skb_queue_splice_tail_init - join two skb lists and reinitialise the emptied list
 *	@list: the new list to add
 *	@head: the place to add it in the first list
 *
 *	Each of the lists is a queue.
 *	The list at @list is reinitialised
 */
static inline void skb_queue_splice_tail_init(struct sk_buff_head *list,
					      struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(list, head->prev, (struct sk_buff *) head);
		head->qlen += list->qlen;
		__skb_queue_head_init(list);
	}
}


#endif	/* __KERNEL__ */

