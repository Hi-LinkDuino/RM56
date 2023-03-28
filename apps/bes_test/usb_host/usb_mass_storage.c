/***************************************************************************
 *
 * Copyright 2015-2020 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifdef RTOS
#include "cmsis_os.h"
#endif
#include "hal_trace.h"
#include "Utils.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "hal_timer.h"
#include "hal_usbhost.h"
#include "usb_host.h"
#include "usb_mass_storage.h"
#include "usb_hcd.h"
#include "usb_hcd_test.h"

#if 1
#define USB_MASS_STORAGE_TRACE(attr,str,...) TRACE(attr,str, ##__VA_ARGS__)
#else
#define USB_MASS_STORAGE_TRACE(attr,str,...)
#endif

#define ERR_BAD_CSW_SIGNATURE -200

#define USB_MASS_STORAGE_XFER_SIZE          4096

#define CBW_SIGNATURE  0x43425355
#define CSW_SIGNATURE  0x53425355

// SCSI Command

// Test Unit Ready  0x00
#define USB_SCSI_CMD_TEST_UNIT_READ           0x00

// Request Sense    0x03
#define USB_SCSI_CMD_REQUEST_SENSE            0x03

// Inquiry          0x12
#define USB_SCSI_CMD_INQUIRY                  0x12

// Mode Sense(6)    0x1A
#define USB_SCSI_CMD_MODE_SENSE_6              0x1A

// Mode Sense(10)   0x5A
#define USB_SCSI_CMD_MODE_SENSE_10             0x5A

// Read Capacity(10) 0x25
#define USB_SCSI_CMD_READ_CAPACITY_10          0x25

// Read(10) 0x28
#define USB_SCSI_CMD_READ_10                   0x28

// Write(10)  0x2A
#define USB_SCSI_CMD_WRITE_10                  0x2A

// Read Format Capacity 0x23
#define USB_SCSI_CMD_READ_FORMAT_CAPACITY_10   0x23

// Mode Select(6) 0x15
#define USB_SCSI_CMD_MODE_SELECT_6             0x15

// Mode Select(10) 0x55
#define USB_SCSI_CMD_MODE_SELECT_10             0x55

// Prevent/Allow Medium Removal  0x1E
#define USB_SCSI_CMD_PREVENT_OR_ALLOW_MEDIUM_REMOVAL   0x1E

// Start/Stop Uint 0x1B
#define USB_SCSI_CMD_START_STOP_UINT           0x1B

// Report LUNs 0xA0
#define USB_SCSI_CMD_REPORT_LUNS               0xA0

// Verify  0x2F
#define USB_SCSI_CMD_VERIFY                    0x2F

// Command Block Wrapper
typedef struct
{
    u32 signature;
    u32 tag;
    u32 trans_len;
    u8 flags;
    u8 lum;
    u8 cb_len;
    u8 cb[16];   // only 6 really
} CBW;

// Command Status Wrapper
typedef struct
{
    u32 signature;
    u32 tag;
    u32 sata_residue;
    u8 status;
} CSW;

static int32_t scsi_request_sense(int32_t device);

uint8_t scsi_read_ep;
uint8_t scsi_write_ep;
uint8_t scsi_itf_num;

static bool disk_init = false;

typedef struct {
    int32_t device;
    int32_t endpoint;
    int32_t status;
    uint8_t data[USB_MASS_STORAGE_XFER_SIZE];
    int32_t len;
    void* user_data;
}SCSI_TRANSFER_DATA_T;

static SCSI_TRANSFER_DATA_T  scsi_transfer_data;
bool is_wait_transfer_done = false;

static void _scsi_handle(int32_t device,
                        int32_t endpoint,
                        int32_t status,
                        uint8_t* data,
                        int32_t length,
                        void* user_data)
{
    USB_MASS_STORAGE_TRACE(0, "_scsi_handle: d=%d e=0x%x s=%d, l = %d,",
                         device, endpoint, status,length);

    scsi_transfer_data.device = device;
    scsi_transfer_data.endpoint = endpoint;

    scsi_transfer_data.status = status;

    memcpy(scsi_transfer_data.data,data,length);

    scsi_transfer_data.len = length;

    scsi_transfer_data.user_data = user_data;
    is_wait_transfer_done = false;


}

static int32_t do_scsi(int32_t device, const uint8_t* cmd, int32_t cmd_len, int32_t flags, uint8_t* data, uint32_t transfer_len)
{
    CBW cbw;
    CSW csw;
    int32_t ret;

    cbw.signature = CBW_SIGNATURE;
    cbw.tag = 0;
    cbw.trans_len = transfer_len;
    cbw.flags = flags;
    cbw.lum = 0;
    cbw.cb_len = cmd_len;
    memset(cbw.cb,0,sizeof(cbw.cb));
    memcpy(cbw.cb,cmd,cmd_len);

    //USB_MASS_STORAGE_TRACE(0, "do_scsi: device=%d flags=0x%x transferLen=%d", device, (uint8_t)flags, transfer_len);

    // Send the command
    //is_wait_transfer_done = true;
    memset(&scsi_transfer_data,0,sizeof(scsi_transfer_data));
    //USB_MASS_STORAGE_TRACE(0, "do_scsi: Step 1");
    ret = test_usb_bulk_tracefer_req(device,scsi_write_ep,(uint8_t*)&cbw,31,NULL,0);
    if (ret < 0)
    {
        if(ret == ERR_IO_PENDING)
        {
           // while(is_wait_transfer_done)
            {
                USB_MASS_STORAGE_TRACE(0, "%s:%d", __func__,__LINE__);

                osDelay(10);
            }
            USB_MASS_STORAGE_TRACE(0, "do_scsi: Send command done");

        }
        else
        {
            return ret;
        }
        if(scsi_transfer_data.status != 0)
        {
            goto func_end;
        }
    }
    //USB_MASS_STORAGE_TRACE(0, "do_scsi: Step 1 done");
    //USB_MASS_STORAGE_TRACE(0, "do_scsi: Step 2");
    if (data)
    {
        USB_MASS_STORAGE_TRACE(0, "do_scsi: to process data");
        is_wait_transfer_done = true;
        memset(&scsi_transfer_data,0,sizeof(scsi_transfer_data));
        ret = test_usb_bulk_tracefer_req(device,
                                     (flags==DEVICE_TO_HOST)?scsi_read_ep:scsi_write_ep,
                                     data,
                                     transfer_len,
                                     _scsi_handle,
                                     0);
        if (ret < 0)
        {
            if(ret == ERR_IO_PENDING)
            {
                while(is_wait_transfer_done)
                {
                    USB_MASS_STORAGE_TRACE(0, "%s:%d", __func__,__LINE__);

                    osDelay(10);
                }
                memcpy(data,scsi_transfer_data.data,transfer_len);
            }
            else
            {
                return ret;
            }
        }
        if(scsi_transfer_data.status != 0)
        {
            goto func_end;
        }


    }
    //USB_MASS_STORAGE_TRACE(0, "do_scsi: Step 2 done");

    //USB_MASS_STORAGE_TRACE(0, "do_scsi: Step 3");
    //is_wait_transfer_done = true;
    memset(&scsi_transfer_data,0,sizeof(scsi_transfer_data));
    csw.signature = 0;
    ret = test_usb_bulk_tracefer_req(device,scsi_read_ep,(uint8_t*)&csw,13,NULL,0);
    if (ret < 0)
    {
        if(ret == ERR_IO_PENDING)
        {
            while(is_wait_transfer_done)
            {
                USB_MASS_STORAGE_TRACE(0, "%s:%d", __func__,__LINE__);

                osDelay(10);
            }
        }
        else
        {
            return ret;
        }
    }
    //USB_MASS_STORAGE_TRACE(0, "do_scsi: Step 3 done");
    if(scsi_transfer_data.status != 0)
    {
        goto func_end;
    }
    if (csw.signature != CSW_SIGNATURE)
    {
        return ERR_BAD_CSW_SIGNATURE;
    }

    if (csw.status == 1 && cmd[0] != USB_SCSI_CMD_REQUEST_SENSE)
    {
        return scsi_request_sense(device);
    }
    //USB_MASS_STORAGE_TRACE(0, "do_scsi: dnoe");
    return csw.status;

func_end:
    USB_MASS_STORAGE_TRACE(0, "do_scsi: fail! status = %d",scsi_transfer_data.status);
    return scsi_transfer_data.status;

}

static int32_t scsi_test_unit_ready(int32_t device)
{
    uint8_t cmd[6];

    memset(cmd,0,6);
    cmd[0] = USB_SCSI_CMD_TEST_UNIT_READ;
    return do_scsi(device,cmd,6,DEVICE_TO_HOST,0,0);
}

static int32_t scsi_request_sense(int device)
{
    uint8_t cmd[6] = {USB_SCSI_CMD_REQUEST_SENSE,0,0,0,18,0};
    uint8_t result[18];
    int32_t ret = do_scsi(device,cmd,6,DEVICE_TO_HOST,result,18);
    return ret;
}

static int32_t scsi_inquiry(int32_t device, uint8_t lun, uint8_t page_code)
{
    uint8_t evpd = (page_code == 0) ? 0 : 1;
    uint8_t cmd[6] = {USB_SCSI_CMD_INQUIRY, ((lun << 5) | evpd), page_code, 0, 36, 0};
    uint8_t result[36+2];
    char vid_pid[17];
    int ret;

    result[36] = '\n';
    result[37] = 0;

    USB_MASS_STORAGE_TRACE(0, "%s",__func__);

    ret = do_scsi(device,cmd,6,DEVICE_TO_HOST,result,36);
    if (ret)
    {
        USB_MASS_STORAGE_TRACE(0, "%s failed", __func__);
        return ret;
    }

    memcpy(vid_pid, &result[8], 8);
    vid_pid[8] = 0;
    USB_MASS_STORAGE_TRACE(0, "MSD [dev: %d] - Vendor ID: %s", device, vid_pid);

    memcpy(vid_pid, &result[16], 16);
    vid_pid[16] = 0;
    USB_MASS_STORAGE_TRACE(0, "MSD [dev: %d] - Product ID: %s", device, vid_pid);

    memcpy(vid_pid, &result[32], 4);
    vid_pid[4] = 0;
    USB_MASS_STORAGE_TRACE(0, "MSD [dev: %d] - Product rev: %s", device, vid_pid);

    return 0;
}

static int32_t scsi_read_capacity(int32_t device, uint32_t* block_count, uint32_t* block_size)
{
    //uint8_t cmd[10] = {USB_SCSI_CMD_READ_CAPACITY_10,0,0,0,8,0,0,0,0,0};
    uint8_t cmd[10] = {USB_SCSI_CMD_READ_CAPACITY_10,0,0,0,0,0,0,0,0,0};
    uint8_t result[8];
    int32_t ret;

    *block_size = 0;
    *block_count = 0;

    USB_MASS_STORAGE_TRACE(0, "%s",__func__);
    ret = do_scsi(device,cmd,10,DEVICE_TO_HOST,result,8);
    if (ret)
    {
        USB_MASS_STORAGE_TRACE(0, "%s failed: %d", __func__,ret);
        return ret;
    }

    *block_count = BE32(result);
    *block_size = BE32(result+4);

    USB_MASS_STORAGE_TRACE(0, "MSD: blockCount=%x, blockSize=%x, capacity=%x MB",
        (uint32_t)*block_count,
        (uint32_t)*block_size,
        (uint32_t)((u64)(*block_count)*(*block_size)/(1<<20)));
    return ret;
}

static int32_t scsi_get_max_lun(int device)
{
    uint8_t buf[4];
    int32_t ret;
    ret = usbhost_control_transfer(1,
                   0,
                   RECIPIENT_INTERFACE | DEVICE_TO_HOST | REQUEST_TYPE_CLASS,
                   0xfe,
                   0,
                   scsi_itf_num,
                   buf,
                   1,
                   NULL,
                   NULL,
                   NULL);
    if (ret)
    {
        USB_MASS_STORAGE_TRACE(0, "failed to get max lun: %d", ret);
    }
    else
    {
        TRACE(1, "max lun: %d", buf[0]);
    }

    return ret;
}

static int32_t scsi_init(int device)
{
    int32_t ret;
    uint32_t i;
    uint32_t timeout = 10;

    USB_MASS_STORAGE_TRACE(0, "%s: %d", __func__, device);
    ret = scsi_get_max_lun(device);
    if (ret) {
        return ret;
    }
    device = 1;
    for (i = 0; i < timeout; i++) {
        hal_sys_timer_delay(MS_TO_TICKS(200));
        USB_MASS_STORAGE_TRACE(0, "%s: %d", __func__, __LINE__);
        if (!scsi_test_unit_ready(device))
        {
            USB_MASS_STORAGE_TRACE(0, "%s: %d", __func__, __LINE__);
            break;
        }
    }

    if (i == timeout) {
        USB_MASS_STORAGE_TRACE(0, "%s: %d", __func__, __LINE__);
        disk_init = false;
        return -1;
    }

    ret = scsi_inquiry(device, 0, 0);
    if (ret) {
        USB_MASS_STORAGE_TRACE(0, "%s: %d", __func__, __LINE__);
        return ret;
    }

    disk_init = true;
    is_wait_transfer_done = false;
    USB_MASS_STORAGE_TRACE(0, "%s: %d", __func__, __LINE__);
    return 0;
}

static int32_t scsi_transfer(int32_t device, uint32_t block_addr, uint32_t block_count, uint8_t* dst, uint32_t block_size, int32_t direction)
{
    uint8_t cmd[10];

    //  USB hardware will only do 4k per transfer
    while (block_count*block_size > USB_MASS_STORAGE_XFER_SIZE)
    {
        int count = USB_MASS_STORAGE_XFER_SIZE/block_size;
        //int r = SCSITransfer(device,block_addr,count,dst,block_size,direction);
        scsi_transfer(device,block_addr,count,dst,block_size,direction);
        dst += count*block_size;
        block_addr += count;
        block_count -= count;
    }

    memset(cmd,0,10);
    cmd[0] = (direction == DEVICE_TO_HOST) ? USB_SCSI_CMD_READ_10 : USB_SCSI_CMD_WRITE_10;
    BE32_1(block_addr,cmd+2);
    BE16(block_count,cmd+7);
    return do_scsi(device,cmd,10,direction,dst,block_size*block_count);
}

int32_t usb_mass_storage_init(int32_t device)
{
    int32_t ret;

    ret = scsi_init(device);
    if(ret)
    {
        USB_MASS_STORAGE_TRACE(0, "%s: scsi_init(%d) failed! ret = %d.", __func__, device, ret);
    }
    return ret;
}

int32_t usb_mass_storage_get_capacity(int32_t device, uint32_t* block_count, uint32_t* blockSize)
{
    return scsi_read_capacity(device,block_count,blockSize);
}

int32_t usb_mass_storage_read(int32_t device,
                                     uint32_t block_addr,
                                     uint32_t block_count,
                                     uint8_t* dst,
                                     u32 blockSize)
{
    return scsi_transfer(device,block_addr,block_count,dst,blockSize,DEVICE_TO_HOST);
}

int32_t usb_mass_storage_write(int32_t device,
                                     uint32_t block_addr,
                                     uint32_t block_count,
                                     uint8_t* dst,
                                     u32 blockSize)
{
    return scsi_transfer(device,block_addr,block_count,dst,blockSize,HOST_TO_DEVICE);
}

void usb_mass_storage_set_read_ep(uint8_t ep)
{
    scsi_read_ep = ep;
}

void usb_mass_storage_set_write_ep(uint8_t ep)
{
    scsi_write_ep = ep;
}

void usb_mass_storage_set_itf_num(uint8_t num)
{
    scsi_itf_num = num;
}

