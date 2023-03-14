
#define HST_LOG_TAG "SMFDEMUXER"
// #define MEDIA_LOG_DEBUG 1

#include "smf_demuxer_plugin.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <new>
#include "foundation/log.h"
#include "osal/thread/scoped_lock.h"
#include "plugin/common/plugin_buffer.h"
#include "plugin/common/plugin_time.h"
#include "osal/utils/util.h"
#include "utils/constants.h"

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Smf {

namespace {
struct IOContext
{
    std::shared_ptr<DataSource> dataSource{nullptr};
    int64_t offset{0};
    bool eos{false};
};

void *SmfIOOpen(const char *filename, const char *modes)
{
    IOContext *iocontext = (IOContext *)filename;
    MEDIA_LOG_I("SmfIOOpen iocontext @ %p", iocontext);
    return iocontext;
}

int SmfIOClose(void *stream)
{
    MEDIA_LOG_I("SmfIOClose");
    return 0;
}

size_t SmfIORead(void *ptr, size_t size, size_t n, void *stream)
{
    MEDIA_LOG_D("SmfIORead process in stream @ %p", stream);
    MEDIA_LOG_D("SmfIORead size= " PUBLIC_LOG "d", size);
    MEDIA_LOG_D("SmfIORead n= " PUBLIC_LOG "d", n);

    int rtv = 0;
    auto ioContext = static_cast<IOContext *>(stream);
    size_t read_size = size * n;
    MEDIA_LOG_D("SmfIORead want_read_size %d", read_size);
    if (ioContext && ioContext->dataSource)
    {
        auto buffer = std::make_shared<Buffer>();
        auto bufData = buffer->WrapMemory((uint8_t *)ptr, read_size, 0);
        auto result = ioContext->dataSource->ReadAt(ioContext->offset, buffer, static_cast<size_t>(read_size));
        MEDIA_LOG_D("SmfIORead read data read_size= " PUBLIC_LOG "d", static_cast<int>(bufData->GetSize()));
        if (result == Status::OK)
        {
            MEDIA_LOG_D("SmfIORead origin_offset %d", ioContext->offset);
            ioContext->offset += buffer->GetMemory()->GetSize();
            rtv = buffer->GetMemory()->GetSize();
            MEDIA_LOG_D("SmfIORead real_read_size %d", rtv);
            MEDIA_LOG_D("SmfIORead offset %d", ioContext->offset);
        }
        else if (result == Status::END_OF_STREAM)
        {
            ioContext->eos = true;
            rtv = -1; // eof
        }
        else
        {
            MEDIA_LOG_E("SmfIORead failed with rtv = " PUBLIC_LOG "d", static_cast<int>(result));
        }
    }
    else
    {
        MEDIA_LOG_E("SmfIORead ERROR");
        MEDIA_LOG_E("ioContext -%p", ioContext);
        MEDIA_LOG_E("ioContext->dataSource -%p", ioContext->dataSource);
    }

    return rtv;
}

size_t SmfIOWrite(const void *ptr, size_t size, size_t nmemb, void *stream)
{
    MEDIA_LOG_E("SmfIOWrite NOT support");
    return 0;
}

int SmfIOSeek(void *stream, long int offset, int whence)
{
    MEDIA_LOG_D("SmfIOSeek process in");
    MEDIA_LOG_D("SmfIOSeek offset= " PUBLIC_LOG "d", offset);
    MEDIA_LOG_D("SmfIOSeek whence= " PUBLIC_LOG "d", whence);

    uint64_t newPos = 0;
    int rtv = 0;
    auto ioContext = static_cast<IOContext *>(stream);
    switch (whence)
    {
    case SEEK_SET:
        newPos = static_cast<uint64_t>(offset);
        MEDIA_LOG_D("SmfIOSeek cruPos %d", offset);
        MEDIA_LOG_D("SmfIOSeek newPos %d", newPos);
        break;
    case SEEK_CUR:
        newPos = ioContext->offset + offset;
        break;
    case SEEK_END:
    {
        size_t mediaDataSize = 0;
        if (ioContext->dataSource->GetSize(mediaDataSize) == Status::OK)
        {
            newPos = mediaDataSize + offset;
            MEDIA_LOG_D("SmfIOSeek cruPos %d", mediaDataSize);
            MEDIA_LOG_D("SmfIOSeek cruPos %d", offset);
            MEDIA_LOG_D("SmfIOSeek newPos %d", newPos);
        }
        break;
    }
    default:
        MEDIA_LOG_E("SmfIOSeek unexpected whence: " PUBLIC_LOG "d", whence);
        rtv = -1;
        break;
    }
    ioContext->offset = newPos;
    MEDIA_LOG_D("current offset: " PUBLIC_LOG PRId64 ", new pos: " PUBLIC_LOG PRIu64,
                ioContext->offset, newPos);
    return rtv;
}

long SmfIOTell(void *stream)
{
    MEDIA_LOG_D("process in");

    auto ioContext = static_cast<IOContext *>(stream);
    if (ioContext && ioContext->dataSource)
    {
        return ioContext->offset;
    }
    return -1;
}

long SmfIOGetSize(void *stream)
{
    MEDIA_LOG_D("process in");

    auto ioContext = static_cast<IOContext *>(stream);
    if (ioContext && ioContext->dataSource)
    {
        size_t mediaDataSize = 0;
        if (ioContext->dataSource->GetSize(mediaDataSize) == Status::OK)
        {
            return mediaDataSize;
        }
    }
    return -1;
}
}

namespace {
int Sniff(const std::string &pluginName, std::shared_ptr<DataSource> dataSource);

Status RegisterPlugin(const std::shared_ptr<Register> &reg);

static bool smf_demuxer_plugin_inited = false;

#define SMF_SUPPORTED_DEMUXER_KEY_COUNT_MAX 2

static const char *smf_supported_demuxer_key[SMF_SUPPORTED_DEMUXER_KEY_COUNT_MAX+1] = {NULL};

static int smf_supported_demuxer_count = 0;

static void smf_demuxer_plugin_init(void);

static const char *smf_demuxer_iterate(int iter);
}

SmfDemuxerPlugin::SmfDemuxerPlugin(std::string name)
    : DemuxerPlugin(std::move(name)),
      ioContext_(),
      callback_(nullptr)
{
    smf_demuxer = nullptr;
    MEDIA_LOG_I("SmfDemuxerPlugin  name %s", pluginName_.c_str());
}

SmfDemuxerPlugin::~SmfDemuxerPlugin()
{
    MEDIA_LOG_I("~SmfDemuxerPlugin");
    smf_demuxer = nullptr;
}

Status SmfDemuxerPlugin::Init()
{
    MEDIA_LOG_D("Init called.");
    Reset();
#ifndef SMF_FS_CALLBACK_REGISTER_IOFILE
    smf_fs_callback_register(SmfIOOpen, SmfIOClose,
                             SmfIORead, SmfIOWrite,
                             SmfIOSeek, SmfIOTell,
                             SmfIOGetSize);
#else
	ohos_file_ops ops {
	    SmfIOOpen,
	    SmfIOClose,
	    SmfIORead,
	    SmfIOWrite,
	    SmfIOSeek,
	    SmfIOTell,
	    SmfIOGetSize
	};
	smf_ohos_fs_callback_register(ops);
#endif

    const char *key = pluginName_.c_str();
    MEDIA_LOG_I("SmfDemuxerPlugin Init. key %s", key);

    smf_demuxer = smf_create_demuxer(key);
    if (!smf_demuxer)
    {
        MEDIA_LOG_E("SmfDemuxerCreate error");
        return Status::ERROR_NULL_POINTER;
    }
    MEDIA_LOG_D("smf_create_demuxer succ");

    return Status::OK;
}

Status SmfDemuxerPlugin::Deinit()
{
    MEDIA_LOG_D("Deinit called.");

    if (smf_demuxer)
    {
        smf_destroy(smf_demuxer);
        smf_demuxer = nullptr;
    }

    smf_uninit();

    return Status::OK;
}

Status SmfDemuxerPlugin::Prepare()
{
    MEDIA_LOG_D("Prepare called.");
    if (!smf_demuxer)
    {
        MEDIA_LOG_E("Prepare smf_demuxer NULL error");
        return Status::ERROR_INVALID_OPERATION;
    }
    return Status::OK;
}

Status SmfDemuxerPlugin::Reset()
{
    MEDIA_LOG_D("Reset called.");
    smf_reset();
    ioContext_.offset = 0;
    ioContext_.eos = false;
    return Status::OK;
}

Status SmfDemuxerPlugin::Start()
{
    MEDIA_LOG_D("Start called.");
    return Status::OK;
}

Status SmfDemuxerPlugin::Stop()
{
    MEDIA_LOG_D("Stop called.");
    return Status::OK;
}

Status SmfDemuxerPlugin::GetParameter(Tag tag, ValueType &value)
{
    MEDIA_LOG_D("GetParameter called.");
    return Status::OK;
}

Status SmfDemuxerPlugin::SetParameter(Tag tag, const ValueType &value)
{
    MEDIA_LOG_D("SetParameter called.");
    return Status::OK;
}

Status SmfDemuxerPlugin::SetCallback(Callback *cb)
{
    MEDIA_LOG_D("SetCallback called.");
    return Status::OK;
}

Status SmfDemuxerPlugin::SetDataSource(const std::shared_ptr<DataSource> &source)
{
    MEDIA_LOG_D("SetDataSource called.");
    ioContext_.dataSource = source;
    return Status::OK;
}

Status SmfDemuxerPlugin::GetMediaInfo(MediaInfo &mediaInfo)
{
    MEDIA_LOG_D("GetMediaInfo called.");
    memset(&smf_demuxer_param, 0, sizeof(smf_demuxer_param));
    smf_demuxer_param.packed = true;
#ifndef SMF_FS_CALLBACK_REGISTER_IOFILE
    smf_demuxer_param.url = (const char *)&ioContext_;
    MEDIA_LOG_D("Prepare url @ %p", smf_demuxer_param.url);
#else
    void *io = smf_ohos_create_io(&ioContext_);
    MEDIA_LOG_D("io %p.", io);
    if (io) {
         smf_demuxer_param.io = io;
    }
#endif
    smf_demuxer_param.sendExtra = false;
    smf_demuxer_param.checkSum = false;
    if (!smf_open(smf_demuxer, &smf_demuxer_param))
    {
        smf_print_error(smf_demuxer);
        MEDIA_LOG_E("Prepare smf_demuxer smf_open error");
        return Status::ERROR_INVALID_PARAMETER;
    }
    if (smf_demuxer_param.media)
    {
        MEDIA_LOG_I("GetMediaInfo codec %s", (const char *)&smf_demuxer_param.media->codec);
        MEDIA_LOG_I("GetMediaInfo sample_rate %d", smf_demuxer_param.media->audio.sample_rate);
        MEDIA_LOG_I("GetMediaInfo sample_bits %d", smf_demuxer_param.media->audio.sample_bits);
        MEDIA_LOG_I("GetMediaInfo channels %d", smf_demuxer_param.media->audio.channels);
        mediaInfo.tracks.resize(1);
        if (smf_demuxer_param.media->audio.channels == 1)
        {
            mediaInfo.tracks[0].Insert<Tag::AUDIO_CHANNEL_LAYOUT>(AudioChannelLayout::MONO);        
        }
        else if (smf_demuxer_param.media->audio.channels == 2)
        {
            mediaInfo.tracks[0].Insert<Tag::AUDIO_CHANNEL_LAYOUT>(AudioChannelLayout::STEREO);
        }
        if (smf_demuxer_param.media->audio.sample_bits == 16)
        {
            mediaInfo.tracks[0].Insert<Tag::AUDIO_SAMPLE_FORMAT>(AudioSampleFormat::S16);
        }
        mediaInfo.tracks[0].Insert<Tag::AUDIO_SAMPLE_RATE>(smf_demuxer_param.media->audio.sample_rate);
        mediaInfo.tracks[0].Insert<Tag::MEDIA_BITRATE>(smf_demuxer_param.media->bitrate);
        mediaInfo.tracks[0].Insert<Tag::AUDIO_CHANNELS>(smf_demuxer_param.media->audio.channels);
        mediaInfo.tracks[0].Insert<Tag::TRACK_ID>(0);
        if (!strcmp("mp3", (const char *)&smf_demuxer_param.media->codec))
        {
            mediaInfo.tracks[0].Insert<Tag::MIME>(MEDIA_MIME_AUDIO_MPEG);
        }
        if (!strcmp("pcm", (const char *)&smf_demuxer_param.media->codec))
        {
            mediaInfo.tracks[0].Insert<Tag::MIME>(MEDIA_MIME_AUDIO_WAV);
            mediaInfo.tracks[0].Insert<Tag::AUDIO_SAMPLE_PER_FRAME>(8192);
        }
        return Status::OK;
    }
    return Status::OK;
}

size_t SmfDemuxerPlugin::GetTrackCount()
{
    MEDIA_LOG_D("GetTrackCount called.");
    return 0;
}

Status SmfDemuxerPlugin::SelectTrack(int32_t trackId)
{
    MEDIA_LOG_D("SelectTrack called.");
    return Status::OK;
}

Status SmfDemuxerPlugin::UnselectTrack(int32_t trackId)
{
    MEDIA_LOG_D("UnselectTrack called.");
    return Status::OK;
}

Status SmfDemuxerPlugin::GetSelectedTracks(std::vector<int32_t> &trackIds)
{
    MEDIA_LOG_D("GetSelectedTracks called.");
    return Status::OK;
}

Status SmfDemuxerPlugin::ReadFrame(Buffer &frameInfo, int32_t timeOutMs)
{
    MEDIA_LOG_D("ReadFrame called.");
    if (!smf_demuxer)
    {
        return Status::ERROR_UNKNOWN;
    }

    smf_frame_t fvlc;
    memset(&fvlc, 0, sizeof(fvlc));
    if (false == smf_output(smf_demuxer, &fvlc))
    {
        MEDIA_LOG_E("fvlc output error");
        return Status::ERROR_UNKNOWN;
    }
    MEDIA_LOG_D("smf demuxer buff %p, size %d, flag 0x%0x, sizeof(fvlc) = %d", fvlc.buff, fvlc.size, fvlc.flags, sizeof(fvlc));

    frameInfo.trackID = static_cast<uint32_t>(0);

    int frameSize = sizeof(fvlc);
    auto data = frameInfo.AllocMemory(nullptr, frameSize);
    if (data)
    {
        size_t writeSize = data->Write((const uint8_t *)&fvlc, frameSize);
        if (writeSize != frameSize)
        {
            MEDIA_LOG_E("fvlc copy error");
            return Status::ERROR_UNKNOWN;
        }
    }

    return Status::OK;
}

Status SmfDemuxerPlugin::SeekTo(int32_t trackId, int64_t hstTime, SeekMode mode)
{
    MEDIA_LOG_D("SeekTo called.");
    return Status::OK;
}

namespace {
int Sniff(const std::string &pluginName, std::shared_ptr<DataSource> dataSource)
{
    MEDIA_LOG_I("SMF ENTER Sniff name: %s", pluginName.c_str());
    if (pluginName.empty() || !dataSource)
    {
        MEDIA_LOG_E("Sniff failed due to empty plugin name or dataSource invalid.");
        return 0;
    }
    IOContext ioContext_;
    ioContext_.dataSource = dataSource;
#ifndef SMF_FS_CALLBACK_REGISTER_IOFILE
    smf_fs_callback_register(SmfIOOpen, SmfIOClose,
                             SmfIORead, SmfIOWrite,
                             SmfIOSeek, SmfIOTell,
                             SmfIOGetSize);
#else
	ohos_file_ops ops {
        SmfIOOpen,
        SmfIOClose,
        SmfIORead,
        SmfIOWrite,
        SmfIOSeek,
        SmfIOTell,
        SmfIOGetSize
    };
    smf_ohos_fs_callback_register(ops);
#endif
    const char *fname = (const char *)&ioContext_;
    MEDIA_LOG_D("SMF Sniff fname @ %p", fname);
#ifndef SMF_FS_CALLBACK_REGISTER_IOFILE
    const char *format = smf_recognize_demuxer(fname);
#else
	const char *format = smf_recognize_demuxer2(fname, &ioContext_);
#endif
    int confidence = 0;
    if (!strcmp(format, pluginName.c_str()))
        confidence = 100;
    MEDIA_LOG_I("SMF LEAVE Sniff real_format: %s, name %s, conf %d", format, pluginName.c_str(), confidence);
    return confidence;
}

static void smf_demuxer_plugin_init(void)
{
    if (smf_demuxer_plugin_inited)
        return;

    smf_init();

    smf_mp3_demuxer_register();
    smf_wav_demuxer_register();
    // TODO: add more format support. SMF_SUPPORTED_DEMUXER_KEY_COUNT_MAX need +1.
    
    smf_demuxer_plugin_inited = true;
}

static const char *smf_demuxer_iterate(int iter)
{
    if (smf_supported_demuxer_count == 0)
    {
        smf_supported_demuxer_count = smf_supported_demuxer(smf_supported_demuxer_key, SMF_SUPPORTED_DEMUXER_KEY_COUNT_MAX);
    }
    return smf_supported_demuxer_key[iter];
}

std::shared_ptr<DemuxerPlugin> SmfDemuxerCreator(const std::string& name)
{
    MEDIA_LOG_D("SmfDemuxerCreator name %s", name.c_str());
    return std::make_shared<SmfDemuxerPlugin>(name);
}

Status RegisterPlugin(const std::shared_ptr<Register> &reg)
{
    MEDIA_LOG_D("RegisterPlugin called.");
    if (!reg)
    {
        MEDIA_LOG_E("RegisterPlugin failed due to nullptr pointer for reg.");
        return Status::ERROR_INVALID_PARAMETER;
    }

    smf_demuxer_plugin_init();

    const char *key = nullptr;
    int iter = 0;
    while (key = smf_demuxer_iterate(iter++))
    {
        MEDIA_LOG_I("smf_demuxer + %s", key);
        std::string pluginName = std::string(key);
        DemuxerPluginDef regInfo;
        regInfo.name = pluginName;
        regInfo.description = "adapter for smf demuxer plugin";
        regInfo.rank = 100;
        regInfo.creator = SmfDemuxerCreator;
        regInfo.sniffer = Sniff;
        auto rtv = reg->AddPlugin(regInfo);
        if (rtv != Status::OK)
        {
            MEDIA_LOG_E("RegisterPlugin AddPlugin failed with return " PUBLIC_LOG "d", static_cast<int>(rtv));
        }
    }
    return Status::OK;
}
}

PLUGIN_DEFINITION(SmfDemuxer, LicenseType::UNKNOWN, RegisterPlugin, [] {});
} // namespace Minimp3
} // namespace Plugin
} // namespace Media
} // namespace OHOS
