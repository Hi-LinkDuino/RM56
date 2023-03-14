/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef MESSAGE_ROUTER_H
#define MESSAGE_ROUTER_H
#include "message_config.h"
#include "osal/osal_mutex.h"
#include "sidecar.h"
#define DEFAULT_DISPATCHER_ID 0

#ifdef __cplusplus
extern "C" {
#endif

enum MessageNodeType {
    MESSAGE_NODE_CONFIG_DEFAULT = 0,
#ifdef KERNEL_SERVER_SUPPORT
    MESSAGE_NODE_REMOTE_KERNEL_SERVER = 2,
#endif
#ifdef USERSPACE_CLIENT_SUPPORT
    MESSAGE_NODE_REMOTE_USERSPACE_CLIENT = 4
#endif
};

ErrorCode StartMessageRouter(uint8_t nodesConfig);

ErrorCode ShutdownMessageRouter(void);

ErrorCode AddDispatcher(DispatcherConfig *config);

ErrorCode EnableDefaultDispatcher(void);

ErrorCode RegistLocalService(const DispatcherId dispatcherId, struct ServiceDef *mapper);

ErrorCode UnregistLocalService(const DispatcherId dispatcherId, ServiceId serviceId);

#ifdef __cplusplus
}
#endif

#endif
