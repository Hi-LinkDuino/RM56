/***************************************************************************
 *
 * Copyright 2015-2019 BES.
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
#if defined(NEW_NV_RECORD_ENABLED)

#ifndef NVRECORD_BT_H
#define NVRECORD_BT_H

#include "nvrecord_extension.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NVRAM_ENV_STREAM_VOLUME_A2DP_VOL_DEFAULT (AUDIO_OUTPUT_VOLUME_DEFAULT)
#define NVRAM_ENV_STREAM_VOLUME_HFP_VOL_DEFAULT (AUDIO_OUTPUT_VOLUME_DEFAULT)

typedef void (*nv_record_btdevice_new_device_paired_func_t)(const uint8_t* btAddr);
typedef bool (*nv_record_check_fake_master_bt_record_func_t)(void);

void nv_record_btdevicerecord_set_a2dp_vol(nvrec_btdevicerecord* pRecord, int8_t vol);
void nv_record_btdevicerecord_set_a2dp_abs_vol(nvrec_btdevicerecord* pRecord, int8_t vol);
void nv_record_btdevicerecord_set_hfp_vol(nvrec_btdevicerecord* pRecord, int8_t vol);
void nv_record_btdevicevolume_set_a2dp_vol(btdevice_volume* device_vol, int8_t vol);
void nv_record_btdevicevolume_set_hfp_vol(btdevice_volume* device_vol, int8_t vol);
void nv_record_btdevicerecord_set_pnp_info(nvrec_btdevicerecord* pRecord, bt_dip_pnp_info_t* pPnpInfo);
void nv_record_btdevicerecord_set_a2dp_profile_active_state(btdevice_profile* device_plf, bool isActive);
void nv_record_btdevicerecord_set_hfp_profile_active_state(btdevice_profile* device_plf, bool isActive);
int nv_record_enum_latest_two_paired_dev(btif_device_record_t* record1,btif_device_record_t* record2);
int nv_record_get_paired_dev_list(nvrec_btdevicerecord** record);
void nv_record_all_ddbrec_print(void);
void nv_record_update_runtime_userdata(void);
void nvrecord_rebuild_paired_bt_dev_info(NV_RECORD_PAIRED_BT_DEV_INFO_T* pPairedBtInfo);
int nv_record_btdevicerecord_find(const bt_bdaddr_t *bd_ddr, nvrec_btdevicerecord **record);
void nv_record_btdevicerecord_set_a2dp_profile_codec(btdevice_profile* device_plf, uint8_t a2dpCodec);
bt_status_t nv_record_ddbrec_delete(const bt_bdaddr_t *bdaddr);
bt_status_t nv_record_ddbrec_delete_all();
bt_status_t nv_record_enum_dev_records(unsigned short index,btif_device_record_t* record);
bt_status_t nv_record_ddbrec_find(const bt_bdaddr_t *bd_ddr, btif_device_record_t*record);
bool nv_record_get_pnp_info(bt_bdaddr_t *bdAddr, bt_dip_pnp_info_t *pPnpInfo);
bt_status_t nv_record_add(SECTIONS_ADP_ENUM type,void *record);
int nv_record_get_paired_dev_count(void);
void ram_record_ddbrec_init(void);
#ifdef FPGA
bt_status_t ram_record_ddbrec_find(const bt_bdaddr_t* bd_ddr, nvrec_btdevicerecord **record);
bt_status_t ram_record_ddbrec_add(const nvrec_btdevicerecord* param_rec);
bt_status_t ram_record_ddbrec_delete(const bt_bdaddr_t *bdaddr);
#endif

#ifdef TOTA_CRASH_DUMP_TOOL_ENABLE
void nv_record_btdevicerecord_crash_dump(void);
#endif

uint8_t* nv_record_btdevice_get_latest_paired_device_bt_addr(void);
void nv_record_bt_device_register_newly_paired_device_callback(nv_record_btdevice_new_device_paired_func_t func);
#ifdef USE_BT_ADAPTER
void nv_record_register_ibrt_role_check_cb(nv_record_check_fake_master_bt_record_func_t cb);
#endif

#ifdef LINK_KEY_ENCRYPT_BY_CUSTOMER
typedef int32_t (*aes256_en_de_crype_cb_t)(uint8_t *input, const uint8_t *key, uint8_t *output);
void AES256_GCM_encrypt_register(aes256_en_de_crype_cb_t cb);
void AES256_GCM_decrypt_register(aes256_en_de_crype_cb_t cb);
int32_t aes256_gcm_encrypt(uint8_t *input, const uint8_t *key, uint8_t *output);
int32_t aes256_gcm_decrypt(uint8_t *input, const uint8_t *key, uint8_t *output);

void aes256_bis_encrypt_register(aes256_en_de_crype_cb_t cb);
void aes256_bis_decrypt_register(aes256_en_de_crype_cb_t cb);
int32_t aes256_bis_encrypt(uint8_t *input, const uint8_t *key, uint8_t *output);
int32_t aes256_bis_decrypt(uint8_t *input, const uint8_t *key, uint8_t *output);
#endif

#ifdef __cplusplus
}
#endif
#endif
#endif // #if defined(NEW_NV_RECORD_ENABLED)
