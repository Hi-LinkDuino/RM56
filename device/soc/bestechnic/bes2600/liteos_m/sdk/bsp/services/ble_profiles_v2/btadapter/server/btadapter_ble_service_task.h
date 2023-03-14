#ifndef __BTADAPTER_SERVICE_TASK_H__
#define __BTADAPTER_SERVICE_TASK_H__
#include <stdint.h>
#include "rwip_task.h" // Task definitions

/// Messages for btadapter Server Profile 
enum btadapter_service_msg_id
{
    BTADAPTER_SERVICE_TX_CCC_CHANGED = TASK_FIRST_MSG(TASK_ID_BLE_ADAPTER_SERVER),
};

struct btadapter_send_notify_req_t
{
	uint16_t 	handle;
	uint32_t 	length;
	uint8_t  	value[__ARRAY_EMPTY];
};

struct btadapter_notif_config_t
{
    struct btadapter_service_env_tag *service_env;
    bool isNotificationEnabled;
};

#endif
