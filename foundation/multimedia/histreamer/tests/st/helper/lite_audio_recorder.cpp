/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#ifdef OHOS_LITE
#include "test_recorder.hpp"

using namespace OHOS::Media;

class IRecorderEngine {};

namespace OHOS::Media::Test {
class TestRecorderImpl : public TestRecorder {
public:
    explicit TestRecorderImpl(std::unique_ptr<IRecorderEngine> recorder) {}
    int32_t Configure(const AudioRecordSource& recordSource) override;
    int32_t Prepare() override;
    int32_t Start() override;
    int32_t Pause() override;
    int32_t Resume() override;
    int32_t Stop() override;
    int32_t GetRecordedFile(std::string& path) override;
};

std::unique_ptr<TestRecorder> TestRecorder::CreateAudioRecorder()
{
    return nullptr;
}

std::string TestRecorder::GetOutputDir()
{
    return "";
}

int32_t TestRecorderImpl::Configure(const AudioRecordSource& recordSource)
{
    return -1;
}

int32_t TestRecorderImpl::Prepare()
{
    return -1;
}

int32_t TestRecorderImpl::Start()
{
    return -1;
}

int32_t TestRecorderImpl::Pause()
{
    return -1;
}

int32_t TestRecorderImpl::Resume()
{
    return -1;
}

int32_t TestRecorderImpl::Stop()
{
    return -1;
}

// The file in output dir is the latest recorded file.
int32_t TestRecorderImpl::GetRecordedFile(std::string& path)
{
    return 0;
}
}
#endif