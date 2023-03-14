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

#include "map_mse_vcard.h"
#include <algorithm>

namespace OHOS {
namespace bluetooth {
MapMseVcard::MapMseVcard(const MseVcard2 &vcard)
    : name_(vcard.name_),
      version_(vcard.version_),
      formattedName_(vcard.formattedName_),
      phoneNumbers_(vcard.phoneNumbers_),
      emailAddresses_(vcard.emailAddresses_),
      envLevel_(vcard.envLevel_)
{}

MapMseVcard::MapMseVcard(const MseVcard3 &vcard)
    : name_(vcard.name_),
      version_(vcard.version_),
      formattedName_(vcard.formattedName_),
      phoneNumbers_(vcard.phoneNumbers_),
      emailAddresses_(vcard.emailAddresses_),
      envLevel_(vcard.envLevel_),
      btUids_(vcard.btUids_),
      btUcis_(vcard.btUcis_)
{}

std::string MapMseVcard::GetFirstPhoneNumber(void) const
{
    std::string number;
    if (phoneNumbers_.size() > 0) {
        number = phoneNumbers_.at(0);
    }
    return number;
}

int MapMseVcard::GetEnvLevel(void) const
{
    return envLevel_;
}

std::string MapMseVcard::GetName(void) const
{
    return name_;
}

std::string MapMseVcard::GetVersion(void) const
{
    return version_;
}

std::string MapMseVcard::GetFormattedName(void) const
{
    return formattedName_;
}

std::string MapMseVcard::GetPhoneNumbersText(void) const
{
    std::string pn;
    for (auto &iter : phoneNumbers_) {
        pn.append(iter + ";");
    }
    return pn;
}

std::string MapMseVcard::GetEmailAddressesText(void) const
{
    std::string addr = "";
    for (auto &iter : emailAddresses_) {
        if (iter != "") {
            addr.append(iter + ";");
        }
    }
    return addr;
}

std::string MapMseVcard::GetBtUidsText(void) const
{
    std::string btUids = "";
    for (auto &iter : btUids_) {
        if (iter != "") {
            btUids.append(iter + ";");
        }
    }
    return btUids;
}

std::string MapMseVcard::GetBtUcisText(void) const
{
    std::string btUcis = "";
    for (auto &iter : btUcis_) {
        if (iter != "") {
            btUcis.append(iter + ";");
        }
    }
    return btUcis;
}

std::string MapMseVcard::GetFirstEmail(void) const
{
    std::string email;
    if (emailAddresses_.size() > 0) {
        email = emailAddresses_.at(0);
    }
    return email;
}

std::string MapMseVcard::ToVcardString(const std::string &bmsgVersion)
{
    std::string vcd;
    vcd.append("BEGIN:VCARD");
    vcd.append("\n");
    vcd.append("VERSION:" + version_);
    vcd.append("\n");
    vcd.append("N:" + name_);
    vcd.append("\n");
    if (version_ == "3.0") {
        vcd.append("FN:" + formattedName_);
        vcd.append("\n");
    }
    if (GetEmailAddressesText() != "") {
        vcd.append("EMAIL:" + GetEmailAddressesText());
        vcd.append("\n");
    }
    if (bmsgVersion == "1.1") {
        if (GetBtUidsText() != "") {
            vcd.append("X-BT-UID:" + GetBtUidsText());
            vcd.append("\n");
        }
        if (GetBtUcisText() != "") {
            vcd.append("X-BT-UCI:" + GetBtUcisText());
            vcd.append("\n");
        }
    }
    vcd.append("END:VCARD");
    return vcd;
}
}  // namespace bluetooth
}  // namespace OHOS