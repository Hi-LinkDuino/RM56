#include "capture_audio.h"
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include "js_async_work.h"
#include "media_log.h"

namespace OHOS {
namespace ACELite {

using namespace Audio;

void CaptureTriggerEventListener(const CaptureEventListener *listener)
{
    if (!JsAsyncWork::DispatchAsyncWork(CaptureAsyncExecuteCallback, const_cast<CaptureEventListener *>(listener))) {
        MEDIA_ERR_LOG("dispatch async work failed.");
    }
}

void CaptureAsyncExecuteCallback(void *arg)
{
    CaptureEventListener *listener = static_cast<CaptureEventListener *>(arg);
    if (listener == nullptr) {
        MEDIA_ERR_LOG("async execute callback failed.");
        return;
    }
    listener->OnTrigger();
}

CaptureEventListener::CaptureEventListener(JSIValue callback) : callback_(JSI::AcquireValue(callback)) {}

CaptureEventListener::~CaptureEventListener()
{
    if (!JSI::ValueIsUndefined(callback_)) {
        JSI::ReleaseValue(callback_);
    }
}

JSIValue CaptureEventListener::GetCallback() const
{
    return callback_;
}


void CaptureEventListener::OnTrigger() const
{
    if (JSI::ValueIsFunction(callback_)) {
        JSI::CallFunction(callback_, JSI::CreateUndefined(), nullptr, 0);
    }
}

CaptureAudio::CaptureAudio() : capturer_(nullptr), 
                               onStartListener_(nullptr),
                               onStopListener_(nullptr),
                               sampleRate_(16000),
                               channelCount_(1),
                               bitRate_(0),
                               dmaBufferSize_(0)
{}

CaptureAudio *CaptureAudio::GetInstance()
{
    static CaptureAudio capture;
    return &capture;
}

bool CaptureAudio::CreateCapturer()
{
    capturer_ = std::shared_ptr<AudioCapturer>(new (std::nothrow) AudioCapturer());
    if(capturer_ == nullptr) {
        MEDIA_ERR_LOG("fail to create capturer");
        return false;
    }

    return true;
}

bool CaptureAudio::DestoryCapturer()
{
    bool result;
    MEDIA_INFO_LOG("audioCaptuer destory enter");
    result = capturer_->Stop();
    if(!result) {
        MEDIA_ERR_LOG("audioCapturer stop error");
        return result;
    }
    result = capturer_->Release();
    if(!result) {
        MEDIA_ERR_LOG("audioCapturer release error");
        return result;
    }
    capturer_ = nullptr;
    MEDIA_INFO_LOG("audioCaptuer destory exit");
    return result;
}

bool CaptureAudio::Start()
{
    bool result;
    MEDIA_INFO_LOG("audioCaptuer start enter");

    AudioCapturerInfo info;
    info.inputSource = AUDIO_MIC;
    info.audioFormat = AUDIO_DEFAULT;
    info.sampleRate = sampleRate_;
    info.channelCount = channelCount_;
    info.bitRate = bitRate_;
    info.dmaBufferSize = dmaBufferSize_;
    info.streamType = TYPE_MEDIA;
    info.bitWidth = BIT_WIDTH_16;
    capturer_->SetCapturerInfo(info);

    result = capturer_->Start();
    if(!result) {
        MEDIA_ERR_LOG("audioCapturer start error");
        return result;
    }
    MEDIA_INFO_LOG("audioCaptuer start exit");
    return result;
}

bool CaptureAudio::stop()
{
    bool result;
    MEDIA_INFO_LOG("audioCaptuer Stop enter");
    result = capturer_->Stop();
    if(!result) {
        MEDIA_ERR_LOG("audioCapturer Stop error");
        return result;
    }
    MEDIA_INFO_LOG("audioCaptuer Stop exit");
    return result;
}

bool CaptureAudio::IsCapturing() const
{
    if(capturer_->GetStatus() == RECORDING) {
        return true;
    } else {
        return false;
    }
}

const CaptureEventListener *CaptureAudio::GetOnStartListener() const
{
    return onStartListener_;
}

const CaptureEventListener *CaptureAudio::GetOnStopListener() const
{
    return onStopListener_;
}

void CaptureAudio::SetOnStartListener(CaptureEventListener *onStartListener)
{
    if(onStartListener_ != nullptr) {
        delete onStartListener_;
    }
    onStartListener_ = onStartListener;
}

void CaptureAudio::SetOnStopListener(CaptureEventListener *onStopListener)
{
    if(onStopListener_ != nullptr) {
        delete onStopListener_;
    }
    onStopListener_ = onStopListener;
}

} // namespace ACELite
} // namespace OHOS