/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_ACE_DIALOG_CALLBACK_PROXY_H
#define OHOS_ACE_DIALOG_CALLBACK_PROXY_H

#include <iremote_proxy.h>
#include "dialog_callback_interface.h"

namespace OHOS {
namespace Ace {
class DialogCallbackProxy : public IRemoteProxy<IDialogCallback> {
public:
    explicit DialogCallbackProxy(const sptr<IRemoteObject> &remote);
    virtual ~DialogCallbackProxy();
    void OnDialogCallback(int32_t id, const std::string& event, const std::string& params) override;
private:
    static inline BrokerDelegator<DialogCallbackProxy> delegator_;
};
}  // namespace Ace
}  // namespace OHOS
#endif  // OHOS_ACE_DIALOG_CALLBACK_PROXY_H
