/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "want_utils.h"
#include "element_name_utils.h"

#include <securec.h>
#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
#include <string>
#include <liteipc_adapter.h>
#endif

#include "log.h"
#include "utils.h"

#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
using UriKeyType = enum {
    BEGIN,
    DEVICE,
    BUNDLE,
    ABILITY,
    END,
};

using UriProperties = struct {
    const char *key;    /* key of uri property */
    uint8_t keyLen;     /* key length of uri property */
    UriKeyType type;    /* key type of uri property */
};

const static UriProperties URI_PROPERTIES[] = {
    { "#Want",    5, BEGIN   },
    { "device=",  7, DEVICE  },
    { "bundle=",  7, BUNDLE  },
    { "ability=", 8, ABILITY },
    { "end",      3, END     },
};

constexpr static char URI_SEPARATOR = ';';
constexpr static int VALUE_NULL = 0;
constexpr static int VALUE_OBJECT = 1;
constexpr static int DATA_LENGTH = 2048;
#endif

constexpr uint8_t INT_VALUE_TYPE = 6;
constexpr uint8_t STRING_VALUE_TYPE = 13;
constexpr uint8_t KEY_VALUE_PAIR_TYPE = 97;

void ClearWant(Want *want)
{
    if (want == nullptr) {
        return;
    }

    ClearElement(want->element);
    AdapterFree(want->element);
#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
    AdapterFree(want->sid);
#endif
    AdapterFree(want->data);
}

bool SetWantElement(Want *want, ElementName element)
{
    if (want == nullptr) {
        return false;
    }

    ClearElement(want->element);
    AdapterFree(want->element);
    want->element = reinterpret_cast<ElementName *>(AdapterMalloc(sizeof(ElementName)));
    if (want->element == nullptr) {
        return false;
    }
    want->element->deviceId = OHOS::Utils::Strdup(element.deviceId);
    want->element->bundleName = OHOS::Utils::Strdup(element.bundleName);
    want->element->abilityName = OHOS::Utils::Strdup(element.abilityName);
    return true;
}

Tlv *EncapTlv(uint8_t type, uint8_t length, const void *value, uint8_t valueLen)
{
    void *entity = nullptr;

    // Tlv header can only has 2 bytes.
    uint8_t totalLen = valueLen + 2;
    entity = calloc(1, totalLen);
    if (entity == nullptr) {
        return nullptr;
    }

    if (memcpy_s((unsigned char *)entity, 1, &type, 1) != 0 ||
        memcpy_s((unsigned char *)entity + 1, 1, &length, 1) != 0 ||
        memcpy_s((unsigned char *)entity + 2, valueLen, value, valueLen) != 0) {
        AdapterFree(entity);
        return nullptr;
    }

    Tlv *newTlv = new Tlv();
    newTlv->type = type;
    newTlv->entity = entity;
    newTlv->totalLen = totalLen;
    return newTlv;
}

void FreeTlvStruct(Tlv *tlv)
{
    AdapterFree(tlv->entity);
    AdapterFree(tlv);
}

Tlv *CombineKeyValueTlv(Tlv *keyTlv, Tlv *valueTlv)
{
    uint8_t newTlvValueLen = keyTlv->totalLen + valueTlv->totalLen;
    void *newTlvValue = calloc(1, newTlvValueLen);
    if (newTlvValue == nullptr) {
        return nullptr;
    }
    if (memcpy_s((unsigned char *)newTlvValue, keyTlv->totalLen, keyTlv->entity, keyTlv->totalLen) != 0 ||
        memcpy_s((unsigned char *)newTlvValue + keyTlv->totalLen, valueTlv->totalLen,
        valueTlv->entity, valueTlv->totalLen) != 0) {
        AdapterFree(newTlvValue);
        return nullptr;
    }

    Tlv *newTlv = EncapTlv(KEY_VALUE_PAIR_TYPE, newTlvValueLen, newTlvValue, newTlvValueLen);
    AdapterFree(newTlvValue);
    return newTlv;
}

bool UpdateWantData(Want *want, Tlv *tlv)
{
    bool result = false;
    if (want->data != nullptr) {
        void *newWantData = calloc(1, tlv->totalLen + want->dataLength);
        if (newWantData == nullptr) {
            return result;
        }
        if (memcpy_s(newWantData, want->dataLength, want->data, want->dataLength) != 0 ||
            memcpy_s((unsigned char*)newWantData + want->dataLength, tlv->totalLen, tlv->entity, tlv->totalLen) != 0) {
            AdapterFree(newWantData);
            return result;
        }
        SetWantData(want, newWantData, tlv->totalLen + want->dataLength);
        AdapterFree(newWantData);
        result = true;
    } else {
        SetWantData(want, tlv->entity, tlv->totalLen);
        result = true;
    }
    return result;
}

bool SetIntParam(Want *want, const char *key, uint8_t keyLen, int32_t value)
{
    bool result = false;
    if (keyLen <= 0) {
        return result;
    }

    Tlv *keyTlv = EncapTlv(STRING_VALUE_TYPE, keyLen, (void *)key, keyLen);
    if (keyTlv == nullptr) {
        return result;
    }
    if (value < 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "SetIntParam value should be positive");
        FreeTlvStruct(keyTlv);
        return result;
    }
    int intBufferbNumber = 4;
    unsigned char intBuffer[4] = {0};
    for (int i = 0; i < intBufferbNumber; i++) {
        intBuffer[i] = value >> (8 * (3- i));
    }
    Tlv *valueTlv = EncapTlv(INT_VALUE_TYPE, sizeof(int), (void *)intBuffer, sizeof(int));
    if (valueTlv == nullptr) {
        FreeTlvStruct(keyTlv);
        return result;
    }
    Tlv *newTlv = CombineKeyValueTlv(keyTlv, valueTlv);
    FreeTlvStruct(keyTlv);
    FreeTlvStruct(valueTlv);
    if (newTlv == nullptr) {
        return result;
    }
    if (UpdateWantData(want, newTlv)) {
        AdapterFree(newTlv);
        result = true;
    }
    AdapterFree(newTlv);
    return result;
}

bool SetStrParam(Want *want, const char *key, uint8_t keyLen, const char *value, uint8_t valueLen)
{
    bool result = false;
    if (keyLen <= 0 || valueLen <= 0) {
        return result;
    }

    Tlv *keyTlv = EncapTlv(STRING_VALUE_TYPE, keyLen, (void *)key, keyLen);
    if (keyTlv == nullptr) {
        return result;
    }

    Tlv *valueTlv = EncapTlv(STRING_VALUE_TYPE, valueLen, (void *)value, valueLen);
    if (valueTlv == nullptr) {
        FreeTlvStruct(keyTlv);
        return result;
    }
    Tlv *newTlv = CombineKeyValueTlv(keyTlv, valueTlv);
    FreeTlvStruct(keyTlv);
    FreeTlvStruct(valueTlv);
    if (newTlv == nullptr) {
        return result;
    }
    if (UpdateWantData(want, newTlv)) {
        AdapterFree(newTlv);
        result = true;
    }
    AdapterFree(newTlv);
    return result;
}

#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
bool SetWantSvcIdentity(Want *want, SvcIdentity sid)
{
    if (want == nullptr) {
        return false;
    }

    AdapterFree(want->sid);
    want->sid = reinterpret_cast<SvcIdentity *>(AdapterMalloc(sizeof(SvcIdentity)));
    if (want->sid == nullptr) {
        return false;
    }
    if (memcpy_s(want->sid, sizeof(SvcIdentity), &sid, sizeof(SvcIdentity)) != EOK) {
        AdapterFree(want->sid);
        return false;
    }

    return true;
}
#endif

bool SetWantData(Want *want, const void *data, uint16_t dataLength)
{
    if (want == nullptr) {
        return false;
    }

    AdapterFree(want->data);
    want->data = OHOS::Utils::Memdup(data, dataLength);
    if (want->data == nullptr) {
        want->dataLength = 0;
        return false;
    }

    want->dataLength = dataLength;
    return true;
}

#ifdef OHOS_APPEXECFWK_BMS_BUNDLEMANAGER
bool SerializeWant(IpcIo *io, const Want *want)
{
    if ((io == nullptr) || (want == nullptr) || (want->dataLength > DATA_LENGTH)) {
        return false;
    }

    if (want->element == nullptr) {
        IpcIoPushInt32(io, VALUE_NULL);
    } else {
        IpcIoPushInt32(io, VALUE_OBJECT);
        if (!SerializeElement(io, want->element)) {
            return false;
        }
    }
    IpcIoPushInt32(io, want->dataLength);
    if (want->dataLength > 0) {
#ifdef __LINUX__
        IpcIoPushFlatObj(io, want->data, want->dataLength);
#else
        const BuffPtr buff = {
            want->dataLength,
            want->data,
        };
        IpcIoPushDataBuff(io, &buff);
#endif
    }
    if (want->sid == nullptr) {
        IpcIoPushInt32(io, VALUE_NULL);
    } else {
        IpcIoPushInt32(io, VALUE_OBJECT);
        IpcIoPushSvc(io, want->sid);
    }

    return true;
}

bool DeserializeWant(Want *want, IpcIo *io)
{
    if ((want == nullptr) || (io == nullptr)) {
        return false;
    }

    if (IpcIoPopInt32(io) == VALUE_OBJECT) {
        want->element = reinterpret_cast<ElementName *>(AdapterMalloc(sizeof(ElementName)));
        if (want->element == nullptr ||
            memset_s(want->element, sizeof(ElementName), 0, sizeof(ElementName)) != EOK ||
            !DeserializeElement(want->element, io)) {
            AdapterFree(want->element);
            return false;
        }
    }
    if (IpcIoPopInt32(io) > 0) {
#ifdef __LINUX__
        uint32_t size = 0;
        void *data = IpcIoPopFlatObj(io, &size);
        if (!SetWantData(want, data, size)) {
            ClearWant(want);
            return false;
        }
#else
        BuffPtr *buff = IpcIoPopDataBuff(io);
        if (buff == nullptr) {
            ClearWant(want);
            return false;
        }
        if (!SetWantData(want, buff->buff, buff->buffSz)) {
            FreeBuffer(nullptr, buff->buff);
            ClearWant(want);
            return false;
        }
        FreeBuffer(nullptr, buff->buff);
#endif
    }
    if (IpcIoPopInt32(io) == VALUE_OBJECT) {
        auto sid = IpcIoPopSvc(io);
        if ((sid == nullptr) || !SetWantSvcIdentity(want, *sid)) {
#ifdef __LINUX__
            AdapterFree(sid);
            sid = nullptr;
#endif
            ClearWant(want);
            return false;
        }
#ifdef __LINUX__
        AdapterFree(sid);
        sid = nullptr;
#endif
    }

    return true;
}

Want *WantParseUri(const char *uri)
{
    if (uri == nullptr) {
        return nullptr;
    }
    char *parseUri = OHOS::Utils::Strdup(uri);
    if (parseUri == nullptr) {
        return nullptr;
    }
    ElementName element = { nullptr, nullptr, nullptr };
    char *beginIndex = parseUri;
    for (auto property : URI_PROPERTIES) {
        if (strstr(beginIndex, property.key) != beginIndex) {
            AdapterFree(parseUri);
            return nullptr;
        }
        if (property.type == END) {
            break;
        }
        char *endIndex = strchr(beginIndex, URI_SEPARATOR);
        if ((endIndex == nullptr) || (endIndex <= beginIndex)) {
            AdapterFree(parseUri);
            return nullptr;
        }
        *endIndex = '\0';
        beginIndex += property.keyLen;
        switch (property.type) {
            case DEVICE: {
                SetElementDeviceID(&element, beginIndex);
                break;
            }
            case BUNDLE: {
                SetElementBundleName(&element, beginIndex);
                break;
            }
            case ABILITY: {
                SetElementAbilityName(&element, beginIndex);
                break;
            }
            default: {
                break;
            }
        }
        beginIndex = endIndex + 1;
    }
    AdapterFree(parseUri);
    Want *want = new Want();
    if ((memset_s(want, sizeof(Want), 0, sizeof(Want)) != EOK) || !SetWantElement(want, element)) {
        ClearElement(&element);
        delete want;
        return nullptr;
    }
    ClearElement(&element);
    return want;
}

const char *WantToUri(Want want)
{
    std::string uriString;

    for (auto property : URI_PROPERTIES) {
        uriString += property.key;
        switch (property.type) {
            case BEGIN: {
                uriString += URI_SEPARATOR;
                break;
            }
            case DEVICE: {
                if ((want.element != nullptr) && (want.element->deviceId != nullptr)) {
                    uriString += want.element->deviceId;
                }
                uriString += URI_SEPARATOR;
                break;
            }
            case BUNDLE: {
                if ((want.element != nullptr) && (want.element->bundleName != nullptr)) {
                    uriString += want.element->bundleName;
                }
                uriString += URI_SEPARATOR;
                break;
            }
            case ABILITY: {
                if ((want.element != nullptr) && (want.element->abilityName != nullptr)) {
                    uriString += want.element->abilityName;
                }
                uriString += URI_SEPARATOR;
                break;
            }
            default: {
                break;
            }
        }
    }

    uint16_t len = uriString.size();
    char *uri = reinterpret_cast<char *>(AdapterMalloc(len + 1));
    if (uri == nullptr) {
        return nullptr;
    }
    if (strncpy_s(uri, len + 1, uriString.c_str(), len) < 0) {
        AdapterFree(uri);
        return nullptr;
    }

    return uri;
}
#endif
