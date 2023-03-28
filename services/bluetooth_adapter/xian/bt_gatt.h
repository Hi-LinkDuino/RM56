#ifndef __BT_GATT_H__
#define __BT_GATT_H__
#include "gatt_server_interface.h"

/** BT-GATT callbacks */
typedef struct {
    /** Set to sizeof(btgatt_callbacks_t) */
    size_t size;

    /** GATT Client callbacks */
    const void* client;

    /** GATT Server callbacks */
    const btgatt_server_callbacks_t* server;
} btgatt_callbacks_t;

/** Represents the standard Bluetooth GATT interface. */
typedef struct {
    /** Set to sizeof(btgatt_interface_t) */
    size_t          size;

    /**
     * Initializes the interface and provides callback routines
     */
    bt_status_t (*init)( const btgatt_callbacks_t* callbacks );

    /** Closes the interface */
    void (*cleanup)( void );

    /** Pointer to the GATT client interface methods.*/
    const void* client;

    /** Pointer to the GATT server interface methods.*/
    const btgatt_server_interface_t* server;
} btgatt_interface_t;

#endif