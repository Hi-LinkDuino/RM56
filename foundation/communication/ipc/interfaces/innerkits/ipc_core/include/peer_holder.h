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

#ifndef OHOS_IPC_PEER_HOLDER_H
#define OHOS_IPC_PEER_HOLDER_H

#include "iremote_object.h"

namespace OHOS {
class PeerHolder {
protected:
    PeerHolder(const sptr<IRemoteObject> &object);

    ~PeerHolder() = default;

    sptr<IRemoteObject> Remote();

private:
    const sptr<IRemoteObject> remoteObject_;
};
} // namespace OHOS
#endif // OHOS_IPC_PEER_HOLDER_H
