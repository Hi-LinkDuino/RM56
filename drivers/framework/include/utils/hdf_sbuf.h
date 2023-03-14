/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */
/**
 * @addtogroup Core
 * @{
 *
 * @brief Provides functions to use the OpenHarmony Driver Foundation (HDF).
 *
 * The HDF implements driver framework capabilities such as driver loading, service management, driver message model,
 *  and power management.
 * You can develop drivers based on the HDF.
 *
 * @since 1.0
 */

/**
 * @file hdf_sbuf.h
 *
 * @brief Declares functions related to the HDF SBUF. The HDF provides data serialization and deserialization
 * capabilities for data transmission between user-space applications and kernel-space drivers.
 *
 * @since 1.0
 */

#ifndef HDF_SBUF_H
#define HDF_SBUF_H

#include "hdf_base.h"

#ifdef __cplusplus
extern "C" {
#else
typedef uint16_t char16_t;
#endif /* __cplusplus */

struct HdfSBuf;
struct HdfSBufImpl;
struct HdfRemoteService;

/**
 * @brief Enumerates HDF SBUF types.
 *
 * @since 1.0
 */
enum HdfSbufType {
    SBUF_RAW = 0,   /* SBUF used for communication between the user space and the kernel space */
    SBUF_IPC,      /* SBUF used for inter-process communication (IPC) */
    SBUF_IPC_HW,    /* Reserved for extension */
    SBUF_TYPE_MAX,  /* Maximum value of the SBUF type */
};

/**
 * @brief Writes a data segment to a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param data Indicates the pointer to the data segment to write.
 * @param writeSize Indicates the size of the data segment to write. The maximum value is 512 KB.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteBuffer(struct HdfSBuf *sbuf, const void *data, uint32_t writeSize);

/**
 * @brief Writes unpadded data to a <b>SBuf</b>. You can call {@link HdfSbufReadUnpadBuffer} to read the unpadded data.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param data Indicates the pointer to the data to write. The value cannot be a null pointer.
 * @param writeSize Indicates the size of the data to write. The value cannot be <b>0</b>.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteUnpadBuffer(struct HdfSBuf *sbuf, const uint8_t *data, uint32_t writeSize);

/**
 * @brief Writes a 64-bit unsigned integer to a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the 64-bit unsigned integer to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteUint64(struct HdfSBuf *sbuf, uint64_t value);

/**
 * @brief Writes a 32-bit unsigned integer to a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the 32-bit unsigned integer to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteUint32(struct HdfSBuf *sbuf, uint32_t value);

/**
 * @brief Writes a 16-bit unsigned integer to a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the 16-bit unsigned integer to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteUint16(struct HdfSBuf *sbuf, uint16_t value);

/**
 * @brief Writes an 8-bit unsigned integer to a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the 8-bit unsigned integer to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteUint8(struct HdfSBuf *sbuf, uint8_t value);

/**
 * @brief Writes a 64-bit signed integer to a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the 64-bit signed integer to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteInt64(struct HdfSBuf *sbuf, int64_t value);

/**
 * @brief Writes a 32-bit signed integer to a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the 32-bit signed integer to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteInt32(struct HdfSBuf *sbuf, int32_t value);

/**
 * @brief Writes a 16-bit signed integer to a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the 16-bit signed integer to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteInt16(struct HdfSBuf *sbuf, int16_t value);

/**
 * @brief Writes an 8-bit signed integer to a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the 8-bit signed integer to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteInt8(struct HdfSBuf *sbuf, int8_t value);

/**
 * @brief Writes a string to a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the string to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteString(struct HdfSBuf *sbuf, const char *value);

/**
 * @brief Writes a wide character string to a <b>SBuf</b>.
 * The SBUF of the <b>SBUF_RAW</b> type does not support this function.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the wide character string to write.
 * @param size Indicates the size of the wide character string to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteString16(struct HdfSBuf *sbuf, const char16_t *value, uint32_t size);

/**
 * @brief Writes a floating-point number to a <b>SBuf</b>.
 * The SBUF of the <b>SBUF_RAW</b> type does not support this function.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the floating-point number to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteFloat(struct HdfSBuf *sbuf, float value);

/**
 * @brief Writes a double-precision floating-point number to a <b>SBuf</b>.
 * The SBUF of the <b>SBUF_RAW</b> type does not support this function.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the double-precision floating-point number to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteDouble(struct HdfSBuf *sbuf, double value);

/**
 * @brief Writes a file descriptor to a <b>SBuf</b>.
 * The SBUF of the <b>SBUF_RAW</b> type does not support this function.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param fd Indicates the file descriptor to write.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufWriteFileDescriptor(struct HdfSBuf *sbuf, int fd);

/**
 * @brief Writes an IPC service to a <b>SBuf</b>. The SBUF of the <b>SBUF_RAW</b> type does not support this function.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param service Indicates the pointer to the IPC service to write.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t HdfSbufWriteRemoteService(struct HdfSBuf *sbuf, const struct HdfRemoteService *service);

/**
 * @brief Reads an IPC service from a <b>SBuf</b>.
 * The SBUF of the <b>SBUF_RAW</b> type does not support this function.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @return Returns the pointer to the IPC service object if the operation is successful;
 * returns a null pointer otherwise.
 *
 * @since 1.0
 */
struct HdfRemoteService *HdfSbufReadRemoteService(struct HdfSBuf *sbuf);

/**
 * @brief Reads a file descriptor from a <b>SBuf</b>.
 * The SBUF of the <b>SBUF_RAW</b> type does not support this function.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @return Returns a valid file descriptor if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int HdfSbufReadFileDescriptor(struct HdfSBuf *sbuf);

/**
 * @brief Reads a double-precision floating-point number from a <b>SBuf</b>.
 * The SBUF of the <b>SBUF_RAW</b> type does not support this function.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the double-precision floating-point number read,
 * which is requested by the caller.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadDouble(struct HdfSBuf *sbuf, double *value);

/**
 * @brief Reads a floating-point number from a <b>SBuf</b>.
 * The SBUF of the <b>SBUF_RAW</b> type does not support this function.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the floating-point number read, which is requested by the caller.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadFloat(struct HdfSBuf *sbuf, float *value);

/**
 * @brief Reads a wide character string from a <b>SBuf</b>.
 * The SBUF of the <b>SBUF_RAW</b> type does not support this function.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @return Returns the pointer to the wide character string if the operation is successful;
 * returns a null pointer otherwise.
 *
 * @since 1.0
 */
const char16_t *HdfSbufReadString16(struct HdfSBuf *sbuf);

/**
 * @brief Reads a data segment from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param data Indicates the double pointer to the data read. The data read is stored in <b>*data</b>,
 * which is requested by the caller. The memory pointed to by <b>*data</b> is managed by
 * the <b>SBuf</b> and they share the same lifecycle.
 * @param readSize Indicates the pointer to the size of the data read.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadBuffer(struct HdfSBuf *sbuf, const void **data, uint32_t *readSize);

/**
 * @brief Reads unpadded data from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param length Indicates the length of the data to read.
 * @return Returns the pointer to the unpadded data if the operation is successful; returns a null pointer otherwise.
 *
 * @since 1.0
 */
const uint8_t *HdfSbufReadUnpadBuffer(struct HdfSBuf *sbuf, size_t length);

/**
 * @brief Reads a 64-bit unsigned integer from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the 64-bit unsigned integer read, which is requested by the caller.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadUint64(struct HdfSBuf *sbuf, uint64_t *value);

/**
 * @brief Reads a 32-bit unsigned integer from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the 32-bit unsigned integer read, which is requested by the caller.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadUint32(struct HdfSBuf *sbuf, uint32_t *value);

/**
 * @brief Reads a 16-bit unsigned integer from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the 16-bit unsigned integer read, which is requested by the caller.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadUint16(struct HdfSBuf *sbuf, uint16_t *value);

/**
 * @brief Reads an 8-bit unsigned integer from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the 8-bit unsigned integer read, which is requested by the caller.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadUint8(struct HdfSBuf *sbuf, uint8_t *value);

/**
 * @brief Reads a 64-bit signed integer from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the 64-bit signed integer read, which is requested by the caller.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadInt64(struct HdfSBuf *sbuf, int64_t *value);

/**
 * @brief Reads a 32-bit signed integer from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the 32-bit signed integer read, which is requested by the caller.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadInt32(struct HdfSBuf *sbuf, int32_t *value);

/**
 * @brief Reads a 16-bit signed integer from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the 16-bit signed integer read, which is requested by the caller.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadInt16(struct HdfSBuf *sbuf, int16_t *value);

/**
 * @brief Reads an 8-bit signed integer from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param value Indicates the pointer to the 8-bit signed integer read, which is requested by the caller.
 * @return Returns <b>true</b> if the operation is successful; returns <b>false</b> otherwise.
 *
 * @since 1.0
 */
bool HdfSbufReadInt8(struct HdfSBuf *sbuf, int8_t *value);

/**
 * @brief Reads a string from a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @return Returns the pointer to the string read if the operation is successful; returns <b>NULL</b> otherwise.
 * The memory pointed to by this pointer is managed by the <b>SBuf</b> and they share the same lifecycle.
 *
 * @since 1.0
 */
const char *HdfSbufReadString(struct HdfSBuf *sbuf);

/**
 * @brief Obtains the pointer to the data stored in a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @return Returns the pointer to the data stored in the target <b>SBuf</b> if the operation is successful;
 * returns a null pointer otherwise.
 *
 * @since 1.0
 */
uint8_t *HdfSbufGetData(const struct HdfSBuf *sbuf);

/**
 * @brief Clears the data stored in a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 *
 * @since 1.0
 */
void HdfSbufFlush(struct HdfSBuf *sbuf);

/**
 * @brief Obtains the capacity of a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @return Returns the <b>SBuf</b> capacity.
 *
 * @since 1.0
 */
size_t HdfSbufGetCapacity(const struct HdfSBuf *sbuf);

/**
 * @brief Obtains the size of the data stored in a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @return Returns the data size.
 *
 * @since 1.0
 */
size_t HdfSbufGetDataSize(const struct HdfSBuf *sbuf);

/**
 * @brief Sets the data size of a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @param size Indicates the data size to set, which cannot exceed the size obtained via {@link HdfSbufGetDataSize}.
 *
 * @since 1.0
 */
void HdfSbufSetDataSize(struct HdfSBuf *sbuf, size_t size);

/**
 * @brief Obtains a <b>SBuf</b> instance.
 *
 * @param capacity Indicates the initial capacity of the <b>SBuf</b>.
 * @return Returns the <b>SBuf</b> instance.
 *
 * @since 1.0
 */
struct HdfSBuf *HdfSbufObtain(size_t capacity);

/**
 * @brief Obtains a <b>SBuf</b> instance of the default capacity (256 bytes).
 *
 * @return Returns the <b>SBuf</b> instance.
 *
 * @since 1.0
 */
struct HdfSBuf *HdfSbufObtainDefaultSize(void);

/**
 * @brief Creates a <b>SBuf</b> instance with the specified data and size.
 * The pointer to the data stored in the <b>SBuf</b> is released by the caller,
 * and the written data size should not exceed the specified value of <b>size</b>.
 *
 * @param base Indicates the base of the data to use.
 * @param size Indicates the size of the data to use.
 * @return Returns the <b>SBuf</b> instance.
 *
 * @since 1.0
 */
struct HdfSBuf *HdfSbufBind(uintptr_t base, size_t size);

/**
 * @brief Releases a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 *
 * @since 1.0
 */
void HdfSbufRecycle(struct HdfSBuf *sbuf);

/**
 * @brief Creates a <b>SBuf</b> instance with an original <b>SBuf</b>.
 * This function moves the data stored in the original <b>SBuf</b> to the new one without memory copy.
 *
 * @param sbuf Indicates the pointer to the original <b>SBuf</b>.
 * @return Returns the new <b>SBuf</b> instance.
 *
 * @since 1.0
 */
struct HdfSBuf *HdfSbufMove(struct HdfSBuf *sbuf);

/**
 * @brief Creates a <b>SBuf</b> instance with an original <b>SBuf</b>.
 * This function copies the data stored in the original <b>SBuf</b> to the new one.
 *
 * @param sbuf Indicates the pointer to the original <b>SBuf</b>.
 * @return Returns the new <b>SBuf</b> instance.
 *
 * @since 1.0
 */
struct HdfSBuf *HdfSbufCopy(const struct HdfSBuf *sbuf);

/**
 * @brief Transfers the data ownership to a <b>SBuf</b>. Once the <b>SBuf</b> is released,
 * the bound data memory is also released. This function is used together with {@link HdfSbufBind}.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 *
 * @since 1.0
 */
void HdfSbufTransDataOwnership(struct HdfSBuf *sbuf);

/**
 * @brief Obtains a <b>SBuf</b> instance of a specified type.
 *
 * @param type Indicates the SBUF type, which is defined in {@link HdfSbufType}.
 * @return Returns the <b>SBuf</b> instance.
 *
 * @since 1.0
 */
struct HdfSBuf *HdfSbufTypedObtain(uint32_t type);

/**
 * @brief Obtains a <b>SBuf</b> instance of a specified type based on the implementation of an existing <b>SBuf</b>.
 *
 * @param type Indicates the SBUF type, which is defined in {@link HdfSbufType}.
 * @param impl Indicates the pointer to the implementation of an existing <b>SBuf</b>.
 * @return Returns the new <b>SBuf</b> instance.
 *
 * @since 1.0
 */
struct HdfSBuf *HdfSbufTypedObtainInplace(uint32_t type, struct HdfSBufImpl *impl);

/**
 * @brief Obtains a <b>SBuf</b> instance of a specified type with the given initial capacity.
 *
 * @param type Indicates the SBUF type, which is defined in {@link HdfSbufType}.
 * @param capacity Indicates the initial capacity of the <b>SBuf</b>.
 * @return Returns the new <b>SBuf</b> instance.
 *
 * @since 1.0
 */
struct HdfSBuf *HdfSbufTypedObtainCapacity(uint32_t type, size_t capacity);

/**
 * @brief Creates a <b>SBuf</b> instance of a specified type with the specified data and size.
 * The pointer to the data stored in the <b>SBuf</b> is released by the caller,
 * and the written data size should not exceed the specified value of <b>size</b>.
 *
 * @param type Indicates the SBUF type, which is defined in {@link HdfSbufType}.
 * @param base Indicates the base of the data to use.
 * @param size Indicates the size of the data to use.
 * @return Returns the <b>SBuf</b> instance.
 *
 * @since 1.0
 */
struct HdfSBuf *HdfSbufTypedBind(uint32_t type, uintptr_t base, size_t size);

/**
 * @brief Obtains the implementation of a <b>SBuf</b>.
 *
 * @param sbuf Indicates the pointer to the target <b>SBuf</b>.
 * @return Returns the pointer to the implementation of the <b>SBuf</b>.
 *
 * @since 1.0
 */
struct HdfSBufImpl *HdfSbufGetImpl(struct HdfSBuf *sbuf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_SBUF_H */
/** @} */
