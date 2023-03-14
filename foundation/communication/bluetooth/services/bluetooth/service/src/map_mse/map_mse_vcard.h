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

#ifndef MAP_MSE_VCARD_H
#define MAP_MSE_VCARD_H

#include <string>
#include <vector>

namespace OHOS {
namespace bluetooth {
struct MseVcard2 {
    std::string name_ = "";
    std::string version_ = "";
    std::string formattedName_ = "";
    std::vector<std::string> phoneNumbers_ {};
    std::vector<std::string> emailAddresses_ {};
    int envLevel_ = 0;
};

struct MseVcard3 {
    std::string name_ = "";
    std::string version_ = "";
    std::string formattedName_ = "";
    std::vector<std::string> phoneNumbers_ {};
    std::vector<std::string> emailAddresses_ {};
    std::vector<std::string> btUids_ {};
    std::vector<std::string> btUcis_ {};
    int envLevel_ = 0;
};

class MapMseVcard {
public:
    explicit MapMseVcard(const MseVcard2 &vcard);

    explicit MapMseVcard(const MseVcard3 &vcard);

    virtual ~MapMseVcard() = default;

    std::string GetFirstPhoneNumber(void) const;

    int GetEnvLevel(void) const;

    std::string GetName(void) const;

    std::string GetVersion(void) const;

    std::string GetFormattedName(void) const;

    std::string GetPhoneNumbersText(void) const;

    std::string GetEmailAddressesText(void) const;

    std::string GetBtUidsText(void) const;

    std::string GetBtUcisText(void) const;

    std::string GetFirstEmail(void) const;

    std::string ToVcardString(const std::string &bmsgVersion);

private:
    void SetPhoneNumbers(std::vector<std::string> numbers);
    std::string name_ = "";
    std::string version_ = "";
    std::string formattedName_ = "";
    std::vector<std::string> phoneNumbers_ {};
    std::vector<std::string> emailAddresses_ {};
    int envLevel_ = 0;
    std::vector<std::string> btUids_ {};
    std::vector<std::string> btUcis_ {};
};
}  // namespace bluetooth
}  // namespace OHOS

#endif  // MAP_MSE_VCARD_H