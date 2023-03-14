/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "font/base_font.h"
namespace OHOS {
uintptr_t BaseFont::GetRamAddr()
{
    return ramAddr_;
}
void BaseFont::SetRamAddr(uintptr_t ramAddr)
{
    ramAddr_ = ramAddr;
}
uint32_t BaseFont::GetRamLen()
{
    return ramLen_;
}
void BaseFont::SetRamLen(uint32_t ramLen)
{
    ramLen_ = ramLen;
}

void BaseFont::SetPsramMemory(uintptr_t psramAddr, uint32_t psramLen)
{
    SetRamAddr(psramAddr);
    SetRamLen(psramLen);
}

int8_t BaseFont::GetDefaultParamByLangId(uint8_t langId, LangTextParam** pParam) const
{
    if ((langId >= UIFontBuilder::GetInstance()->GetTotalLangId()) || (pParam == nullptr)) {
        return INVALID_RET_VALUE;
    }

    LangTextParam *pTable = UIFontBuilder::GetInstance()->GetLangTextDefaultParamTable();
    if (pTable == nullptr) {
        return INVALID_RET_VALUE;
    }

    *pParam = (pTable + langId);
    return RET_VALUE_OK;
}
} // namespace OHOS
