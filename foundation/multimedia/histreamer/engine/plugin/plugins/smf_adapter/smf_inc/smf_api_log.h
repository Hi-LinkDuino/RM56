#pragma once
#ifndef __SMF_API_LOG_H__
#define __SMF_API_LOG_H__
#include "smf_common.h"

//
typedef int (*smf_log_send_cb_t)(const char* str);
typedef int (*smf_log_send_cb1_t)(const char* str, int size);
typedef int (*smf_log_send_cb2_t)(const char* str, int size, int channel);


/*log*/
EXTERNC void smf_log_init(void* tempbuff, int size);
EXTERNC void smf_log_redirect(int (*send)(const char* str));
EXTERNC void smf_log_redirect1(int (*send)(const char* str, int size));
EXTERNC void smf_log_redirect2(int (*send)(const char* str, int size, int channel));
EXTERNC void smf_log_disable_output_channels(uint32_t channels_mask);
EXTERNC void smf_log_disable_output_channel(int channels);
EXTERNC void smf_log_enable_output_channel(int channels);
#endif
