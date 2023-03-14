#ifndef HISTREAMER_SMF_DECODER_PLUGIN_H
#define HISTREAMER_SMF_DECODER_PLUGIN_H

#include <functional>
#include <map>
#include "utils/blocking_queue.h"
#include "plugin/interface/codec_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "smf_inc/smf_api.h"
#include "smf_api_histreamer.h"
#ifdef __cplusplus
};
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Smf {
class SmfDecoderPlugin : public CodecPlugin {
public:
    explicit SmfDecoderPlugin(std::string name);

    ~SmfDecoderPlugin() override;

    Status Init() override;

    Status Deinit() override;

    Status Prepare() override;

    Status Reset() override;

    Status Start() override;

    Status Stop() override;

    Status GetParameter(Tag tag, ValueType& value) override;

    Status SetParameter(Tag tag, const ValueType& value) override;

    // std::shared_ptr<Allocator> GetAllocator() override;

    Status SetCallback(Callback* cb) override;

    Status QueueInputBuffer(const std::shared_ptr<Buffer>& inputBuffer, int32_t timeoutMs) override;

    Status QueueOutputBuffer(const std::shared_ptr<Buffer>& outputBuffer, int32_t timeoutMs) override;

    Status Flush() override;

    Status SetDataCallback(DataCallback* dataCallback) override;

private:
    SMF_DECODER_T *smf_decoder;
    SMF_DECODER_PARAM_T *smf_decoder_param;
    mutable OSAL::Mutex ioMutex_{};
    std::shared_ptr<Buffer> inputBuffer_{nullptr};
    std::shared_ptr<Buffer> outputBuffer_{nullptr};
    DataCallback* dataCb_ {};
    smf_frame_t fpcm;
    Status SmfDecoderProcess(std::shared_ptr<Buffer> inBuffer, std::shared_ptr<Buffer> outBuffer);
    Status GetPcmDataProcess(const std::shared_ptr<Buffer> &inputBuffer,
                             std::shared_ptr<Buffer> &outputBuffer);
    Status SendOutputBuffer();
};
} // namespace Smf
} // namespace Plugin
} // namespace Media
} // namespace OHOS
#endif