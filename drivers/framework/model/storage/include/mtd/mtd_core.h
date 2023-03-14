/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MTD_CORE_H
#define MTD_CORE_H

#include "los_vm_phys.h"

#include "hdf_base.h"
#include "osal_mutex.h"
#include "platform_core.h"

#define MTD_FLASH_ID_LEN_MAX 8
#define MTD_DEVICE_NUM_MAX   3

// #define MTD_DEBUG /* open this macro for debug

struct MtdDevice;
struct MtdDeviceMethod;

/**
 * @brief Enumerates the types of mtd devices.
 *
 */
enum MtdDevType {
    MTD_TYPE_NOR,
    MTD_TYPE_NAND,
    MTD_TYPE_SPI_NOR,
    MTD_TYPE_SPI_NAND,
    MTD_TYPE_MAX,
};

/**
 * @brief Enumerates the types of mtd message.
 *
 */
enum MtdMsgType {
    MTD_MSG_TYPE_WRITE,
    MTD_MSG_TYPE_READ,
    MTD_MSG_TYPE_ERASE,
};

/**
 * @Defines the mtd request message used during request.
 *
 * This struct describes a custom access request typed by MtdMsgType
 *
 */
struct MtdMsg {
    /** Type of the mtd message */
    enum MtdMsgType type;
    /** Start address of the mtd device to access */
    off_t addr;
    /** Indicates the failed address if request failed */
    off_t faddr;
    /** Pointer to the buffer for storing request data */
    uint8_t *buf;
    /** Length of the data */
    size_t len;
    /** Indicates whether request with oob data */
    bool withOob;
    /** Indicates whether skip on bad block */
    bool skipBad;
};

/**
 * @Defines the mtd page struct used during page transfer.
 *
 * This struct describes a page transfer request typed by MtdMsgType
 *
 */
struct MtdPage {
    /** Page transfer type */
    enum MtdMsgType type;
    /** Start address of the mtd device to transfer from */
    off_t addr;
    /** Indicates the failed address if transfer failed */
    off_t failAddr;
    /** Pointer to the data buffer */
    uint8_t *dataBuf;
    /** Length of the data */
    size_t dataLen;
    /** Pointer to the oob data buffer */
    uint8_t *oobBuf;
    /** Length of the oob data */
    size_t oobLen;
};

/**
 * @Defines the callback method set of a mtd device.
 *
 * The methods defined in this struct expected to be implemented by drivers
 *
 */
struct MtdDeviceMethod {
    int32_t (*read)(struct MtdDevice *mtdDevice, off_t from, size_t len, uint8_t *buf);
    int32_t (*write)(struct MtdDevice *mtdDevice, off_t to, size_t len, const uint8_t *buf);
    int32_t (*erase)(struct MtdDevice *mtdDevice, off_t addr, size_t len, off_t *faddr);
    int32_t (*pageTransfer)(struct MtdDevice *mtdDevice, struct MtdPage *mtdPage);
    bool (*isBadBlock)(struct MtdDevice *mtdDevice, off_t addr);
    int32_t (*markBadBlock)(struct MtdDevice *mtdDevice, off_t addr);
    void (*dump)(struct MtdDevice *mtdDevice);
    int32_t (*lock)(struct MtdDevice *mtdDevice);
    void (*unlock)(struct MtdDevice *mtdDevice);
};

/**
 * @Defines the structure used to identify a physical mtd chip.
 *
 */
struct MtdDevice {
    /** The parent device */
    struct PlatformDevice device;
    /** Number of the mtd device */
    int16_t index;
    /** Name of the mtd device */
    const char *name;
    /** Chip name of the mtd device */
    const char *chipName;
    /** Type of the mtd device */
    enum MtdDevType type;
    union {
        /** Vendor id of the mtd device */
        uint8_t id[MTD_FLASH_ID_LEN_MAX];
        struct {
            uint8_t mfrId; // id[0]: Manufacture ID
            uint8_t devId; // id[0]: Device ID
        };
    };
    /** Length of the vendor id */
    uint16_t idLen;
    /** Length of the vendor id */
    size_t capacity;
    /** Erase size of the mtd device */
    size_t eraseSize;
    /** Write size of the mtd device */
    size_t writeSize;
    /** Read size of the mtd device */
    size_t readSize;
    /** Oob size of the mtd device */
    size_t oobSize;
    /** Write size shift bits of the mtd device */
    unsigned int writeSizeShift;
    /** Mutex lock for accessing protection */
    struct OsalMutex lock;
    /** The callback method set to access the device */
    struct MtdDeviceMethod *ops;
    /** Os specific data */
    void *osData;
    /** Flash memory controller specific data */
    void *cntlr;
    /** Private data of the mtd device */
    void *priv;
};

/**
 * @brief Get the manager of mtd
 *
 * @return Returns the pointer to the manager if success;
 * returns NULL otherwise.
 */
struct PlatformManager *MtdManagerGet(void);

/**
 * @brief Add a mtd device.
 *
 * do not call in irq context cause it may sleep
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 * @return Returns 0 if add successfully; returns a negative value otherwise.
 */
int32_t MtdDeviceAdd(struct MtdDevice *mtdDevice);

/**
 * @brief Delete a mtd device.
 *
 * do not call in irq context cause it may sleep
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 */
void MtdDeviceDel(struct MtdDevice *mtdDevice);

/**
 * @brief Increase reference count for a mtd device.
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 * @return Returns the pointer to the mtd device on success; returns NULL otherwise.
 */
static inline struct MtdDevice *MtdDeviceGet(struct MtdDevice *mtdDevice)
{
    if (mtdDevice != NULL && PlatformDeviceGet(&mtdDevice->device) == HDF_SUCCESS) {
        return mtdDevice;
    }
    return NULL;
}

/**
 * @brief Increase reference count for a mtd device by number.
 *
 * @param mtdDevice Indicates the pointer to the mtd device.
 *
 * @return Returns the pointer to the mtd device on success; returns NULL otherwise.
 */
static inline struct MtdDevice *MtdDeviceGetByNum(int16_t num)
{
    struct PlatformDevice *device = PlatformManagerGetDeviceByNumber(MtdManagerGet(), num);

    if (device != NULL) {
        return CONTAINER_OF(device, struct MtdDevice, device);
    }
    return NULL;
}

/**
 * @brief Decrease reference count for a mtd device.
 *  
 * @param mtdDevice Indicates the pointer to the mtd device.
 */
static inline void MtdDevicePut(struct MtdDevice *mtdDevice)
{
    if (mtdDevice != NULL) {
        PlatformDevicePut(&mtdDevice->device);
    }
}

/**
 * @brief Lock the mtd device exclusivly.
 *  
 * do not call in irq context cause it may sleep
 *  
 * @param mtdDevice Indicates the pointer to the mtd device.
 *  
 * @return Returns 0 on success; returns a negative value otherwise.
 */
int32_t MtdDeviceLock(struct MtdDevice *mtdDevice);

/**
 * @brief Unlock the mtd device.
 *  
 * do not call in irq context cause it may sleep
 *  
 * @param mtdDevice Indicates the pointer to the mtd device.
 */  
void MtdDeviceUnlock(struct MtdDevice *mtdDevice);

/**
 * @brief Reads data of a specified size from a mtd device.
 *  
 * do not call in irq context cause it may sleep
 *  
 * @param mtdDevice Indicates the pointer to the mtd device.
 * @param from Indicates the start address to read from.
 * @param len Indicates the size of the data to read.
 * @param buf Indicates the pointer to the buffer for receiving the data.
 *  
 * @return Returns the size of the data read on success; returns a negative value otherwise.
 */
ssize_t MtdDeviceRead(struct MtdDevice *mtdDevice, off_t from, size_t len, uint8_t *buf);

/**
 * @brief Writes data of a specified size to a mtd device.
 *  
 * do not call in irq context cause it may sleep
 *  
 * @param mtdDevice Indicates the pointer to the mtd device.
 * @param to Indicates the start address to write to.
 * @param len Indicates the size of the data to write.
 * @param buf Indicates the pointer to the buffer which provide the data.
 *  
 * @return Returns the size of the data written on success; returns a negative value otherwise.
 */
ssize_t MtdDeviceWrite(struct MtdDevice *mtdDevice, off_t to, size_t len, const uint8_t *buf);

/**
 * @brief Erases data of a specified size to a mtd device.
 *  
 * do not call in irq context cause it may sleep
 *  
 * @param mtdDevice Indicates the pointer to the mtd device.
 * @param from Indicates the start address to erase from.
 * @param len Indicates the size of the data to erase.
 * @param faddr Indicates the pointer for receiving the address when erasing failed.
 *  
 * @return Returns the size of the data erased on success; returns a negative value otherwise.
 */
ssize_t MtdDeviceErase(struct MtdDevice *mtdDevice, off_t from, size_t len, off_t *faddr);

/**
 * @brief Reads data of a specified size from a mtd device including oob data.
 *  
 * It is similar to MtdDeviceRead but read oob data together.
 *  
 * @param mtdDevice Indicates the pointer to the mtd device.
 * @param from Indicates the start address to read from.
 * @param len Indicates the size of the data, including oob, to read.
 * @param buf Indicates the pointer to the buffer for receiving the data, including oob data.
 *  
 * @return Returns the size of the data read on success; returns a negative value otherwise.
 */
ssize_t MtdDeviceReadWithOob(struct MtdDevice *mtdDevice, off_t from, size_t len, uint8_t *buf);

/**
 * @brief Writes data of a specified size to a mtd device including oob data.
 *  
 * It is similar to MtdDeviceWrite but write oob data together.
 *  
 * @param mtdDevice Indicates the pointer to the mtd device.
 * @param to Indicates the start address to write to.
 * @param len Indicates the size of the data, including oob, to .
 * @param buf Indicates the pointer to the buffer which provide the data including oob.
 *  
 * @return Returns the size of the data read on success; returns a negative value otherwise.
 */
ssize_t MtdDeviceWriteWithOob(struct MtdDevice *mtdDevice, off_t to, size_t len, const uint8_t *buf);

/**
 * @brief Judge whether the block where the specified address is located is a bad block
 *  
 * @param mtdDevice Indicates the pointer to the mtd device.
 * @param addr Indicates the address to judge, which is not necessary to align by block size.
 *  
 * @return Returns true is the specified address falls into a bad block; returns false otherwise.
 */
bool MtdDeviceIsBadBlock(struct MtdDevice *mtdDevice, off_t addr);

/**
 * @brief Mark the block where the specified address is located as a bad block
 *  
 * @param mtdDevice Indicates the pointer to the mtd device.
 * @param addr Indicates the address to mark, which is not necessary to align by block size.
 *  
 * @return Returns 0 on success; returns a negative value otherwise.
 */
int32_t MtdDeviceMarkBadBlock(struct MtdDevice *mtdDevice, off_t addr);


/***************************** Other Utils *******************************************/

/**
 * @brief A debugging macro which an dump all the attributes of a mtd device
 *  
 */
#define MTD_DEVICE_DUMP(mtd) \
    do { \
        uint16_t i; \
        HDF_LOGI("%s: name = %s(%s), type = %d", __func__, mtd->name, mtd->chipName, mtd->type); \
        for (i = 0; i < mtd->idLen; i++) { \
            HDF_LOGI("%s: id[%u] = 0x%x", __func__, i, mtd->id[i]); \
        } \
        HDF_LOGI("%s: capacity: %zu", __func__, mtd->capacity); \
        HDF_LOGI("%s: eraseSize: %zu", __func__, mtd->eraseSize); \
        HDF_LOGI("%s: writeSize: %zu", __func__, mtd->writeSize); \
        HDF_LOGI("%s: readSize: %zu", __func__, mtd->readSize); \
        HDF_LOGI("%s: oobSize: %zu", __func__, mtd->oobSize); \
    } while (0)

/**
 * @brief Judge whether the specified address is aligned by page size
 *  
 */
static inline bool MtdDeviceIsPageAligned(struct MtdDevice *mtdDevice, off_t addr)
{
    return (((uint64_t)addr & (mtdDevice->writeSize - 1)) == 0);
}

/**
 * @brief Transfer the specified address to page number
 *  
 */
static inline size_t MtdDeviceAddrToPage(struct MtdDevice *mtdDevice, off_t addr)
{
    return (size_t)((uint64_t)addr >> mtdDevice->writeSizeShift);
}

/**
 * @brief Flush the cache for dma transfer
 *  
 */
static inline void MtdDmaCacheClean(void *addr, size_t size)
{
    uintptr_t start = (uintptr_t)addr & ~(CACHE_ALIGNED_SIZE - 1);
    uintptr_t end = (uintptr_t)addr + (uintptr_t)size;

    end = ALIGN(end, CACHE_ALIGNED_SIZE);
    DCacheFlushRange(start, end);
    return;
}

/**
 * @brief Invalid the cache for dma transfer
 *  
 */
static inline void MtdDmaCacheInv(void *addr, size_t size)
{
    uintptr_t start = (uintptr_t)addr & ~(CACHE_ALIGNED_SIZE - 1);
    uintptr_t end = (uintptr_t)addr + (uintptr_t)size;

    end = ALIGN(end, CACHE_ALIGNED_SIZE);
    DCacheInvRange(start, end);
    return;
}

/**
 * @brief Find the first bit set in word
 *  
 * @return Returns the position of first bit set; returns 0 if all zero.
 */
int MtdFfs(int x);

#endif /* MTD_CORE_H */
