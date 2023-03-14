/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "stub.h"

#include <cstddef>
#include <cstdint>
#include "iosfwd"
#include "string"
#include "stub/a2dp_data_service.h"
#include "stub/map_service.h"
#include "stub/media_service.h"
#include "stub/message_digest.h"
#include "stub/telephone_client_service.h"
#include "stub/telephone_service.h"
#include "vector"

namespace stub {
class MD5 : public MessageDigest {
public:
    static constexpr size_t MD5_LENGTH = 16;

    static MD5 &GetInstance();
    virtual std::vector<uint8_t> Digest(const uint8_t *input, const size_t length) override;

private:
    MD5() = default;
    ~MD5() = default;
};

MD5 &MD5::GetInstance()
{
    static MD5 instance;
    return instance;
}

std::vector<uint8_t> MD5::Digest(const uint8_t *input, const size_t length)
{
    std::vector<uint8_t> digest(MD5_LENGTH, 0);
    return digest;
}

MessageDigest *MessageDigestFactory::GetInstance(const std::string &type)
{
    if (type.compare(DIGEST_TYPE_MD5) == 0) {
        return &MD5::GetInstance();
    } else {
        return nullptr;
    }
}

MapService *MapService::GetInstance()
{
    static MapService instance;
    return &instance;
}

MediaService *MediaService::GetInstance()
{
    static MediaService instance;
    return &instance;
}

TelePhoneClientService &TelePhoneClientService::GetInstance()
{
    static TelePhoneClientService instance;
    return instance;
}

TelephoneService *TelephoneService::GetInstance()
{
    static TelephoneService instance;
    return &instance;
}

A2dpService *A2dpService::GetInstance()
{
    static A2dpService instance;
    return &instance;
}
} // namespace stub