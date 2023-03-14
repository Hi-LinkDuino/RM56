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

#include "wlan_hal_proxy.h"
#include "Iwifi_hal.h"
#include "wlan_hdi_service_stub.h"
#include "iservmgr_hdi.h"
#include "securec.h"
#include "hdf_log.h"
#include <osal_mem.h>
#include <message_parcel.h>
#include <cstring>

using namespace OHOS;
using namespace OHOS::HDI::WLAN::V1_0;
using namespace std;
#define ETH_ADDR_LEN 6

std::function<int32_t(int32_t event, struct HdfSBuf *sbuf)> WlanInterfaceProxy::callback_ = nullptr;

sptr<IWlan> IWlan::Get(const std::string& serviceName)
{
    do {
        using namespace OHOS::HDI::ServiceManager::V1_0;
        auto servMgr = IServiceManager::Get();
        if (servMgr == nullptr) {
            break;
        }
        sptr<IRemoteObject> remote = servMgr->GetService(serviceName.c_str());
        if (remote != nullptr) {
            return iface_cast<IWlan>(remote);
        }
    } while (false);
    HDF_LOGE("%s: get DataWlanService failed!", __func__);
    return nullptr;
}

int32_t WlanInterfaceProxy::wifiConstruct()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor())) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_CONSTRUCT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    return ret;
}

int32_t WlanInterfaceProxy::wifiDestruct()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor())) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_DECONSTRUCT, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    return ret;
}

int32_t WlanInterfaceProxy::start()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor())) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_START, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    return ret;
}

int32_t WlanInterfaceProxy::stop()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor())) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_STOP, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    return ret;
}

int32_t WlanInterfaceProxy::createFeature(int32_t type, std::shared_ptr<WifiFeatureInfo>& ifeature)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteInt32(type)) {
        HDF_LOGE("%s: write type failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ifeature = std::make_shared<WifiFeatureInfo>();
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_CREATE_FEATURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }

    const char *name = reply.ReadCString();
    if (name == nullptr) {
        HDF_LOGE("%s: read ifName failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ifeature->ifName = strdup(name);

    int32_t wlan_type = reply.ReadInt32();
    ifeature->type = wlan_type;
    return ret;
}

int32_t WlanInterfaceProxy::getSupportFeature(std::vector<uint8_t>& supType)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor())) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_GET_SUPPORT_FEATURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    uint32_t size = reply.ReadUint32();
    uint8_t *supTypeCp = (uint8_t *)reply.ReadUnpadBuffer(size);
    supType.assign(supTypeCp, supTypeCp + size);
    return ret;
}

int32_t WlanInterfaceProxy::getSupportCombo(std::vector<uint64_t>& combo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor())) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_GET_SUPPORT_COMBO, data, reply, option);
    if (ret == HDF_ERR_NOT_SUPPORT) {
        HDF_LOGW("%s: not support to getting combo!", __func__);
        return HDF_ERR_NOT_SUPPORT;
    } else if (ret == HDF_SUCCESS) {
        for (int i = 0; i < ETH_ADDR_LEN; i++) {
            combo.push_back(reply.ReadUint64());
        }
    }
    return ret;
}

int32_t WlanInterfaceProxy::getFeatureByIfName(std::string& ifName, std::shared_ptr<WifiFeatureInfo>& ifeature)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifName.c_str())) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_GET_FEATURE_NAME, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    int32_t wlan_type = reply.ReadInt32();
    ifeature->type = wlan_type;
    return ret;
}

int32_t WlanInterfaceProxy::CallbackWlanProxy(int32_t event, struct HdfSBuf *reqData)
{
    HDF_LOGI("%s: enter", __func__);
    if (reqData == nullptr) {
        HDF_LOGE("%s: ptr NULL", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    callback_(event, reqData);
    return HDF_SUCCESS;
}

int IPCObjectStubWlan::OnRemoteRequest(uint32_t code, OHOS::MessageParcel &data,
    OHOS::MessageParcel &reply, OHOS::MessageOption &option)
{
    int32_t ret = 0;
    int32_t status = -1;

    HDF_LOGI("IPCObjectStubWlan::OnRemoteRequest called, code = %u", code);
    struct HdfSBuf *dataSbuf = HdfSbufTypedObtain(SBUF_IPC);
    if (dataSbuf == nullptr) {
        HDF_LOGE("%s: dataSbuf malloc failed!", __func__);
        HdfSbufRecycle(dataSbuf);
    }
    switch (code) {
        case WIFI_EVENT_RESET_DRIVER: {
            const char *name = data.ReadCString();
            if (name == nullptr) {
                HDF_LOGE("%s: read ifName falied!", __func__);
                ret = HDF_ERR_INVALID_PARAM;
                break;
            }
            HDF_LOGI("IPCObjectStubWlan::OnRemoteRequest called, ifName = %{public}s", name);
            status = data.ReadInt32();
            if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
                !data.WriteInt32(status)) {
                HDF_LOGE("%s: write status failed!", __func__);
                ret = HDF_ERR_INVALID_PARAM;
                break;
            }
            ret = WlanInterfaceProxy::CallbackWlanProxy((int32_t)code, dataSbuf);
            if (ret != 0) {
                HDF_LOGE("%s:  failed, error code is %d", __func__, ret);
            }
            break;
        }
        default:
            HDF_LOGE("%s: unknown code:%d", __func__, code);
            ret = HDF_FAILURE;
            break;
    }
    HdfSbufRecycle(dataSbuf);
    return ret;
}

int32_t WlanInterfaceProxy::registerEventCallback(std::function<int32_t(int32_t event, struct HdfSBuf *sbuf)> cb)
{
    sptr<IPCObjectStub> callback = new IPCObjectStubWlan();
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor())) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = data.WriteRemoteObject(callback);
    if (!ret) {
        HDF_LOGE("%s: set callback write remote obj failed!", __func__);
        return HDF_FAILURE;
    }
    ret = Remote()->SendRequest(WLAN_SERVICE_REGISTER_CALLBACK, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    callback_ = cb;
    return ret;
}

int32_t WlanInterfaceProxy::unregisterEventCallback()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor())) {
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_UNREGISTER_CALLBACK, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    return ret;
}

int32_t WlanInterfaceProxy::destroyFeature(std::shared_ptr<WifiFeatureInfo> ifeature)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifeature->ifName)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!data.WriteInt32(ifeature->type)) {
        HDF_LOGE("%s: write type failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_DESTROY_FEATURE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    if (ifeature->ifName != NULL) {
        free(ifeature->ifName);
    }
    return ret;
}

int32_t WlanInterfaceProxy::resetDriver(const uint8_t chipId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteUint8(chipId)) {
        HDF_LOGE("%s: write chipid failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_RESET_DRIVER, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    return ret;
}

int32_t WlanInterfaceProxy::getAsscociatedStas(std::shared_ptr<WifiFeatureInfo> ifeature,
    std::shared_ptr<StaInfo> staInfo, uint32_t count, std::vector<uint32_t>& num)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    HDF_LOGI("%s: enter GetAsscociatedStas", __func__);
    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifeature->ifName)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_GET_ASSCOCIATE_STA, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    count = reply.ReadUint32();
    return ret;
}

int32_t WlanInterfaceProxy::setCountryCode(std::shared_ptr<WifiFeatureInfo> ifeature, std::string& code, uint32_t len)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifeature->ifName)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!data.WriteCString(code.c_str())) {
        HDF_LOGE("%s: write code failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_SET_COUNTRY_CODE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    return ret;
}

int32_t WlanInterfaceProxy::getNetworkIfaceName(std::shared_ptr<WifiFeatureInfo> ifeature)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifeature->ifName)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_GET_NETWORK_NAME, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
        return ret;
    }
    const char *name = reply.ReadCString();
    if (name == nullptr) {
        HDF_LOGE("%s:read ifName failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ifeature->ifName = strdup(name);
    return ret;
}

int32_t WlanInterfaceProxy::getFeatureType(std::shared_ptr<WifiFeatureInfo> ifeature)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteInt32(ifeature->type)) {
        HDF_LOGE("%s: write type failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_GET_FEATURE_TYPE, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    int32_t wifi_type = reply.ReadInt32();
    ifeature->type = wifi_type;
    return ret;
}

int32_t WlanInterfaceProxy::setMacAddress(std::shared_ptr<WifiFeatureInfo> ifeature, std::vector<uint8_t>& mac)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifeature->ifName)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    for (uint8_t i = 0; i < mac.size(); i++) {
        if (!data.WriteUint8(mac[i])) {
            HDF_LOGE("%s: write mac failed!", __func__);
        }
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_SET_MAC_ADDR, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    return ret;
}

int32_t WlanInterfaceProxy::getDeviceMacAddress(std::shared_ptr<WifiFeatureInfo> ifeature, std::vector<uint8_t>& mac,
    uint8_t len)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifeature->ifName)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!data.WriteInt32(ifeature->type)) {
        HDF_LOGE("%s: write type failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_GET_MAC_ADDR, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    uint8_t *macBuff = (uint8_t *)reply.ReadUnpadBuffer(len);
    mac.assign(macBuff, macBuff + len);
    return ret;
}

int32_t WlanInterfaceProxy::getFreqsWithBand(std::shared_ptr<WifiFeatureInfo> ifeature, int32_t band,
    std::vector<int32_t> freq, uint32_t count, uint32_t& num)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifeature->ifName)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!data.WriteInt32(band)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!data.WriteInt32(count)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_GET_FREQ_WITHBAND, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    num = reply.ReadUint32();
    for (uint32_t i = 0; i < num; i++) {
        freq.push_back(reply.ReadInt32());
        HDF_LOGI("%s: freqs[%d] is %d", __func__, i, freq[i]);
    }
    return ret;
}

int32_t WlanInterfaceProxy::setTxPower(std::shared_ptr<WifiFeatureInfo> ifeature, int32_t power)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifeature->ifName)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!data.WriteInt32(power)) {
        HDF_LOGE("%s: write power failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_SET_TX_POWR, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    return ret;
}

int32_t WlanInterfaceProxy::getChipId(std::shared_ptr<WifiFeatureInfo> ifeature, uint8_t& chipId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifeature->ifName)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!data.WriteInt32(ifeature->type)) {
        HDF_LOGE("%s: write type failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_GET_CHIPID, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    chipId = reply.ReadUint8();
    return ret;
}

int32_t WlanInterfaceProxy::getIfNamesByChipId(const uint8_t chipId, std::string& ifNames, uint32_t& num)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteUint8(chipId)) {
        HDF_LOGE("%s: write chipid failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_GET_NAME_BYCHIPID, data, reply, option);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }

    const char *name = reply.ReadCString();
    if (name == nullptr) {
        HDF_LOGE("%s: read ifNames failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    ifNames = name;

    num = reply.ReadUint32();
    return ret;
}

int32_t WlanInterfaceProxy::setScanningMacAddress(std::shared_ptr<WifiFeatureInfo> ifeature,
    std::vector<uint8_t>& scanMac, uint8_t len)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(WlanInterfaceProxy::GetDescriptor()) ||
        !data.WriteCString(ifeature->ifName)) {
        HDF_LOGE("%s: write ifname failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (!data.WriteUint8(len)) {
        HDF_LOGE("%s: write len failed!", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    for (int i = 0; i < ETH_ADDR_LEN; i++) {
        if (!data.WriteUint8(scanMac[i])) {
            HDF_LOGE("%s: write scanMac failed!", __func__);
        }
        HDF_LOGE("%s: mac addr is %x!", __func__, scanMac[i]);
    }
    int32_t ret = Remote()->SendRequest(WLAN_SERVICE_SET_SACN_MACADDR, data, reply, option);
    if (ret != HDF_ERR_NOT_SUPPORT) {
        HDF_LOGE("%s: SendRequest failed, error code is %d", __func__, ret);
    }
    return ret;
}