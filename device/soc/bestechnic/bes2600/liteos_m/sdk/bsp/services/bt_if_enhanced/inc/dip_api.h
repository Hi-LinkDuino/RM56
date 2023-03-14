#ifndef __DIP_API__H__
#define __DIP_API__H__

#include "bluetooth.h"
#include "sdp_api.h"


#define SRC_BT (1)
#define SRC_USB (2)
#define SRC_BT_SANSUMG (0x0075)
#define SRC_BT_APPLE   (0X004c)
#define SRC_USB_APPLE   (0x05AC)

#ifdef __cplusplus
extern "C" {
#endif

typedef void btif_dip_client_t;

typedef struct 
{
    uint16_t spec_id;
    uint16_t vend_id;
    uint16_t prod_id;
    uint16_t prod_ver;
    uint8_t  prim_rec;
    uint16_t vend_id_source;
} btif_dip_pnp_info;

btif_dip_pnp_info* btif_dip_get_device_info(bt_bdaddr_t *remote);

typedef void (*DipApiCallBack)(bt_bdaddr_t *_addr, bool ios_flag);

void btif_dip_init(DipApiCallBack callback);
void btif_dip_clear(btif_remote_device_t *bt_dev);
bt_status_t btif_dip_query_for_service(btif_dip_client_t *client_t,btif_remote_device_t *btDevice);
bool btif_dip_check_is_ios_device(btif_remote_device_t *btDevice);
void btif_dip_get_remote_info(btif_remote_device_t *btDevice);
bool btif_dip_get_process_status(btif_remote_device_t *btDevice);
bool btif_dip_check_is_ios_by_vend_id(uint16_t vend_id, uint16_t vend_id_source);

#ifdef __cplusplus
}
#endif

#endif

