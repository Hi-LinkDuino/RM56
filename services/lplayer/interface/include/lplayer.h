/***************************************************************************
 *
 * Copyright 2015-2022 BES.
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
#ifndef AUD_PLAYER_C
#error "NEVER include lplayer.h any more."
#endif

#ifndef __LPLAYER_H__
#define __LPLAYER_H__

#include "plat_types.h"

/**
 * @brief type of a player instance handle
 * **/
typedef struct lplayer lplayer_t;

/**
 * @brief type of a player state
 * **/
typedef enum
{
    PLAYER_STATE_ERROR = 0,
    PLAYER_IDLE = 1 << 0,
    PLAYER_INITIALIZED = 1 << 1,
    PLAYER_PREPARED = 1 << 2,
    PLAYER_STARTED = 1 << 3,
    PLAYER_PAUSED = 1 << 4,
} lplayer_state_t;

/**
 * @brief prototype of a player complete notify callback function.
 * **/
typedef int (*lplayer_complete_notify)(void);

/**
 * @brief prototype of a player error notify callback function.
 * **/
typedef int (*lplayer_error_notify)(int);

/**
 * @brief Create a player instance handle.
 *        This player supports playing local resources and network resources.
 * @param[in] url: Specifies the location of resources to play.
 * @return: a player instance handle
 */
lplayer_t *lplayer_open_url(const char *url);

/**
 * @brief Create a player instance handle.
 *        This player supports playing data on memory.
 * @param[in] buffer_addr: Pointer the data address resources to play.
 * @return: a player instance handle
 */
lplayer_t *lplayer_open_buffer(void *format, uint32_t total_buffer_size_max);

/**
 * @brief Register a play complete callback function to a player.
 *        This callback will be invoked by internal on play complete.
 *        NOTICE: NOT invoke lplayer_close() in this callbck,
 *                should invoke lplayer_close() in other thread.
 * @param[in] lplayer: a player instance handle.
 * @param[in] notify: the play complete callback function.
 *                    @see typedef lplayer_complete_notify
 * @return: - 0: success; - other: fail
 */
int lplayer_register_complete_notify(lplayer_t *lplayer, lplayer_complete_notify notify);

/**
 * @brief Register a play error callback function to a player.
 *        This callback will be invoked by internal on player error occured.
 * @param[in] lplayer: a player instance handle.
 * @param[in] notify: the play error callback function.
 *                    @see typedef lplayer_error_notify
 * @return: - 0: success; - other: fail
 */
int lplayer_register_error_notify(lplayer_t *lplayer, lplayer_error_notify notify);

/**
 * @brief Start a player.
 * @param[in] lplayer:  a player instance handle
 * @return: - 0: success; - other: fail
 */
int lplayer_play(lplayer_t *lplayer);

/**
 * @brief Pause a player.
 * @param[in] lplayer:  a player instance handle
 * @return: - 0: success; - other: fail
 */
int lplayer_pause(lplayer_t *lplayer);

/**
 * @brief Seek a player to a specific positon.
 * @param[in] lplayer:  a player instance handle
 * @param[in] seconds:  Specifies the positon of the resource. unit:s
 * @return: - 0: success; - other: fail
 */
int lplayer_seek(lplayer_t *lplayer, uint32_t seconds);

/**
 * @brief Set the volume of a player.
 * @param[in] lplayer:  a player instance handle
 * @param[in] vol:  the volume to set, range frome 0 to 100.
 * @return: - 0: success; - other: fail
 */
int lplayer_set_vol(lplayer_t *lplayer, uint32_t vol);

/**
 * @brief Get the volume of a player.
 * @param[in] lplayer:  a player instance handle
 * @param[out] vol:  the got volume.
 * @return: - 0: success; - other: fail
 */
int lplayer_get_vol(lplayer_t *lplayer, uint32_t *vol);

/**
 * @brief Set the single cycle mode of a player.
 * @param[in] lplayer:  a player instance handle
 * @param[in] en_single_loop:  enable(true) or disable(false) single cycle mode.
 * @return: - 0: success; - other: fail
 */
int lplayer_single_loop_en(lplayer_t *lplayer, bool en_single_loop);

/**
 * @brief Get if the player is in single cycle mode.
 * @param[in] lplayer:  a player instance handle
 * @return: - true: the player is in single cycle mode;
 *          - false: the player is not in single cycle mode;
 */
bool lplayer_is_enable_single_loop(lplayer_t *lplayer);

/**
 * @brief Get Get the current playback position of a player.
 * @param[in] lplayer:  a player instance handle
 * @param[out] cur_pos_ms:  the got current playback position. unit:ms
 * @return:  - 0: success; - other: fail
 */
int lplayer_get_current_pos(lplayer_t *lplayer, uint32_t *cur_pos_ms);

/**
 * @brief Get Get the current playing resource duration of a player.
 * @param[in] lplayer:  a player instance handle
 * @param[out] duration_ms:  the got current playing resource duration. unit:ms
 * @return:  - 0: success; - other: fail
 */
int lplayer_get_duration_ms(lplayer_t *lplayer, uint32_t *duration_ms);

/**
 * @brief Get Get the current status of a player.
 * @param[in] lplayer:  a player instance handle
 * @param[out] status:  the got current status. @see typedef lplayer_state_t.
 * @return:  - 0: success; - other: fail
 */
int lplayer_get_current_status(lplayer_t *lplayer, lplayer_state_t *status);

/**
 * @brief Stop play and delete a player instance.
 * @param[in] lplayer:  a player instance handle
 * @return: - 0: success; - other: fail
 */
int lplayer_close(lplayer_t *lplayer);

int lplayer_buffer_write(lplayer_t *player, uint8_t* data, uint32_t size, bool eos);

#endif