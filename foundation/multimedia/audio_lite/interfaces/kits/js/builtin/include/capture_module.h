#ifndef AUDIO_LITE_CAPTURE_MODULE_H
#define AUDIO_LITE_CAPTURE_MODULE_H

#include "capture_audio.h"

namespace OHOS {
namespace ACELite {
class CaptureModule final : public MemoryHeap {
public:
    CaptureModule() = default;

    ~CaptureModule() = default;

    static void Onterminate();

    static void
        DefineProperty(JSIValue target, const char *propName, JSIFunctionHandler getter, JSIFunctionHandler setter);

    static JSIValue CaptureStart(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue CaptureStop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue OnCapturerStartGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onCaptureStartSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onCaptureStopGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

    static JSIValue onCaptureStopSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize);

private:
    static JSIValue GetEventCallback(const CaptureEventListener *listener);

    static CaptureEventListener *CreateCaptureEventListener(const JSIValue *args, uint8_t argsSize);
};
void InitCaptureModule(JSIValue exports);
} // namespace ACELite
} // namespace OHOS
#endif