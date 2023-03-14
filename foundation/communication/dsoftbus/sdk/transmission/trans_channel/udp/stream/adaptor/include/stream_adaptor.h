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

#ifndef CLIENT_TRANS_UDP_STREAM_ADAPTOR_H_
#define CLIENT_TRANS_UDP_STREAM_ADAPTOR_H_

#include <atomic>
#include <sys/types.h>

#include "client_trans_udp_stream_interface.h"
#include "i_stream_manager.h"
#include "softbus_adapter_crypto.h"
#include "stream_common.h"

namespace OHOS {
class StreamAdaptor : public std::enable_shared_from_this<StreamAdaptor> {
public:
    StreamAdaptor() = delete;
    explicit StreamAdaptor(const std::string &pkgName);
    ~StreamAdaptor() = default;

    static ssize_t Encrypt(const void *in, ssize_t inLen, void *out, ssize_t outLen, const char* sessionKey);
    static ssize_t Decrypt(const void *in, ssize_t inLen, void *out, ssize_t outLen, const char *sessionKey);
    static ssize_t GetEncryptOverhead();
    int GetStreamType();
    const char *GetSessionKey();
    int64_t GetChannelId();
    const IStreamListener *GetListenerCallback();
    std::shared_ptr<Communication::SoftBus::IStreamManager> GetStreamManager();
    void SetAliveState(bool state);
    void InitAdaptor(int32_t channelId, const VtpStreamOpenParam *param, bool isServerSide,
        const IStreamListener *callback);
    void ReleaseAdaptor();
    bool GetAliveState();

private:
    int64_t channelId_ = -1;
    std::atomic<bool> aliveState_ = {false};
    std::shared_ptr<Communication::SoftBus::IStreamManager> streamManager_ = nullptr;
    int streamType_ = StreamType::INVALID;
    bool serverSide_;
    std::string pkgName_ {};
    std::string sessionKey_;
    const IStreamListener *callback_ = nullptr;
    std::atomic<bool> enableState_ = {false};
};
} // namespace OHOS

#endif // !defined(CLIENT_TRANS_UDP_STREAM_ADAPTOR_H_