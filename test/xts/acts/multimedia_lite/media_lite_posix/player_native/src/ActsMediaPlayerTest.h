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
 
#ifndef PLAYER_LITE_TEST_H
#define PLAYER_LITE_TEST_H
#include <sys/prctl.h>
#include <fcntl.h>
#include <climits>
#include "securec.h"
#include "gtest/gtest.h"
#include "source.h"
#include "player.h"
#include "format.h"
#include "fstream"
#include "iostream"
#include "thread"
#include "unistd.h"

namespace OHOS {
using OHOS::Media::Player;
using OHOS::Media::PlayerSeekMode;
using OHOS::Media::Source;
using OHOS::Media::Format;
using OHOS::Media::StreamSource;
using OHOS::Media::StreamCallback;
using OHOS::Media::SourceType;
using OHOS::Media::PlayerCallback;

using namespace std;
using namespace OHOS::Media;
using namespace testing::ext;

const int FILE_PATH_LEN = 2048;

class StreamSourceSample;
using TestSample = struct TagTestSample {
    std::shared_ptr<Player> adaptr;
    pthread_t process;
    pthread_mutex_t mutex;
    int32_t isThreadRunning;
    int32_t sourceType;
    char filePath[FILE_PATH_LEN];
    std::shared_ptr<StreamSourceSample> streamSample;
};

using IdleBuffer =  struct TagIdleBuffer {
    size_t idx;
    size_t offset;
    size_t size;
};

class StreamSourceSample : public StreamSource {
public:
    StreamSourceSample(void);
    ~StreamSourceSample(void);
    void OnBufferAvailable(size_t index, size_t offset, size_t size);
    void SetStreamCallback(const std::shared_ptr<StreamCallback> &callback);
    uint8_t *GetBufferAddress(size_t idx);
    void QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags);
    int GetAvailableBuffer(IdleBuffer *buffer);
    std::weak_ptr<StreamCallback> m_callBack;
    pthread_mutex_t m_mutex;
private:
    std::vector<IdleBuffer> aviableBuffer;
};

void *StreamProcess(const void *arg);
void SetSchParam(void);

const int32_t HI_SUCCESS = 0;
const int32_t HI_FAILURE = -1;
static TagTestSample g_tagTestSample;
static Surface *g_surface = Surface::CreateSurface();
} // namespace OHOS
#endif // PLAYER_LITE_TEST_H