
#ifndef LPLAYER_WRAPPER_H
#define LPLAYER_WRAPPER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
  AUD_SUCCESS = 0,
  AUD_E_BADPARM = -1,
  AUD_E_NOT_INITIALIZED = -2,
  AUD_E_OPEN = -3,
  AUD_E_NOT_OPEN = -4,
  AUD_E_NOMEMORY = -5, 
  AUD_E_WOULDBLOCK = -6, //high priority audio in progress now, wait complete
  AUD_E_PAUSED = -7,
  AUD_E_REFUSED = -8,
  AUD_E_PREEMPT = -9,
}t_AudPlayerErrorType;


typedef enum {
  AUD_S_IDLE = 0,
  AUD_S_PLAYING,
  AUD_S_PAUSED_ACTIVELY,
  AUD_S_PAUSED_PASSIVELY,
  AUD_S_STOPPED,
  AUD_S_ERROR_STOPPED,
} t_AudPlayerStatus;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief prototype of player complete notify callback function.
 * errCode: 0-success, < 0: error
 */
typedef void (*audplayer_complete_notify)(int errCode);

int aud_player_start_buffer(const uint8_t *buffer, uint32_t size, audplayer_complete_notify callback);
int aud_player_start(const char *urlOrFile, audplayer_complete_notify callback);
int aud_player_stop(void);
int aud_player_pause(void);
int aud_player_resume(void);
int aud_player_seek(uint32_t sec);
//0~16
uint8_t aud_player_set_vol(uint8_t vol);
uint8_t aud_player_get_vol(void);

/**
 * @brief Get the current playback position
 * 
 * @return int 
 * < 0     errors
 * >=0     current position, in milliseconds
 */
int aud_player_tell(void);
/**
 * @brief  Get the current playing resource duration
 * 
 * @return int 
 * < 0     errors
 * >=0     duration, in milliseconds
 */
int aud_player_duration(void);
/**
 * @brief Get the current status of the audio player
 * 
 * @return int 
 * <0       error
 * >=0      @see typedef t_AudPlayerStatus.
 */
int aud_player_status(void);

//true means handled
bool aud_player_vol_key_handler(bool up);

#ifdef __cplusplus
}
#endif

#endif