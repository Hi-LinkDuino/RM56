#ifndef __NET_DEBUG_H
#define __NET_DEBUG_H

#include "hal_trace.h"

enum {
    DUMP_PREFIX_NONE,
    DUMP_PREFIX_ADDRESS,
    DUMP_PREFIX_OFFSET
};
#ifdef __WIFI_METRICS_LOG__
#define WIFI_METRICS_PREFIX			"[METRICS]"
#else
#define WIFI_METRICS_PREFIX			""
#endif
#define LOG_NET(str, ...) TR_INFO(TR_MOD(NET), str, ##__VA_ARGS__)
#define LOG_NET_M(str, ...) TR_INFO(TR_MOD(NET), WIFI_METRICS_PREFIX str, ##__VA_ARGS__)

//#define SENSITIVE_LOG_HIDE

#define wlan_debug(fmt, ...) TRACE(0, fmt, ##__VA_ARGS__)

//#define NET_ASSERT(cond, ...)       { if (!(cond)) {wlan_debug("wlan assert %s line %d \n",__func__, __LINE__); while(1); } }
#define NET_ASSERT(cond, ...)       {ASSERT(cond, "net assert:%s:%d",__func__, __LINE__)}

#define KERN_ALERT		0
#define KERN_CRITICAL	1
#define KERN_ERR		2
#define KERN_WARN		3
#define KERN_INFO		4
#define KERN_DEBUG		5
#define KERN_LOUD		6

#define KERN_DBG_LEVEL		KERN_INFO

//#define printk TRACE
#define printk(level, str, ...) \
({								\
	if ((level) <= KERN_DBG_LEVEL) \
        	TRACE(0, str, ##__VA_ARGS__); \
})
#define printk_m(level, str, ...) \
({								\
	if ((level) <= KERN_DBG_LEVEL) \
		TRACE(0, WIFI_METRICS_PREFIX str, ##__VA_ARGS__); \
})


#define BUILD_BUG_ON(condition)

#define WARN_ON(condition)	\
({								\
	int __ret_warn_on = !!(condition);			\
	if (unlikely(__ret_warn_on)){				\
		wlan_debug("WARN_ON:%s %d \n", __func__, __LINE__);			\
    }\
	unlikely(__ret_warn_on);				\
})


#ifndef WARN
#define WARN(condition, format...) ({						\
	int __ret_warn_on = !!(condition);				\
	if (unlikely(__ret_warn_on))					\
		wlan_debug(format);					\
	unlikely(__ret_warn_on);					\
})
#endif

#ifndef WARN_ON_ONCE
#define WARN_ON_ONCE(condition)	({				\
	static bool  __warned;		\
	int __ret_warn_once = !!(condition);			\
								\
	if (unlikely(__ret_warn_once))				\
		if (WARN_ON(!__warned)) 			\
			__warned = true;			\
	unlikely(__ret_warn_once);				\
})

#endif

#define BUG()

#define BUG_ON(cond)	ASSERT(!(cond), "ASSERT:%s line:%d", __func__, __LINE__)
extern uint32_t  rt_get_PSP (void);
//#define ENTER() printk(KERN_INFO, "Enter:%s %d\n", __func__, __LINE__);
//#define LEAVE() printk(KERN_INFO, "Leave:%s %d\n", __func__, __LINE__);
#define ENTER()
#define LEAVE()

extern void print_hex_dump_bytes(const char *prefix_str, int prefix_type,
                                 const void *buf, size_t len);

extern int hal_trace_printf_without_crlf(const char *fmt, ...);
extern int printf(const char *fmt, ...);
#ifdef SENSITIVE_LOG_HIDE
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3]
#define MACSTR "%02x:%02x:%02x:%02x:*:*"
#endif
#define print_addr(k) TRACE(0, "***addr:%02x:%02x:%02x:%02x:*:*\n", (k)[0],(k)[1],(k)[2],(k)[3])
#define DB_MAGIC_STR   "sdb:"
#define SECRETE_TRACE(attr, str, ...) TRACE(attr, DB_MAGIC_STR#str, ##__VA_ARGS__)
#define passwd_out(k)  "***"
static inline char *ssid_out(char *k,int len)
{
	static char sec_ssid_buf[6] = {0};
	if (len > 3) {
		sec_ssid_buf[0] = *k;
		sec_ssid_buf[1] = *(k + 1);
		sec_ssid_buf[2] = '*';
		sec_ssid_buf[3] = '*';
		sec_ssid_buf[4] = *(k + len - 1);
		sec_ssid_buf[5] = '\0';
		return sec_ssid_buf;
	}
	return k;
}
#else // SENSITIVE_LOG_HIDE
#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif
#define print_addr(k) TRACE(0, "***addr:%02x:%02x:%02x:%02x:%02x:%02x\n", (k)[0],(k)[1],(k)[2],(k)[3],(k)[4],(k)[5])

#define SECRETE_TRACE(attr, str, ...) TRACE(attr, str, ##__VA_ARGS__)
#define passwd_out(k)   k
#define ssid_out(k,len) k
#endif //SENSITIVE_LOG_HIDE
#endif /* __NET_DEBUG_H */
