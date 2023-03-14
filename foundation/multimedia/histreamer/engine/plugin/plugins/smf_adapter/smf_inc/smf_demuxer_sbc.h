#pragma once
#ifndef __SMF_DEMUXER_SBC_H__
#define __SMF_DEMUXER_SBC_H__

#include "smf_demuxer.h"

///
typedef struct{
	smf_source_param_t src;
}smf_sbc_demux_open_param_t;

/* register lc3 demuxer,support file "*.sbc".
 * @see libsmfpluginsbc.a
 */
EXTERNC void smf_sbc_demuxer_register();


///
typedef struct{
	smf_source_param_t src;
}smf_msbc_demux_open_param_t;

/* register lc3 demuxer,support file "*.msbc".
 * @see libsmfpluginsbc.a
 */
EXTERNC void smf_msbc_demuxer_register();

typedef struct {
	smf_source_param_t src;
}smf_gsbc_demux_open_param_t;

/* register lc3 demuxer,support file "*.msbc".
 * @see libsmfpluginsbc.a
 */
EXTERNC void smf_gsbc_demuxer_register();

#endif
