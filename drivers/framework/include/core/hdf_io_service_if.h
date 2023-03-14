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
 * @brief Provides functions to use the Hardware Driver Foundation (HDF).
 *
 * The HDF implements driver framework capabilities such as driver loading, service management, driver message model,
 * and power management. You can develop drivers based on the HDF.
 *
 * @since 1.0
 */

/**
 * @file hdf_io_service_if.h
 *
 * @brief Declares the structures defining driver service objects and event listeners, as well as the functions
 * for obtaining a driver service object, dispatching a driver service call, and registering or unregistering
 * an event listener.
 *
 * @since 1.0
 */

#ifndef HDF_IO_SERVICE_IF_H
#define HDF_IO_SERVICE_IF_H

#include "hdf_dlist.h"
#include "hdf_object.h"
#include "hdf_sbuf.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct HdfDevEventlistener;
struct HdfIoService;

/**
 * @brief Enumerates different classes of driver devices.
 *
 * @since 1.0
 */
typedef enum {
    DEVICE_CLASS_DEFAULT = 0x1 << 0,    /** Default device */
    DEVICE_CLASS_PLAT    = 0x1 << 1,    /** Platform device */
    DEVICE_CLASS_SENSOR  = 0x1 << 2,    /** Sensor device */
    DEVICE_CLASS_INPUT   = 0x1 << 3,    /** Input device */
    DEVICE_CLASS_DISPLAY = 0x1 << 4,    /** Display device */
    DEVICE_CLASS_AUDIO   = 0x1 << 5,    /** Audio device */
    DEVICE_CLASS_CAMERA  = 0x1 << 6,    /** Camera device */
    DEVICE_CLASS_USB     = 0x1 << 7,    /** USB device */
    DEVICE_CLASS_MAX     = 0x1 << 8,    /** Maximum value of a device class */
} DeviceClass;

/**
 * @brief Called when a driver event occurs.
 *
 * You can implement this function and bind it to the custom {@link HdfDevEventlistener} object. \n
 *
 * @param priv Indicates the pointer to the private data bound to this listener.
 * @param id Indicates the serial number of the driver event occurred.
 * @param data Indicates the pointer to the content data of the driver event.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
typedef int (*OnEventReceived)(void *priv, uint32_t id, struct HdfSBuf *data);

/**
 * @brief Called when a driver event occurs.
 *
 * You can implement this function and bind it to the custom {@link HdfDevEventlistener} object. \n
 *
 * @param listener Indicates the pointer to the listener that receives the driver event.
 * @param service Indicates the pointer to the driver service object that generates the driver event.
 * @param id Indicates the serial number of the driver event occurred.
 * @param data Indicates the pointer to the content data of the driver event.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
typedef int (*OnDevEventReceive)(
    struct HdfDevEventlistener *listener, struct HdfIoService *service, uint32_t id, struct HdfSBuf *data);

/**
 * @brief Defines a driver event listener object.
 *
 * @since 1.0
 */
struct HdfDevEventlistener {
    /** Deprecated. Use {@link onReceive} instead. */
    OnEventReceived callBack;
    /** Callback invoked when the monitored device reports an event */
    OnDevEventReceive onReceive;
    /** Intrusive list node used by the HDF to manage listeners. You can ignore this node. */
    struct DListHead listNode;
    /** Private data of the listener */
    void *priv;
};

/**
 * @brief Defines a driver service call dispatcher.
 *
 * @since 1.0
 */
struct HdfIoDispatcher {
    /** Dispatches a driver service call. <b>service</b> indicates the pointer to the driver service object, <b>id</b>
    indicates the command word of the function, <b>data</b> indicates the pointer to the data you want to pass to
    the driver, and <b>reply</b> indicates the pointer to the data returned by the driver. */
    int (*Dispatch)(struct HdfObject *service, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply);
};

/**
 * @brief Defines a driver service object.
 *
 * @since 1.0
 */
struct HdfIoService {
    /** Base class object */
    struct HdfObject object;
    /** Pointer to the bound service entity, which is used for framework management. You can ignore it. */
    struct HdfObject *target;
    /** Service call dispatcher */
    struct HdfIoDispatcher *dispatcher;
    /** Private data of the service */
    void *priv;
};

/**
 * @brief Defines a driver service group object.
 *
 * @since 1.0
 */
struct HdfIoServiceGroup {
    /** Base class object */
    struct HdfObject object;
};

/**
 * @brief Obtains an instance of the driver service group object.
 *
 * @return Returns the pointer to the driver service group object if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
struct HdfIoServiceGroup *HdfIoServiceGroupObtain(void);

/**
 * @brief Destroys a driver service group object.
 *
 * @param group Indicates the pointer to the driver service group object to destroy.
 *
 * @since 1.0
 */
void HdfIoServiceGroupRecycle(struct HdfIoServiceGroup *group);

/**
 * @brief Adds a driver service object to a specified driver service group.
 *
 * @param group Indicates the pointer to the driver service group object to
 * which the driver service object is to be added.
 * @param service Indicates the pointer to the driver service object to add.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t HdfIoServiceGroupAddService(struct HdfIoServiceGroup *group, struct HdfIoService *service);

/**
 * @brief Removes a driver service object from a specified driver service group.
 *
 * @param group Indicates the pointer to the driver service group object from
 * which the driver service object is to be removed.
 * @param service Indicates the pointer to the driver service object to remove.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
void HdfIoServiceGroupRemoveService(struct HdfIoServiceGroup *group, struct HdfIoService *service);

/**
 * @brief Registers a custom {@link HdfDevEventlistener} for listening for events reported by driver services
 * in a specified driver service group object.
 *
 * @param group Indicates the pointer to the driver service group object to listen to,
 *              which is obtained via {@link HdfIoServiceGroupObtain}.
 * @param listener Indicates the pointer to the {@link HdfDevEventlistener} to register.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t HdfIoServiceGroupRegisterListener(struct HdfIoServiceGroup *group, struct HdfDevEventlistener *listener);

/**
 * @brief Unregisters a previously registered {@link HdfDevEventlistener} that is used for listening for events
 * reported by driver services in a specified driver service group object.
 *
 * @param group Indicates the pointer to the driver service group object that has been listened to.
 * @param listener Indicates the pointer to the {@link HdfDevEventlistener} to unregister.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t HdfIoServiceGroupUnregisterListener(struct HdfIoServiceGroup *group, struct HdfDevEventlistener *listener);

/**
 * @brief Obtains a driver service object.
 *
 *
 *
 * @param serviceName Indicates the pointer to the name of the service to obtain.
 * @return Returns the pointer to the driver service object if the operation is successful;
 * returns <b>NULL</b> otherwise.
 *
 * @since 1.0
 */
struct HdfIoService *HdfIoServiceBind(const char *serviceName);

/**
 * @brief Destroys a specified driver service object to release resources if it is no longer required.
 *
 * @param service Indicates the pointer to the driver service object to destroy.
 *
 * @since 1.0
 */
void HdfIoServiceRecycle(struct HdfIoService *service);

/**
 * @brief Registers a custom {@link HdfDevEventlistener} for listening for events reported
 * by a specified driver service object.
 *
 * @param target Indicates the pointer to the driver service object to listen, which is obtained through
 * the {@link HdfIoServiceBind} function.
 * @param listener Indicates the pointer to the listener to register.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int HdfDeviceRegisterEventListener(struct HdfIoService *target, struct HdfDevEventlistener *listener);

/**
 * @brief Unregisters a previously registered {@link HdfDevEventlistener} to release resources
 * if it is no longer required.
 *
 * @param target Indicates the pointer to the driver service object that has been listened.
 * @param listener Indicates the listener object registered by {@link HdfDeviceRegisterEventListener}.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int HdfDeviceUnregisterEventListener(struct HdfIoService *target, struct HdfDevEventlistener *listener);

/**
 * @brief Obtains the number of event listeners that are registered for a specified driver service object.
 *
 * @param service Indicates the pointer to the driver service object for which the event listeners are registered.
 * @return Returns the number of listeners if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int HdfIoserviceGetListenerCount(const struct HdfIoService *service);

/**
 * @brief Obtains the number of event listeners that are registered for a specified driver service group object.
 *
 * @param group Indicates the pointer to the driver service group object for which the event listeners are registered.
 * @return Returns the number of listeners if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int HdfIoserviceGroupGetListenerCount(const struct HdfIoServiceGroup *group);

/**
 * @brief Obtains the number of driver services included in a specified driver service group.
 *
 * @param group Indicates the pointer to the driver service group object.
 * @return Returns the number of services if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int HdfIoserviceGroupGetServiceCount(const struct HdfIoServiceGroup *group);

/**
 * @brief Obtains the names of device services of a specified device class defined by {@link DeviceClass}.
 *
 *
 * @param deviceClass Indicates the device class to query.
 * @param reply Indicates the pointer to the service name, which is stored as a string in an <b>HdfSBuf</b> structure.
 * You can use {@link HdfSbufReadString} to continuously read the service names until a null pointer appears.
 * @return Returns <b>0</b> if the operation is successful; returns a negative value otherwise.
 *
 * @since 1.0
 */
int32_t HdfGetServiceNameByDeviceClass(DeviceClass deviceClass, struct HdfSBuf *reply);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HDF_IO_SERVICE_IF_H */
/** @} */
