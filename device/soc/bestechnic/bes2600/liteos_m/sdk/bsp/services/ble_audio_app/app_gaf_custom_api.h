/**
 ****************************************************************************************
 *
 * @file app_gaf_custom_api.h
 *
 * @brief BLE Audio APIs For Customer
 *
 * Copyright 2015-2019 BES.
 *
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
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP_GAF_CUSTOM_API
 * @{
 ****************************************************************************************
 */
#ifndef APP_GAF_CUSTOM_API_H_
#define APP_GAF_CUSTOM_API_H_

#if BLE_AUDIO_ENABLED

#ifdef __cplusplus
extern "C"{
#endif

#include "prf_types.h"
#include "app_bap_uc_srv_msg.h"
#include "app_bap_uc_cli_msg.h"
#include "app_bap_bc_scan_msg.h"
#include "app_bap_bc_sink_msg.h"
#include "app_bap_bc_assist_msg.h"
#include "app_bap_bc_deleg_msg.h"
#include "hal_trace.h"

/* Enable/Disable ACC/BAP/ARC/ATC */
#define APP_GAF_BAP_ENABLE (1)
#define APP_GAF_ARC_ENABLE (0)
#define APP_GAF_ACC_ENABLE (1)
#define APP_GAF_ATC_ENABLE (0)

//For Broadcast Stream(BIS)
#define APP_GAF_BC_SDU_INTERVAL_US          10000
#define APP_GAF_BC_MAX_SDU_SIZE             240
#define APP_GAF_BC_MAX_TRANS_LATENCY_MS     10

typedef enum {
    BIS_CMD_SUCCESS = 0,
    BIS_CMD_FAILED  = 1,
} BIS_CMD_STATE_E;

typedef enum {
    BIS_SINK_SYNC_SUCCESS = 0,
    BIS_SINK_SYNC_FAILED = 1,
    BIS_SINK_SYNC_DISCONNECT = 2,
    BIS_SINK_SYNC_DISCONNECT_FAILED = 3,
    BIS_SINK_SYNC_LOST  = 4,
} BIS_SINK_SYNC_STATE_E;

typedef enum {
    BIS_PLAY_START = 0,
    BIS_PLAY_STOP  = 1,
} BIS_PLAY_STATE_E;

typedef enum {
    BIS_AUDIO_CHANNEL_SELECT_STEREO,
    BIS_AUDIO_CHANNEL_SELECT_LRMERGE,
    BIS_AUDIO_CHANNEL_SELECT_LCHNL,
    BIS_AUDIO_CHANNEL_SELECT_RCHNL,
} BIS_AUDIO_CHANNEL_SELECT_E;

extern void (*BisLogInfo)(const char *fmt, ...);
#define HwBisLogInfo(fmt, ...) \
    if (BisLogInfo) \
        BisLogInfo(fmt, ##__VA_ARGS__); \
    else \
        LOG_I(fmt, ##__VA_ARGS__);

typedef void (* app_cmd_cb_t)(uint8_t cmd, uint8_t status);
typedef void (* app_sync_cb_t)(uint8_t status);
typedef void (* app_bis_src_underflow_cb_t)(void);
typedef void (* app_bis_sink_scan_stop_cb_t)(void);

void app_register_src_sink_cmd_callback(app_cmd_cb_t cmd_cb);
void app_register_sink_sync_callback(app_sync_cb_t sync_cb);
void app_register_sink_scan_stop_callback(app_bis_sink_scan_stop_cb_t scan_cb);
void app_set_src_cmd(uint8_t src_cmd);
void app_set_sink_cmd(uint8_t sink_cmd);
void app_src_cmd_result(uint8_t status);
void app_sink_cmd_result(uint8_t status);
void app_sink_sync_result(uint8_t status);
void app_customer_set_ibrt_role(uint8_t role);
void app_bis_set_local_volume(uint16_t volume_level);
int app_bis_set_channel_select(BIS_AUDIO_CHANNEL_SELECT_E chnl_sel);
bool app_bis_is_run(void);
void app_register_bis_underflow_callback(app_bis_src_underflow_cb_t cb);
void app_register_user_log_printf_callback(void (*cb)(const char *fmt, ...));

/*****************************Custom APIs For Audio Stream Control******************************************/
/* NOTICE: ASC&PAC&BAS services are init on earbuds, earbuds are GATT Server, mobile is GATT Client*/

/*BAP PACS APIs (Earbuds)*/
/**
 ****************************************************************************************
 * @brief add a record to specific PAC.
 *
 * @param[in] pac_lid              PAC local index
 * @param[in] codec_id             Codec Id, LC3 or Vendor Specific Codec @see app_gaf_codec_id_t
 * @param[in] p_codec_capa         Codec capabilities configuration, @see app_bap_lc3_capa_t and app_bap_vendor_codec_capa_t
 *
 ****************************************************************************************
 */
void app_bap_capa_srv_add_pac_record(uint8_t pac_lid, app_gaf_codec_id_t *codec_id, void* p_codec_capa);

/**
 ****************************************************************************************
 * @brief clear all records of specific PAC.
 *
 * @param[in] pac_lid              PAC local index
 *
 ****************************************************************************************
 */
void app_bap_capa_srv_clear_pac_record(uint8_t pac_lid);

/**
 ****************************************************************************************
 * @brief set supported location_bf (for each direction).
 *
 * @param[in] direction              ASE Direction, @see app_gaf_direction
 * @param[in] location_bf            Supported location bitfield, @see gaf_bap_supported_locations_bf
 *
 ****************************************************************************************
 */
void app_bap_capa_srv_set_supp_location_bf(enum app_gaf_direction direction, uint32_t location_bf);

/**
 ****************************************************************************************
 * @brief set supported Audio Contexts Bitfield (for each direction).
 *
 * @param[in] direction              ASE Direction, @see app_gaf_direction
 * @param[in] context_bf_supp        Supported context bitfield, @see enum gaf_bap_context_type_bf
 *
 ****************************************************************************************
 */
void app_bap_capa_srv_set_supp_context_bf(enum app_gaf_direction direction, uint16_t context_bf_supp);

/**
 ****************************************************************************************
 * @brief set Available Audio Contexts Bitfield (one for each connection, and with both direction).
 *
 * @param[in] con_lid              connection local index
 * @param[in] direction            ASE Direction, @see app_gaf_direction
 * @param[in] context_bf_ava       Available Audio Contexts bitfield, @see enum gaf_bap_context_type_bf
 *
 ****************************************************************************************
 */
void app_bap_capa_srv_set_ava_context_bf(uint8_t con_lid, enum app_gaf_direction direction, uint16_t context_bf_ava);



/*BAP ASCS APIs (Earbuds)*/
/**
 ****************************************************************************************
 * @brief Get the number of instances of the ASE characteristic.
 *
 * @return ASE number
 *
 ****************************************************************************************
 */
uint8_t app_bap_uc_srv_get_nb_ase_chars();

/**
 ****************************************************************************************
 * @brief Get the number of ASE configurations that can be maintained.
 *
 * @return ASE Number
 *
 ****************************************************************************************
 */
uint8_t app_bap_uc_srv_get_nb_ases_cfg();

/**
 ****************************************************************************************
 * @brief set ASE qos req params.
 *
 * @param[in] ase_lid              ASE local index
 * @param[in] qos_req              QoS Requirement params send from server to client when 
 *                                 ASE enter Codec Confiured State @see app_bap_qos_req_t
 *
 ****************************************************************************************
 */
void app_bap_uc_srv_set_ase_qos_req(uint8_t ase_lid, app_bap_qos_req_t *qos_req);

/**
 ****************************************************************************************
 * @brief get ASE info.
 *
 * @param[in] ase_lid              ASE local index
 *
 * @return ASE Information @see app_bap_ascs_ase_t
 ****************************************************************************************
 */
app_bap_ascs_ase_t *app_bap_uc_srv_get_ase_info(uint8_t ase_lid);

/**
 ****************************************************************************************
 * @brief Release a CIS Stream(Stream stop and CIS disconnected).
 *
 * @param[in] ase_lid          ASE local index
 * @param[in] idle
 * <table>
 * <tr><th>Value        <th>Description
 * <tr><td>0  <td>return to IDLE state
 * <tr><td>!0  <td>return CODEC CONFIGURED state
 * </table>
 *
 ****************************************************************************************
 */
void app_bap_uc_srv_stream_release(uint8_t ase_lid, uint8_t idle);

/**
 ****************************************************************************************
 * @brief Disable a CIS Stream(Stream stop but CIS still connect).
 *
 * @param[in] ase_lid          ASE local index
 *
 ****************************************************************************************
 */
void app_bap_uc_srv_stream_disable(uint8_t ase_lid);



/**
 ****************************************************************************************
 * @brief Get characteristic info of PAC Server.
 *
 * @param[in] con_lid          Connection local index
 * @param[in] char_type        Characteristic type
 * @param[in] pac_lid          PAC local index(only used when get PAC characteristic info)
 *
 ****************************************************************************************
 */
void app_bap_capa_cli_get(uint8_t con_lid, uint8_t char_type, uint8_t pac_lid);

/**
 ****************************************************************************************
 * @brief Set location_bf of PAC Server.
 *
 * @param[in] con_lid          Connection local index
 * @param[in] direction        ASE Direction, @see app_gaf_direction
 * @param[in] location_bf      ASE Location Bitfield
 *
 ****************************************************************************************
 */
void app_bap_capa_cli_set_remote_location_bf(uint8_t con_lid, enum app_gaf_direction direction, uint32_t location_bf);

/**
 ****************************************************************************************
 * @brief Set Supported Codec Params.
 *
 * @param[in] direction        ASE Direction, @see app_gaf_direction
 * @param[in] codec_id         Codec Id LC3 or Vendor Specific Codec @see app_gaf_codec_id_t
 * @param[in] cfg_len          Codec configure length
 * @param[in] p_codec_capa     Codec capabilities configuration, @see app_bap_lc3_capa_t and app_bap_vendor_codec_capa_t
 *
 ****************************************************************************************
 */
void app_bap_capa_cli_set_supp_codec_params(enum app_gaf_direction direction, app_gaf_codec_id_t codec_id,
                                                    uint16_t cfg_len, void *p_codec_capa);

/**
 ****************************************************************************************
 * @brief Set supported location Bitfield.
 *
 * @param[in] direction        ASE Direction, @see app_gaf_direction
 * @param[in] location_bf      Supported Location Bitfield
 *
 ****************************************************************************************
 */
void app_bap_capa_cli_set_supp_location_bf(enum app_gaf_direction direction, uint32_t location_bf);

/**
 ****************************************************************************************
 * @brief Set supported context Bitfield.
 *
 * @param[in] direction         ASE Direction, @see app_gaf_direction
 * @param[in] context_bf_supp   Supported context Bitfield, @see enum gaf_bap_context_type_bf
 *
 ****************************************************************************************
 */
void app_bap_capa_cli_set_supp_context_bf(enum app_gaf_direction direction, uint16_t context_bf_supp);



/*BAP ASCC APIs (Mobile)*/
/**
 ****************************************************************************************
 * @brief Disable a CIS Stream(Stream stop but CIS still connect).
 *
 * @param[in] ase_lid          ASE local index
 *
 ****************************************************************************************
 */
void app_bap_uc_cli_stream_disable(uint8_t ase_lid);

/**
 ****************************************************************************************
 * @brief Release a CIS Stream(Stream stop and CIS disconnected).
 *
 * @param[in] ase_lid          ASE local index
 *
 ****************************************************************************************
 */
void app_bap_uc_cli_stream_release(uint8_t ase_lid);

/**
 ****************************************************************************************
 * @brief Restart a CIS Stream.
 *
 * @param[in] ase_lid              ASE local index
 *
 ****************************************************************************************
 */
void app_bap_uc_cli_stream_resume(uint8_t ase_lid);

/**
 ****************************************************************************************
 * @brief Establish a CIS Stream.
 *
 * @param[in] ase_lid              ASE local index
 * @param[in] direction            ASE Direction, @see app_gaf_direction
 * @param[in] cis_id               cis index
 *
 ****************************************************************************************
 */
void app_bap_uc_cli_stream_establish(uint8_t ase_lid, enum app_gaf_direction direction, uint8_t cis_id);

/**
 ****************************************************************************************
 * @brief get ASE info.
 *
 * @param[in] ase_lid              ASE local index
 *
 * @return ASE Information @see app_bap_ascc_ase_t
 ****************************************************************************************
 */
app_bap_ascc_ase_t *app_bap_uc_cli_get_ase_info(uint8_t ase_lid);

/**
 ****************************************************************************************
 * @brief get ASE list of specific BLE connection.
 *
 * @param[in] con_lid              connection local index
 *
 * @return number of ASE
 * @param[out] ase_lid_list
 ****************************************************************************************
 */
uint8_t app_bap_uc_cli_get_ase_lid_list(uint8_t con_lid, uint8_t *ase_lid_list);

/**
 ****************************************************************************************
 * @brief Get the number of ASE configurations that can be maintained.
 *
 * @return ASE Number
 *
 ****************************************************************************************
 */
uint8_t app_bap_uc_cli_get_nb_ases_cfg();


/*BAP Broadcast Src APIs*/
/**
 ****************************************************************************************
 * @brief Start a Broadcase Source Audio Stream.
 *
 * @param[in] big_idx          BIS Group(BIG) local index
 *
 ****************************************************************************************
 */
void app_bap_bc_src_start(uint8_t big_idx);

/**
 ****************************************************************************************
 * @brief Stop a Broadcase Source Audio Stream.
 *
 * @param[in] big_idx          BIS Group(BIG) local index
 *
 ****************************************************************************************
 */
void app_bap_bc_src_stop(uint8_t big_idx);

/**
 ****************************************************************************************
 * @brief Set BIG group params.
 *
 * @param[in] big_idx          BIS Group(BIG) local index
 * @param[in] grp_param        BIG Group params, @see app_bap_bc_grp_param_t
 *
 ****************************************************************************************
 */
void app_bap_bc_src_set_grp_params(uint8_t big_idx, app_bap_bc_grp_param_t *grp_param);

/**
 ****************************************************************************************
 * @brief Set BIG advertising params.
 *
 * @param[in] big_idx          BIS Group(BIG) local index
 * @param[in] adv_param        BIG Advertising params, @see app_bap_bc_adv_param_t
 *
 ****************************************************************************************
 */
void app_bap_bc_src_set_adv_params(uint8_t big_idx, app_bap_bc_adv_param_t *adv_param);

/**
 ****************************************************************************************
 * @brief Set BIG periodic advertising params.
 *
 * @param[in] big_idx          BIS Group(BIG) local index
 * @param[in] per_adv_param    BIG Periodic Advertising params, @see app_bap_bc_per_adv_param_t
 *
 ****************************************************************************************
 */
void app_bap_bc_src_set_per_adv_params(uint8_t big_idx, app_bap_bc_per_adv_param_t *per_adv_param);

/**
 ****************************************************************************************
 * @brief Set BIG presentation delay in us.
 *
 * @param[in] big_idx          BIS Group(BIG) local index
 * @param[in] pres_delay_us    Presentation Delay(Unit: us)
 *
 ****************************************************************************************
 */
void app_bap_bc_src_set_pres_delay_us(uint8_t big_idx, uint32_t pres_delay_us);


/**
 ****************************************************************************************
 * @brief Set BIG Codec configuration.
 *
 * @param[in] big_idx          BIS Group(BIG) local index
 * @param[in] codec_id         Codec Id LC3 or Vendor Specific Codec @see app_gaf_codec_id_t
 * @param[in] p_codec_cfg      Codec capabilities configuration, @see app_bap_lc3_cfg_t and app_bap_vendor_codec_cfg_t
 *
 ****************************************************************************************
 */
void app_bap_bc_src_set_big_codec_cfg(uint8_t big_idx, app_gaf_codec_id_t *codec_id, void* p_codec_cfg);

/**
 ****************************************************************************************
 * @brief Set Codec configuration of specific BIS stream.
 *
 * @param[in] big_idx          BIS Group(BIG) local index
 * @param[in] stream_lid       Stream local index
 * @param[in] p_codec_cfg      Codec capabilities configuration, @see app_bap_lc3_capa_t and app_bap_vendor_codec_capa_t
 *
 ****************************************************************************************
 */
void app_bap_bc_src_set_stream_codec_cfg(uint8_t big_idx, uint8_t stream_lid, void* p_codec_cfg);

/**
 ****************************************************************************************
 * @brief Set BIG encrypt configuration.
 *
 * @param[in] big_idx          BIS Group(BIG) local index
 * @param[in] is_encrypted     0:Not encrypted, !=0:encrypted
 * @param[in] bcast_code       Broadcast Code, @see app_gaf_bcast_code_t, only meaningful when is_encrypted != 0
 *
 ****************************************************************************************
 */
void app_bap_bc_src_set_encrypt(uint8_t big_idx, uint8_t is_encrypted, app_gaf_bcast_code_t bcast_code);

/**
 ****************************************************************************************
 * @brief Set BIG adv data and periodic adv data.
 *
 * @param[in] big_idx          BIS Group(BIG) local index
 * @param[in] adv              adv data
 * @param[in] adv_len          adv data length
 * @param[in] per_adv          periodic adv data
 * @param[in] per_adv_len      periodic adv data length
 *
 ****************************************************************************************
 */
void app_bap_bc_src_set_adv(uint8_t big_idx, char* adv, uint8_t adv_len, char* per_adv, uint8_t per_adv_len);


/*BAP Broadcast Scan APIs*/
/**
 ****************************************************************************************
 * @brief Start scanning.
 *
 * @param[in] scan_trigger_method         Trigger method, @see enum app_bap_bc_scan_method
 *
 ****************************************************************************************
 */
void app_bap_bc_scan_start(enum app_bap_bc_scan_method scan_method);

/**
 ****************************************************************************************
 * @brief Stop bis sink.
 *
 ****************************************************************************************
 */
void app_bis_sink_stop(void);

/**
 ****************************************************************************************
 * @brief Stop scanning.
 *
 ****************************************************************************************
 */
void app_bap_bc_scan_stop(void);

/**
 ****************************************************************************************
 * @brief Set scan parameters.
 * @param[in] scan_timeout_s     Scan timeout.Unit:s
 * @param[in] scan_param         Scan parameters, @see app_bap_bc_scan_param_t
 *
 ****************************************************************************************
 */
void app_bap_bc_scan_set_scan_param(    uint16_t scan_timeout_s, app_bap_bc_scan_param_t *scan_param);

/**
 ****************************************************************************************
 * @brief Set synchronize parameters.
 * @param[in] sync_param         Synchronize parameters, @see app_bap_bc_scan_sync_param_t
 *
 ****************************************************************************************
 */
void app_bap_bc_scan_set_sync_param(app_bap_bc_scan_sync_param_t *sync_param);

/**
 ****************************************************************************************
 * @brief Start periodic advertising synchronize.
 * @param[in] addr         Address info about the device to synchronize, @see app_gaf_per_adv_bdaddr_t
 *
 ****************************************************************************************
 */
void app_bap_bc_scan_pa_sync(app_gaf_per_adv_bdaddr_t *addr);

/**
 ****************************************************************************************
 * @brief Start periodic advertising synchronize with a connected device.
 * @param[in] con_lid        Connection local index
 *
 ****************************************************************************************
 */
void app_bap_bc_scan_pa_sync_con(uint8_t con_lid);

/**
 ****************************************************************************************
 * @brief Set the periodic advertising information reported by stack.
 * @param[in] pa_lid                Periodic advertising local index
 * @param[in] report_filter_bf      Report filter Bitfield, @see app_bap_bc_scan_report_filter_bf
 *                                  Bit set to 1 means report, 0 means Not report
 *
 ****************************************************************************************
 */
void app_bap_bc_scan_pa_report_ctrl(uint8_t pa_lid, uint8_t report_filter_bf);

/**
 ****************************************************************************************
 * @brief Terminate periodic advertising synchronize.
 * @param[in] pa_lid         Periodic advertising local index
 *
 ****************************************************************************************
 */
void app_bap_bc_scan_pa_terminate(uint8_t pa_lid);


/*BAP Broadcast Sink APIs*/
/**
 ****************************************************************************************
 * @brief Enable a group of sink streams.
 * @param[in] p_sink_enable         Sink Stream group information, @see app_bap_bc_sink_enable_t
 *
 ****************************************************************************************
 */
void app_bap_bc_sink_enable(app_bap_bc_sink_enable_t *p_sink_enable);

/**
 ****************************************************************************************
 * @brief Disable a group of sink streams.
 * @param[in] grp_lid         Group local index
 *
 ****************************************************************************************
 */
void app_bap_bc_sink_disable(uint8_t grp_lid);

/**
 ****************************************************************************************
 * @brief Start one of a group of sink streams.
 * @param[in] stream         The information of stream to start, @see app_bap_bc_sink_start_streaming_t
 *
 ****************************************************************************************
 */
void app_bap_bc_sink_start_streaming(app_bap_bc_sink_start_streaming_t *stream);

/**
 ****************************************************************************************
 * @brief Stop one of a group of sink streams.
 * @param[in] grp_lid         Sink streams group local index
 * @param[in] stream_pos      Stream position in group
 *
 ****************************************************************************************
 */
void app_bap_bc_sink_stop_streaming(uint8_t grp_lid, uint8_t stream_pos);


/*BAP Broadcast Delegator APIs*/
/**
 ****************************************************************************************
 * @brief Set advertising data of solicite advertising.
 * @param[in] adv_data        Advertising data
 * @param[in] adv_data_len    Advertising data length
 *
 ****************************************************************************************
 */
void app_bap_bc_deleg_set_solicite_adv_data(char *adv_data, uint8_t adv_data_len);

/**
 ****************************************************************************************
 * @brief Set advertising params of solicite advertising.
 * @param[in] adv_param        Advertising params, @see app_bap_bc_adv_param_t
 *
 ****************************************************************************************
 */
void app_bap_bc_deleg_set_adv_params( app_bap_bc_adv_param_t *adv_param);

/**
 ****************************************************************************************
 * @brief Start solicite advertising.
 * @param[in] timeout_s         Timeout duration of adv, Unit:s
 * @param[in] context_bf        Available audio contexts bit field in adv data, , @see enum gaf_bap_context_type_bf
 *
 ****************************************************************************************
 */
void app_bap_bc_deleg_start_solicite(uint16_t timeout_s, uint32_t context_bf);

/**
 ****************************************************************************************
 * @brief Sync to periodic advertising.
 * @param[in] src_lid         Broadcast source local index
 *
 ****************************************************************************************
 */
void app_bap_bc_deleg_pa_sync(uint8_t src_lid);

/**
 ****************************************************************************************
 * @brief Sync to big.
 * @param[in] p_sink_enable         Infomation of BIG to sync, @see app_bap_bc_deleg_sink_enable_t
 *
 ****************************************************************************************
 */
void app_bap_bc_deleg_sink_enable(app_bap_bc_deleg_sink_enable_t *p_sink_enable);

/**
 ****************************************************************************************
 * @brief Stop solicite advertising.
 *
 ****************************************************************************************
 */
void app_bap_bc_deleg_stop_solicite(void);



/*BAP Broadcast Assistant APIs*/
/**
 ****************************************************************************************
 * @brief Send the information regarding a Broadcast Source to Delegator.
 * @param[in] p_src_add         Broadcast Source Information, @see app_bap_bc_assist_add_src_t
 *
 ****************************************************************************************
 */
void app_bap_bc_assist_source_add(app_bap_bc_assist_add_src_t *p_src_add);

/**
 ****************************************************************************************
 * @brief Request the Delegator to remove information for a Broadcast Source.
 * @param[in] con_lid         Connection local index
 * @param[in] src_lid         Broadcast local index
 *
 ****************************************************************************************
 */
void app_bap_bc_assist_source_remove(uint8_t con_lid, uint8_t src_lid);

/**
 ****************************************************************************************
 * @brief Get the Broadcast Receive State of Delegator.
 * @param[in] con_lid         Connection local index
 * @param[in] src_lid         Broadcast local index
 *
 ****************************************************************************************
 */
void app_bap_bc_assist_get_state(uint8_t con_lid, uint8_t src_lid);

/**
 ****************************************************************************************
 * @brief Start scanning for solicitation.
 * @param[in] timeout_s         Scanning time duration, Unit:s
 *
 ****************************************************************************************
 */
void app_bap_bc_assist_start_scan(uint16_t timeout_s);

/**
 ****************************************************************************************
 * @brief Stop scanning for solicitation.
 *
 ****************************************************************************************
 */
void app_bap_bc_assist_stop_scan(void);


/*****************************Custom APIs For Audio Rendering Control******************************************/
/* NOTICE: ARC services are init on earbuds, earbuds are GATT Server, mobile is GATT Client*/
/* Audio Input Control APIs for mobile*/
void app_arc_aicc_set_desc(uint8_t con_lid, uint8_t input_lid,
                                        char *desc, uint16_t desc_len);
void app_arc_aicc_set_cfg(uint8_t con_lid, uint8_t input_lid,
                                    uint8_t char_type, uint8_t enable);
void app_arc_aicc_control(uint8_t con_lid, uint8_t input_lid,
                                    uint8_t opcode, int16_t value);

/* Audio Input Control APIs for earbuds*/
void app_arc_aics_set(uint8_t input_lid, uint8_t set_type, uint32_t value);
void app_arc_aics_set_desc(uint8_t input_lid, uint8_t *desc, uint8_t desc_len);

/* Microphone Control APIs for mobile */
void app_arc_micc_set_cfg(uint8_t con_lid, uint8_t enable);
void app_arc_micc_set_mute(uint8_t con_lid, uint8_t mute);

/* Microphone Control API for earbuds */
void app_arc_mics_set_mute(uint8_t mute);

/* Volume Control API for mobile */
void app_arc_vcc_control(uint8_t con_lid, uint8_t opcode, uint8_t volume);

/* Volume Control API for earbuds */
void app_arc_vcs_control(uint8_t opcode, uint8_t volume);

/* Volume Offset Control APIs for mobile */
void app_arc_vocc_set_desc(uint8_t con_lid, uint8_t output_lid,
                                         char *desc, uint16_t desc_len);
void app_arc_vocc_set_cfg(uint8_t con_lid, uint8_t output_lid,
                                    uint8_t char_type, uint8_t enable);
void app_arc_vocc_control(uint8_t con_lid, uint8_t output_lid,
                                    uint8_t opcode, int16_t value);

/* Volume Offset Control APIs for earbuds */
void app_arc_vocs_set(uint8_t output_lid, uint8_t set_type, int16_t value);
void app_arc_vocs_set_desc(uint8_t output_lid, uint8_t *desc, uint8_t desc_len);



/*****************************Custom APIs For Audio Topology Control******************************************/
/* NOTICE: ATC services are init on earbuds, earbuds are GATT Server, mobile is GATT Client*/
/* Coordinated Set Identification APIs for mobile */
void app_atc_csisc_resolve(uint8_t *psri);
void app_atc_csisc_lock(uint8_t con_lid, uint8_t set_lid, uint8_t lock);
void app_atc_csisc_get(uint8_t con_lid, uint8_t set_lid, uint8_t char_type);
void app_atc_csisc_get_cfg(uint8_t con_lid, uint8_t set_lid, uint8_t char_type);
void app_atc_csisc_set_cfg(uint8_t con_lid, uint8_t set_lid, uint8_t char_type, uint8_t enable);
void app_atc_csisc_add_sirk(uint8_t *sirk);
void app_atc_csisc_remove_sirk(uint8_t key_lid);

/* Coordinated Set Identification APIs for earbuds */
void app_atc_csism_set_sirk(uint8_t set_lid, uint8_t *sirk);
void app_atc_csism_update_psri(uint8_t set_lid);
void app_atc_csism_set_size(uint8_t set_lid, uint8_t size);


/*****************************Custom APIs For Audio Content Control******************************************/
/* NOTICE: ACC services are init on mobile phone, mobile is GATT Server, earbuds are GATT Client*/
/* Media Control APIs for earbuds*/
void app_acc_mcc_get(uint8_t con_lid,uint8_t media_lid, uint8_t char_type);
void app_acc_mcc_get_cfg(uint8_t con_lid,uint8_t media_lid, uint8_t char_type);
void app_acc_mcc_set_cfg(uint8_t con_lid,uint8_t media_lid, uint8_t char_type, uint8_t enable);
void app_acc_mcc_set(uint8_t con_lid, uint8_t media_lid, uint8_t char_type, int32_t val);
void app_acc_mcc_set_obj_id(uint8_t con_lid, uint8_t media_lid, uint8_t char_type, uint8_t *obj_id);
void app_acc_mcc_control(uint8_t con_lid, uint8_t media_lid, uint8_t opcode, uint32_t val);
void app_acc_mcc_search(uint8_t con_lid, uint8_t media_lid, uint8_t param_len, uint8_t *param);

/* Media Control APIs for mobile*/
void app_acc_mcs_set(uint8_t media_lid, uint8_t char_type, int32_t val);
void app_acc_mcs_set_obj_id(uint8_t media_lid, uint8_t *obj);
void app_acc_mcs_set_player_name(uint8_t media_lid, uint8_t *name, uint8_t name_len);
void app_acc_mcs_action(uint8_t media_lid, uint8_t action,
                                int32_t track_pos, int8_t seeking_speed);
void app_acc_mcs_track_change(uint8_t media_lid, int32_t track_dur,
                                        uint8_t *title, uint8_t title_len);

/* Object Transfer APIs for earbuds*/
void app_acc_otc_get(uint8_t con_lid,uint8_t transfer_lid, uint8_t get_type, uint8_t char_type);
void app_acc_otc_get_cfg(uint8_t con_lid,uint8_t transfer_lid, uint8_t ind_char_type);
void app_acc_otc_set_cfg(uint8_t con_lid,uint8_t transfer_lid, uint8_t ind_char_type, uint8_t enable);
void app_acc_otc_set_name(uint8_t con_lid, uint8_t transfer_lid, uint8_t *name, uint8_t name_len);
void app_acc_otc_set_time(uint8_t con_lid, uint8_t transfer_lid, uint8_t char_type, prf_date_time_t *time);
void app_acc_otc_set_properties(uint8_t con_lid, uint8_t transfer_lid, uint32_t properties);
void app_acc_otc_object_create(uint8_t con_lid, uint8_t transfer_lid, uint8_t size, uint16_t uuid);
void app_acc_otc_object_control(uint8_t con_lid, uint8_t transfer_lid, uint8_t opcode);
void app_acc_otc_object_manipulate(uint8_t con_lid, uint8_t transfer_lid, uint8_t opcode,
                                        uint32_t offset, uint32_t length, uint8_t mode);
void app_acc_otc_object_execute(uint8_t con_lid, uint8_t transfer_lid, uint8_t param_len, uint8_t *param);
void app_acc_otc_list_control(uint8_t con_lid, uint8_t transfer_lid, uint8_t opcode, uint8_t order);
void app_acc_otc_list_goto(uint8_t con_lid, uint8_t transfer_lid, uint8_t opcode, uint8_t *object_id);
void app_acc_otc_list_filter_set(uint8_t con_lid, uint8_t transfer_lid, uint8_t filter_lid, uint8_t filter_val);
void app_acc_otc_list_filter_set_time(uint8_t con_lid, uint8_t transfer_lid, uint8_t filter_lid,
                                                uint8_t filter_val, prf_date_time_t *time_start, prf_date_time_t *time_end);
void app_acc_otc_list_filter_set_size(uint8_t con_lid, uint8_t transfer_lid, uint8_t filter_lid,
                                                uint8_t filter_val, uint32_t size_min, uint32_t size_max);
void app_acc_otc_list_filter_set_name(uint8_t con_lid, uint8_t transfer_lid, uint8_t filter_lid,
                                                uint8_t filter_val, uint32_t *name, uint32_t name_len);
void app_acc_otc_list_filter_set_type(uint8_t con_lid, uint8_t transfer_lid, uint8_t filter_lid, uint16_t uuid);
void app_acc_otc_coc_connect(uint8_t con_lid, uint16_t local_max_sdu);
void app_acc_otc_coc_disconnect(uint8_t con_lid);
void app_acc_otc_coc_send(uint8_t con_lid, uint8_t length, uint8_t *sdu);
void app_acc_otc_coc_release(uint8_t con_lid);

/* Object Transfer APIs for mobile*/
void app_acc_ots_coc_disconnect(uint8_t con_lid);
void app_acc_ots_coc_send(uint8_t con_lid, uint8_t length, uint8_t *sdu);
void app_acc_ots_coc_release(uint8_t con_lid);
void app_acc_ots_obj_add(uint8_t mc_obj_idx, uint32_t allocated_size, uint16_t uuid);
void app_acc_ots_obj_remove(uint8_t object_lid);
void app_acc_ots_obj_change(uint8_t con_lid, uint8_t transfer_lid, uint8_t object_lid);
void app_acc_ots_obj_changed(uint8_t flags, uint8_t *object_id);
void app_acc_ots_set(uint8_t object_lid, uint8_t set_type, uint32_t val);
void app_acc_ots_set_time(uint8_t object_lid, prf_date_time_t *time);

/* Call Control APIs for earbuds */
/**
 * @brief Read characteristic value
 * 
 * @param con_lid       BLE connection local ID
 * @param bearer_lid    Bearer local ID
 * @param char_type     char_type < ACC_TB_CHAR_TYPE_MAX)
 *                      char_type != ACC_TB_CHAR_TYPE_CALL_CTL_PT
 *                      char_type != ACC_TB_CHAR_TYPE_TERM_REASON
 */
void app_acc_tbc_get(uint8_t con_lid,uint8_t bearer_lid, uint8_t char_type);

/**
 * @brief Configure characteristic for notification
 * 
 * @param con_lid       BLE connection local ID
 * @param bearer_lid    Bearer local ID
 * @param char_type     Characteristic type, @see enum acc_tb_char_type
 *                      char_type < ACC_TB_NTF_CHAR_TYPE_MAX
 *                      char_type != ACC_TB_CHAR_TYPE_TERM_REASON
 * @param enable        Enable/disable
 */
void app_acc_tbc_set_cfg(uint8_t con_lid,uint8_t bearer_lid, uint8_t char_type, uint8_t enable);

/**
 * @brief Get configuration for notification
 * 
 * @param con_lid       BLE connection local ID
 * @param bearer_lid    Bearer local ID
 * @param char_type     Characteristic type, @see enum acc_tb_char_type
 */
void app_acc_tbc_get_cfg(uint8_t con_lid,uint8_t bearer_lid, uint8_t char_type);

/**
 * @brief Set report interval
 * 
 * @param con_lid       BLE connection local ID
 * @param bearer_lid    Bearer local ID
 * @param interval      Interval in second
 */
void app_acc_tbc_set_rpt_intv(uint8_t con_lid, uint8_t bearer_lid, uint8_t interval);

/**
 * @brief Originate a call
 * 
 * @param con_lid       BLE connection local ID
 * @param bearer_lid    Bearer local ID
 * @param uri           Pointer of URI
 * @param uri_len       Length of URI
 */
void app_acc_tbc_call_outgoing(uint8_t con_lid, uint8_t bearer_lid, uint8_t *uri, uint8_t uri_len);

/**
 * @brief Handle a incoming call
 * 
 * @param con_lid       BLE connection local ID
 * @param bearer_lid    Bearer local ID
 * @param call_id       Call ID
 * @param opcode        ACC_TB_CHAR_TYPE_CALL_CTL_PT:
 *                      opcode != ACC_TB_OPCODE_ORIGINATE
 *                      opcode != ACC_TB_OPCODE_JOIN
 */
void app_acc_tbc_call_action(uint8_t con_lid, uint8_t bearer_lid, uint8_t call_id, uint8_t opcode);

/**
 * @brief Join two or more calls
 * 
 * @param con_lid       BLE connection local ID
 * @param bearer_lid    Bearer local ID
 * @param nb_calls      Number of calls to join
 * @param call_ids      Pointer of call IDs
 */
void app_acc_tbc_call_join(uint8_t con_lid, uint8_t bearer_lid, uint8_t nb_calls, uint8_t *call_ids);

/**
 * @brief Add instance for telephone bearer service
 * 
 * @param ccid          CCID to add
 * @param uci           UCI pointer
 * @param uci_len       Length of UCI string
 */
void app_acc_tbs_add(uint8_t ccid, uint8_t *uci, uint8_t uci_len);

/**
 * @brief Enable telephone bearer service
 * 
 */
void app_acc_tbs_enable(void);

/* Call Control APIs for mobile */
/* app_acc_tbs_set:
char_type:
ACC_TB_CHAR_TYPE_SIGN_STRENGTH (< ACC_TB_SIGN_STRENGTH_MAX)
ACC_TB_CHAR_TYPE_TECHNO: see (enum acc_tb_techno)
*/
void app_acc_tbs_set(uint8_t bearer_lid, uint8_t char_type, uint8_t val);

/* app_acc_tbs_set_status: set ACC_TB_CHAR_TYPE_STATUS_FLAGS
status_type:
/// Inband ringtone
ACC_TBS_STATUS_TYPE_INBAND_RINGTONE = 0,
/// Silent mode
ACC_TBS_STATUS_TYPE_SILENT_MODE,
*/
void app_acc_tbs_set_status(uint8_t bearer_lid, uint8_t status_type, uint8_t val);

/* char_type:
ACC_TB_CHAR_TYPE_PROV_NAME
ACC_TB_CHAR_TYPE_URI_SCHEMES_LIST
*/
void app_acc_tbs_set_long(uint8_t bearer_lid, uint8_t char_type, uint8_t *val, uint8_t len);

/**
 * @brief Inform local stack and peer device(ex. earbuds) the incoming call information
 * 
 * @param bearer_lid    bearer local ID
 * @param val           pointer of incoming call information, sequentially include URI, TGT_URI, FRIENDLY_NAME
 * @param uri_len       URI length
 * @param tgt_uri_len   TGT_URI length
 * @param friendly_name_len FRIENDLY_NAME length
 */
void app_acc_tbs_call_incoming(uint8_t bearer_lid,  uint8_t *val, uint8_t uri_len,
                               uint8_t tgt_uri_len, uint8_t friendly_name_len);

/**
 * @brief Inform local stack and peer device(ex. earbuds) the outgoing call information
 * 
 * @param bearer_lid    bearer local ID
 * @param val           pointer of incoming call information, sequentially include URI, FRIENDLY_NAME
 * @param uri_len       URI length
 * @param friendly_name_len FRIENDLY_NAME length 
 */
void app_acc_tbs_call_outgoing(uint8_t bearer_lid,  uint8_t *val,
                               uint8_t uri_len, uint8_t friendly_name_len);

/**
 * @brief Inform an call related action to local stack and peer device 
 * 
 * @param bearer_lid    bearer local ID, cmd response value of command @see ACC_TBS_ADD
 * @param call_id       call ID, cmd response value of command @see ACC_TBS_CALL_INCOMING
 * @param action        call action, @see enum acc_tbs_call_action
 * @param reason        reason of action, @see enum acc_tb_term_reason, only useful when action is ACC_TBS_ACTION_TERMINATE
 */
void app_acc_tbs_call_action(uint8_t bearer_lid,  uint8_t call_id,
                             uint8_t action, uint8_t reason);

/**
 * @brief Inform local stack and peer device that join the call
 * 
 * @param bearer_lid    bearer local ID, cmd response value of command @see ACC_TBS_ADD
 * @param nb_calls      number of call
 * @param call_ids      Pointer of the call ID
 */
void app_acc_tbs_call_join(uint8_t bearer_lid,  uint8_t nb_calls, uint8_t *call_ids);

#ifdef __cplusplus
}
#endif

#endif
#endif // APP_GAF_CUSTOM_API_H_

/// @} APP_GAF_CUSTOM_API_H_
