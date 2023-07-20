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


#include "acelite_config.h"

#if (FEATURE_COMPONENT_QRCODE == 1)
#include "component.h"
#include "key_parser.h"
#include "non_copyable.h"
#include "ui_qrcode.h"

namespace OHOS {
namespace ACELite {
class QrcodeComponent final : public Component {
public:
    ACE_DISALLOW_COPY_AND_MOVE(QrcodeComponent);
    QrcodeComponent() = delete;
    QrcodeComponent(jerry_value_t options, jerry_value_t children, AppStyleManager *manager);
    ~QrcodeComponent() override {}
    void ReleaseNativeViews() override;

protected:
    UIView *GetComponentRootView() const override
    {
        return (const_cast<UIQrcode *>(&qrcode_));
    }
    bool ApplyPrivateStyle(const AppStyleItem *style) override;
    void OnViewAttached() override;
    void PostUpdate(uint16_t attrKeyId) override;
    bool SetPrivateAttribute(uint16_t attrKeyId, jerry_value_t attrValue) override;

private:
    bool SetColor(const AppStyleItem &style, ColorType &colortype);
    void DrawQrcode();

    UIQrcode qrcode_;
    char *qrcodeValue_;
    ColorType qrcodeBackGroundColor_;
    ColorType qrcodeColor_;
    uint8_t codeType_;
    
    static constexpr uint16_t QRCODE_VAL_MAX = 256;
};
} // namespace ACELite
} // namespace OHOS
#endif // #ifdef FEATURE_COMPONENT_QRCODE
