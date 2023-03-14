#ifndef HISTREAMER_SMF_DEMUXER_PLUGIN_H
#define HISTREAMER_SMF_DEMUXER_PLUGIN_H

#include <memory>
#include <string>
#include <vector>
#include "foundation/osal/thread/mutex.h"
#include "plugin/interface/demuxer_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "smf_inc/smf_api.h"
#include "smf_api_histreamer.h"
#ifdef SMF_FS_CALLBACK_REGISTER_IOFILE
#include "smf_inc/smf_ohos.h"
#endif
#ifdef __cplusplus
}
#endif

namespace OHOS {
namespace Media {
namespace Plugin {
namespace Smf {
class SmfDemuxerPlugin : public DemuxerPlugin {
public:
    explicit SmfDemuxerPlugin(std::string name);
    ~SmfDemuxerPlugin() override;

    Status Init() override;
    Status Deinit() override;
    Status Prepare() override;
    Status Reset() override;
    Status Start();
    Status Stop();
    Status GetParameter(Tag tag, ValueType& value) override;
    Status SetParameter(Tag tag, const ValueType& value) override;
    // std::shared_ptr<Allocator> GetAllocator() override;
    Status SetCallback(Callback* cb) override;

    Status SetDataSource(const std::shared_ptr<DataSource>& source) override;
    Status GetMediaInfo(MediaInfo& mediaInfo) override;
    size_t GetTrackCount() override;
    Status SelectTrack(int32_t trackId) override;
    Status UnselectTrack(int32_t trackId) override;
    Status GetSelectedTracks(std::vector<int32_t>& trackIds) override;
    Status ReadFrame(Buffer& info, int32_t timeOutMs) override;
    Status SeekTo(int32_t trackId, int64_t hstTime, SeekMode mode) override;

    struct IOContext {
        std::shared_ptr<DataSource> dataSource {nullptr};
        int64_t offset {0};
        bool eos {false};
    };
private:

    // static void* SmfIOOpen(const char* filename, const char* modes);
    // static int SmfIOClose(void* stream);
    // static size_t SmfIORead(void* ptr, size_t size, size_t n, void* stream);
    // static size_t SmfIOWrite(const void* ptr, size_t size, size_t nmemb, void* stream);
    // static int SmfIOSeek(void* stream, long int offset, int whence);
    // static long SmfIOTell(void *stream);
    // static long SmfIOGetSize(void* stream);

    SMF_DEMUXER_T *smf_demuxer;
    SMF_DEMUXER_PARAM_T smf_demuxer_param;

    IOContext ioContext_;
    Callback* callback_ {};

};
} // namespace Smf
} // namespace Plugin
} // namespace Media
} // namespace OHOS

#endif // HISTREAMER_SMF_DEMUXER_PLUGIN_H