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

#ifndef MESSAGE_DIGEST_H
#define MESSAGE_DIGEST_H

#include <vector>
#include <string>
#include <stdint.h>

namespace stub {
const std::string DIGEST_TYPE_MD5 = "MD5";

class MessageDigest {
public:
    virtual std::vector<uint8_t> Digest(const uint8_t *input, const size_t length) = 0;
    virtual ~MessageDigest() = default;
};

class MessageDigestFactory {
public:
    static MessageDigest *GetInstance(const std::string &type);
};
} // namespace stub
#endif // MESSAGE_DIGEST_H