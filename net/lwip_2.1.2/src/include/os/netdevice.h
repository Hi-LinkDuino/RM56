#ifndef __NET_DEVICE_H__
#define __NET_DEVICE_H__
#include "linux_list.h"
#include "skbuff.h"
#include "net_bitops.h"

struct ieee80211_sub_if_data;
/* source back-compat hooks */
#define SET_ETHTOOL_OPS(netdev,ops) \
	( (netdev)->ethtool_ops = (ops) )

#define HAVE_ALLOC_NETDEV		/* feature macro: alloc_xxxdev
					   functions are available. */
#define HAVE_FREE_NETDEV		/* free_netdev() */
#define HAVE_NETDEV_PRIV		/* netdev_priv() */

/* hardware address assignment types */
#define NET_ADDR_PERM		0	/* address is permanent (default) */
#define NET_ADDR_RANDOM		1	/* address is generated randomly */
#define NET_ADDR_STOLEN		2	/* address is stolen from other device */

/* Backlog congestion levels */
#define NET_RX_SUCCESS		0	/* keep 'em coming, baby */
#define NET_RX_DROP		1	/* packet dropped */

/* qdisc ->enqueue() return codes. */
#define NET_XMIT_SUCCESS	0x00
#define NET_XMIT_DROP		0x01	/* skb dropped			*/
#define NET_XMIT_CN		0x02	/* congestion notification	*/
#define NET_XMIT_POLICED	0x03	/* skb is shot by police	*/
#define NET_XMIT_MASK		0x0f	/* qdisc flags in net/sch_generic.h */

/* NET_XMIT_CN is special. It does not guarantee that this packet is lost. It
 * indicates that the device will soon be dropping packets, or already drops
 * some packets of the same priority; prompting us to send less aggressively. */
#define net_xmit_eval(e)	((e) == NET_XMIT_CN ? 0 : (e))
#define net_xmit_errno(e)	((e) != NET_XMIT_CN ? -ENOBUFS : 0)

/* Driver transmit return codes */
#define NETDEV_TX_MASK		0xf0

enum netdev_tx {
	__NETDEV_TX_MIN	 = -1,	/* make sure enum is signed */
	NETDEV_TX_OK	 = 0x00,	/* driver took care of packet */
	NETDEV_TX_BUSY	 = 0x10,	/* driver tx path was busy*/
	NETDEV_TX_LOCKED = 0x20,	/* driver tx lock was already taken */
};
typedef enum netdev_tx netdev_tx_t;

/*
 * Current order: NETDEV_TX_MASK > NET_XMIT_MASK >= 0 is significant;
 * hard_start_xmit() return < NET_XMIT_MASK means skb was consumed.
 */
static inline bool dev_xmit_complete(int rc)
{
	/*
	 * Positive cases with an skb consumed by a driver:
	 * - successful transmission (rc == NETDEV_TX_OK)
	 * - error while transmitting (rc < 0)
	 * - error while queueing to a different device (rc & NET_XMIT_MASK)
	 */
	if (likely(rc < NET_XMIT_MASK))
		return true;

	return false;
}

#define MAX_ADDR_LEN	32		/* Largest hardware address length */

/* Initial net device group. All devices belong to group 0 by default. */
#define INIT_NETDEV_GROUP	0


/* Media selection options. */
enum {
	IF_PORT_UNKNOWN = 0,
	IF_PORT_10BASE2,
	IF_PORT_10BASET,
	IF_PORT_AUI,
	IF_PORT_100BASET,
	IF_PORT_100BASETX,
	IF_PORT_100BASEFX
};

enum netdev_state_t {
	__LINK_STATE_START,
	__LINK_STATE_PRESENT,
	__LINK_STATE_NOCARRIER,
	__LINK_STATE_LINKWATCH_PENDING,
	__LINK_STATE_DORMANT,
};

#define TC_MAX_QUEUE	16
#define TC_BITMASK	15
/* HW offloaded queuing disciplines txq count and offset maps */
struct netdev_tc_txq {
	u16 count;
	u16 offset;
};

#define NETDEV_HW_ADDR_T_LAN		1
#define NETDEV_HW_ADDR_T_SAN		2
#define NETDEV_HW_ADDR_T_SLAVE		3
#define NETDEV_HW_ADDR_T_UNICAST	4
#define NETDEV_HW_ADDR_T_MULTICAST	5
struct netdev_hw_addr {
	struct list_head	list;
	unsigned char		addr[MAX_ADDR_LEN];
	unsigned char		type;
	bool			global_use;
	int			refcount;
	int			synced;
	struct rcu_head		rcu_head;
};

struct netdev_hw_addr_list {
	struct list_head	list;
	int			count;
};
#define netdev_hw_addr_list_count(l) ((l)->count)
#define netdev_hw_addr_list_for_each(ha, l) \
	list_for_each_entry(ha, &(l)->list, list)

struct net_device_stats {
	unsigned long	rx_packets;
	unsigned long	tx_packets;
	unsigned long	rx_bytes;
	unsigned long	tx_bytes;
	unsigned long	rx_errors;
	unsigned long	tx_errors;
	unsigned long	rx_dropped;
	unsigned long	tx_dropped;
	unsigned long	multicast;
	unsigned long	collisions;
	unsigned long	rx_length_errors;
	unsigned long	rx_over_errors;
	unsigned long	rx_crc_errors;
	unsigned long	rx_frame_errors;
	unsigned long	rx_fifo_errors;
	unsigned long	rx_missed_errors;
	unsigned long	tx_aborted_errors;
	unsigned long	tx_carrier_errors;
	unsigned long	tx_fifo_errors;
	unsigned long	tx_heartbeat_errors;
	unsigned long	tx_window_errors;
	unsigned long	rx_compressed;
	unsigned long	tx_compressed;
};

struct net_device;
/*
 * __QUEUE_STATE_DRV_XOFF is used by drivers to stop the transmit queue.  The
 * netif_tx_* functions below are used to manipulate this flag.  The
 * __QUEUE_STATE_STACK_XOFF flag is used by the stack to stop the transmit
 * queue independently.  The netif_xmit_*stopped functions below are called
 * to check if the queue has been stopped by the driver or stack (either
 * of the XOFF bits are set in the state).	Drivers should not need to call
 * netif_xmit*stopped functions, they should only be using netif_tx_*.
 */

struct netdev_queue {
	/*
	 * read mostly part
	 */
	struct net_device	*dev;

	/*
	 * write mostly part
	 */
	int			xmit_lock_owner;
	/*
	 * please use this field instead of dev->trans_start
	 */
	unsigned long		trans_start;

	/*
	 * Number of TX timeouts for this queue
	 * (/sys/class/net/DEV/Q/trans_timeout)
	 */
	unsigned long		trans_timeout;

	unsigned long		state;

} ;


#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

#ifndef ETH_ALEN
#define ETH_ALEN			6
#endif

struct ieee80211_sub_if_data;
struct net_device {
	/*
	 * This is the first field of the "visible" part of this structure
	 * (i.e. as seen by users in the "Space.c" file).  It is the name
	 * of the interface.
	 */
	char	name[IFNAMSIZ];

	struct net_device_stats	stats;

	/* Management operations */
	const struct net_device_ops *netdev_ops;
	/* Interface index. Unique device identifier	*/
	int					ifindex;
	/* Hardware header description */
	unsigned int		flags;	/* interface flags (a la BSD)	*/
	unsigned long		state;
	spinlock_t		    addr_list_lock;
	struct netdev_hw_addr_list	dev_addrs; /* list of device*/
	struct netdev_hw_addr_list	mc;	/* Multicast mac addresses */
	unsigned int		mtu;	/* interface MTU value		*/
	unsigned short		type;	/* interface hardware type	*/
	unsigned short		hard_header_len;	/* hardware hdr length	*/

	unsigned char		perm_addr[MAX_ADDR_LEN]; /* permanent hw address */
	unsigned char		addr_len;	/* hardware address length	*/
	unsigned char       wmm_en;
	struct wireless_dev	*ieee80211_ptr;	/* IEEE 802.11 specific data,
						   assign before registering */


	/* Interface address info used in eth_type_trans() */
	unsigned char		dev_addr[ETH_ALEN];	/* hw address, (before bcast
						   because most packets are
						   unicast) */

	unsigned char		broadcast[MAX_ADDR_LEN];	/* hw bcast add	*/

	struct netdev_queue *_tx;
	unsigned int		num_tx_queues;
	unsigned int		real_num_tx_queues;
	unsigned long		tx_queue_len;	/* Max frames per queue allowed */
	unsigned char		is_used;
	unsigned long		trans_start;
	void (*destructor)(struct net_device *dev);
	struct ieee80211_sub_if_data* priv;
	void* vif_data;
};

#define to_net_dev(d) container_of(d, struct net_device, dev)

#define	NETDEV_ALIGN		32

static inline void *netdev_priv(const struct net_device *dev)
{
	return dev->priv;
}

struct net_device_ops {
	void		(*ndo_uninit)(struct net_device *dev);
	int			(*ndo_open)(struct net_device *dev);
	int			(*ndo_stop)(struct net_device *dev);
	netdev_tx_t			(*ndo_start_xmit) (struct sk_buff *skb,
						   struct net_device *dev);
	int			(*ndo_set_mac_address)(struct net_device *dev,
							   void *addr);
    int			(*ndo_change_mtu)(struct net_device *dev,
						  int new_mtu);
    void			(*ndo_set_rx_mode)(struct net_device *dev);
	int			(*ndo_do_ioctl)(struct net_device *dev,
							/*struct ifreq*/void *ifr, int cmd);
	void			(*ndo_tx_timeout) (struct net_device *dev);
	struct net_device_stats* (*ndo_get_stats)(struct net_device *dev);
	int 			(*set_smart_link_recv_hook) (struct net_device *dev, void* callback);
	void			(*ndo_set_multicast_list)(struct net_device *dev);
	u16			(*ndo_select_queue)(struct net_device *dev,
							struct sk_buff *skb);
};

enum netdev_queue_state_t {
	__QUEUE_STATE_DRV_XOFF,
	__QUEUE_STATE_STACK_XOFF,
	__QUEUE_STATE_FROZEN,
};

#define QUEUE_STATE_ANY_XOFF ((1 << __QUEUE_STATE_DRV_XOFF)		| \
			      (1 << __QUEUE_STATE_STACK_XOFF))
#define QUEUE_STATE_ANY_XOFF_OR_FROZEN (QUEUE_STATE_ANY_XOFF		| \
					(1 << __QUEUE_STATE_FROZEN))

static inline bool netif_running(const struct net_device *dev)
{
	return test_bit(__LINK_STATE_START, (volatile void *)&dev->state);
}

static inline void netdev_for_each_tx_queue(struct net_device *dev,
						void (*f)(struct net_device *,
							  struct netdev_queue *,
							  void *),
						void *arg)
{
	unsigned int i;

	for (i = 0; i < dev->num_tx_queues; i++)
		f(dev, &dev->_tx[i], arg);
}

static inline
struct netdev_queue *netdev_get_tx_queue(const struct net_device *dev,
					 unsigned int index)
{
	return &dev->_tx[index];
}

/* Are all TX queues of the device empty?  */
static inline bool qdisc_all_tx_empty(const struct net_device *dev)
{
	return true;
}

static inline bool netdev_uses_dsa_tags(struct net_device *dev)
{
	return 0;
}

static inline bool netdev_uses_trailer_tags(struct net_device *dev)
{
	return 0;
}

static inline void netif_tx_stop_queue(struct netdev_queue *dev_queue)
{
	if (WARN_ON(!dev_queue))
		return;
	set_bit(__QUEUE_STATE_DRV_XOFF, (volatile void *)&dev_queue->state);
}

/**
 *	netif_stop_queue - stop transmitted packets
 *	@dev: network device
 *
 *	Stop upper layers calling the device hard_start_xmit routine.
 *	Used for flow control when transmit resources are unavailable.
 */
static inline void netif_stop_queue(struct net_device *dev)
{
	netif_tx_stop_queue(netdev_get_tx_queue(dev, 0));
}

static inline void netif_tx_stop_all_queues(struct net_device *dev)
{
	unsigned int i;

	for (i = 0; i < dev->num_tx_queues; i++) {
		struct netdev_queue *txq = netdev_get_tx_queue(dev, i);
		netif_tx_stop_queue(txq);
	}
}

/**
 *	netif_stop_subqueue - stop sending packets on subqueue
 *	@dev: network device
 *	@queue_index: sub queue index
 *
 * Stop individual transmit queue of a device with multiple transmit queues.
 */
static inline void netif_stop_subqueue(struct net_device *dev, u16 queue_index)
{
	struct netdev_queue *txq = netdev_get_tx_queue(dev, queue_index);

	netif_tx_stop_queue(txq);
}


static inline void netif_tx_start_queue(struct netdev_queue *dev_queue)
{
	clear_bit(__QUEUE_STATE_DRV_XOFF, (volatile void *)&dev_queue->state);
}

static inline void netif_tx_start_all_queues(struct net_device *dev)
{
	unsigned int i;

	for (i = 0; i < dev->num_tx_queues; i++) {
		struct netdev_queue *txq = netdev_get_tx_queue(dev, i);
		netif_tx_start_queue(txq);
	}
}

static inline void netif_tx_wake_queue(struct netdev_queue *dev_queue)
{
	test_and_clear_bit(__QUEUE_STATE_DRV_XOFF, (volatile void *)&dev_queue->state);
}

static inline void netif_tx_wake_all_queues(struct net_device *dev)
{
	unsigned int i;

	for (i = 0; i < dev->num_tx_queues; i++) {
		struct netdev_queue *txq = netdev_get_tx_queue(dev, i);
		netif_tx_wake_queue(txq);
	}
}

/**
 *	netif_wake_subqueue - allow sending packets on subqueue
 *	@dev: network device
 *	@queue_index: sub queue index
 *
 * Resume individual transmit queue of a device with multiple transmit queues.
 */
static inline void netif_wake_subqueue(struct net_device *dev, u16 queue_index)
{
	struct netdev_queue *txq = netdev_get_tx_queue(dev, queue_index);

	netif_tx_wake_queue(txq);
}

static inline bool netif_tx_queue_stopped(const struct netdev_queue *dev_queue)
{
	return test_bit(__QUEUE_STATE_DRV_XOFF, (volatile void *)&dev_queue->state);
}

static inline bool netif_xmit_stopped(const struct netdev_queue *dev_queue)
{
	return dev_queue->state & QUEUE_STATE_ANY_XOFF;
}

/**
 *	netif_subqueue_stopped - test status of subqueue
 *	@dev: network device
 *	@queue_index: sub queue index
 *
 * Check individual transmit queue of a device with multiple transmit queues.
 */
static inline bool netif_subqueue_stopped(const struct net_device *dev,
					  u16 queue_index)
{
	struct netdev_queue *txq = netdev_get_tx_queue(dev, queue_index);

	return netif_tx_queue_stopped(txq);
}

/**
 *	netif_carrier_ok - test if carrier present
 *	@dev: network device
 *
 * Check if carrier is present on device
 */
static inline bool netif_carrier_ok(const struct net_device *dev)
{
	return !test_bit(__LINK_STATE_NOCARRIER, (volatile void *)&dev->state);
}

/**
 *	netif_carrier_on - set carrier
 *	@dev: network device
 *
 * Device has detected that carrier.
 */
static inline void netif_carrier_on(struct net_device *dev)
{
	test_and_clear_bit(__LINK_STATE_NOCARRIER, (volatile void *)&dev->state);
}

/**
 *	netif_carrier_off - clear carrier
 *	@dev: network device
 *
 * Device has detected loss of carrier.
 */
static inline void netif_carrier_off(struct net_device *dev)
{
	test_and_set_bit(__LINK_STATE_NOCARRIER, (volatile void *)&dev->state);
}


static inline void netif_addr_lock(struct net_device *dev)
{
	spin_lock(&dev->addr_list_lock);
}
static inline void netif_addr_lock_bh(struct net_device *dev)
{
	spin_lock_bh(&dev->addr_list_lock);
}

static inline void netif_addr_unlock(struct net_device *dev)
{
	spin_unlock(&dev->addr_list_lock);
}

static inline void netif_addr_unlock_bh(struct net_device *dev)
{
	spin_unlock_bh(&dev->addr_list_lock);
}

extern void		__dev_set_rx_mode(struct net_device *dev);
extern void dev_addr_flush(struct net_device *dev);
extern int dev_addr_init(struct net_device *dev);
extern int dev_mc_del(struct net_device *dev, unsigned char *addr);
extern int dev_mc_add(struct net_device *dev, unsigned char *addr);
extern int net_ratelimit(void);
extern void free_netdev(struct net_device *dev);
extern  int dev_open(struct net_device *dev);
extern struct net_device* get_net_dev(uint8_t index);
extern struct net_device* get_net_dev_by_name(const char *name);
extern int dev_close(struct net_device *dev);
extern void ether_setup(struct net_device *dev);
extern struct net_device *alloc_netdev_mqs(const char *name,
					void (*setup)(struct net_device *),
					unsigned int txqs, unsigned int rxqs);
extern int dev_alloc_name(struct net_device *dev, const char *name);
extern int dev_queue_xmit(struct net_device *dev, struct sk_buff *skb);
extern int netif_receive_skb(struct sk_buff *skb);
#endif /* __KERNEL__ */

