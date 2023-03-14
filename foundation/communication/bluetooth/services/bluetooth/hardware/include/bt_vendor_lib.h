/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BT_VENDOR_LIB_H
#define BT_VENDOR_LIB_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define HCI channel descriptors array used in BT_OP_HCI_CHANNEL_OPEN operation.
 */
typedef enum {
    HCI_CMD,        // HCI Command channel
    HCI_EVT,        // HCI Event channel
    HCI_ACL_OUT,    // HCI ACL downstream channel
    HCI_ACL_IN,     // HCI ACL upstream channel
    HCI_MAX_CHANNEL // Total channels
} hci_channels_t;

typedef enum {
    BTC_OP_RESULT_SUCCESS,
    BTC_OP_RESULT_FAIL,
} bt_op_result_t;

/**
 * BT vendor lib cmd.
 */
typedef enum {
    /**
     * Power on the BT Controller.
     * @return 0 if success.
     */
    BT_OP_POWER_ON,

    /**
     * Power off the BT Controller.
     * @return 0 if success.
     */
    BT_OP_POWER_OFF,

    /**
     * Establish hci channels. it will be called after BT_OP_POWER_ON.
     * @param int (*)[HCI_MAX_CHANNEL].
     * @return fd count.
     */
    BT_OP_HCI_CHANNEL_OPEN,

    /**
     * Close all the hci channels which is opened.
     */
    BT_OP_HCI_CHANNEL_CLOSE,

    /**
     * initialization the BT Controller. it will be called after BT_OP_HCI_CHANNEL_OPEN.
     * Controller Must call init_cb to notify the host once it has been done.
     */
    BT_OP_INIT,

    /**
     * Get the LPM idle timeout in milliseconds.
     * @param (uint_32 *)milliseconds, btc will return the value of lpm timer.
     * @return 0 if success.
     */
    BT_OP_GET_LPM_TIMER,

    /**
     * Enable LPM mode on BT Controller.
     */
    BT_OP_LPM_ENABLE,

    /**
     * Disable LPM mode on BT Controller.
     */
    BT_OP_LPM_DISABLE,

    /**
     * Wakeup lock the BTC.
     */
    BT_OP_WAKEUP_LOCK,

    /**
     * Wakeup unlock the BTC.
     */
    BT_OP_WAKEUP_UNLOCK,

    /**
     * transmit event response to vendor lib.
     * @param (void *)buf, struct of HC_BT_HDR.
     */
    BT_OP_EVENT_CALLBACK
} bt_opcode_t;

/**
 * initialization callback.
 */
typedef void (*init_callback)(bt_op_result_t result);

/** 
 * call the callback to malloc a size of buf.
 */
typedef void* (*malloc_callback)(int size);

/**
 * call the callback to free buf
 */
typedef void (*free_callback)(void* buf);

/**
 *  hci command packet transmit callback
 *  Vendor lib calls cmd_xmit_cb function in order to send a HCI Command
 *  packet to BT Controller. 
 *
 *  The opcode parameter gives the HCI OpCode (combination of OGF and OCF) of
 *  HCI Command packet. For example, opcode = 0x0c03 for the HCI_RESET command
 *  packet.
 */
typedef size_t (*cmd_xmit_callback)(uint16_t opcode, void* p_buf);

typedef struct {
    /**
     * set to sizeof(bt_vendor_callbacks_t)
     */
    size_t size;

    /* notifies caller result of init request */
    init_callback init_cb;

    /* buffer allocation request */
    malloc_callback alloc;

    /* buffer free request */
    free_callback dealloc;

    /* hci command packet transmit request */
    cmd_xmit_callback xmit_cb;
} bt_vendor_callbacks_t;

/**
 * Bluetooth Host/Controller VENDOR Interface
 */
typedef struct {
    /**
     * Set to sizeof(bt_vndor_interface_t)
     */
    size_t size;

    /**
     * Caller will open the interface and pass in the callback routines
     * to the implemenation of this interface.
     */
    int (*init)(const bt_vendor_callbacks_t* p_cb, unsigned char* local_bdaddr);

    /**
     * Vendor specific operations
     */
    int (*op)(bt_opcode_t opcode, void* param);

    /**
     * Closes the interface
     */
    void (*close)(void);
} bt_vendor_interface_t;

typedef struct
{
    uint16_t event;
    uint16_t len;
    uint16_t offset;
    uint16_t layer_specific;
    uint8_t data[];
} HC_BT_HDR;

#ifdef __cplusplus
}
#endif

#endif /* BT_VENDOR_LIB_H */
