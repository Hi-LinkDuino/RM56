#ifndef __BT_GATT_TYPES_H__
#define __BT_GATT_TYPES_H__

/**
 * GATT Service types
 */
#define BTGATT_SERVICE_TYPE_PRIMARY 0
#define BTGATT_SERVICE_TYPE_SECONDARY 1

typedef int bt_status_t;

/** Bluetooth Error Status */
/** We need to build on this */

typedef enum {
    WH_BT_STATUS_SUCCESS,
    WH_BT_STATUS_FAIL,
    WH_BT_STATUS_NOT_READY,
    WH_BT_STATUS_NOMEM,
    WH_BT_STATUS_BUSY,
    WH_BT_STATUS_DONE,        /* request already completed */
    WH_BT_STATUS_UNSUPPORTED,
    WH_BT_STATUS_PARM_INVALID,
    WH_BT_STATUS_UNHANDLED,
    WH_BT_STATUS_AUTH_FAILURE,
    WH_BT_STATUS_RMT_DEV_DOWN,
    WH_BT_STATUS_AUTH_REJECTED
} wh_bt_status_t;

typedef struct{     
uint8_t data[16];
}uint128_t;

typedef struct {

       enum {

              BT_UUID_UNSPEC = 0,

              BT_UUID16 = 16,

              BT_UUID32 = 32,

              BT_UUID128 = 128,

       } type;

       union {

              uint16_t  u16;

              uint32_t  u32;

              uint128_t u128;

       } value;

} bt_uuid_t;

/** Bluetooth Address */
typedef struct {
    uint8_t address[6];
} __attribute__ ((packed)) bt_bdaddr_t;

/** GATT ID adding instance id tracking to the UUID */
typedef struct
{
    bt_uuid_t           uuid;
    uint8_t             inst_id;
} btgatt_gatt_id_t;

/** GATT Service ID also identifies the service type (primary/secondary) */
typedef struct
{
    btgatt_gatt_id_t    id;
    uint8_t             is_primary;
} btgatt_srvc_id_t;

/** Preferred physical Transport for GATT connection */
typedef enum
{
    GATT_TRANSPORT_AUTO,
    GATT_TRANSPORT_BREDR,
    GATT_TRANSPORT_LE
} btgatt_transport_t;

#endif

