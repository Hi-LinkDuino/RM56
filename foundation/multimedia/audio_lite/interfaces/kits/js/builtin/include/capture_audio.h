#ifndef AUDIO_LITE_CAPTURE_AUDIO_H
#define AUDIO_LITE_CAPTURE_AUDIO_H

#include <pthread.h>
#include "jsi.h"
#include "audio_capturer.h"

namespace OHOS {
namespace ACELite {
class CaptureAudio;

class CaptureEventListener {
public:
    CaptureEventListener() = default;

    explicit CaptureEventListener(JSIValue callback);

    ~CaptureEventListener();

    JSIValue GetCallback() const;

    void OnTrigger() const;
private:
    JSIValue callback_;
};

void CaptureTriggerEventListener(const CaptureEventListener *listener);
void CaptureAsyncExecuteCallback(void *arg);

class CaptureAudio {
public:
    static CaptureAudio *GetInstance();

    bool CreateCapturer();

    bool DestoryCapturer();

    bool Start();

    bool stop();

    bool IsCapturing() const;

    const CaptureEventListener *GetOnStartListener() const;

    const CaptureEventListener *GetOnStopListener() const;

    void SetOnStartListener(CaptureEventListener *onStartListener);

    void SetOnStopListener(CaptureEventListener *onStopListener);

private:
    CaptureAudio();
    
    ~CaptureAudio() = default;

    std::shared_ptr<Audio::AudioCapturer>capturer_;
    CaptureEventListener *onStartListener_;
    CaptureEventListener *onStopListener_;
    int32_t sampleRate_;
    int32_t channelCount_;
    int32_t bitRate_;
    int32_t dmaBufferSize_;    
};
}
}
#endif