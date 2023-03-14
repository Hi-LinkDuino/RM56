/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "res_config_impl.h"

#include "locale_info.h"
#include "locale_matcher.h"
#include "res_locale.h"
#include "utils/utils.h"

namespace OHOS {
namespace Global {
namespace Resource {
ResConfigImpl::ResConfigImpl()
    : resLocale_(nullptr),
      direction_(DIRECTION_NOT_SET),
      screenDensity_(SCREEN_DENSITY_NOT_SET),
      deviceType_(DEVICE_NOT_SET),
      isCompletedScript_(false), localeInfo_(nullptr)
{}

RState ResConfigImpl::SetLocaleInfo(LocaleInfo &localeInfo)
{
    return this->SetLocaleInfo(localeInfo.GetLanguage(), localeInfo.GetScript(), localeInfo.GetRegion());
}

RState ResConfigImpl::SetLocaleInfo(const char *language,
    const char *script,
    const char *region)
{
    RState state = SUCCESS;
    if (Utils::IsStrEmpty(language)) {
        delete this->resLocale_;
        delete this->localeInfo_;
        this->resLocale_ = nullptr;
        this->localeInfo_ = nullptr;
        return state;
    }
    ResLocale *resLocale =
        ResLocale::BuildFromParts(language, script, region, state);
    if (state == SUCCESS) {
        this->isCompletedScript_ = false;
        if (script == nullptr || script[0] == '\0') {
            if (LocaleMatcher::Normalize(resLocale)) {
                this->isCompletedScript_ = true;
            } else {
                delete resLocale;
                return NOT_ENOUGH_MEM;
            }
        }
        LocaleInfo *tempLocale = new(std::nothrow) LocaleInfo(resLocale->GetLanguage(),
            resLocale->GetScript(), resLocale->GetRegion());
        if (tempLocale == nullptr) {
            state = NOT_ENOUGH_MEM;
            delete resLocale;
            return state;
        }
        delete resLocale_;
        delete localeInfo_;
        resLocale_ = resLocale;
        localeInfo_ = tempLocale;
    }

    return state;
}

void ResConfigImpl::SetDeviceType(DeviceType deviceType)
{
    this->deviceType_ = deviceType;
}

void ResConfigImpl::SetDirection(Direction direction)
{
    this->direction_ = direction;
}

void ResConfigImpl::SetScreenDensity(ScreenDensity screenDensity)
{
    this->screenDensity_ = screenDensity;
}

const LocaleInfo *ResConfigImpl::GetLocaleInfo() const
{
    return localeInfo_;
}

const ResLocale *ResConfigImpl::GetResLocale() const
{
    return this->resLocale_;
}

Direction ResConfigImpl::GetDirection() const
{
    return this->direction_;
}

ScreenDensity ResConfigImpl::GetScreenDensity() const
{
    return this->screenDensity_;
}

DeviceType ResConfigImpl::GetDeviceType() const
{
    return this->deviceType_;
}
bool ResConfigImpl::CopyLocale(ResConfig &other)
{
    bool needCopy = false;
    if (this->GetLocaleInfo() == nullptr && other.GetLocaleInfo() != nullptr) {
        needCopy = true;
    }
    if (this->GetLocaleInfo() != nullptr && other.GetLocaleInfo() == nullptr) {
        delete this->resLocale_;
        delete this->localeInfo_;
        this->resLocale_ = nullptr;
        this->localeInfo_ = nullptr;
        return true;
    }
    if (this->GetResLocale() != nullptr && other.GetLocaleInfo() != nullptr) {
        uint64_t encodedLocale = Utils::EncodeLocale(
            this->GetResLocale()->GetLanguage(),
            this->GetResLocale()->GetScript(), this->GetResLocale()->GetRegion());
        uint64_t otherEncodedLocale = Utils::EncodeLocale(
            other.GetLocaleInfo()->GetLanguage(),
            other.GetLocaleInfo()->GetScript(), other.GetLocaleInfo()->GetRegion());
        if (encodedLocale != otherEncodedLocale) {
            needCopy = true;
        }
    }
    if (needCopy) {
        ResLocale *temp = new(std::nothrow) ResLocale;
        if (temp == nullptr) {
            return false;
        }
        RState rs = temp->CopyFromLocaleInfo(other.GetLocaleInfo());
        if (rs != SUCCESS) {
            delete temp;
            return false;
        }
        LocaleInfo *tempLocale = new(std::nothrow) LocaleInfo(*other.GetLocaleInfo());
        if (tempLocale == nullptr) {
            delete tempLocale;
            delete temp;
            return false;
        }
        delete this->resLocale_;
        delete this->localeInfo_;
        this->resLocale_ = temp;
        this->localeInfo_ = tempLocale;
    }
    return true;
}
bool ResConfigImpl::Copy(ResConfig &other)
{
    bool isSuccess = this->CopyLocale(other);
    if (!isSuccess) {
        return false;
    }
    if (this->GetDeviceType() != other.GetDeviceType()) {
        this->SetDeviceType(other.GetDeviceType());
    }
    if (this->GetDirection() != other.GetDirection()) {
        this->SetDirection(other.GetDirection());
    }
    if (this->GetScreenDensity() != other.GetScreenDensity()) {
        this->SetScreenDensity(other.GetScreenDensity());
    }
    return true;
}

bool ResConfigImpl::Match(const ResConfigImpl *other) const
{
    if (other == nullptr) {
        return false;
    }
    if (!(LocaleMatcher::Match(this->resLocale_, other->GetResLocale()))) {
        return false;
    }
    if (this->direction_ != DIRECTION_NOT_SET &&
        other->direction_ != DIRECTION_NOT_SET) {
        if (this->direction_ != other->direction_) {
            return false;
        }
    }
    if (this->deviceType_ != DEVICE_NOT_SET &&
        other->deviceType_ != DEVICE_NOT_SET) {
        if (this->deviceType_ != other->deviceType_) {
            return false;
        }
    }
    return true;
}

/**
 * compare this  and target
 * if this more match request,then return true
 * else
 * return false
 *
 */
bool ResConfigImpl::IsMoreSuitable(const ResConfigImpl *other,
    const ResConfigImpl *request) const
{
    if (request != nullptr && other != nullptr) {
        int8_t result =
            LocaleMatcher::IsMoreSuitable(this->GetResLocale(), other->GetResLocale(),
                                          request->GetResLocale());
        if (result > 0) {
            return true;
        }
        if (result < 0) {
            return false;
        }
        /**
         * direction must full match.
         * when request is set direction and this is not equal other.
         * this or other oriention is not set.
         */
        if (this->direction_ != other->direction_ &&
            request->direction_ != Direction::DIRECTION_NOT_SET) {
            return this->direction_ != Direction::DIRECTION_NOT_SET;
        }
        if (this->deviceType_ != other->deviceType_ &&
            request->deviceType_ != DeviceType::DEVICE_NOT_SET) {
            return this->deviceType_ != DeviceType::DEVICE_NOT_SET;
        }
        if (request->screenDensity_ != ScreenDensity::SCREEN_DENSITY_NOT_SET &&
            this->screenDensity_ != other->screenDensity_) {
            int thisDistance = this->screenDensity_ - request->screenDensity_;
            int otherDistance = other->screenDensity_ - request->screenDensity_;
            if (thisDistance >= 0 && otherDistance >= 0) {
                return (thisDistance <= otherDistance);
            }
            if (thisDistance > 0) {
                return true;
            }
            if (otherDistance > 0) {
                return false;
            }
            return (thisDistance >= otherDistance);
        }
    }
    return this->IsMoreSpecificThan(other);
}

ResConfigImpl::~ResConfigImpl()
{
    delete resLocale_;
    delete localeInfo_;
}

void ResConfigImpl::CompleteScript()
{
    if (isCompletedScript_) {
        return;
    }
    if (LocaleMatcher::Normalize(this->resLocale_)) {
        isCompletedScript_ = true;
    }
}

bool ResConfigImpl::IsCompletedScript() const
{
    return isCompletedScript_;
}

bool ResConfigImpl::IsMoreSpecificThan(const ResConfigImpl *other) const
{
    if (other == nullptr) {
        return true;
    }
    int8_t result = LocaleMatcher::IsMoreSpecificThan(
        this->GetResLocale(),
        (other == nullptr) ? nullptr : other->GetResLocale());
    if (result > 0) {
        return true;
    }
    if (result < 0) {
        return false;
    }
    if (this->direction_ != other->direction_) {
        return (this->direction_ != Direction::DIRECTION_NOT_SET);
    }
    if (this->deviceType_ != other->deviceType_) {
        return (this->deviceType_ != DeviceType::DEVICE_NOT_SET);
    }
    if (this->screenDensity_ != other->screenDensity_) {
        return  (this->screenDensity_ != ScreenDensity::SCREEN_DENSITY_NOT_SET);
    }
    return true;
}

ResConfig *CreateResConfig()
{
    ResConfigImpl *temp = new(std::nothrow) ResConfigImpl;
    return temp;
}
} // namespace Resource
} // namespace Global
} // namespace OHOS