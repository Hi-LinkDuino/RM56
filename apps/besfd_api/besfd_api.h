#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define FTP_HANDLE  void *

typedef uint32_t (*AudioDataHandler)(uint8_t *buf, uint32_t len);


#ifdef __cplusplus
extern "C" {
#endif

//send AT
void cmd_send(const char *str);


/**
 * Connect ftp server and save settings to NV once success
 */
FTP_HANDLE ftp_connect_with(char *server, char *port, char *user, char *passwd);
/**
 * Connect ftp server with setttings from NV
 */
FTP_HANDLE ftp_connect(void);
void ftp_close(FTP_HANDLE handler);
int ftp_get_file(FTP_HANDLE ftp_handler, char *fname);
int ftp_put_file(void *ftp_handler, char *fname);

/**
 * @brief 
 * mic record and upload 3-channels raw audio data to ftp
 * 
 * @param ftp_handler 
 * @param sample_rate 16000/48000
 * @param seconds   duration
 * @return int      <0: error failed, >= 0: datalength in bytes uploaded to ftp
 */
int mic_to_ftp(FTP_HANDLE ftp_handler, uint32_t sample_rate, uint32_t seconds);

int mic_to_file_start(uint32_t seconds, char *fname);
void mic_to_file_stop(void);

/**
 * @brief switch wifi mode
 *
 * @param action    action to switch mode:
 *      - 'ap'      - switch to AP mode
 *      - 'sta'     - switch to STA mode
 *      - 'toggle'  - toggle between AP and STA mode
 *      - NULL      - same as 'toggle'
 */
int wifi_switch_mode(char *action);
bool wifi_in_ap_mode(void);
bool wifi_sta_connected(void);

/********************************************************
 * Audio
 ********************************************************/
void aud_playback_stop(void);
void aud_playback_start(uint8_t channel, uint32_t sample_rate, uint8_t vol, AudioDataHandler handler);
void aud_capture_stop(void);
void aud_capture_start(uint8_t channel, uint32_t sample_rate, uint8_t vol, AudioDataHandler handler);
void aud_loop_test_start(uint8_t channel, uint8_t vol);
void aud_loop_test_stop(void);
enum MediaType {
  MT_HFP,
  MT_A2DP,
  MT_PROMPT,
  MT_LPLAYER,
};

int wav_buffer_new(uint8_t *buf, uint32_t sample_rate, uint8_t chan);
void wav_buffer_done(uint8_t *buf, uint32_t file_size);

FILE *wav_file_new(char *fname, uint32_t sample_rate, uint8_t chan);
int wav_file_write(FILE *fp, uint8_t *buf, uint32_t size);
void wav_file_done(FILE *fp, uint32_t pcm_size);
/**
 * Multi types of media can be active at the same time, 
 * but only one is playing for some priority strategy
 */
bool media_is_current_playing(enum MediaType m);
bool media_is_playing(void);
bool media_is_active(enum MediaType m); 

/********************************************************
 * BT
 ********************************************************/
bool app_is_stack_ready(void);
#define bt_stack_ready()    app_is_stack_ready()
void bt_enter_paring_mode(void);
int ble_adv_start(uint16_t advInterval);
void ble_adv_stop(void);

uint8_t app_bt_audio_count_connected_hfp(void);
uint8_t app_bt_audio_count_connected_a2dp(void);

#define bt_hfp_connected()  app_bt_audio_count_connected_hfp()
#define bt_a2dp_connected() app_bt_audio_count_connected_a2dp()


/********************************************************
 * MIC MUTE
 ********************************************************/
bool swift_mic_is_mute();
void swift_mic_mute_toggle(void);


/********************************************************
 * LED Scene
 ********************************************************/
void swift_led_scene_update(void);

#ifdef __cplusplus
}
#endif