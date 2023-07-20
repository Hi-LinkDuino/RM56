#define HST_LOG_TAG "SMFDECODER"
// #define MEDIA_LOG_DEBUG 1

#include "smf_decoder_plugin.h"
#include <cstring>
#include <map>
#include <set>
#include "plugin/common/plugin_audio_tags.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_caps_builder.h"
#include "plugin/interface/codec_plugin.h"
#include "utils/constants.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Smf {
SmfDecoderPlugin::SmfDecoderPlugin(std::string name) : CodecPlugin(std::move(name)) 
{
    smf_decoder = nullptr;
    smf_decoder_param = nullptr;
    MEDIA_LOG_I("SmfDecoderPlugin, plugin name: %s", pluginName_.c_str());
}

SmfDecoderPlugin::~SmfDecoderPlugin()
{
    MEDIA_LOG_I("~SmfDecoderPlugin");
}

Status SmfDecoderPlugin::Init()
{
    MEDIA_LOG_D("SmfDecoderPlugin Init");

    memset(&fpcm, 0, sizeof(fpcm));
    MEDIA_LOG_D("fpcm @ %p", &fpcm);

    const char *key = pluginName_.c_str();
    MEDIA_LOG_I("SmfDecoderPlugin Init. key %s", key);

    smf_decoder = smf_create_decoder(key);
    if (!smf_decoder)
    {
        MEDIA_LOG_E("SmfDecoderCreate error");
        return Status::ERROR_NULL_POINTER;
    }
    MEDIA_LOG_D("smf_create_decoder succ");

    smf_decoder_param = smf_alloc_open_param(smf_decoder);
    if (!smf_decoder_param)
    {
        MEDIA_LOG_E("smf_alloc_open_param error");
        smf_destroy(smf_decoder);
        return Status::ERROR_NULL_POINTER;
    }
    MEDIA_LOG_D("smf_alloc_open_param succ");

    if (false == smf_open(smf_decoder, smf_decoder_param))
    {
        MEDIA_LOG_E("smf_open error");
        smf_free(smf_decoder, smf_decoder_param);
        smf_destroy(smf_decoder);
        return Status::ERROR_NULL_POINTER;
    }
    MEDIA_LOG_D("smf_open succ");

    return Status::OK;
}

Status SmfDecoderPlugin::Deinit()
{
    MEDIA_LOG_I("SmfDecoderPlugin Deinit");

    if (smf_decoder_param)
    {
        smf_free(smf_decoder, smf_decoder_param);
        smf_decoder_param = nullptr;
    }
    if (smf_decoder)
    {
        smf_destroy(smf_decoder);
        smf_decoder = nullptr;
    }

    return Status::OK;
}

Status SmfDecoderPlugin::Prepare()
{
    MEDIA_LOG_D("SmfDecoderPlugin Prepare");
    return Status::OK;
}

Status SmfDecoderPlugin::Reset()
{
    MEDIA_LOG_D("SmfDecoderPlugin Reset");
    return Status::OK;
}

Status SmfDecoderPlugin::Start()
{
    MEDIA_LOG_D("SmfDecoderPlugin Start");
    return Status::OK;
}

Status SmfDecoderPlugin::Stop()
{
    MEDIA_LOG_D("SmfDecoderPlugin Stop");
    return Status::OK;
}

Status SmfDecoderPlugin::GetParameter(Tag tag, ValueType &value)
{
    MEDIA_LOG_D("SmfDecoderPlugin GetParameter");
    return Status::OK;
}

Status SmfDecoderPlugin::SetParameter(Tag tag, const ValueType &value)
{
    MEDIA_LOG_D("SmfDecoderPlugin SetParameter");
    return Status::OK;
}

Status SmfDecoderPlugin::SetCallback(Callback *cb)
{
    MEDIA_LOG_D("SmfDecoderPlugin SetCallback");
    return Status::OK;
}

Status SmfDecoderPlugin::SmfDecoderProcess(std::shared_ptr<Buffer> inBuffer, std::shared_ptr<Buffer> outBuffer)
{
    auto inData = inBuffer->GetMemory();
    auto outData = outBuffer->GetMemory();
    smf_frame_t *fvlc = (smf_frame_t *)inData->GetReadOnlyData();

    MEDIA_LOG_D("smf_decode fvlc: buff %p, size %d, flag 0x%0x", fvlc->buff, fvlc->size, fvlc->flags);
    MEDIA_LOG_D("smf_decode fpcm: buff %p, size %d, flag 0x%0x @%p", fpcm.buff, fpcm.size, fpcm.flags, &fpcm);

    if (false == smf_decode(smf_decoder, fvlc, &fpcm))
    {
        MEDIA_LOG_E("smf_decode error");
        return Status::ERROR_UNKNOWN;
    }
    MEDIA_LOG_D("smf decode: buff %p, size %d", fpcm.buff, fpcm.size);

    if (fpcm.buff == nullptr)
    {
        MEDIA_LOG_E("smf_decode buff nullptr error");
        return Status::ERROR_UNKNOWN;
    }
    if (fpcm.size == 0)
    {
        MEDIA_LOG_E("smf_decode size 0 error");
        return Status::ERROR_UNKNOWN;
    }
    if (fpcm.size > outData->GetCapacity())
    {
        MEDIA_LOG_E("smf_decode size too large %d error", fpcm.size);
        return Status::ERROR_UNKNOWN;
    }
    uint8_t *pcm_buffer = (uint8_t *)outData->GetWritableAddr(fpcm.size, 0);
    if (!pcm_buffer)
    {
        MEDIA_LOG_E("smf_decode GetWritableAddr size %d error", fpcm.size);
        return Status::ERROR_UNKNOWN;
    }
    memcpy(pcm_buffer, fpcm.buff, fpcm.size);
    fpcm.size = 0;
    outBuffer->pts = inBuffer->pts;
    return Status::OK;
}

Status SmfDecoderPlugin::GetPcmDataProcess(const std::shared_ptr<Buffer> &inputBuffer,
                                           std::shared_ptr<Buffer> &outputBuffer)
{
    if (inputBuffer == nullptr)
    {
        return Status::ERROR_NOT_ENOUGH_DATA;
    }
    if (outputBuffer == nullptr || outputBuffer->IsEmpty())
    {
        MEDIA_LOG_W("outputBuffer nullptr warning");
        return Status::ERROR_INVALID_PARAMETER;
    }
    if (inputBuffer->IsEmpty() && (inputBuffer->flag & BUFFER_FLAG_EOS) != 0)
    {
        MEDIA_LOG_I("eos received");
        outputBuffer->GetMemory()->Reset();
        outputBuffer->flag |= BUFFER_FLAG_EOS;
        return Status::END_OF_STREAM;
    }
    return SmfDecoderProcess(inputBuffer_, outputBuffer);
}

Status SmfDecoderPlugin::QueueInputBuffer(const std::shared_ptr<Buffer> &inputBuffer, int32_t timeoutMs)
{
    MEDIA_LOG_D("SmfDecoderPlugin QueueInputBuffer");
    (void)timeoutMs;
    if (inputBuffer->IsEmpty() && !(inputBuffer->flag & BUFFER_FLAG_EOS))
    {
        MEDIA_LOG_E("decoder does not support fd buffer");
        return Status::ERROR_INVALID_DATA;
    }
    else
    {
        // TODO: eos
        inputBuffer_ = inputBuffer;
        return Status::OK;
    }
}

Status SmfDecoderPlugin::SendOutputBuffer()
{
    MEDIA_LOG_D("send output buffer");
    OSAL::ScopedLock lock(ioMutex_);
    Status status = GetPcmDataProcess(inputBuffer_, outputBuffer_);
    inputBuffer_.reset();
    inputBuffer_ = nullptr;
    if (status == Status::OK || status == Status::END_OF_STREAM) {
        dataCb_->OnOutputBufferDone(outputBuffer_);
    }
    outputBuffer_.reset();
    outputBuffer_ = nullptr;
    return status;
}

Status SmfDecoderPlugin::QueueOutputBuffer(const std::shared_ptr<Buffer> &outputBuffer, int32_t timeoutMs)
{
    MEDIA_LOG_D("SmfDecoderPlugin QueueOutputBuffer");
    (void)timeoutMs;
    if (!outputBuffer)
    {
        return Status::ERROR_INVALID_PARAMETER;
    }
    outputBuffer_ = outputBuffer;
    return SendOutputBuffer();
}

Status SmfDecoderPlugin::Flush()
{
    MEDIA_LOG_D("SmfDecoderPlugin Flush");
    return Status::OK;
}

Status SmfDecoderPlugin::SetDataCallback(DataCallback *dataCallback)
{
    MEDIA_LOG_D("SmfDecoderPlugin SetDataCallback");
    dataCb_ = dataCallback;
    return Status::OK;
}

namespace {

using namespace OHOS::Media::Plugin;

typedef enum {
    SMF_HISTREAMER_DECODER_MP3,
    SMF_HISTREAMER_DECODER_PCM,
    SMF_HISTREAMER_DECODER_SBC,

    SMF_HISTREAMER_DECODER_UNKNOWN_BTU,
} smf_histreamer_decoder_t;

static bool smf_decoder_plugin_inited = false;

static uint32_t smf_supported_decoder_count = 0;

#define SMF_SUPPORTED_DECOER_KEY_COUNT_MAX SMF_HISTREAMER_DECODER_UNKNOWN_BTU

static const char *smf_supported_decoder_key[SMF_SUPPORTED_DECOER_KEY_COUNT_MAX + 1] = {nullptr};

void UpdatePluginDefinition(const char* key, CodecPluginDef &definition);

static void smf_decoder_plugin_init(void)
{
    if (smf_decoder_plugin_inited)
        return;

    smf_mp3_decoder_register();
    smf_pcm_decoder_register();
    smf_sbc_decoder_register();
    // TODO: add more decoder support. smf_histreamer_decoder_t need modify.

    smf_decoder_plugin_inited = true;
}

static const char *smf_decoder_iterate(int iter)
{
    if (smf_supported_decoder_count == 0)
    {
        MEDIA_LOG_I("SMF_SUPPORTED_DECOER_KEY_COUNT_MAX = %d", SMF_SUPPORTED_DECOER_KEY_COUNT_MAX);
        smf_supported_decoder_count = smf_supported_decoder(smf_supported_decoder_key, SMF_SUPPORTED_DECOER_KEY_COUNT_MAX);
    }
    return smf_supported_decoder_key[iter];
}

std::shared_ptr<CodecPlugin> SmfDecoderCreator(const std::string& name)
{
    MEDIA_LOG_D("name %s", name.c_str());
    return std::make_shared<SmfDecoderPlugin>(name);
}

Status RegisterAudioDecoderPlugins(const std::shared_ptr<Register> &reg)
{
    MEDIA_LOG_D("smf decoder register");
    if (!reg)
    {
        MEDIA_LOG_E("RegisterPlugins failed due to nullptr pointer for reg.");
        return Status::ERROR_INVALID_PARAMETER;
    }

    smf_decoder_plugin_init();

    const char *key = nullptr;
    int iter = 0;
    while (key = smf_decoder_iterate(iter++))
    {
        MEDIA_LOG_I("smf_decoder + %s", key);
        std::string pluginName = std::string(key);
        SMF_DECODER_T *smf_decoder = smf_create_decoder(key);
        CodecPluginDef definition;
        definition.name = key;
        definition.codecType = CodecType::AUDIO_DECODER;
        definition.rank = 100;
        definition.creator = SmfDecoderCreator;
        UpdatePluginDefinition(key, definition);
        if (reg->AddPlugin(definition) != Status::OK)
        {
            MEDIA_LOG_W("register smf decoder plugin failed");
        }
    }
    return Status::OK;
}

void UpdateInCaps(smf_histreamer_decoder_t shd, CodecPluginDef &definition)
{
    CapabilityBuilder capBuilder;
    DiscreteCapability<AudioChannelLayout> channelLayoutValues;
    DiscreteCapability<uint32_t> values;

    switch (shd)
    {
    case SMF_HISTREAMER_DECODER_MP3:
        capBuilder.SetMime(OHOS::Media::MEDIA_MIME_AUDIO_MPEG)
            .SetAudioMpegVersion(1)
            .SetAudioMpegLayerRange(1, 3);
        values = {8000, 16000, 22050, 44100, 48000, 32000};
        capBuilder.SetAudioSampleRateList(values);
        channelLayoutValues = {AudioChannelLayout::MONO, AudioChannelLayout::STEREO};
        capBuilder.SetAudioChannelLayoutList(channelLayoutValues);
        definition.inCaps.push_back(capBuilder.Build());
        break;
    case SMF_HISTREAMER_DECODER_PCM:
        capBuilder.SetMime(OHOS::Media::MEDIA_MIME_AUDIO_WAV);
        values = {8000, 16000, 22050, 44100, 48000, 32000};
        capBuilder.SetAudioSampleRateList(values);
        channelLayoutValues = {AudioChannelLayout::MONO, AudioChannelLayout::STEREO};
        capBuilder.SetAudioChannelLayoutList(channelLayoutValues);
        definition.inCaps.push_back(capBuilder.Build());
        break;
    case SMF_HISTREAMER_DECODER_SBC:
        capBuilder.SetMime(OHOS::Media::MEDIA_MIME_AUDIO_SBC);
        values = {8000, 16000, 22050, 44100, 48000, 32000};
        capBuilder.SetAudioSampleRateList(values);
        channelLayoutValues = {AudioChannelLayout::MONO, AudioChannelLayout::STEREO};
        capBuilder.SetAudioChannelLayoutList(channelLayoutValues);
        definition.inCaps.push_back(capBuilder.Build());
        break;
    default:
        MEDIA_LOG_E("%d not support update incaps", shd);
        break;
    }
}

static smf_histreamer_decoder_t smf_key_to_histreamer_doecer_type(const char *key)
{
    if (!strcmp(key, KEY_MP3))
        return SMF_HISTREAMER_DECODER_MP3;

    if (!strcmp(key, KEY_PCM))
        return SMF_HISTREAMER_DECODER_PCM;

    if(!strcmp(key, KEY_SBC))
        return SMF_HISTREAMER_DECODER_SBC;

    return SMF_HISTREAMER_DECODER_UNKNOWN_BTU;
}

void UpdateOutCaps(CodecPluginDef& definition)
{
    CapabilityBuilder capBuilder;
    capBuilder.SetMime(OHOS::Media::MEDIA_MIME_AUDIO_RAW);
    capBuilder.SetAudioSampleFormatList({AudioSampleFormat::S16});
    definition.outCaps.emplace_back(capBuilder.Build());
}

void UpdatePluginDefinition(const char* key, CodecPluginDef &definition)
{
    smf_histreamer_decoder_t shd = smf_key_to_histreamer_doecer_type(key);
    UpdateInCaps(shd, definition);
    UpdateOutCaps(definition);
}

void UnRegisterAudioDecoderPlugin()
{
    MEDIA_LOG_D("UnRegisterPlugins called.");
}

} // namespace
PLUGIN_DEFINITION(SmfDecoder, LicenseType::UNKNOWN, RegisterAudioDecoderPlugins, UnRegisterAudioDecoderPlugin);

} // namespace Smf
} // namespace Plugin
} // namespace Media
} // namespace OHOS
