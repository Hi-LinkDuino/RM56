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

#ifndef OBEX_MP_CLIENT_H
#define OBEX_MP_CLIENT_H

#include "obex_client.h"

namespace OHOS {
namespace bluetooth {
class ObexMpClient : public ObexClient {
public:
    // create obex client
    explicit ObexMpClient(
        const ObexClientConfig &config, ObexClientObserver &observer, utility::Dispatcher &dispatcher);
    // destroy obex client
    ~ObexMpClient() override = default;

    int Put(const ObexHeader &req);
    /**
     * @brief send obex put request \n
     * When receiving Continue response, automatically send next Put request
     *
     * @param req the header of obex request for put(without body)
     * @param reader obex body reader
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Put(const ObexHeader &req, std::shared_ptr<ObexBodyObject> reader);
    /**
     * @brief send obex get request
     *
     * @param req the data of obex request for get
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Get(const ObexHeader &req);
    /**
     * @brief send obex get request \n
     * When receiving Continue response, automatically send next Get request
     * @param req the data of obex request for get
     * @param writer obex body writer
     * @param srmpCount srmp request count
     * @return int Request processing result:0:succeeded -1:failed
     */
    int Get(const ObexHeader &req, std::shared_ptr<ObexBodyObject> writer, int srmpCount = 0);
    /**
     * @brief send obex set_path request
     *
     * @param flag SETPATH flag \n
     *             bit 0:backup a level before applying name (equivalent to ../ on many systems)\n
     *                   @see OBEX_SETPATH_BACKUP
     *             bit 1:Don’t create folder if it does not exist, return an error instead.
     *                   @see OBEX_SETPATH_NOCREATE
     * @param path path
     * @return int Request processing result:0:succeeded -1:failed
     */
    int SetPath(uint8_t flag, const std::u16string &path);
    /**
     * @brief send obex set_path request
     *
     * @param paths path list.
     * @return int Request processing result:0:succeeded -1:failed
     */
    int SetPath(const std::vector<std::u16string> &paths);

protected:
    void PutDataAvailable(const ObexHeader &resp) override;
    void GetDataAvailable(const ObexHeader &resp) override;
    void SetPathDataAvailable(const ObexHeader &resp) override;
    void AbortDataAvailable(const ObexHeader &resp) override;
    void HandleTransportDataBusy(uint8_t isBusy) override;

    void ProcessGetContinueData(const ObexHeader &resp);
    void ProcessGetSuccessData(const ObexHeader &resp);
    int ProcessSendPutWithSrm();

private:
    void HandlePutData(const ObexHeader &resp);
};
}  // namespace bluetooth
}  // namespace OHOS
#endif  // OBEX_MP_CLIENT_H
