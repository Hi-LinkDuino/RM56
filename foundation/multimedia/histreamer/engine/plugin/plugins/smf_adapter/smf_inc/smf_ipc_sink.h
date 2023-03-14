#pragma once
#ifndef __SMF_IPC_SINK_H__
#define __SMF_IPC_SINK_H__
#include "smf_common.h"

typedef struct smf_ipc_sink_open_param_t {
	smf_sink_param_t sink;
}smf_ipc_sink_open_param_t;

/**
 * register ipc sink
 */

EXTERNC void smf_ipc_sink_register(void);
#endif