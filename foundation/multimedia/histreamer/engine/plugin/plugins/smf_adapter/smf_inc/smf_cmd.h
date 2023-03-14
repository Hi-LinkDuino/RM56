#pragma once
#include "smf_common.h"

enum smf_cmd_path_e {
	SMF_CORE_Local = 0,
	SMF_CORE_BTH,
	SMF_CORE_BTC,
	SMF_CORE_DSP,
	SMF_CORE_M55,	
};

/* send data
 * @param data[in]:data pointer
 * @param size[in]:data size
 * @return true/false
 */
EXTERNC bool smf_send(void* data, int size, enum smf_cmd_path_e target);


/* register cmd
 */
EXTERNC void smf_cmd_register();
EXTERNC void smf_rpc_register();

//
typedef struct {
	uint16_t id;//smf_cmd_id_e
	uint8_t target;//smf_cmd_path_e
	uint8_t size;
	void* data;
}smf_cmd_t;

/* send cmd
 * @param cmd[in]:cmd struct
 * @return true/false
 */
EXTERNC bool smf_cmd(smf_cmd_t* cmd);

//
enum smf_cmd_id_e {
	SMF_CMD_DestroyObject = 0x0010,
	SMF_CMD_CreateObject,
	SMF_CMD_CreateElement,
};
//
typedef struct {
	uint64_t type;
	uint64_t keys;
	uint64_t subkeys0;
	uint64_t subkeys1;
	//open params
	void* openParam;
	uint32_t openParamSize;
	//response
	void* object;
	uint32_t* sts;
	//cmd
	uint32_t* cmd_id;
	void* cmd_data;
	//flags	
	uint32_t* flags;
	//element
	smf_elememt_data_t* idata;
	smf_elememt_data_t* odata;
	smf_fifo_t* ififo;
	smf_fifo_t* ofifo;
	//other data
	void* data;	
}smf_cmd_create_object_t;
