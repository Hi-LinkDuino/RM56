/***************************************************************************
 *
 * Copyright 2015-2020 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#ifndef __OBEX_I_H__
#define __OBEX_I_H__

#ifdef BT_OBEX_SUPPORT

#include "bt_common.h"
#include "bt_co_list.h"
#include "sdp.h"
#include "rfcomm_i.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OBEX_TARGET_PROFILE_UUID_SIZE (16)

#define OBEX_AUTH_NONCE_LEN (16)

typedef enum {
    OBEX_OPCODE_NULL = 0x00,
    OBEX_OPCODE_CONNECT = 0x80,
    OBEX_OPCODE_DISCONNECT = 0x81,
    OBEX_OPCODE_PUT = 0x02,
    OBEX_OPCODE_PUT_FINAL = 0x82,
    OBEX_OPCODE_GET = 0x03,
    OBEX_OPCODE_GET_FINAL = 0x83,
    OBEX_OPCODE_SETPATH = 0x85,
    OBEX_OPCODE_ACTION = 0x86,
    OBEX_OPCODE_SESSION = 0x87,
    OBEX_OPCODE_ABORT = 0xff,
    // 0x04 0x84 0x08~0x0f are reserved
    // 0x10 to 0x1f are user definable
    // opcode bits 5 and 6 are reserved and should be set to zero
    // bit 7 of the opcode means Final packet or not
} obex_opcode_t;

typedef enum {
    OBEX_STATE_CLOSED,
    OBEX_STATE_STANDBY = 1, // ready
    OBEX_STATE_CHANNEL_CONNECTING,
    OBEX_STATE_CHANNEL_OPEN,
    OBEX_STATE_CONNECTED,
    OBEX_STATE_DISCONNECTING,
} obex_state_t;

typedef enum {
    OBEX_EVENT_CHANNEL_OPEN = 1,
    OBEX_EVENT_CONNECTED,
    OBEX_EVENT_DISCONNECTED,
    OBEX_EVENT_TX_DONE,
    OBEX_EVENT_GET_RSP,
    OBEX_EVENT_PUT_RSP,
    OBEX_EVENT_SETPATH_RSP,
    OBEX_EVENT_ABORT_RSP,
} obex_event_enum_t;

typedef enum {
    OBEX_RSP_CONTINUE = 0x90,                      // 100 http status code
    OBEX_RSP_SUCCESS = 0xa0,                       // 200
    OBEX_RSP_CREATED = 0xa1,                       // 201
    OBEX_RSP_ACCEPTED = 0xa2,                      // 202
    OBEX_RSP_NOT_AUTHORITATIVE = 0xa3,             // 203
    OBEX_RSP_NO_CONTENT = 0xa4,                    // 204
    OBEX_RSP_RESET_CONTENT = 0xa5,                 // 205
    OBEX_RSP_PARTIAL_CONTENT = 0xa6,               // 206
    OBEX_RSP_MULT_CHOICE = 0xb0,                   // 300
    OBEX_RSP_MOVED_PERMANENTLY = 0xb1,             // 301
    OBEX_RSP_MOVED_TEMPORARILY = 0xb2,             // 302
    OBEX_RSP_SEE_OTHER = 0xb3,                     // 303
    OBEX_RSP_NOT_MODIFIED = 0xb4,                  // 304
    OBEX_RSP_USE_PROXY = 0xb5,                     // 305
    OBEX_RSP_BAD_REQUEST = 0xc0,                   // 400
    OBEX_RSP_UNAUTHORIZED = 0xc1,                  // 401
    OBEX_RSP_PAYMENT_REQUIRED = 0xc2,              // 402
    OBEX_RSP_FORBIDDEN = 0xc3,                     // 403
    OBEX_RSP_NOT_FOUND = 0xc4,                     // 404
    OBEX_RSP_BAD_METHOD = 0xc5,                    // 405
    OBEX_RSP_NOT_ACCEPTABLE = 0xc6,                // 406
    OBEX_RSP_PROXY_AUTH_REQUIRED = 0xc7,           // 407
    OBEX_RSP_REQUEST_TIMEOUT = 0xc8,               // 408
    OBEX_RSP_CONFLICT = 0xc9,                      // 409
    OBEX_RSP_GONE = 0xca,                          // 410
    OBEX_RSP_LENGTH_REQUIRED = 0xcb,               // 411
    OBEX_RSP_PRECONDITION_FAILED = 0xcc,           // 412
    OBEX_RSP_REQUESTED_ENTIRY_TOO_LARGE = 0xcd,    // 413
    OBEX_RSP_REQUESTED_URL_TOO_LARGE = 0xce,       // 414
    OBEX_RSP_UNSUPPORTED_MEDIA_TYPE = 0xcf,        // 415
    OBEX_RSP_INTERNAL_ERROR = 0xd0,                // 500
    OBEX_RSP_NOT_IMPLEMENTED = 0xd1,               // 501
    OBEX_RSP_BAD_GATEWAY = 0xd2,                   // 502
    OBEX_RSP_SERVICE_UNAVAILABLE = 0xd3,           // 503
    OBEX_RSP_GATEWAY_TIMEOUT = 0xd4,               // 504
    OBEX_RSP_HTTP_VERSION_NOT_SUPPORT = 0xd5,      // 505
    OBEX_RSP_DATABASE_FULL = 0xe0,                 // ---
    OBEX_RSP_DATABASE_LOCKED = 0xe1,               // ---
} obex_rsp_code_t;

struct obex_target_profile_uuid
{
    uint8 uuid[OBEX_TARGET_PROFILE_UUID_SIZE];
};

struct obex_callback_parms_t
{
    obex_event_enum_t event;
    uint8_t error_code;
    const uint8_t *data;
    uint32_t data_len;
    uint8 *app_parameters;
    uint16 app_parameters_length;
    uint8 *body_data;
    uint16 body_data_length;
    bool is_final_packet;
};

struct obex_conn_t {
    struct bdaddr_t remote;
    bool initiator;
    bool inuse;
    bool is_over_l2cap;
    bool remote_srm_is_enabled;
    bool srm_is_in_wait_state;
    bool srm_header_is_sent;
    uint16 remote_max_obex_rx_packet_length;
    uint32 channel_handle;
    obex_opcode_t curr_opcode;
    obex_state_t obex_state;
    uint8 target_rfcomm_channel;
    uint16 target_profile_psm;
    bool has_target_uuid;
    bool has_connection_id;
    struct obex_target_profile_uuid target_profile_uuid;
    uint32 connection_id;
    int (*obex_callback)(uint8_t device_id, struct obex_conn_t *obex_conn, const struct obex_callback_parms_t *info);
    void *priv;
    uint16 curr_header_field_len;
    bool auth_connect_req_en;
    bool auth_sent_user_id;
    uint8 auth_remote_challenge_nonce[OBEX_AUTH_NONCE_LEN];
    uint8 auth_local_challenge_nonce[OBEX_AUTH_NONCE_LEN];
};

int obex_stack_init(void);

obex_state_t obex_get_state(struct obex_conn_t *obex_conn);

bool obex_channel_is_connected(struct obex_conn_t *obex_conn);

bool obex_is_srm_in_effect(struct obex_conn_t *obex_conn);

bool obex_register_profile(uint16 target_profile_psm, int8 target_rfcomm_channel, l2cap_psm_target_profile_t target_profile,
        int (*target_profile_callback)(uint8_t, struct obex_conn_t *, const struct obex_callback_parms_t *));

struct obex_conn_t *obex_create_channel(uint8_t device_id, struct bdaddr_t* remote,
        uint16 target_profile_psm, uint8 target_rfcomm_channel, l2cap_psm_target_profile_t target_profile,
        int (*target_profile_callback)(uint8_t, struct obex_conn_t *, const struct obex_callback_parms_t *), void *priv);

void obex_close_channel(uint8_t device_id, struct obex_conn_t *obex_conn);

bool obex_send_connect_req(uint8_t device_id, struct obex_conn_t *obex_conn,
                uint8 *application_parameters, uint16 application_parameters_length);

bool obex_send_disconnect_req(uint8_t device_id, struct obex_conn_t *obex_conn);

bool obex_send_get_req(uint8_t device_id, struct obex_conn_t *obex_conn,
                const char *object_name, const char *object_type,
                uint8 *application_parameters, uint16 application_parameters_length);

bool obex_setpath_to_root_folder_req(uint8_t device_id, struct obex_conn_t *obex_conn);

bool obex_setpath_to_parent_folder_req(uint8_t device_id, struct obex_conn_t *obex_conn);

bool obex_setpath_to_child_folder_req(uint8_t device_id, struct obex_conn_t *obex_conn, const char* name);

bool obex_send_abort_req(uint8_t device_id, struct obex_conn_t *obex_conn);


/* IrDA Object Exchange Protocol IrOBEX 2.1 OBEX Headers */
/*
For Unicode text, the length field (immediately following the header ID) includes the 2 bytes of the null
terminator (0x00, 0x00). Therefore the length of the string ”Jumar” would be 12 bytes; 5 visible
characters plus the null terminator, each two bytes in length.
*/

/* It is not a real UNICODE. It is 2 bytes presentation of ASCII. */

uint32 obex_ascii_to_unicode(uint8 *ascii, uint32 ascii_len_without_null, uint8 *unicode, uint32 unicode_len_with_null);

#define OBEX_AUTHENTICATION	0
#define AUTH_NONCE_LEN			16

#define OBEX_HOST16_TO_BE(value,ptr) \
    *(((uint8*)ptr)+0) = ((value)>>8)&0xFF; \
    *(((uint8*)ptr)+1) = ((value))&0xFF

#define OBEX_HOST16_TO_LE(value,ptr) \
    *(((uint8*)ptr)+0) = ((value))&0xFF; \
    *(((uint8*)ptr)+1) = ((value)>>8)&0xFF

#define OBEX_BE_TO_HOST16(ptr) \
    (*(((uint8*)ptr)+0)<<8)|(*(((uint8*)ptr)+1))

#define OBEX_HOST32_TO_BE(value,ptr) \
    *(((uint8*)ptr)+0) = ((value)>>24)&0xFF; \
    *(((uint8*)ptr)+1) = ((value)>>16)&0xFF; \
    *(((uint8*)ptr)+2) = ((value)>>8)&0xFF; \
    *(((uint8*)ptr)+3) = ((value))&0xFF \

#define OBEX_HOST32_TO_LE(value,ptr) \
    *(((uint8*)ptr)+0) = ((value))&0xFF; \
    *(((uint8*)ptr)+1) = ((value)>>8)&0xFF; \
    *(((uint8*)ptr)+2) = ((value)>>16)&0xFF; \
    *(((uint8*)ptr)+3) = ((value)>>24)&0xFF \

#define OBEX_BE_TO_HOST32(ptr) \
    (*(((uint8*)ptr)+0)<<24)|(*(((uint8*)ptr)+1)<<16)|(*(((uint8*)ptr)+2)<<8)|(*(((uint8*)ptr)+3))

//----- Max 128 ASCII for 'Name' ... Unicode String ------//
#define OBEX_UNICODE_STRING_SIZE_MAX (256)

//------ GOEP_SPEC_V1.1 5.2 OBEX HEADERS ------//
typedef unsigned char obex_header_type_t;
#define obex_header_type_UnicodeText          0x00
#define obex_header_type_ByteSequence         0x40
#define obex_header_type_1ByteQuality         0x80
#define obex_header_type_4ByteQuality         0xC0

typedef unsigned char obex_header_id_t;

#define obex_header_id_Count                  0xC0
#define obex_header_id_Name                   0x01
#define obex_header_id_Type                   0x42
#define obex_header_id_Length                 0xC3
#define obex_header_id_Time_ISO8601           0x44
#define obex_header_id_Time_4byte             0xC4
#define obex_header_id_Description            0x05
#define obex_header_id_Target                 0x46
#define obex_header_id_HTTP                   0x47
#define obex_header_id_Body                   0x48
#define obex_header_id_EndofBody              0x49
#define obex_header_id_Who                    0x4A
#define obex_header_id_ConnectionID           0xCB
#define obex_header_id_ApplicationParameters  0x4C
#define obex_header_id_AuthenticateChallenge  0x4D
#define obex_header_id_AuthenticateResponse   0x4E
#define obex_header_id_ObjectClass            0x4F

//------ IrDA Object Exchange Protocol 3.3 OBEX Operations and Opcode definitions ------//
typedef unsigned char obex_operation_opcode_t;

#define obex_opcode_CONNECT         0x80
#define obex_opcode_DISCONNECT      0x81
#define obex_opcode_PUT             0x02
#define obex_opcode_GET             0x03
#define obex_opcode_RESERVED        0x04
#define obex_opcode_SETPATH         0x85
#define obex_opcode_ABORT           0xFF
#define obex_opcode_FINAL           0x80

//------ IrDA Object Exchange Protocol 3.2.1 Response Code values ------//
typedef unsigned char obex_operation_response_code_t;

#define obex_response_code_Continue                      0x10
#define obex_response_code_OK_Success                    0x20
#define obex_response_code_Created                       0x21
#define obex_response_code_Accepted                      0x22
#define obex_response_code_Non_Authoritative_Information 0x23
#define obex_response_code_No_Content                    0x24
#define obex_response_code_Reset_Content                 0x25
#define obex_response_code_Partial_Content               0x26
#define obex_response_code_Multiple_Choices              0x30
#define obex_response_code_Moved_Permanently             0x31
#define obex_response_code_Moved_temporarily             0x32
#define obex_response_code_See_Other                     0x33
#define obex_response_code_Not_modified                  0x34
#define obex_response_code_Use_Proxy                     0x35
#define obex_response_code_Bad_Request                   0x40
#define obex_response_code_Unauthorized                  0x41
#define obex_response_code_Payment_required              0x42
#define obex_response_code_Forbidden                     0x43
#define obex_response_code_Not_Found                     0x44
#define obex_response_code_Method_not_allowed            0x45
#define obex_response_code_Not_Acceptable                0x46
#define obex_response_code_Proxy_Authentication_required 0x47
#define obex_response_code_Request_Time_Out              0x48
#define obex_response_code_Conflict                      0x49
#define obex_response_code_Gone                          0x4A
#define obex_response_code_Length_Required               0x4B
#define obex_response_code_Precondition_failed           0x4C
#define obex_response_code_Requeste_dentity_too_large    0x4D
#define obex_response_code_Request_URL_too_large         0x4E
#define obex_response_code_Unsupported_media_type        0x4F
#define obex_response_code_Internal_Server_Error         0x50
#define obex_response_code_Not_Implemented               0x51
#define obex_response_code_Bad_Gateway                   0x52
#define obex_response_code_Service_Unavailable           0x53
#define obex_response_code_Gateway_Timeout               0x54
#define obex_response_code_HTTP_version_not_supported    0x55
#define obex_response_code_Database_Full                 0x60
#define obex_response_code_Database_Locked               0x61

enum obex_event_t{
	OBEX_EVENT_TP_CONNECTED,
	OBEX_EVENT_TP_DISCONNECTED = 1,
	OBEX_EVENT_GET = 4,
	OBEX_EVENT_SET_PATH,
	OBEX_EVENT_ABORT,
	OBEX_EVENT_SUCCESS,
	OBEX_EVENT_FAILED,
	OBEX_EVENT_TIMEOUT
};

#define OBEX_FINAL_BIT				0x80

typedef uint8 obex_respcode;

#define OBEX_RESP_CONTINUE				0x10 /* Continue */
#define OBEX_RESP_SUCCESS					0x20 /* OK, Success */

#define OBEX_RESP_CREATED					0x21 /* Created */
#define OBEX_RESP_ACCEPTED				0x22 /* Accepted */
#define OBEX_RESP_NON_AUTHOR_INFO			0x23 /* Non-Authoritative Information */
#define OBEX_RESP_NO_CONTENT				0x24 /* No Content */
#define OBEX_RESP_RESET_CONTENT			0x25 /* Reset Content */
#define OBEX_RESP_PARTIAL_CONTENT			0x26 /* Partial Content */

#define OBEX_RESP_MULTIPLE_CHOICES		0x30 /* Multiple Choices */
#define OBEX_RESP_MOVED_PERMANENT			0x31 /* Moved Permanently */
#define OBEX_RESP_MOVED_TEMPORARY			0x32 /* Moved Temporarily */
#define OBEX_RESP_SEE_OTHER				0x33 /* See Other */
#define OBEX_RESP_NOT_MODIFIED			0x34 /* Not Modified */
#define OBEX_RESP_USE_PROXY				0x35 /* Use Proxy */

#define OBEX_RESP_BAD_REQUEST				0x40 /* Bad Request */
#define OBEX_RESP_UNAUTHORIZED			0x41 /* Unauthorized */
#define OBEX_RESP_PAYMENT_REQUIRED		0x42 /* Payment Required */
#define OBEX_RESP_FORBIDDEN				0x43 /* Forbidden - operation is understood but refused */
#define OBEX_RESP_NOT_FOUND				0x44 /* Not Found */
#define OBEX_RESP_METHOD_NOT_ALLOWED		0x45 /* Method Not Allowed */
#define OBEX_RESP_NOT_ACCEPTABLE			0x46 /* Not Acceptable */
#define OBEX_RESP_PROXY_AUTHEN_REQ		0x47 /* Proxy Authentication Required */
#define OBEX_RESP_REQUEST_TIME_OUT		0x48 /* Request Timed Out */
#define OBEX_RESP_CONFLICT				0x49 /* Conflict */

#define OBEX_RESP_GONE						0x4a /* Gone */
#define OBEX_RESP_LENGTH_REQUIRED			0x4b /* Length Required */
#define OBEX_RESP_PRECONDITION_FAILED	0x4c /* Precondition Failed */
#define OBEX_RESP_REQ_ENTITY_TOO_LARGE	0x4d /* Requested entity is too large */
#define OBEX_RESP_REQ_URL_TOO_LARGE		0x4e /* Requested URL is too large */
#define OBEX_RESP_UNSUPPORT_MEDIA_TYPE	0x4f /* Unsupported Media Type */

#define OBEX_RESP_INTERNAL_SERVER_ERR	0x50 /* Internal Server Error */
#define OBEX_RESP_NOT_IMPLEMENTED			0x51 /* Not Implemented */
#define OBEX_RESP_BAD_GATEWAY				0x52 /* Bad Gateway */
#define OBEX_RESP_SERVICE_UNAVAILABLE	0x53 /* Service Unavailable */
#define OBEX_RESP_GATEWAY_TIMEOUT			0x54 /* Gateway Timeout */
#define OBEX_RESP_HTTP_VER_NO_SUPPORT	0x55 /* HTTP version not supported */

#define OBEX_RESP_DATABASE_FULL			0x60 /* Database Full */
#define OBEX_RESP_DATABASE_LOCKED			0x61 /* Database Locked */

typedef uint8 obex_opcode;

enum obex_conn_state_enum{
	OBEX_TP_CONNECTING,
	OBEX_TP_CONNECTED,
	OBEX_TP_DISCONNECTING,
	OBEX_TP_DISCONNECTED
};

enum obex_state_enum{
	OBEX_IDLE,
	OBEX_STANDBY,
	OBEX_CONNECTING,
	OBEX_CONNECTED,
	OBEX_DISCONNECTING,
	//OBEX_DISCONNECTED,
	OBEX_GET,
	OBEX_ABORT,
	OBEX_SET_PATH
};

#define OBEX_FLAG_CONTINUE	0x01
#define OBEX_FLAG_CHALLENGE	0x02

struct obex_client{
	uint8 flags;
	obex_opcode opcode;
	
	void (*indicate_cb)(enum obex_event_t event);
	void (*data_cb)(uint8 *data, uint16 len);

	uint8 *tx_buffer;
	uint16 tx_length;
	
	uint8 rfcomm_handle;
};

struct obex_control_t{
	uint8_t timer_handle;
	struct bdaddr_t remote;
	//uint8 rfcomm_handle;

	enum obex_conn_state_enum conn_state;
	enum obex_state_enum state;

	/*uint16 sendlen;
	char sendData[OBEX_SENDDATA_BUF_SIZE];*/

	void (*indicate_cb)(enum obex_event_t event);
	void (*data_cb)(struct pp_buff *ppb);

	struct obex_client *pbap_client;
};

int8 obex_turnOn(struct obex_client *client);
int8 obex_turnOff(void);
int8 obex_parse_tx(struct obex_client *client);

typedef enum {
    OBEX_TRANSPORT_TYPE_RFCOMM = 0,
    OBEX_TRANSPORT_TYPE_L2CAP, // 1
} obex_transport_type_t;

typedef enum {
    OBEX_TRANSPORT_ROLE_CLIENT = 0,
    OBEX_TRANSPORT_ROLE_SERVER, // 1
} obex_transport_role_t;

typedef enum {
    OBEX_TRANSPORT_CONN_STATE_CLOSE = 0,
    OBEX_TRANSPORT_CONN_STATE_CONNECTING, // 1
    OBEX_TRANSPORT_CONN_STATE_CONNECTED, // 2
    OBEX_TRANSPORT_CONN_STATE_DISCONNECTING, // 3
} obex_transport_conn_state_t;

typedef enum {
    OBEX_TRANSPORT_LISTEN_STATE_IDLE = 0,
    OBEX_TRANSPORT_LISTEN_STATE_LISTENING, // 1
} obex_transport_listen_state_t;

typedef struct {
    obex_transport_type_t type;
    union {
        struct {
            uint32 remote_channel;
            uint32 local_channel;
            uint32 rfcomm_handle;
        } rfcomm;
        struct {
            uint32 psm;
            uint32 channel;
        } l2cap;
    } u;
    void *priv;
} obex_transport_config_t;

typedef enum {
    OBEX_TRANSPORT_EVENT_OPEN_IND = 0,
    OBEX_TRANSPORT_EVENT_OPEN, // 1
    OBEX_TRANSPORT_EVENT_TX_HANDLED, // 2
    OBEX_TRANSPORT_EVENT_CLOSE,// 3
    OBEX_TRANSPORT_EVENT_DATA, // 4
} obex_transport_event_t;

typedef struct {
    union {
        struct {
        } open_ind;
        struct {
        } open;
        struct {
        } tx_handled;
        struct {
        } close;
        struct {
            uint8 *buff;
            uint32 len;
        } data;
    } u;
} obex_transport_cb_param_t;

typedef uint8 obex_header;

#define OBEX_HEADER_TIME_ISO		0x44 /* (ByteSeq, ISO 8601 format) Creation or modification time for object (preferred format). */
#define OBEX_HEADER_TIME_COMPAT	0xC4 /* (4-byte) Creation or modification time for object for backward-compatibility. */
#define OBEX_HEADER_END_BODY		0x49 /* Not for use by OBEX applications */
#define OBEX_HEADER_CONNID		0xCB /* (4-byte) Identifies the connection for which the operation is directed */
#define OBEX_HEADER_APP_PARAMS	0x4C /* (ByteSeq) Application parameters */
#define OBEX_HEADER_AUTH_CHAL		0x4D /* (ByteSeq) Authentication challenge */
#define OBEX_HEADER_AUTH_RESP		0x4E /* (ByteSeq) Authentication response */

#define MAX_OBEX_TX_BUF_LEN	128		//need to be confirm
#define MAX_OBEX_PACKET_SIZE	0x0200	//need to be confirm, 512

#define OBEX_CFG_SERVER_CHANNEL	0x14	//need to be confirm

int8 obex_init(void (*indicate_callback)(enum obex_event_t event));
int8 obex_close(void);
void obex_rfcomm_notifyCallback(enum rfcomm_event_enum event, uint32 rfcomm_handle, void *pData, uint8 reason, void *priv);
void obex_rfcomm_dataRecvCallback(uint32 rfcomm_handle, struct pp_buff *ppb, void *priv);
//int8 obex_sendData(struct obex_client *client);

struct _obex_transport_t;

typedef int32 (*obex_transport_connect_t) (struct _obex_transport_t *transport, struct bdaddr_t *remote);
typedef int32 (*obex_transport_listen_t) (struct _obex_transport_t *transport);
typedef int32 (*obex_transport_disconnect_t) (struct _obex_transport_t *transport);
typedef uint32 (*obex_transport_send_t)(struct _obex_transport_t *transport, uint8 *buff, uint32 buff_len);
typedef int32 (*obex_transport_event_handler_t)(struct _obex_transport_t *transport, obex_transport_event_t event, obex_transport_cb_param_t *param);

typedef struct _obex_transport_t{
    struct list_node node;
    obex_transport_conn_state_t conn_state;
    obex_transport_listen_state_t listen_state;
    obex_transport_config_t config;

    obex_transport_send_t send;
    obex_transport_listen_t listen;
    obex_transport_connect_t connect;
    obex_transport_disconnect_t disconnect;
    obex_transport_event_handler_t event_handler;
} obex_transport_t;

typedef enum {
    OBEX_SESSION_EVT_OPEN_IND = 0,
    OBEX_SESSION_EVT_OPEN, // 1
    OBEX_SESSION_EVT_REQUEST, // 2
    OBEX_SESSION_EVT_RESPONSE, // 3
    OBEX_SESSION_EVT_CLOSE, // 4
} obex_session_event_t;

typedef struct {
    uint8 opcode;
    uint8 *packet;
    uint16 packet_len;
    union {
        struct {
            struct {
                uint8 obex_version_num;
                uint8 flags;
                uint16 obex_max_packet_len;
            } connect;
            struct {
                uint8 flags;
                uint16 constants;
            } setpath;
        } request;
        struct {
            struct {
                uint8 obex_version_num;
                uint8 flags;
                uint16 obex_max_packet_len;
            } connect;
        } response;
    } p;
} obex_session_cb_param_t;

struct _obex_session_t;

typedef int32 (*obex_session_event_handler_t)(struct _obex_session_t *session, obex_session_event_t event, obex_session_cb_param_t *param);

typedef enum {
    OBEX_SESSION_ROLE_CLIENT = 0,
    OBEX_SESSION_ROLE_SERVER, // 1
} obex_session_role_t;

typedef enum {
    OBEX_SESSION_STATE_CLOSE = 0,
    OBEX_SESSION_STATE_CONNECTING, // 1
    OBEX_SESSION_STATE_OPEN, // 2
    OBEX_SESSION_STATE_DISCONNECTING, // 3
} obex_session_state_t;

typedef struct {
    uint8 *buff;
    uint16 packet_len;
    uint16 max_buff_len;
    bool is_final;
    uint8 flag;
} obex_transmission_t;

typedef struct {
    obex_session_role_t role;
    uint32 remote_rfcomm_channel;
    uint32 remote_l2cap_psm;
    uint8 local_version;
    uint16 max_obex_packet_length;
    uint8 local_rfcomm_channel;
    uint8 local_l2cap_psm;
    obex_transport_type_t transport_type;
    void *priv;
} obex_session_config_t;

typedef struct _obex_session_t{
    obex_session_config_t config;
    obex_session_state_t state;
    obex_transport_t transport;
    obex_transmission_t *transmission;
    struct bdaddr_t remote;
    uint32 connectionID;
    obex_operation_opcode_t op;
    obex_session_event_handler_t event_handler;
} obex_session_t;

int32 obex_session_init(obex_session_t *session, obex_session_config_t *config, obex_session_event_handler_t event_handler);
int32 obex_session_open(obex_session_t *session, struct bdaddr_t *remote);
int32 obex_session_connect(obex_session_t *session, obex_transmission_t *transmission);
int32 obex_session_disconnect(obex_session_t *session, obex_transmission_t *transmission);
int32 obex_session_put(obex_session_t *session, obex_transmission_t *transmission);
int32 obex_session_get(obex_session_t *session, obex_transmission_t *transmission);
int32 obex_session_setpath(obex_session_t *session, obex_transmission_t *transmission);
int32 obex_session_abort(obex_session_t *session, obex_transmission_t *transmission);
int32 obex_session_response(obex_session_t *session, obex_transmission_t *transmission);
int32 obex_session_close(obex_session_t *session);
int32 obex_session_restore(obex_session_t *session,uint32 connectionID,uint32 server_chnl);

const char *obex_transport_conn_state_2_str(obex_transport_conn_state_t state);
const char *obex_transport_event_2_str(obex_transport_event_t event);
int32 obex_transport_init(obex_transport_t *transport, obex_transport_config_t *config, obex_transport_event_handler_t event_handler);
int32 obex_transport_restore(obex_transport_t *transport,uint32 server_chnl);
int obex_rfcomm_notify_cb(uint8 device_id, enum rfcomm_event_enum event,uint32 rfcomm_handle,void *data, uint8 reason, void *priv);
void obex_rfcomm_data_recv_cb(uint8 device_id, uint32 rfcomm_handle,struct pp_buff *ppb, void *priv);


/*------ What Is A Transmission ------*/
/* An user (application) fires a operation (CONNECT/DISCONNECT/PUT/GET/SETPATH/ABORT) to server to do something */
/* The operation is in fact a packet of data which is called Transmission here */
/* A Transmission is composed of fixed 'Field' plus some optional 'HEADER' */
/* For the fixed 'Field', here using obex_transmisson_prepare to reserve the buffer in Transmission */
/* For the optional 'Header', here using obex_transmisson_add_XXX to add it in Transmission */

typedef struct {
    uint16 len;
    uint8 *text;
    uint8 valid;
} obex_transmission_header_Unicode_t;

typedef struct {
    uint16 len;
    uint8 *buff;
    uint8 valid;
} obex_transmission_header_ByteSequence_t;

typedef struct {
    uint8 value;
    uint8 valid;
} obex_transmission_header_1ByteQuality;

typedef struct {
    uint32 value;
    uint8 valid;
} obex_transmission_header_4ByteQuality;

typedef struct {
    obex_transmission_header_4ByteQuality Count;
    obex_transmission_header_Unicode_t Name;
    obex_transmission_header_ByteSequence_t Type;
    obex_transmission_header_4ByteQuality Length;
    obex_transmission_header_ByteSequence_t Time_ISO8601;
    obex_transmission_header_4ByteQuality Time_4Bytes;
    obex_transmission_header_Unicode_t Description;
    obex_transmission_header_ByteSequence_t Target;
    obex_transmission_header_ByteSequence_t HTTP;
    obex_transmission_header_ByteSequence_t Body;
    obex_transmission_header_ByteSequence_t EndOfBody;
    obex_transmission_header_ByteSequence_t Who;
    obex_transmission_header_4ByteQuality ConnectionID;
    obex_transmission_header_ByteSequence_t AppParameters;
    obex_transmission_header_ByteSequence_t AuthChallenge;
    obex_transmission_header_ByteSequence_t AuthResponse;
    obex_transmission_header_ByteSequence_t ObjectClass;
} obex_transmission_headers_t;

//------ Initialize a transmission in a give buffer ------//
int32 obex_transmission_init(obex_transmission_t *trasmission, uint8 *buff, uint32 len);
int32 obex_transmission_prepare(obex_transmission_t *trasmission, obex_operation_opcode_t operation_code);

int32 obex_transmission_parse_headers(uint8 *buff, uint32 len, obex_transmission_headers_t *headers);

bool obex_is_final_transmission(obex_transmission_t *transmission);
void obex_transmission_set_final(obex_transmission_t *transmission, bool final);
uint8 obex_transmission_get_flag(obex_transmission_t *transmission);
void obex_transmission_set_flag(obex_transmission_t *transmission, uint8 flag);

#define obex_transmission_add_Count(tr,Count) \
    obex_transmission_add_4ByteQuantity(tr,obex_header_id_Count,Count)

#define obex_transmission_add_Name(tr,name,name_len_without_null) \
    obex_transmission_add_Unicode(tr,obex_header_id_Name,name,name_len_without_null)

#define obex_transmission_add_Type(tr,type,type_len_with_null) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_Type,type,type_len_with_null)

#define obex_transmission_add_Length(tr,Length) \
    obex_transmission_add_4ByteQuantity(tr,obex_header_id_Length,Length)

#define obex_transmission_add_Time_ISO8601t(tr,time,time_len) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_Time_ISO8601,time,time_len)

#define obex_transmission_add_Time_4Byte(tr,Time) \
    obex_transmission_add_4ByteQuantity(tr,obex_header_id_Time_4byte,Time)

#define obex_transmission_add_Description(tr,desc,desc_len_without_null) \
    obex_transmission_add_Unicode(tr,obex_header_id_Description,desc,desc_len_without_null)

#define obex_transmission_add_Target(tr,uuid,uuid_len) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_Target,uuid,uuid_len)

#define obex_transmission_add_HTTP(tr,http,http_len) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_HTTP,http,http_len)

#define obex_transmission_add_Body(tr,body,body_len) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_Body,body,body_len)

#define obex_transmission_add_EndOfBody(tr,body,body_len) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_EndofBody,body,body_len)

#define obex_transmission_add_Who(tr,who,who_len) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_Who,who,who_len)

#define obex_transmission_add_ConnectionID(tr,ConnectionID) \
    obex_transmission_add_4ByteQuantity(tr,obex_header_id_ConnectionID,ConnectionID)

#define obex_transmission_add_ApplicationParameters(tr,tlv,tlv_len) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_ApplicationParameters,tlv,tlv_len)

#define obex_transmission_add_AuthenticateChallenge(tr,ac,ac_len) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_AuthenticateChallenge,ac,ac_len)

#define obex_transmission_add_AuthenticateResponse(tr,as,as_len) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_AuthenticateResponse,as,as_len)

#define obex_transmission_add_ObjectClass(tr,oc,oc_len) \
    obex_transmission_add_ByteSequence(tr,obex_header_id_ObjectClass,oc,oc_len)

//------ null terminated Unicode text, length prefixed with 2 byte unsigned integer ------//
int32 obex_transmission_add_Unicode(obex_transmission_t *tr, obex_header_id_t id, uint8 *ascii, uint32 ascii_len_without_null);
//------ byte sequence, length prefixed with 2 byte unsigned integer ------//
int32 obex_transmission_add_ByteSequence(obex_transmission_t *tr, obex_header_id_t id, uint8 *bytes, uint32 bytes_len);
//------ 1 byte quantity ------//
int32 obex_transmission_add_1ByteQuantity(obex_transmission_t *tr, obex_header_id_t id, uint8 value);
//------ 4 byte quantity ------//
int32 obex_transmission_add_4ByteQuantity(obex_transmission_t *tr, obex_header_id_t id, uint32 value);

typedef unsigned char obex_tlv_id_t;
typedef unsigned char obex_tlv_length_t;

typedef struct { /* tag-length-value encoding */
    uint8 *buff;
    uint32 buff_max_len;
    uint32 packet_len;
} obex_tlv_t;

int32 obex_tlv_init(obex_tlv_t *tlv, uint8 *buff, uint32 buff_len);
int32 obex_tlv_add_1Byte(obex_tlv_t *tlv, obex_tlv_id_t id, uint8 value);
int32 obex_tlv_add_2Bytes(obex_tlv_t *tlv, obex_tlv_id_t id, uint16 value, bool is_le);
int32 obex_tlv_add_4Bytes(obex_tlv_t *tlv, obex_tlv_id_t id, uint32 value, bool is_le);
int32 obex_tlv_add_buffer(obex_tlv_t *tlv, obex_tlv_id_t id, obex_tlv_length_t lenght, uint8 *value);
uint32 obex_tlv_get_length(obex_tlv_t *tlv);

#ifdef __cplusplus
}
#endif

#endif /* BT_OBEX_SUPPORT */

#endif /* __OBEX_I_H__ */

