#include <rtthread.h>
#include <drivers/usb_device.h>
#include <drivers/usb_host.h>
#include <cdc.h>
#include "hal_timer.h"

#define VENDOR_ID_REALTEK   0x0bda
#define PRODUCT_RTL8152     0x8152

/* Enable or disable autonegotiation. */
#define AUTONEG_DISABLE     0x00
#define AUTONEG_ENABLE      0x01

/* The forced speed, in units of 1Mb. All values 0 to INT_MAX are legal.
 * Update drivers/net/phy/phy.c:phy_speed_to_str() and
 * drivers/net/bonding/bond_3ad.c:__get_link_speed() when adding new values.
 */
#define SPEED_10        10
#define SPEED_100       100
#define SPEED_1000      1000
#define SPEED_2500      2500
#define SPEED_5000      5000
#define SPEED_10000     10000
#define SPEED_14000     14000
#define SPEED_20000     20000
#define SPEED_25000     25000
#define SPEED_40000     40000
#define SPEED_50000     50000
#define SPEED_56000     56000
#define SPEED_100000    100000
#define SPEED_UNKNOWN   -1

/* Duplex, half or full. */
#define DUPLEX_HALF     0x00
#define DUPLEX_FULL     0x01

void r8152b_init(struct uhintf *intf);
void r8152b_hw_phy_cfg(struct uhintf *intf);
int rtl8152_set_mac_address(struct uhintf *intf, uint8_t *mac_addr);
int rtl8152_set_speed(struct uhintf *intf, uint8_t autoneg, uint16_t speed, uint8_t duplex);
void rtl8152_up(struct uhintf *intf);
int rtl8152_enable(struct uhintf *intf);
int rtl8152_get_mac_address(struct uhintf *intf, uint8_t *mac_addr);
uint16_t rtl8152_get_speed(struct uhintf *intf);
void rtl8152_disable(struct uhintf *intf);
