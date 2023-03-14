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
#include "res_desc.h"
#include "hilog_wrapper.h"
#include "securec.h"
#include "utils/common.h"
#include "utils/string_utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
std::string KeyParam::GetScreenDensityStr() const
{
    std::string ret("not_screen_density");
    if (type_ == KeyType::SCREEN_DENSITY) {
        switch (value_) {
            case ScreenDensity::SCREEN_DENSITY_SDPI:
                ret = std::string(RE_120_STR);
                break;
            case ScreenDensity::SCREEN_DENSITY_MDPI:
                ret = std::string(RE_160_STR);
                break;
            case ScreenDensity::SCREEN_DENSITY_LDPI:
                ret = std::string(RE_240_STR);
                break;
            case ScreenDensity::SCREEN_DENSITY_XLDPI:
                ret = std::string(RE_320_STR);
                break;
            case ScreenDensity::SCREEN_DENSITY_XXLDPI:
                ret = std::string(RE_480_STR);
                break;
            case ScreenDensity::SCREEN_DENSITY_XXXLDPI:
                ret = std::string(RE_640_STR);
                break;
            default:
                break;
        }
    }
    return ret;
}
std::string KeyParam::GetDeviceTypeStr() const
{
    std::string ret("not_device_type");
    if (type_ == KeyType::DEVICETYPE) {
        switch (value_) {
            case DeviceType::DEVICE_PHONE:
                ret = std::string(PHONE_STR);
                break;
            case DeviceType::DEVICE_TABLET:
                ret = std::string(TABLET_STR);
                break;
            case DeviceType::DEVICE_CAR:
                ret = std::string(CAR_STR);
                break;
            case DeviceType::DEVICE_PAD:
                ret = std::string(PAD_STR);
                break;
            case DeviceType::DEVICE_TV:
                ret = std::string(TV_STR);
                break;
            case DeviceType::DEVICE_WEARABLE:
                ret = std::string(WEARABLE_STR);
                break;
            default:
                break;
        }
    }
    return ret;
}
const std::string KeyParam::ConvertToStr() const
{
    if ((type_ == KeyType::LANGUAGES) || (type_ == KeyType::REGION) || (type_ == KeyType::SCRIPT)) {
        char tmp[4], tmp2[5];
        errno_t eret = memcpy_s(tmp, sizeof(tmp), &value_, 4);
        if (eret != EOK) {
            HILOG_ERROR("memcpy_s error : %d", eret);
        }
        int j = 0;
        // 4 means langauages/region/script key value max length
        for (int i = 0; i < 4; ++i) {
            // 3 means reverse temp value to temp2
            if (tmp[3 - i]) {
                tmp2[j++] = tmp[3 - i];
            }
        }
        tmp2[j] = '\0';
        return std::string(tmp2);
    }
    if (type_ == KeyType::DIRECTION) {
        return std::string((value_ == 0) ? VERTICAL : HORIZONTAL);
    }
    if (type_ == KeyType::DEVICETYPE) {
        return GetDeviceTypeStr();
    }
    if (type_ == KeyType::SCREEN_DENSITY) {
        return GetScreenDensityStr();
    }
    return std::string();
}

std::string KeyParam::ToString() const
{
    std::string ret = FormatString("[type:%d, value:%u", type_, value_);
    if (str_.length() > 0) {
        ret.append(FormatString(", str:%s", str_.c_str()));
    }
    ret.append("]");
    return ret;
}

// IdItem

std::map<ResType, std::string> IdItem::resTypeStrList;

bool IdItem::sInit = IdItem::Init();

bool IdItem::Init()
{
    resTypeStrList.insert(make_pair(ResType::STRING, std::string("string")));
    resTypeStrList.insert(make_pair(ResType::BOOLEAN, std::string("boolean")));
    resTypeStrList.insert(make_pair(ResType::COLOR, std::string("color")));
    resTypeStrList.insert(make_pair(ResType::FLOAT, std::string("float")));
    resTypeStrList.insert(make_pair(ResType::INTEGER, std::string("integer")));
    resTypeStrList.insert(make_pair(ResType::PATTERN, std::string("pattern")));
    resTypeStrList.insert(make_pair(ResType::THEME, std::string("theme")));
    resTypeStrList.insert(make_pair(ResType::MEDIA, std::string("media")));
    return true;
}

bool IdItem::HaveParent() const
{
    if (!(resType_ == THEME || resType_ == PATTERN)) {
        return false;
    }
    // the values_ storage map(key, value) and parent ref
    // if have parent, size would be odd number
    return (values_.size() % 2 == 1);
}

bool IdItem::IsRef(const std::string &value, ResType &resType, int &id)
{
    const char *it = value.c_str();
    const char *st = it;
    if (*st != '$') {
        return false;
    }
    auto index = value.find(":");
    // there are atleast one letter between '$' and ':'
    if (index == std::string::npos || index < 2) {
        return false;
    }
    std::string typeStr, idStr;
    typeStr.assign(it + 1, index - 1);
    idStr.assign(it + index + 1, value.size() - index);

    int idd = atoi(idStr.c_str());
    if (idd <= 0) {
        return false;
    }

    for (auto iit = resTypeStrList.begin(); iit != resTypeStrList.end(); ++iit) {
        auto tValue = iit->second;
        auto type = iit->first;
        if (typeStr == tValue) {
            id = idd;
            resType = type;
            return true;
        }
    }

    return false;
}

std::string IdItem::ToString() const
{
    std::string ret = FormatString(
        "[size:%u, resType:%d, id:%u, valueLen:%u, isArray:%d, name:'%s', value:",
        size_, resType_, id_, valueLen_, isArray_, name_.c_str());
    if (isArray_) {
        ret.append("[");
        for (size_t i = 0; i < values_.size(); ++i) {
            ret.append(FormatString("'%s',", values_[i].c_str()));
        }
        ret.append("]");
    } else {
        ret.append(FormatString("'%s'", value_.c_str()));
    }
    ret.append("]");
    return ret;
}

IdParam::~IdParam()
{
    delete (idItem_);
}

std::string IdParam::ToString() const
{
    return FormatString("[id:%u, offset:%u, data:%s]", id_, offset_,
        idItem_->ToString().c_str());
}

ResId::~ResId()
{
    for (size_t i = 0; i < idParams_.size(); ++i) {
        auto ptr = idParams_[i];
        delete (ptr);
    }
}

std::string ResId::ToString() const
{
    std::string ret = FormatString("idcount:%u, ", count_);
    for (size_t i = 0; i < idParams_.size(); ++i) {
        ret.append(idParams_[i]->ToString());
    }
    return ret;
}

ResKey::~ResKey()
{
    HILOG_DEBUG("~ResKey()");
    for (size_t i = 0; i < keyParams_.size(); ++i) {
        auto ptr = keyParams_[i];
        delete (ptr);
    }
    delete (resId_);
}

std::string ResKey::ToString() const
{
    std::string ret = FormatString("offset:%u, keyParamsCount:%u, keyParams:", offset_, keyParamsCount_);
    for (uint32_t i = 0; i < keyParamsCount_; ++i) {
        ret.append(keyParams_[i]->ToString());
    }
    ret.append("\nid: ");
    ret.append(resId_->ToString());
    return ret;
}

ResDesc::ResDesc() : resHeader_(nullptr)
{}

ResDesc::~ResDesc()
{
    HILOG_DEBUG("~ResDesc()");
    delete (resHeader_);
    for (size_t i = 0; i < keys_.size(); ++i) {
        auto ptr = keys_[i];
        delete (ptr);
    }
}

std::string ResDesc::ToString() const
{
    if (resHeader_ == nullptr) {
        return "empty";
    }
    std::string ret = FormatString("version:%s, length:%u, keyCount:%u\n",
        resHeader_->version_, resHeader_->length_, resHeader_->keyCount_);
    for (size_t i = 0; i < keys_.size(); ++i) {
        ret.append(keys_[i]->ToString());
        ret.append("\n");
    }
    return ret;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS
