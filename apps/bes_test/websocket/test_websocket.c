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
static int aiot_ws_result = -1;
#define __NET_LIBRWS_SUPPORT__
#if defined(__NET_LIBRWS_SUPPORT__)
#include "librws.h"
#include "string.h"
#include "assert.h"
#include "cmsis_os.h"
static void on_socket_received_text(rws_socket socket, const char * text, const unsigned int length)
{
    char buff[8*1024];
    memcpy(buff, text, length);
    buff[length] = 0;

    printf("\nSocket text: %s", text);
}

static void on_socket_received_bin(rws_socket socket, const void * data, const unsigned int length)
{
    char buff[8*1024];
    memcpy(buff, data, length);
    buff[length] = 0;

    printf("\nSocket bin: <%s>", buff);
}

static void on_socket_connected(rws_socket socket)
{
    const char * test_send_text =
        "{\"version\":\"1.0\",\"supportedConnectionTypes\":[\"websocket\"],\"minimumVersion\":\"1.0\",\"channel\":\"/meta/handshake\"}";

    printf("\nSocket connected");

    rws_socket_send_text(socket, test_send_text);
    aiot_ws_result = 0;
}

static void on_socket_disconnected(rws_socket socket)
{
    rws_error error = rws_socket_get_error(socket);
    if (error)
    {
        printf("\nSocket disconnect with code, error: %i, %s",
               rws_error_get_code(error),
               rws_error_get_description(error));
        aiot_ws_result = 1;
    }
}
#endif
int bes_websocket_test(void)
{
#if defined(__NET_LIBRWS_SUPPORT__)
    rws_socket socket;

    aiot_ws_result = -1;

    const char * scheme = "ws";
    const char * host = "echo.websocket.org";
    const char * path = "/";

    socket = rws_socket_create();
    assert(socket);

    rws_socket_set_scheme(socket, scheme);
    printf("%i\n", (int)__LINE__);
    assert(strcmp(rws_socket_get_scheme(socket), scheme) == 0);
    printf("%i\n", (int)__LINE__);

    rws_socket_set_host(socket, host);
    printf("%i\n", (int)__LINE__);
    assert(strcmp(rws_socket_get_host(socket), host) == 0);
    printf("%i\n", (int)__LINE__);

    rws_socket_set_path(socket, path);
    printf("%i\n", (int)__LINE__);
    assert(strcmp(rws_socket_get_path(socket), path) == 0);
    printf("%i\n", (int)__LINE__);

    rws_socket_set_port(socket, 80);
    printf("%i\n", (int)__LINE__);
    assert(rws_socket_get_port(socket) == 80);
    printf("%i\n", (int)__LINE__);

    rws_socket_set_port(socket, 443);
    printf("%i\n", (int)__LINE__);
    assert(rws_socket_get_port(socket) == 443);
    printf("%i\n", (int)__LINE__);

    rws_socket_set_scheme(socket, NULL);
    printf("%i\n", (int)__LINE__);
    assert(rws_socket_get_scheme(socket) == NULL);
    printf("%i\n", (int)__LINE__);

    rws_socket_set_host(socket, NULL);
    printf("%i\n", (int)__LINE__);
    assert(rws_socket_get_host(socket) == NULL);
    printf("%i\n", (int)__LINE__);

    rws_socket_set_path(socket, NULL);
    printf("%i\n", (int)__LINE__);
    assert(rws_socket_get_path(socket) == NULL);
    printf("%i\n", (int)__LINE__);

    rws_socket_disconnect_and_release(socket);

    printf("%i\n", (int)__LINE__);

    socket = rws_socket_create(); // create and store socket handle
    assert(socket);
    printf("%i\n", (int)__LINE__);

    rws_socket_set_scheme(socket, "ws");
    rws_socket_set_host(socket, "192.168.0.108");
    rws_socket_set_path(socket, "/echo");
    rws_socket_set_port(socket, 81);

    rws_socket_set_on_disconnected(socket, &on_socket_disconnected);
    rws_socket_set_on_connected(socket, &on_socket_connected);
    rws_socket_set_on_received_text(socket, &on_socket_received_text);
    rws_socket_set_on_received_bin(socket, &on_socket_received_bin);
    printf("%i\n", (int)__LINE__);

    rws_socket_connect(socket);

    printf("%i\n", (int)__LINE__);

    while (aiot_ws_result == -1)
    {
        osDelay(100);
    }
	 
	// Warning: librws is too old. To run the test successfully, must make server and client compatibility.
	//For HyBi 06 and later, ws_protocol is always set to None when
	//web_socket_do_extra_handshake is called. If ws_requested_protocols is not
	//None, you must choose one subprotocol from this list and set it to
	//ws_protocol. 
	// Two way:
	// 1. remove "Sec-WebSocket-Protocol: chat, superchat\r\n"
	// or 2. modify server not to check the subprotocol or set ws_requested_protocols to None

	//socket will close by rws.
    //rws_socket_disconnect_and_release(socket);
    osDelay(2000); // wait a while
#endif /* __NET_LIBRWS_SUPPORT__ */
    return aiot_ws_result;
}
