/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

/**
 * @addtogroup WLAN
 * @{
 *
 * @brief Provides cross-OS migration, component adaptation, and modular assembly and compilation.
 *
 * Based on the unified APIs provided by the WLAN module, developers of the Hardware Driver Interface
 * (HDI) are capable of creating, disabling, scanning for, and connecting to WLAN hotspots, managing WLAN chips,
 * network devices, and power, and applying for, releasing, and moving network data buffers.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file flow_control.h
 *
 * @brief Declares flow control modules and provides functions such as initializing and deinitializing a
 * flow control module, and transmitting and receiving data packets based on the flow control priority.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef WIFI_FLOW_CONTROL_H
#define WIFI_FLOW_CONTROL_H
#include "osal_sem.h"
#include "osal_spinlock.h"
#include "osal_thread.h"
#include "hdf_netbuf.h"

/**
 * @brief Enumerates flow control queue IDs.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    CTRL_QUEUE_ID = 0,  /**< Control queue ID */
    VIP_QUEUE_ID,       /**< VIP queue ID */
    NORMAL_QUEUE_ID,    /**< Normal queue ID */
    TCP_DATA_QUEUE_ID,  /**< TCP data queue ID */
    TCP_ACK_QUEUE_ID,   /**< TCP ACK queue ID */
    BK_QUEUE_ID,        /**< Background flow queue ID */
    BE_QUEUE_ID,        /**< Best-effort flow queue ID */
    VI_QUEUE_ID,        /**< Video flow queue ID */
    VO_QUEUE_ID,        /**< Voice flow queue ID */
    QUEUE_ID_COUNT      /**< Total number of queue IDs */
} FlowControlQueueID;

/**
 * @brief Enumerates flow directions.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    FLOW_TX = 0,    /**< Transmit */
    FLOW_RX,        /**< Receive */
    FLOW_DIR_COUNT  /**< Total number of flow directions */
} FlowDir;

/**
 * @brief Enumerates flow control thread statuses.
 *
 * @since 1.0
 * @version 1.0
 */
typedef enum {
    THREAD_INIT_FAIL = 0,  /**< Failed to initialize */
    THREAD_INIT_SUCCESS,   /**< Initialized successfully */
    THREAD_STARTING,       /**< Starting */
    THREAD_WAITING,        /**< Waiting */
    THREAD_RUNNING,        /**< Running */
    THREAD_STOPPING,       /**< Stopping */
    THREAD_STOPPED,        /**< Stopped */
    THREAD_DESTROYED,      /**< Destroyed */
    THREAD_STATUS_COUNT    /**< Total number of thread statuses */
}FcThreadStatus;

/**
 * @brief Describes a flow control queue.
 *
 * @since 1.0
 * @version 1.0
 */
struct FlowControlQueue {
    FlowControlQueueID queueID;  /**< Flow control queue ID */
    NetBufQueue dataQueue;       /**< Network data queue */
    uint32_t queueThreshold;     /**< Network data queue threshold */
    OsalSpinlock lock;           /**< Queue lock */
    uint32_t pktCount;           /**< Number of packets received by the network data queue */
};

/**
 * @brief Manages flow control queues.
 *
 * @since 1.0
 * @version 1.0
 */
struct FlowControlQueues {
    struct FlowControlQueue queues[QUEUE_ID_COUNT];  /**< Array of flow control queues */
};


/**
 * @brief Provides flow control operations.
 *
 * @since 1.0
 * @version 1.0
 */
struct FlowControlOp {
    /**
     * @brief Checks whether the device is a station or P2P client.
     *
     * @return Returns <b>true</b> if the device is a station or P2P client; returns <b>false</b> otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    bool (*isDeviceStaOrP2PClient)(void);

    /**
     * @brief Transmits data packets.
     *
     * @param q Indicates the pointer to the network data queue.
     * @param fcmPrivate Indicates the pointer to the private structure in the WLAN driver for
     * transmitting data packets.
     * @param fwPriorityId Indicates the ID of the flow control priority.
     * @return Returns <b>0</b> if data packets are transmitted; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*txDataPacket)(NetBufQueue *q, void *fcmPrivate, int32_t fwPriorityId);

    /**
     * @brief Receives data packets.
     *
     * @param q Indicates the pointer to the network data queue.
     * @param fcmPrivate Indicates the pointer to the private structure in the WLAN driver for receiving data packets.
     * @param fwPriorityId Indicates the ID of the flow control priority.
     * @return Returns <b>0</b> if data packets are received; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*rxDataPacket)(NetBufQueue *q, void *fcmPrivate, int32_t fwPriorityId);

    /**
     * @brief Obtains the transmit flow control queue ID defined by the driver vendor.
     *
     * @param para Indicates the pointer to the structure defined by the driver and used to obtain the transmit
     * flow control queue ID.
     * @return Returns the pointer to the transmit flow control queue ID, as enumerated in {@link FlowControlQueueID}.
     *
     * @since 1.0
     * @version 1.0
     */
    FlowControlQueueID (*getTxQueueId)(const void *para);

    /**
     * @brief Obtains the receive flow control queue ID defined by the driver vendor.
     *
     * @param para Indicates the pointer to the structure defined by the driver and used to obtain the receive
     * flow control queue ID.
     * @return Returns the pointer to the receive flow control queue ID, as enumerated in {@link FlowControlQueueID}.
     *
     * @since 1.0
     * @version 1.0
     */
    FlowControlQueueID (*getRxQueueId)(const void *para);

    /**
     * @brief Obtains the ID of the priority of a specified transmit flow control queue.
     *
     * @param id Indicates the ID of the transmit flow control queue.
     * @return Returns <b>0</b> if the priority ID is obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getTxPriorityId)(FlowControlQueueID id);

    /**
     * @brief Obtains the ID of the priority of a specified receive flow control queue.
     *
     * @param id Indicates the ID of the receive flow control queue.
     * @return Returns <b>0</b> if the priority ID is obtained; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*getRxPriorityId)(FlowControlQueueID id);
};

/**
 * @brief Describes a flow control module.
 *
 * @since 1.0
 * @version 1.0
 */
struct FlowControlModule {
    OSAL_DECLARE_THREAD(txTransferThread);              /**< Transmit flow transfer thread */
    OSAL_DECLARE_THREAD(rxTransferThread);              /**< Receive flow transfer thread */
    struct OsalSem sem[FLOW_DIR_COUNT];                 /**< Array of semaphores */
    FcThreadStatus threadStatus[FLOW_DIR_COUNT];        /**< Array of flow control thread statuses */
    struct FlowControlQueues fcmQueue[FLOW_DIR_COUNT];  /**< Array of flow control queues */
    struct FlowControlOp *op;                           /**< Flow control operation */
    struct FlowControlInterface *interface;             /**< Flow control function */
    void *fcmPriv;                                      /**< Private data of the flow control module */
};

/**
 * @brief Provides flow control functions, such as obtaining the queue ID and registering a flow control operation API.
 *
 * @since 1.0
 * @version 1.0
 */
struct FlowControlInterface {
    /**
     * @brief Obtains the flow control queue ID based on the network data buffer.
     *
     * @param buff Indicates the pointer to the network data buffer.
     * @return Returns the {@link FlowControlQueueID}.
     *
     * @since 1.0
     * @version 1.0
     */
    FlowControlQueueID (*getQueueIdByEtherBuff)(const NetBuf *buff);

    /**
     * @brief Sets the threshold for a specified flow control queue.
     *
     * @param fcm Indicates the pointer to the {@link FlowControlModule} that contains the flow control queue.
     * @param queueThreshold Indicates the threshold to set.
     * @param id Indicates the ID of the flow control queue.
     * @param dir Indicates the flow control direction, as enumerated in {@link FlowDir}.
     * @return Returns <b>0</b> if the threshold is set; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*setQueueThreshold)(struct FlowControlModule *fcm, uint32_t queueThreshold, uint32_t id, uint32_t dir);

    /**
     * @brief Sends data to a specified {@link FlowControlModule}.
     *
     * @param fcm Indicates the pointer to the {@link FlowControlModule}.
     * @param buff Indicates the pointer to the buffer that stores the data to send.
     * @param id Indicates the ID of the flow control queue.
     * @param dir Indicates the flow control direction, as enumerated in {@link FlowDir}.
     * @return Returns <b>0</b> if the data is sent; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*sendBuffToFCM)(struct FlowControlModule *fcm, NetBuf *buff, uint32_t id, uint32_t dir);

    /**
     * @brief Schedules a specified {@link FlowControlModule}.
     *
     * @param fcm Indicates the pointer to the {@link FlowControlModule}.
     * @param dir Indicates the flow control direction, as enumerated in {@link FlowDir}.
     * @return Returns <b>0</b> if the {@link FlowControlModule} is scheduled; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*schedFCM)(struct FlowControlModule *fcm, FlowDir dir);

    /**
     * @brief Registers a specified flow control operation API.
     *
     * @param fcm Indicates the pointer to the {@link FlowControlModule}.
     * @param op Indicates the pointer to the flow control operation API to register.
     * @return Returns <b>0</b> if the API is registered; returns a negative value otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t (*registerFlowControlOp)(struct FlowControlModule *fcm, struct FlowControlOp *op);
};

/**
 * @brief Processes Ethernet data.
 *
 * @param buff Indicates the pointer to the data to process.
 * @param len Indicates the data length.
 *
 * @return Returns the {@link FlowControlQueueID}.
 *
 * @since 1.0
 * @version 1.0
 */
typedef FlowControlQueueID (*EtherTypeProcessFun)(const void *buff, uint32_t len);

/**
 * @brief Describes how to process Ethernet data.
 *
 * @since 1.0
 * @version 1.0
 */
struct EtherProcessMap {
    uint16_t etherType;              /**< Ethernet data type */
    EtherTypeProcessFun processFun;  /**< Function for processing Ethernet data */
};

/**
 * @brief Initializes a specified {@link FlowControlModule}.
 *
 * @param fcmPriv Indicates the pointer to the private data of the {@link FlowControlModule}.
 *
 * @return Returns the pointer to the initialized {@link FlowControlModule}.
 *
 * @since 1.0
 * @version 1.0
 */
struct FlowControlModule *InitFlowControl(void *fcmPriv);

/**
 * @brief Obtains a {@link FlowControlModule}.
 *
 * @return Returns the pointer to the {@link FlowControlModule}.
 *
 * @since 1.0
 * @version 1.0
 */
struct FlowControlModule *GetFlowControlModule(void);

/**
 * @brief Deinitializes a specified {@link FlowControlModule}.
 *
 * @param fcm Indicates the pointer to the {@link FlowControlModule}.
 *
 * @since 1.0
 * @version 1.0
 */
void DeInitFlowControl(struct FlowControlModule *fcm);

/**
 * @brief Sends a flow control queue.
 *
 * @param fcm Indicates the pointer to the {@link FlowControlModule} that contains the flow control queue.
 * @param id Indicates the ID of the flow control queue.
 * @param dir Indicates the flow control direction, as enumerated in {@link FlowDir}.
 *
 * @return Returns <b>0</b> if the flow control queue is sent; returns a negative value otherwise.
 *
 * @since 1.0
 * @version 1.0
 */
int32_t SendFlowControlQueue(struct FlowControlModule *fcm, uint32_t id, uint32_t dir);

#endif /* WIFI_FLOW_CONTROL_H */
/** @} */
