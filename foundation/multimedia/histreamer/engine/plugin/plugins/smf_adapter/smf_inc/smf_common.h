#pragma once
#ifndef __SMF_COMMON_H__
#define __SMF_COMMON_H__
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif

//#ifdef __cplusplus 
//#if __cplusplus < 201103L
//#define override
//#define final
//#endif
//#endif
struct smf_media_info_t;
typedef struct smf_frame_t {
	void* buff;
	int max;
	int offset;
	int size;
	unsigned flags;
	struct smf_media_info_t* media;
	unsigned index;
	unsigned timestamp;
	struct smf_frame_t* frame;
}smf_frame_t;
typedef bool(*CbFrame)(smf_frame_t* frm,void*priv);

enum {
	SMF_FRAME_PACKED_MASK = 0xff,
	SMF_FRAME_IS_FAIL = 1u << 0,//0x01 error packet
	SMF_FRAME_IS_EOS = 1u << 1,//0x02 end of scan
	SMF_FRAME_IS_FIRST = 1u << 2,//0x04 first of scan
	SMF_FRAME_IS_EXTRA = 1u << 4,//0x10
	SMF_FRAME_IS_MEDIA = 1u << 5,//0x20	
	SMF_FRAME_INCOMPLETE = 1u << 6,//0x40
	SMF_FRAME_IS_MUTE = 1u << 7,//0x80	
	//
	SMF_FRAME_FROM_IPORT = 1u << 8,//0x80	
	SMF_FRAME_FROM_OPORT = 1u << 9,//0x80	
	//
	SMF_FRAME_CUST0 = 1u << 16,
	SMF_FRAME_CUST1 = 1u << 17,
	SMF_FRAME_CUST2 = 1u << 18,
	SMF_FRAME_CUST3 = 1u << 19,
	SMF_FRAME_CUST4 = 1u << 20,
	SMF_FRAME_CUST5 = 1u << 21,
	SMF_FRAME_CUST6 = 1u << 22,
	SMF_FRAME_CUST7 = 1u << 23,
	//
	//SMF_FRAME_INDEPENDENT_CHANNELS = 1u << 26,
	SMF_FRAME_PROCESS_MULTIFRAMES = 1u << 26,
	SMF_FRAME_BUFF_CACHED = 1u << 27,
	SMF_FRAME_IS_PLC = 1u << 28,
	SMF_FRAME_UNPACKED = 1u << 29,
	SMF_FRAME_PACKED = 1u << 30,
	SMF_FRAME_IS_PCM_NONINTERLACE = 1u << 31,	
	//
	SMF_FRAMR_RESULT = SMF_FRAME_IS_EOS | SMF_FRAME_IS_FAIL,
};
//
enum {
	SMF_PACKET_FLAGS_FAIL = SMF_FRAME_IS_FAIL,// 1u << 0,//error packet
	SMF_PACKET_FLAGS_EOS = SMF_FRAME_IS_EOS,//1u << 1,//end of scan
	SMF_PACKET_FLAGS_FIRST = SMF_FRAME_IS_FIRST,//1u << 2,//first of scan
	SMF_PACKET_FLAGS_EXTRA = SMF_FRAME_IS_EXTRA,//1u << 4,//extra data package
	SMF_PACKET_FLAGS_MEDIA = SMF_FRAME_IS_MEDIA,//1u << 5,//shm_media_t
	SMF_PACKET_INCOMPLETE = SMF_FRAME_INCOMPLETE,//1u << 6,
	//SMF_PACKET_INFOMATION = SMF_FRAME_INFOMATION,//1u << 7,
};
//
typedef struct {
	uint8_t seqNO:4;
	uint8_t crc:4;
	uint8_t flags;
	uint16_t payload_size;
}smf_packet_t;
//
typedef struct smf_media_info_t {
	uint64_t codec;	
	union {
		uint16_t flags;
		struct {
			bool is_video : 1;
			bool is_update : 1;
		};
	};	
	uint16_t extra_data_size;	
	uint8_t* extra_data;
	uint32_t frame_max;
	uint32_t frame_size;
	uint32_t frame_dms;//0.1ms
	uint32_t bitrate;
	union {
		struct{///audio
			uint32_t sample_rate;
			uint8_t sample_bits;
			uint8_t sample_width;//byte align
			uint8_t channels;
			uint8_t package;
			uint16_t frame_samples_max;
			uint16_t frame_samples;
			uint32_t band_width;
			union {
				uint8_t aflags;
				struct {
					bool is_non_interlace : 1;
					bool is_signed : 1;
					bool is_big_endian : 1;
					bool is_float : 1;
				};
			};
			uint8_t others;
			uint8_t alpha;
			uint8_t alpha_target;
		}audio;
		struct{///video
			uint16_t width;
			uint16_t height;
			uint16_t frame_rate;
			uint16_t frame_rate_denum;
			uint8_t format;
			uint8_t pixel_bits;
			union {
				uint8_t vflags;
				struct {
					bool is_v000 : 1;
				};
			};
		}video;
	};	
	struct smf_media_info_t* media;
}smf_media_info_t;
//
typedef struct {
	const char* url;
	const char* title;
	const char* artist;
	const char* album;
	uint32_t duration;
}smf_meta_info_t;
//
typedef struct {
	uint32_t playtime;
	uint32_t frameindex;
	uint32_t position;
	uint32_t frameMax;
	uint32_t frameMin;
	smf_media_info_t* media;
}smf_stream_status_t;
//
typedef struct {
	uint32_t params[6];
}smf_open_param_t;
//
typedef struct {	
	smf_media_info_t* media;
	smf_stream_status_t* total;
	smf_stream_status_t* current;
	bool packed;
	bool send_extra;
}smf_source_param_t;
//
typedef struct {
	smf_media_info_t media;
}smf_sink_param_t;
//
typedef struct {
	smf_media_info_t media;
}smf_filter_param_t;
//
typedef struct {
	smf_media_info_t media;
}smf_encode_param_t;
//
typedef struct {
	smf_media_info_t media;
}smf_decode_param_t;
//
typedef struct {
	const char* url;
	const char* para;
	void* other;
}smf_io_param_t;
//
typedef struct {
	smf_io_param_t file;
	char* buff;
	int size;
}smf_io_buff_param_t;
//
typedef struct {
	smf_io_param_t file;
	void* handle;
	void*(*open)(void*);
	bool(*close)(void* hd);
	bool(*seek)(void* hd, int offset, int pos);
	unsigned(*getsize)(void* hd);
	unsigned(*offset)(void* hd);
	unsigned(*read)(void* hd, void* buff, unsigned size);
	unsigned(*write)(void* hd, void* buff, unsigned size);
}smf_io_callback_param_t;
//
typedef struct {
	void* data;
	unsigned size;
}smf_pair_t;
//
typedef struct {
	unsigned keys;
	unsigned vals;
}smf_keys_value_t;
//
typedef union {
	int8_t i8[4];	
	int16_t i16[2];	
	int32_t i32;
	uint8_t u8[4];
	uint16_t u16[2];
	uint32_t u32;
}smf_int8x4_t;
//
typedef union {
	int8_t  i8[8];	
	int16_t i16[4];	
	int32_t i32[2];
	int64_t i64;
	uint8_t u8[8];
	uint16_t u16[4];
	uint32_t u32[2];
	uint64_t u64;
}smf_int8x8_t;

typedef struct smf_fifo_t {
	void* data;
	uint32_t max;
	uint32_t mi;
	uint32_t wi;
	uint32_t ri;
}smf_fifo_t;

typedef struct smf_ring_t {
	void* data;
	uint32_t max;
	uint32_t mi;
	uint32_t wi;
}smf_ring_t;

typedef struct smf_elememt_data_t {
	int frameMax;
	int frameMin;
	smf_frame_t frame;
	smf_media_info_t media;
	smf_stream_status_t status;
}smf_elememt_data_t;

typedef struct {
	uint8_t* begin;
	uint8_t* end;
	uint8_t* ptr;
	uint8_t* ptr_end;
}smf_shared_pool_t;

typedef void (*smf_hook_t)(void* data, uint32_t size, void* priv);
#endif
