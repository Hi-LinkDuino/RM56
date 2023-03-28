#ifndef  __WIFI_SOCKET_H__
#define  __WIFI_SOCKET_H__

#ifdef __cplusplus
extern "C" {
#endif

struct WIFI_SOCKET_C
{
    uint8_t wifi_socket_ip[32];
    uint32_t wifi_socket_port;
};

int wifi_socket_start(struct WIFI_SOCKET_C *wifisocket);

void set_wifi_audio_state(bool wifi_audio_state);

#ifdef __cplusplus
}
#endif
#endif