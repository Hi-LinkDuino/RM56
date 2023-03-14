/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup Bluetooth
 * @{
 *
 * @brief This file is a part of BTStack.
 *
 * @since 6
 */

/**
 * @file rfcomm.h
 *
 * @brief RFCOMM protocal interface.
 *
 * @since 6
 */

#ifndef RFCOMM_H
#define RFCOMM_H

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "packet.h"
#include "btstack.h"

#ifdef __cplusplus
extern "C" {
#endif

// RFCOMM event declarations
// The event is triggered when rfcomm server accept a new connection,
// @see RfcommIncomingInfo for (void* event_data)'s struct.
#define RFCOMM_CHANNEL_EV_CONNECT_INCOMING 0x00000001
// The event is triggered when connection complete successfully.
// @see RfcommConnectedInfo for (void* event_data)'s struct.
#define RFCOMM_CHANNEL_EV_CONNECT_SUCCESS 0x00000002
// The event is triggered when connection failed.
// (void* event_data) is null.
#define RFCOMM_CHANNEL_EV_CONNECT_FAIL 0x00000004
// This event is triggered when a disconnect request is received.
// (void* event_data) is null.
#define RFCOMM_CHANNEL_EV_DISCONNECTED 0x00000008
// This event is triggered when the disconnection process is successful.
// (void* event_data) is null.
#define RFCOMM_CHANNEL_EV_DISCONNECT_SUCCESS 0x00000010
// This event is triggered when the disconnection process fails.
// (void* event_data) is null.
#define RFCOMM_CHANNEL_EV_DISCONNECT_FAIL 0x00000020
// The event is triggered when peer or RFCOMM is available to receive data,
// (void* event_data) is null.
#define RFCOMM_CHANNEL_EV_FC_ON 0x00000040
// The event is triggered when data is received from L2CAP,
// (void* event_data) is null.
#define RFCOMM_CHANNEL_EV_REV_DATA 0x00000080
// The event is triggered when remote's V.24 control signals are changed.
// @see RfcommModemStatus for (void* event_data)'s struct.
#define RFCOMM_CHANNEL_EV_MODEM_STATUS 0x00000100
// The event is triggered when remote port negotiation settings are changed,
// @see RfcommRemotePortConfig for (void* event_data)'s struct.
#define RFCOMM_CHANNEL_EV_REMOTE_PORT_CONFIG 0x00000200
// The event is triggered when remote line status are changed,
// @see RfcommRemoteLineStatus for (void* event_data)'s struct.
#define RFCOMM_CHANNEL_EV_REMOTE_LINE_STATUS 0x00000400

// RFCOMM function's return value.
#define RFCOMM_SUCCESS BT_NO_ERROR                   // Function successful
#define RFCOMM_ERR_NO_RESOURCES BT_OPERATION_FAILED  // Not enough resources
#define RFCOMM_ERR_NOT_CONNECTED BT_BAD_STATUS       // Channel(DLC) is not connected
#define RFCOMM_ERR_PARAM BT_BAD_PARAM                // paramas is error
#define RFCOMM_ALREADY_EXIST BT_ALREADY              // Already exist
#define RFCOMM_NO_DATA BT_OPERATION_FAILED           // No data
#define RFCOMM_FAILED BT_OPERATION_FAILED            // Function fail
#define RFCOMM_QUEUE_FULL BT_OPERATION_FAILED        // Send queue is full
#define RFCOMM_OVERRUN BT_BAD_PARAM                  // The length of the sent data exceeds mtu

/**
 * @brief The peer's bt-address and connected server number.
 */
typedef struct {
    // The client's bt-address
    BtAddr addr;
    // The connected server number
    uint8_t scn;
} RfcommIncomingInfo;

/**
 * @brief The channel's basic information.
 */
typedef struct {
    // The maximum size of data that can be transmitted.
    uint16_t sendMTU;
    // The maximum data size that the peer can send.
    uint16_t recvMTU;
} RfcommConnectedInfo;

/**
 * @brief Modem Status.(V.24 control signals).
 */
typedef struct {
    // Length of break in units of 200ms.Range 0~15.
    uint8_t break_length;
    // A break signal.The bit is set to 1 to indicate that a break signal detected in the data stream.
    uint8_t break_signal;
    // Flow Control (FC). The bit is set to 1(one) when the device is unable to accept frames.
    uint8_t fc;
    // Ready To Communicate (RTC). The bit is set to 1 when the device is ready to communicate.
    uint8_t rtc;
    // Ready To Receive (RTR). The bit is set to 1 when the device is ready to receive data.
    uint8_t rtr;
    // Incoming call indicator (IC). The bit is set to 1 to indicate an incoming call.
    uint8_t ic;
    // Data Valid (DV). The bit is set to 1 to indicate that valid data is being sent.
    uint8_t dv;
} RfcommModemStatus;

/**
 * @brief Remote port communication settings.
 */
typedef struct {
    // The baudrate.The default value is 1100 0000 (9600 bit/s).
    uint8_t baudrate;
    // Data bits.The default value is 11 (8 bits )
    uint8_t data_bits;
    // Stop bits: S=0: 1 stop bit, S=1: 1,5 stop bits. Default value = 0 (1 stop bit)
    uint8_t stop_bit;
    // The parity. P=0: no parity, P=1: parity. Default value = 0 (no parity)
    uint8_t parity;
    // Parity type.00 odd parity,01 even parity,10 mark parity,11 space parity
    uint8_t parity_type;
    // Flow control.(Default value=0, no flow control)
    // Bit1 XON/XOFF on input,Bit2 XON/XOFF on output,Bit3 RTR on input,
    // Bit4 RTR on output,Bit5 RTC on input,Bit6 RTC on output.
    uint8_t fc;
    // XON character (default DC1)
    uint8_t xon_char;
    // XOFF character.(default DC3)
    uint8_t xoff_char;
    // The parameter mask is used to indicate which parameters in the RPN command are negotiated.
    uint8_t parameter_mask1;
    uint8_t parameter_mask2;
} RfcommRemotePortConfig;

/**
 * @brief Remote Line Status.
 */
typedef struct {
    // Overrun Error - Received character overwrote an unread character
    uint8_t overrunErr;
    // Parity Error - Received characters parity was incorrect
    uint8_t parityErr;
    // Framing Error - a character did not terminate with a stop bit
    uint8_t frameErr;
} RfcommRemoteLineStatus;

/**
 * @brief Port's information.
 */
typedef struct {
    // The amount of data received
    uint32_t receivedBytes;
    // The amount of data sent
    uint32_t transmittedBytes;
} RfcommPortState;

/**
 * @brief The callback function is used to notify the upper layer of the event and related information.
 *
 * @param handle     The channel(DLC)'s handle number
 * @param eventId    Event Id
 * @param event_data The event's data
 * @param context    The content passed in from the upper layer.
 * @since 6
 */
typedef void (*RFCOMM_EventCallback)(uint16_t handle, uint32_t eventId, const void *eventData, void *context);

/**
 * @brief Connection request information.
 */
typedef struct {
    BtAddr addr;
    uint8_t scn;
    uint16_t mtu;
    uint32_t eventMask;
    RFCOMM_EventCallback callback;
    void *context;
} RfcommConnectReqInfo;

/**
 * @brief The function is used to assign server numbers to individual servers.
 *        Server number is used to register with the RFCOMM service interface, range is 1~30.
 *        When the return value is 0, it means that there is no available server number.
 *        This Server number shall be registered in the Service Discovery Database;
 *        RFCOMM_SPEC_V12 #5.4
 *
 * @return Server number.0(unavailable number),1~30(available number)
 * @since 6
 */
uint8_t BTSTACK_API RFCOMM_AssignServerNum();

/**
 * @brief After close the server, free the server number.
 *
 * @param  scn Server number.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_FreeServerNum(uint8_t scn);

/**
 * @brief The function is used by the client to establish a connection of the channel.
 *
 * @param reqInfo   Connection request information.
 * @param handle    The handle of the channel created by rfcomm
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_ConnectChannel(const RfcommConnectReqInfo *reqInfo, uint16_t *handle);

/**
 * @brief The function is used for the server to register with RFCOMM and
 *        wait for the client to connect.
 *
 * @param scn       The server's number.
 * @param mtu       The maximum size of data received at a time.
 * @param eventMask The collection of events followed by upper layers.
 * @param callback  The callback function used by rfcomm to notify uppers of data or events.
 * @param context   The content passed in from the upper layer.
 *                  It will be brought back to the upper layer when callback is called.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_RegisterServer(uint8_t scn, uint16_t mtu, uint32_t eventMask,
                                      RFCOMM_EventCallback callback, void *context);

/**
 * @brief When the server is shut down, call this function to
 *        release the resources about the server held in RFCOMM.
 *
 * @param scn The server's number
 * @param isRemoveCallback true:remove callback,RFCOMM will not notify event to upper layer.otherwise:false.
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_DeregisterServer(uint8_t scn, bool isRemoveCallback);

/**
 * @brief The function is used to tell RFCOMM to accept the connection request when the server
 *        receives the connection notification(eventId is RFCOMM_EVENT_CONNECT_INCOMING).
 *        After receiving the response from the upper layer, RFCOMM notifies the client
 *        of the peer device to accept the connection request.
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_AcceptConnection(uint16_t handle);

/**
 * @brief This function is used to tell RFCOMM to reject the connection request when the server
 *        receives the connection notification(eventId is RFCOMM_EVENT_CONNECT_INCOMING).
 *        After receiving the response from the upper layer, RFCOMM notifies the client
 *        of the peer device to reject the connection request.
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_RejectConnection(uint16_t handle);

/**
 * @brief The function is used to disconnect the specified channel.
 *        RFCOMM_SPEC_V12 #5.2
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_DisconnectChannel(uint16_t handle);

/**
 * @brief The function is used for set the remote port communication settings.
 *        The command may be used before a new DLC is opened and shall be used
 *        whenever the port settings change.
 *        RFCOMM_SPEC_V12 #5.5.1
 *
 * @param handle The channel(DLC)'s handle number
 * @param config Remote port negotiation parameters
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_SetPortConfig(uint16_t handle, const RfcommRemotePortConfig *config);

/**
 * @brief The function is used to obtain the remote port negotiation information
 *        of the peer device.
 *
 * @param handle The channel(DLC)'s handle number
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_ReqPortConfig(uint16_t handle);

/**
 * @brief The function is used for indication of remote port line status.
 *        RFCOMM_SPEC_V12 #5.5.2
 *
 * @param handle     The channel(DLC)'s handle number
 * @param lineStatus Remote line status
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_SendRemoteLineStatus(uint16_t handle, const RfcommRemoteLineStatus *lineStatus);

/**
 * @brief The function is used to convey the RS-232 control signals and the break signal.
 *        RFCOMM_SPEC_V12 #2.2
 *
 * @param handle      The channel(DLC)'s handle number
 * @param modemStatus Control signals and the break signal
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_SetModemStatus(uint16_t handle, const RfcommModemStatus *modemStatus);

/**
 * @brief This function is used to obtain port related information.
 *        Currently, the amount of data sent and received by the port can be obtained.
 *
 * @param handle The channel(DLC)'s handle number
 * @param state The port's information
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_GetPortState(uint16_t handle, RfcommPortState *state);

/**
 * @brief This function is used to get the payload packet sent by the peer from RFCOMM.
 *        After the caller finishes using this interface, it creates a packet reference or
 *        reads the payload buffer as needed, and must free the packet obtained from RFCOMM.
 *
 * @param handle The channel(DLC)'s handle number
 * @param pkt    The packet point for receiving data
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_Read(uint16_t handle, Packet **pkt);

/**
 * @brief This function is used to write the data to be transmitted to the opposite end to RFCOMM.
 *
 * @param handle The channel(DLC)'s handle number
 * @param pkt    The packet for sending data
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_Write(uint16_t handle, Packet *pkt);

/**
 * @brief This function is used to send Test Command to the peer.
 *
 * @param handle The channel(DLC)'s handle number
 * @param pkt    The packet for sending data
 * @return Returns <b>RFCOMM_SUCCESS</b> if the operation is successful, otherwise the operation fails.
 * @since 6
 */
int BTSTACK_API RFCOMM_SendTestCmd(uint16_t handle, Packet *pkt);

#ifdef __cplusplus
}
#endif

#endif  // RFCOMM_H