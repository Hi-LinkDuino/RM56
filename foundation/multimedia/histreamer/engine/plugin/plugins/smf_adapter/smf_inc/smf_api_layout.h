#pragma once
#include <smf_common.h>
#ifndef EXTERNC
#ifndef __cplusplus
#define EXTERNC
#else
#define EXTERNC extern "C"
#endif
#endif
typedef struct {
	uint8_t index;
	uint8_t parent;
	uint8_t links[5];
	uint8_t scheduled;
	struct {
		const char* type;
		const char* keys;
		const char* subkeys0;
		const char* subkeys1;
	};
	struct proprety_t {
		const char* keys;
		uint32_t vals;
	}props[16];
}smf_layout_t;

/** create a pipeline from layout script.
 * @param script[in] pipeline script list, end for index=0.
 * @param parent[in] set the parent for pipeline.
 * @return return the pipeline handle.
 */
EXTERNC void* smf_create_pipeline(smf_layout_t* script, void* parent);

/** register pipeline
 */
EXTERNC void smf_pipeline_register();
