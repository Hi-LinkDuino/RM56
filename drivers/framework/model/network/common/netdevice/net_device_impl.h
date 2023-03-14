/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDF_NET_DEVICE_IMPL_MODULE_H
#define HDF_NET_DEVICE_IMPL_MODULE_H

#include "net_device.h"

#define MAX_NETDEVICE_COUNT 20

struct NetDeviceImpl {
    struct NetDevice *netDevice;
    struct NetDeviceImplOp *interFace;
    void *osPrivate;
};
typedef enum {
    NO_IN_INTERRUPT,
    IN_INTERRUPT,
    MAX_RECEIVE_FLAG
} ReceiveFlag;

struct NetDeviceImplOp {
    int32_t (*init)(struct NetDeviceImpl *netDevice);
    int32_t (*deInit)(struct NetDeviceImpl *netDevice);
    int32_t (*add)(struct NetDeviceImpl *netDevice);
    int32_t (*delete)(struct NetDeviceImpl *netDevice);
    int32_t (*setStatus)(struct NetDeviceImpl *netDevice, NetIfStatus status);
    int32_t (*setLinkStatus)(struct NetDeviceImpl *netDevice, NetIfLinkStatus status);
    int32_t (*getLinkStatus)(struct NetDeviceImpl *netDevice, NetIfLinkStatus *status);
    int32_t (*receive)(struct NetDeviceImpl *netDevice, NetBuf *buff, ReceiveFlag flag);
    int32_t (*setIpAddr)(struct NetDeviceImpl *netDevice, const IpV4Addr *ipAddr, const IpV4Addr *netMask,
        const IpV4Addr *gw);
    int32_t (*dhcpsStart)(struct NetDeviceImpl *netDevice, char *ip, uint16_t ipNum);
    int32_t (*dhcpsStop)(struct NetDeviceImpl *netDevice);
    int32_t (*dhcpStart)(struct NetDeviceImpl *netDevice);
    int32_t (*dhcpStop)(struct NetDeviceImpl *netDevice);
    int32_t (*dhcpIsBound)(struct NetDeviceImpl *netDevice);
    int32_t (*changeMacAddr)(struct NetDeviceImpl *netDevice);
};

#endif /* HDF_NET_DEVICE_IMPL_MODULE_H */