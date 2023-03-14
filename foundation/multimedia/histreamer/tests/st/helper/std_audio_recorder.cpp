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
#ifndef OHOS_LITE
#include "test_recorder.hpp"

#include <thread>
#include <chrono>
#include <dirent.h>
#include <sys/stat.h>

#include "i_engine_factory.h"
#include "foundation/log.h"

extern "C" {
__attribute__((visibility("default"))) OHOS::Media::IEngineFactory* CreateEngineFactory();
}

using namespace OHOS::Media;

namespace OHOS::Media::Test {
class RecorderEngineObs : public IRecorderEngineObs {
public:
    ~RecorderEngineObs() override = default;

    void OnError(ErrorType errorType, int32_t errorCode) override
    {
        MEDIA_LOG_I("media_standard recorder error : " PUBLIC_LOG_D32, errorCode);
    }

    void OnInfo(InfoType type, int32_t extra) override
    {
    }
};

class TestRecorderImpl : public TestRecorder {
public:
    explicit TestRecorderImpl(std::unique_ptr<IRecorderEngine> recorder) : recorder_(std::move(recorder)) {}
    int32_t Configure(const AudioRecordSource& recordSource) override;
    int32_t Prepare() override;
    int32_t Start() override;
    int32_t Pause() override;
    int32_t Resume() override;
    int32_t Stop() override;
    int32_t Reset() override;
    int32_t Release() override;
    int32_t GetRecordedFile(std::string& path) override;
private:
    std::unique_ptr<IRecorderEngine> recorder_;
};

std::unique_ptr<TestRecorder> TestRecorder::CreateAudioRecorder()
{
    auto engineFactory = std::unique_ptr<OHOS::Media::IEngineFactory>(CreateEngineFactory());
    auto recorder = engineFactory->CreateRecorderEngine();
    auto obs = std::make_shared<RecorderEngineObs>();
    recorder->SetObs(obs);
    return std::make_unique<TestRecorderImpl>(std::move(recorder));
}

std::string TestRecorder::GetOutputDir()
{
    return std::string(HST_WORKING_DIR) + "/" + outputDirName;
}

int32_t TestRecorderImpl::Configure(const AudioRecordSource& recordSource)
{
    int32_t audioSourceId = 0;
    OHOS::AudioStandard::AudioCaptureCreator::GetInstance().SetPcmPath(recordSource.pcmPath_);
    recorder_->SetAudioSource(recordSource.sourceType_, audioSourceId);
    recorder_->SetOutputFormat(recordSource.outputFormat_);
    auto audSampleRate = AudSampleRate {recordSource.sampleRate_};
    auto audChannel = AudChannel {recordSource.channel_};
    auto audBitRate = AudBitRate {recordSource.bitRate_};
    auto auEncoder = AudEnc {recordSource.encodeType_};

    FALSE_RETURN_V((recorder_->Configure(audioSourceId, audSampleRate)) == 0, ERR_INVALID_VALUE);
    FALSE_RETURN_V((recorder_->Configure(audioSourceId, audChannel)) == 0, ERR_INVALID_VALUE);
    FALSE_RETURN_V((recorder_->Configure(audioSourceId, audBitRate)) == 0, ERR_INVALID_VALUE);
    FALSE_RETURN_V((recorder_->Configure(audioSourceId, auEncoder)) == 0, ERR_INVALID_VALUE);

    if (recordSource.isFD_) {
        auto outFileFD = OutFd {recordSource.outFD_};
        return recorder_->Configure(DUMMY_SOURCE_ID, outFileFD);
    } else {
        auto outFilePath = OutFilePath {TestRecorder::GetOutputDir()};
        return recorder_->Configure(DUMMY_SOURCE_ID, outFilePath); // result record file name
    }

    return 0;
}

int32_t TestRecorderImpl::Prepare()
{
    return recorder_->Prepare();
}

int32_t TestRecorderImpl::Start()
{
    return recorder_->Start();
}

int32_t TestRecorderImpl::Pause()
{
    return recorder_->Pause();
}

int32_t TestRecorderImpl::Resume()
{
    return recorder_->Resume();
}

int32_t TestRecorderImpl::Stop()
{
    return recorder_->Stop();
}

int32_t TestRecorderImpl::Reset()
{
    return recorder_->Reset();
}

int32_t TestRecorderImpl::Release()
{
    recorder_ = nullptr;
    return 0;
}

// The file in output dir is the latest recorded file.
int32_t TestRecorderImpl::GetRecordedFile(std::string& path)
{
    DIR *directory;
    struct dirent *info;
    if ((directory = opendir(GetOutputDir().c_str())) != nullptr) {
        while ((info = readdir(directory)) != nullptr) {
            if (strcmp(info->d_name, ".") == 0 || strcmp(info->d_name, "..") == 0) {
                continue;
            }
            path = GetOutputDir() + "/" + info->d_name;
            MEDIA_LOG_D("GetRecordedFile : " PUBLIC_LOG_S, path.c_str());
        }
        closedir(directory);
    }

    int32_t fileSize = 0;
    struct stat fileStatus {};
    if (stat(path.c_str(), &fileStatus) == 0) {
        fileSize = static_cast<int32_t>(fileStatus.st_size);
    }
    return fileSize;
}
}
#endif