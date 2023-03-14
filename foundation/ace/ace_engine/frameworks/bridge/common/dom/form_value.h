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

#ifndef FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_COMMON_DOM_FORM_VALUE_H
#define FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_COMMON_DOM_FORM_VALUE_H

#include <string>

#include "base/memory/ace_type.h"

namespace OHOS::Ace::Framework {

class FormValue : public virtual AceType {
    DECLARE_ACE_TYPE(FormValue, AceType)

public:
    std::pair<std::string, std::string> GetFormValue()
    {
        if (isCheckbox_) {
            if (!isChecked_) {
                return { "", "" };
            }
            if (value_.empty()) {
                return { name_, "on" };
            }
        }
        if (isRadio_) {
            if (isChecked_) {
                return { name_, value_ };
            }
            return { "", "" };
        }
        return { name_, value_ };
    }
    void Reset()
    {
        value_ = originValue_;
        isChecked_ = originChecked_;
        OnReset();
    }

    const std::string& GetName() const
    {
        return name_;
    }
    void SetName(const std::string& name)
    {
        name_ = name;
    }
    const std::string& GetOriginValue() const
    {
        return originValue_;
    }
    void SetOriginValue(const std::string& originValue)
    {
        originValue_ = originValue;
        value_ = originValue;
    }
    const std::string& GetValue() const
    {
        return value_;
    }
    void SetValue(const std::string& value)
    {
        value_ = value;
        if (isRadio_) {
            if (value_ == originValue_) {
                isChecked_ = true;
            } else {
                isChecked_ = false;
            }
        }
    }
    bool IsChecked() const
    {
        return isChecked_;
    }
    void SetChecked(bool checked)
    {
        isChecked_ = checked;
    }
    bool IsCheckbox() const
    {
        return isCheckbox_;
    }
    void SetIsCheckbox(bool isCheckbox)
    {
        isCheckbox_ = isCheckbox;
    }
    bool IsOriginChecked() const
    {
        return originChecked_;
    }
    void SetOriginChecked(bool originChecked)
    {
        originChecked_ = originChecked;
        isChecked_ = originChecked;
    }
    bool IsRadio() const
    {
        return isRadio_;
    }
    void SetIsRadio(bool isRadio)
    {
        isRadio_ = isRadio;
    }

protected:
    virtual void OnReset() {}

private:
    std::string name_;
    std::string originValue_;
    std::string value_;
    bool isCheckbox_ = false;
    bool isRadio_ = false;
    bool isChecked_ = false;
    bool originChecked_ = false;
};

} // namespace OHOS::Ace::Framework

#endif // FOUNDATION_ACE_ACE_ENGINE_FRAMEWORKS_BRIDGE_COMMON_DOM_FORM_VALUE_H
