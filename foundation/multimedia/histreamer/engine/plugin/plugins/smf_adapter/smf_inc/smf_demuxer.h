#pragma once
#ifndef __SMF_DEMUXER_H__
#define __SMF_DEMUXER_H__

#include "smf_api.h"
typedef struct {
	bool estimated;
	bool unsupported;
	int bitrate_max_kbps;
	int bitrate_avg_kbps;
	int demux_cpu_max_mips;
	int demux_cpu_avg_mips;
	int demux_memory_max_kbytes;
	int demux_memory_avg_kbytes;
	int decode_cpu_max_mips;
	int decode_cpu_avg_mips;
	int decode_memory_max_kbytes;
	int decode_memory_avg_kbytes;
}smf_demuxer_estimate_t;
//
typedef struct {
	//smf_source_param_t src;
	smf_media_info_t* media;
	smf_stream_status_t* total;
	smf_stream_status_t* current;	
	bool packed;
	bool sendExtra;
	//smf_source_param_t end
	bool checkSum;
	bool parseMeta;
	bool non_header;
	///
	smf_demuxer_estimate_t* estimate;
	smf_meta_info_t* meta;
	///
	const char* url;
	int startFrameIndex;
	int startPostion;
	int startTimepoint;
	void* other;
	///
	void* io;
	int ioReadCacheLen;
	int ioReadAlign;
	///			
	uint32_t frame_size;
	uint32_t frame_duration_us;
}smf_demuxer_param_t;

#endif
