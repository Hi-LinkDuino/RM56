#include "capture_module.h"
#include "media_log.h"

namespace OHOS {
namespace ACELite {

void CaptureModule::Onterminate()
{
    CaptureAudio *captureAudio = CaptureAudio::GetInstance();
    if(captureAudio == nullptr) {
        MEDIA_ERR_LOG("capture audio is null");
        return;
    }
    captureAudio->DestoryCapturer();
}

void CaptureModule::DefineProperty(JSIValue target, 
                                   const char *propName, 
                                   JSIFunctionHandler getter,
                                   JSIFunctionHandler setter)
{
    JSPropertyDescriptor descriptor;
    descriptor.getter = getter;
    descriptor.setter = setter;
    JSI::DefineNamedProperty(target, propName, descriptor);    
}


JSIValue CaptureModule::CaptureStart(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    return JSI::CreateBoolean(CaptureAudio::GetInstance()->Start());
}

JSIValue CaptureModule::CaptureStop(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    return JSI::CreateBoolean( CaptureAudio::GetInstance()->stop());
}

JSIValue CaptureModule::OnCapturerStartGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    return GetEventCallback(CaptureAudio::GetInstance()->GetOnStartListener());
}

JSIValue CaptureModule::onCaptureStartSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    CaptureEventListener *listener = CreateCaptureEventListener(args, argsSize);
    if(listener == nullptr) {
        return JSI::CreateBoolean(false);
    }
    CaptureAudio::GetInstance()->SetOnStartListener(listener);
    return JSI::CreateBoolean(true);
}

JSIValue CaptureModule::onCaptureStopGetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    return GetEventCallback(CaptureAudio::GetInstance()->GetOnStopListener());
}

JSIValue CaptureModule::onCaptureStopSetter(const JSIValue thisVal, const JSIValue *args, uint8_t argsSize)
{
    CaptureEventListener *listener = CreateCaptureEventListener(args, argsSize);
    if(listener == nullptr) {
        return JSI::CreateBoolean(false);
    }
    CaptureAudio::GetInstance()->SetOnStopListener(listener);
    return JSI::CreateBoolean(true);
}

JSIValue CaptureModule::GetEventCallback(const CaptureEventListener *listener)
{
    return (listener == nullptr) ? JSI::CreateUndefined() : listener->GetCallback();
}

CaptureEventListener *CaptureModule::CreateCaptureEventListener(const JSIValue *args, uint8_t argsSize)
{
    if (argsSize < 1) {
        MEDIA_ERR_LOG("1 argument is required.");
        return nullptr;
    }

    JSIValue callback = args[0];
    if (!JSI::ValueIsFunction(callback)) {
        MEDIA_ERR_LOG("a function is required.");
        return nullptr;
    }

    CaptureEventListener *listener = new CaptureEventListener(callback);
    if (listener == nullptr) {
        MEDIA_ERR_LOG("out of memory.");
    }

    return listener;    
}

void InitCaptureModule(JSIValue exports)
{
    CaptureAudio::GetInstance()->CreateCapturer();

    JSI::SetOnTerminate(exports, CaptureModule::Onterminate);
    JSI::SetModuleAPI(exports, "capture", CaptureModule::CaptureStart);
    JSI::SetModuleAPI(exports, "stop", CaptureModule::CaptureStop);

    // CaptureModule::DefineProperty(exports, "sample_rate", CaptureAudio::SampleRateGetter, CaptureAudio::SampleRateSetter);

    CaptureModule::DefineProperty(exports, "oncapture", CaptureModule::OnCapturerStartGetter, CaptureModule::onCaptureStartSetter);
    CaptureModule::DefineProperty(exports, "onstop", CaptureModule::onCaptureStopGetter, CaptureModule::onCaptureStopSetter);
}
} // namespace ACELite
} // namespace OHOS