/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

/**
 * @addtogroup USB
 * @{
 *
 * @brief Declares USB-related APIs, including the custom data types and functions
 * used to obtain descriptors, interface objects, and request objects, and to submit requests.
 *
 * @since 3.0
 * @version 1.0
 */

/**
 * @file usbd_client.h
 *
 * @brief Defines usbd APIs.
 *
 * @since 3.0
 * @version 1.0
 */

#ifndef USBD_CLIENT_H
#define USBD_CLIENT_H

#include "usb_param.h"
#include "usbd_subscriber.h"

namespace OHOS {
namespace USB {
class UsbdClient {
public:
    static UsbdClient &GetInstance();

    /* *
     * @brief Opens a USB device to set up a connection.
     *
     * @param dev Indicates the USB device address.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t OpenDevice(const UsbDev &dev);

    /* *
     * @brief Closes a USB device to release all system resources related to the device.
     *
     * @param dev Indicates the USB device address.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t CloseDevice(const UsbDev &dev);

    /* *
     * @brief Obtains the USB device descriptor.
     *
     * @param dev Indicates the USB device address.
     * @param descriptor Indicates the USB device descriptor.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t GetDeviceDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);

    /* *
     * @brief Obtains the string descriptor of a USB device based on the specified string ID.
     *
     * @param dev Indicates the USB device address.
     * @param descId Indicates string descriptor ID.
     * @param descriptor Indicates the configuration descriptor of the USB device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t GetStringDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);

    /* *
     * @brief Obtains the configuration descriptor of a USB device based on the specified config ID.
     *
     * @param dev Indicates the USB device address.
     * @param descId Indicates configuration descriptor ID.
     * @param descriptor Indicates the configuration descriptor of the USB device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t GetConfigDescriptor(const UsbDev &dev, uint8_t descId, std::vector<uint8_t> &decriptor);

    /* *
     * @brief Obtains the raw descriptor.
     *
     * @param dev Indicates the USB device address.
     * @param descriptor Indicates the raw descriptor of the USB device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t GetRawDescriptor(const UsbDev &dev, std::vector<uint8_t> &decriptor);
    
    /* *
     * @brief Obtains the file descriptor.
     *
     * @param dev Indicates the USB device address.
     * @param fd USB device file descriptor
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t GetFileDescriptor(const UsbDev &dev, int32_t &fd);

    /* *
     * @brief Sets the configuration information of a USB device.
     *
     * @param dev Indicates the USB device address.
     * @param configIndex Indicates the configuration information of the USB device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t SetConfig(const UsbDev &dev, uint8_t configIndex);

    /* *
     * @brief Obtains the configuration information of a USB device.
     *
     * @param dev Indicates the USB device address.
     * @param configIndex Indicates the configuration information of the USB device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t GetConfig(const UsbDev &dev, uint8_t &configIndex);

    /* *
     * @brief Claims a USB interface exclusively. This must be done before data transfer.
     *
     * @param dev Indicates the USB device address.
     * @param interfaceid Indicates the interface ID of the USB device.
     * @param force Indicates whether to enable the force mode. The value <b>1</b> indicates that the force mode is
     * enabled, and value <b>0</b> indicates the opposite.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t ClaimInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t force);

    /* *
     * @brief Releases a USB interface. This is usually done after data transfer.
     *
     * @param dev Indicates the USB device address.
     * @param interfaceid Indicates the interface ID of the USB device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t ReleaseInterface(const UsbDev &dev, uint8_t interfaceid);

    /* *
     * @brief Sets the alternate settings for the specified USB interface. This allows you to switch between two
     * interfaces with the same ID but different alternate settings.
     *
     * @param dev Indicates the USB device address.
     * @param interfaceid Indicates the interface ID of the USB device.
     * @param altIndex Indicates the alternate settings of the USB interface.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t SetInterface(const UsbDev &dev, uint8_t interfaceid, uint8_t altIndex);

    /* *
     * @brief Reads data on a specified endpoint during bulk transfer. The endpoint must be in the data reading
     * direction. You can specify a timeout duration if needed.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     * @param timeout Indicates the timeout duration.
     * @param data Indicates the read data.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t BulkTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, std::vector<uint8_t> &data);

    /* *
     * @brief Writes data on a specified endpoint during bulk transfer. The endpoint must be in the data writing
     * direction.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     * @param timeout Indicates the timeout duration.
     * @param data Indicates the written data.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t BulkTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout,
        const std::vector<uint8_t> &data);

    /* *
     * @brief Performs control transfer for endpoint 0 of the device. The data transfer direction is determined by the
     * request type. If the result of <b>requestType</b>&
     * <b>USB_ENDPOINT_DIR_MASK</b> is <b>USB_DIR_OUT</b>, the endpoint is in the data writing direction; if the result
     * is <b>USB_DIR_IN</b>, the endpoint is in the data reading direction.
     *
     * @param dev Indicates the USB device address.
     * @param ctrl Indicates the control data packet structure.
     * @param data Indicates the read or written data.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t ControlTransfer(const UsbDev &dev, const UsbCtrlTransfer &ctrl, std::vector<uint8_t> &data);

    /* *
     * @brief Reads data on a specified endpoint during interrupt transfer. The endpoint must be in the data reading
     * direction.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     * @param timeout Indicates the timeout duration.
     * @param data Indicates the read data.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t InterruptTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, std::vector<uint8_t> &data);

    /* *
     * @brief Writes data on a specified endpoint during interrupt transfer. The endpoint must be in the data writing
     * direction.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     * @param timeout Indicates the timeout duration.
     * @param data Indicates the read data.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t InterruptTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, std::vector<uint8_t> &data);

    /* *
     * @brief Reads data on a specified endpoint during isochronous transfer. The endpoint must be in the data reading
     * direction.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     * @param timeout Indicates the timeout duration.
     * @param data Indicates the read data.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t IsoTransferRead(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, std::vector<uint8_t> &data);

    /* *
     * @brief Writes data on a specified endpoint during isochronous transfer. The endpoint must be in the data writing
     * direction.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     * @param timeout Indicates the timeout duration.
     * @param data Indicates the read data.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t IsoTransferWrite(const UsbDev &dev, const UsbPipe &pipe, int32_t timeout, std::vector<uint8_t> &data);

    /* *
     * @brief Sends or receives requests for isochronous transfer on a specified endpoint. The data transfer direction
     * is determined by the endpoint direction.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     * @param clientData Indicates the client data.
     * @param buffer Indicates the transferred data.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t RequestQueue(const UsbDev &dev, const UsbPipe &pipe, const std::vector<uint8_t> &clientData,
        const std::vector<uint8_t> &buffer);

    /* *
     * @brief Waits for the operation result of the isochronous transfer request in <b>RequestQueue</b>.
     *
     * @param dev Indicates the USB device address.
     * @param clientData Indicates the client data.
     * @param buffer Indicates the transferred data.
     * @param timeout Indicates the timeout duration.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t RequestWait(const UsbDev &dev, std::vector<uint8_t> &clientData, std::vector<uint8_t> &buffer,
        int32_t timeout);

    /* *
     * @brief Cancels the data transfer requests to be processed.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t RequestCancel(const UsbDev &dev, const UsbPipe &pipe);

    /* *
     * @brief Obtains the list of functions (represented by bit field) supported by the current device.
     *
     * @param funcs Indicates the list of functions supported by the current device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t GetCurrentFunctions(int32_t &funcs);

    /* *
     * @brief Sets the list of functions (represented by bit field) supported by the current device.
     *
     * @param funcs Indicates the list of functions supported by the current device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t SetCurrentFunctions(int32_t funcs);

    /* *
     * @brief Closes a USB device to release all system resources related to the device.
     *
     * @param portId Indicates the port ID of the USB interface.
     * @param powerRole Indicates the power role.
     * @param dataRole Indicates the data role.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t SetPortRole(int32_t portId, int32_t powerRole, int32_t dataRole);

    /* *
     * @brief Queries the current settings of a port.
     *
     * @param portId Indicates the port ID of the USB interface.
     * @param powerRole Indicates the power role.
     * @param dataRole Indicates the data role.
     * @param mode Indicates the mode.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t QueryPort(int32_t &portId, int32_t &powerRole, int32_t &dataRole, int32_t &mode);

    /* *
     * @brief Binds a subscriber.
     *
     * @param subscriber Indicates the subscriber.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t BindUsbdSubscriber(const sptr<UsbdSubscriber> &subscriber);

    /* *
     * @brief Unbinds a subscriber.
     *
     * @param subscriber Indicates the subscriber.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t UnbindUsbdSubscriber(const sptr<UsbdSubscriber> &subscriber);

    /* *
     * @brief Registers a callback for isochronous bulk transfer.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     * @param cb Indicates the reference to the callback.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t RegBulkCallback(const UsbDev &dev, const UsbPipe &pipe, const sptr<IRemoteObject> &cb);

    /* *
     * @brief Unregisters the callback for isochronous bulk transfer.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t UnRegBulkCallback(const UsbDev &dev, const UsbPipe &pipe);

    /* *
     * @brief Reads data during isochronous bulk transfer.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     * @param ashmem Indicates the shared memory, which is used to store the read data.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t BulkRead(const UsbDev &dev, const UsbPipe &pipe, sptr<Ashmem> &ashmem);

    /* *
     * @brief Writes data during isochronous bulk transfer.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     * @param ashmem Indicates the shared memory, which is used to store the written data.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t BulkWrite(const UsbDev &dev, const UsbPipe &pipe, sptr<Ashmem> &ashmem);

    /* *
     * @brief Cancels the isochronous bulk transfer. The read and write operations on the current USB interface will
     * be cancelled.
     *
     * @param dev Indicates the USB device address.
     * @param pipe Indicates the pipe of the USB device.
     *
     * @return Returns <b>0</b> if the operation is successful; returns a non-0 value if the operation fails.
     * @since 3.0
     */
    int32_t BulkCancel(const UsbDev &dev, const UsbPipe &pipe);

private:
    int32_t SetDeviceMessage(MessageParcel &data, const UsbDev &dev);
    int32_t SetBufferMessage(MessageParcel &data, const std::vector<uint8_t> &bufferData);
    int32_t GetBufferMessage(MessageParcel &data, std::vector<uint8_t> &bufferData);
    sptr<IRemoteObject> GetUsbdService();
    int32_t DoDispatch(uint32_t cmd, MessageParcel &data, MessageParcel &reply);
    static inline const std::u16string metaDescriptor_ = u"hdf.usb.usbd";
    static inline const std::u16string &GetDescriptor()
    {
        return metaDescriptor_;
    }
};
} // namespace USB
} // namespace OHOS
#endif // USBD_CLIENT_H