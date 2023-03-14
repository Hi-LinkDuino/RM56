#pragma once
#ifndef __SMF_API_H__
#define __SMF_API_H__
#include "smf_common.h"
#include "smf_error.h"
#include "smf_api_def.h"
#include "smf_api_os.h"
#include "smf_api_pool.h"
#include "smf_api_log.h"
#include "smf_api_fs.h"
#include "smf_api_supported.h"
#include "smf_api_other.h"

/** get smf version. the first called api.
 * @return version string.
 */
EXTERNC const char* smf_version(void);

/** initialize smf, the first called api.
 */
EXTERNC bool smf_init(void);

/** uninitialize smf, called by last
 */
EXTERNC void smf_uninit(void);

/** reinitialize smf
 */
EXTERNC bool smf_reset(void);

/** create a decoder with codec keywords.
 * @param codec[in]:the codec keywords
 * @return the decoder instance
 */
EXTERNC void* smf_create_decoder(const char* codec);

/** create a encoder with codec keywords.
 * @param codec[in]:the codec keywords
 * @return the encoder instance
 */
EXTERNC void* smf_create_encoder(const char* codec);

/** create a filter with keywords.
 * @param keywords[in]:the filter keywords
 * @return the filter instance
 */
EXTERNC void* smf_create_processer(const char* keywords);

/** create a demuxer with keywords.
 * @param keywords[in]:the demuxer keywords
 * @return the demuxer instance
 */
EXTERNC void* smf_create_demuxer(const char* keywords);

/** create a muxer with keywords.
 * @param keywords[in]:the muxer keywords
 * @return the muxer instance
 */
EXTERNC void* smf_create_muxer(const char* keywords);

/** get the supported filter list.
 * @param type[in],
 * @param keys[out], the filter keywords list, end with NULL.
 * @param max[in], the keys buff max
 * @return the output keys count
 */
EXTERNC int smf_supports(const char* type, const char* keys[], int max);

/** check keys-type object is supported.
 * @param type[in],
 * @param keywords[in],
 * @return true/false
 */
EXTERNC int smf_is_supported(const char* type, const char* keywords);

/** create a object with keywords and type.
 * @param type[in]:the object type
 * @param keywords[in]:the object keywords
 * @param subkeywords0[in]:the object sub-keywords-0
 * @param subkeywords1[in]:the object sub-keywords-1
 * @return the object instance
 */
EXTERNC void* smf_create(const char* type,const char* keywords,const char* subkeywords0,const char* subkeywords1);

/** destroy the audio filter instance.
 * @hd[in]:the instance
 */
EXTERNC void smf_destroy(void* hd);

/** find a object with name.
 * @param name[in]:the object name
 * @return the object instance
 */
EXTERNC void* smf_find(const char* name);

/** get keywords from instance
* @hd[in]:the instance
* @return: type keywords string
*/
EXTERNC const char* smf_get_keywords(void* hd);

/** get type from instance
* @hd[in]:the instance
* @return: type string
*/
EXTERNC const char* smf_get_type(void* hd);

/** open the filter with params
 * @param hd[in]:the instance
 * @param param[in]:the params.
 * @return true/false
 */
EXTERNC bool smf_open(void* hd, void* param);

/** close the filter
 * @param hd[in]:the instance
 */
EXTERNC void smf_close(void* hd);

/** decode process
 * @param hd[in]:the instance
 * @param input[in/out]:the input frame
 * @param output[in/out]:the output frame
 * @return true/false
 */
EXTERNC bool smf_decode(void* hd, smf_frame_t* input, smf_frame_t* output);

/** encode process
 * @param hd[in]:the instance
 * @param input[in/out]:the input frame
 * @param output[in/out]:the output frame
 * @return true/false
 */
EXTERNC bool smf_encode(void* hd, smf_frame_t* input, smf_frame_t* output);

/** filter process
 * @param hd[in]:the instance
 * @param input[in/out]:the input frame
 * @param output[in/out]:the output frame
 * @return true/false
 */
EXTERNC bool smf_process(void* hd, smf_frame_t* input, smf_frame_t* output);

/** output frame from source
 * @param hd[in]:the instance
 * @param input[out]:the output frame
 * @return true/false
 */
EXTERNC bool smf_output(void* hd, smf_frame_t* output);

/** input frame from source
 * @param hd[in]:the instance
 * @param input[in]:the input frame
 * @return true/false
 */
EXTERNC bool smf_input(void* hd, smf_frame_t* input);

/** set parameter to instance
 * @param hd[in]:the instance
 * @param key[in]:the parameter keywords, see:
 *        smf_set_param_e
 *        xxx_xxx_set_param_e
 * @param val[in]:the parameter value
 * @return true/false
 */
EXTERNC bool smf_set(void* hd, uint32_t key, void* val);

/** get parameter from instance
 * @param hd[in]:the instance
 * @param key[in]:the parameter keywords
 *        smf_get_param_e
 *        xxx_xxx_get_param_e
 * @param val[out]:the parameter value
 * @return true/false
 */
EXTERNC bool smf_get(void* hd, uint32_t key, void*val);

/** get the minimum size of input
 * @param hd[in]:the instance
 * @return the minimum size of input
 */
EXTERNC int smf_get_input_minsize(void* hd);

/** get the maximum size of input
 * @param hd[in]:the instance
 * @return the maximum size of input
 */
EXTERNC int smf_get_input_maxsize(void* hd);

/** get the minimum size of output
 * @param hd[in]:the instance
 * @return the minimum size of output
 */
EXTERNC int smf_get_output_minsize(void* hd);

/** get the maximum size of output
 * @param hd[in]:the instance
 * @return the maximum size of output
 */
EXTERNC int smf_get_output_maxsize(void* hd);

/** get the current error
 * @param hd[in]:the instance
 * @return the current error
 */
EXTERNC smf_error_t* smf_get_error(void* hd);

/** get the current error ID(16bit)
 * @param hd[in]:the instance
 * @return the current error ID(16bit)
 */
EXTERNC unsigned smf_get_err32(void* hd);

/** print the current error
 * @param hd[in]:the instance
 */
EXTERNC void smf_print_error(void*hd);

/** register input callback
 * @param hd[in]:the instance
 * @param cb[in]:callback
 * @param priv[in]:the callback private parameter
 * @return true/false
 */
EXTERNC bool smf_register_input (void*hd, CbFrame cb,void*priv);

/** register output callback
 * @param hd[in]:the instance
 * @param cb[in]:callback
 * @param priv[in]:the callback private parameter
 * @return true/false
 */
EXTERNC bool smf_register_output(void*hd, CbFrame cb,void*priv);

/** register io type with callback
 * @param keywords[in]: keywords
 * @param open[in]: open callback
 * @param close[in]: close callback
 * @param seek[in]: seek callback
 * @param tell[in]: tell callback
 * @param getsize[in]: getsize callback
 * @param read[in]: read callback
 * @param write[in]: write callback
 * @return true/false
 */
EXTERNC void io_callback_register(const char* keywords
	, void* (*open)(void*)
	, bool(*close)(void* hd)
	, bool(*seek)(void* hd, int offset, int pos)
	, unsigned(*tell)(void* hd)
	, unsigned(*getsize)(void* hd)
	, unsigned(*read)(void* hd, void* buff, unsigned size)
	, unsigned(*write)(void* hd, void* buff, unsigned size)
);

/* recognize demuxer automatically 
* @param fname[in]:file path
* @return the demuxer keywords.
*/
EXTERNC const char* smf_recognize_demuxer(const char* fname);
EXTERNC const char* smf_recognize_demuxer2(const char* fname, void* iopara);


/* print error
 * @param err[in]:error 
 */
EXTERNC void smf_error_print(smf_error_t* err);

#endif

