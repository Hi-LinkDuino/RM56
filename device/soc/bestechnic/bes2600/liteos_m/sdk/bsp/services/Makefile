obj-y :=

ifeq ($(WIFI_APP_SUPPORT),1)
obj-y += wifi_app/
endif

#obj-y += wifi_app/wifi_console/

ifeq ($(BT_CLOSE),1)
ifeq ($(WIFI_RF_TEST_ONLY),1)
ifeq ($(ROM_BUILD),1)
obj-y += ../utils/crc32/ ../utils/crc16/ ../utils/heap/
else
obj-y += ../utils/crc32/ ../utils/crc16/ ../utils/heap/ norflash_api/ overlay/
endif
else
obj-y += hw_dsp/ resources/ voice_dev/ \
			../utils/crc32/ ../utils/crc16/ ../utils/heap/ overlay/
ifneq ($(NUTTX_BUILD),1)
obj-y += audioflinger/ audio_process/ norflash_api/ audio_dump/
endif

endif
else
obj-y += audio_dump/ \
         audioflinger/ \
         audio_process/ \
         hw_dsp/ \
         voice_dev/ \
         bt_app/ \
         overlay/ \
         resources/ \
         ../utils/crc32/ \
         ../utils/crc16/ \
         ../utils/heap/ \
         osif/ \
         norflash_api/

ifeq ($(AI_VOICE),1)
obj-y += app_ai/
endif

endif

ifeq ($(HISTREAMER_HTTP_EN), 1)
obj-y += histreamer_http/
endif

ifeq ($(BT_SOURCE),1)
obj-y += app_bt_source/
endif

ifeq ($(SPEECH_BONE_SENSOR),1)
obj-y += bone_sensor/
endif

ifneq ($(filter 1,$(INTERCONNECTION) $(INTERACTION) $(AI_VOICE)),)
obj-y += interconnection/
endif

ifneq ($(BT_CLOSE),1)
obj-y += bt_profiles_enhanced/
obj-y += bt_if_enhanced/
endif
ifeq ($(MBED),1)
obj-y += fs/
endif

#ifneq ($(filter 1, $(MBED) $(AI_VOICE) $(TOTA_v2)),)
obj-y +=../utils/kfifo/
#endif

ifeq ($(UTILS_INTERSYSHCI),1)
obj-y +=../utils/intersyshci/
endif

ifeq ($(UTILS_BES_KV),1)
obj-y +=../utils/bes_kv/
endif

ifeq ($(VOICE_DATAPATH_ENABLED),1)
obj-y += voicepath/
obj-y += voicepath/$(VOICE_DATAPATH_TYPE)/
endif

ifneq ($(BT_CLOSE),1)
ifneq ($(filter 1, $(VOICE_DATAPATH_ENABLED) $(AI_VOICE)),)
obj-y += app_ai/
endif
endif

ifneq ($(BT_CLOSE),1)
ifeq ($(BLE),1)
ifeq ($(BLE_V2),1)
obj-y += \
    ble_app_v2/ \
    bridge/ \
    ble_stack_v2/ \
    ../utils/retention_ram/ \
    ble_profiles_v2/

ifeq ($(BLE_AOB_UX_ENABLED),1)
obj-y += aob_core/
endif

ifeq ($(BLE_AUDIO_ENABLED),1)
obj-y += ble_audio_app/
endif

ifneq ($(IBRT_CORE_V2),1)
obj-y += ../utils/hsm/
endif

else
obj-y += \
    ble_app/ \
    bridge/ \
    ble_stack/ \
    ../utils/retention_ram/ \
    ble_profiles/

endif
endif
endif

ifeq ($(AUTO_TEST),1)
obj-y += auto_test/
endif

ifneq ($(NUTTX_BUILD),1)
obj-y += nv_section/

ifeq ($(SYS_TIME_SUPPORT),1)
obj-y += sys_time/
endif

ifneq ($(FPGA),1)
ifneq ($(WIFI_RF_TEST_ONLY),1)
ifneq ($(ROM_BUILD),1)
obj-y += nvrecord/
endif
endif
endif
endif

ifeq ($(VOICE_RECOGNITION),1)
obj-y += voice_recognition/
endif

ifeq ($(APP_ANC_TEST),1)
obj-y += anc_spp_tool/
endif

ifeq ($(TEST_OVER_THE_AIR),1)
obj-y += tota/
endif

ifeq ($(TEST_OVER_THE_AIR_v2),1)
obj-y += tota_v2/
endif

ifneq ($(filter 1, $(BES_OTA) $(AI_OTA) $(GMA_VOICE)),)
ifeq ($(FPGA_IBRT_OTA),1)
obj-y += fpga_ibrt_ota/
else
obj-y += ota/
endif
endif

ifneq ($(filter 1, $(BES_OTA) $(AI_OTA) $(BES_OTA_BASIC)),)
ifeq ($(IBRT_OTA),1)
ifeq ($(FPGA_IBRT_OTA),1)
obj-y += fpga_ibrt_ota/
else
obj-y += ibrt_ota/
endif

else
obj-y += ota/
endif
endif

ifeq ($(TILE_DATAPATH_ENABLED),1)
obj-y += ../thirdparty/tile/
endif

ifeq ($(AI_VOICE),1)
obj-y += ai_voice/ \
         ../utils/crc16/
endif

ifneq ($(NUTTX_BUILD),1)
ifeq ($(CHIP_HAS_CP),1)
obj-y += cp_boot/
ifeq ($(CP_ENABLE),1)
obj-y += cp_server/
endif
ifeq ($(CP_DSP),1)
obj-y += cp_dsp/
endif
endif
endif

ifeq ($(MCU_DSP),1)
obj-y += mcu_dsp/
endif

ifeq ($(IBRT),1)

ifeq ($(IBRT_UI_V1),1)
obj-y += ibrt_core/
obj-y += app_ibrt/
obj-y += ibrt_ui/
endif

ifeq ($(IBRT_CORE_V2),1)
obj-y += ../utils/hsm/
obj-y += app_ibrt_v2/
obj-y += ibrt_core_v2/
obj-y += custom_api/
endif

ifeq ($(IBRT_UI_V2),1)
obj-y += ibrt_ui_v2/
endif

endif

ifeq ($(TWS_SYSTEM_ENABLED),1)
obj-y += ibrt_middleware/
endif

ifeq ($(RSA_SHA),1)
obj-y +=../utils/sha256/ ../utils/rsa2048/
endif

ifeq ($(AES256_FOR_BIN),1)
obj-y +=../utils/aes256/
endif

obj-y += communication/


ifeq ($(RPC_SUPPORT),1)
obj-y += rpc/
endif

ifeq ($(THROUGH_PUT),1)
obj-y += through_put/
endif

ifeq ($(EPLAYER_ON),1)
obj-y += eplayer/eplayer/
ifeq ($(EPLAYER_TEST),1)
obj-y += eplayer/tests/
endif
endif

ifeq ($(ETRANSPORT_ON),1)
obj-y += etransport/
endif

ifeq ($(ECOMM_ON),1)
obj-y += ecomm/
endif

ifeq ($(EAUDIO_ON),1)
obj-y += eaudio/
obj-y += eaudio/device/
obj-y += eaudio/policy/
obj-y += eaudio/effect/
obj-y += eaudio/resample/
obj-y += eaudio/heap/
ifeq ($(EAUDIO_TEST),1)
obj-y += eaudio/tests/
endif
endif

ifeq ($(A7_DSP_ENABLE),1)
obj-y += a7_dsp/
obj-y += transq_msg/
endif

ifeq ($(USE_BT_SERVICE_ADAPTER),1)
obj-y += bt_service/
endif

ifneq ($(BT_CLOSE),1)
ifeq ($(USE_BT_ADAPTER),1)
obj-y += bluetooth_adapter/
endif
endif

ifeq ($(LPLAYER_EN),1)
obj-y += lplayer/
endif

ifeq ($(MINI_WAV_PLAYER_EN),1)
obj-y += lplayer/mini/
endif

ifeq ($(LINEIN_TRANS_ON),1)
obj-y += besaud_service/
endif

ccflags-y += \
    -Inet/wifi/ \
    -Iservices/wifi_app/ \
    -Iservices/wifi_app/wifi_console/ \
    -Inet/mbedtls/include \
    -Inet/mbedtls/include/mbedtls \

ifeq ($(WIFI_APP_SUPPORT),1)
obj-y += wifi_app/wifiapp.o
endif

#obj-y += libservices_lplayer.a

export REL_AT_C ?= 0
ifeq ($(REL_AT_C),1)

obj-y += wifi_app/wifi_console/at.o

ccflags-y += \
    -Iservices/wifi_app/ping/ \
    -Iservices/nv_section/factory_section/ \
    -Iservices/wifi_app/iperf/ \
    -Iplatform/drivers/wifi/ \
    -Inet/wpa_supplicant_l/src/utils/ \
    -Inet/utils/ \
    -Iservices/wifi_app/sniffer/ \
    -Iservices/wifi_app/bes_airkiss/ \
    -Inv_section/factory_section/ \
    -Iservices/nv_section/factory_section/ \
    -Iutils/hexdump/ \
    -Inet/core/ \
    -Iservices/wifi_app/wifi_socket/ \
    -Iplatform/hal/$(CHIP)/ \
    -Iplatform/drivers/ana/ \
    -Iservices/a7_dsp/common/ \
    -Iutils/ocd/ \
    -Iservices/lplayer/interface/include/ \
    -Iservices/lplayer/interface/include/ \
    -Iservices/lplayer/component/audio_sink/include/ \
    -Iservices/lplayer/component/include/ \
    -Iservices/wifi_app/alsa_adapter/
endif

ifeq ($(PCM_AUDIO_INSTANCE_ENABLE),1)
ccflags-y += -Iservices/a7_dsp/os/test/pcm_audio_instance/
endif

ifeq ($(SPEECH_MSG_TEST_CASE),1)
ccflags-y += -Iservices/a7_dsp/os/test/speech_msg_test/
endif

