#!/bin/bash

ar=arm-none-eabi-ar


function f_lib_net()
{
  objs=(
    netbuf.o
    api_lib.o
    netifapi.o
    tcpip.o
    netdb.o
    if_api.o
    err.o
    api_msg.o
    sockets.o
    sys_arch.o
    stats.o
    mem.o
    init.o
    memp.o
    tcp_in.o
    timeouts.o
    altcp_alloc.o
    dns.o
    ip.o
    def.o
    raw.o
    tcp.o
    pbuf.o
    netif.o
    sys.o
    altcp.o
    inet_chksum.o
    altcp_tcp.o
    udp.o
    tcp_out.o
    ip4_frag.o
    dhcp.o
    etharp.o
    ip4_addr.o
    igmp.o
    ip4.o
    autoip.o
    icmp.o
    ip6_frag.o
    dhcp6.o
    inet6.o
    ip6_addr.o
    ip6.o
    nd6.o
    mld6.o
    ethip6.o
    icmp6.o
    slipif.o
    ethernet.o
    bridgeif_fdb.o
    zepif.o
    lowpan6.o
    lowpan6_ble.o
    lowpan6_common.o
    bridgeif.o
    ethernetif.o
    eap.o
    pppol2tp.o
    magic.o
    pppapi.o
    chap_ms.o
    ppp.o
    ecp.o
    chap-md5.o
    vj.o
    ipv6cp.o
    ccp.o
    chap-new.o
    auth.o
    pppoe.o
    pppcrypt.o
    multilink.o
    mppe.o
    pppos.o
    fsm.o
    ipcp.o
    lcp.o
    eui64.o
    demand.o
    upap.o
    utils.o
    sntp.o
    altcp_tls_mbedtls_mem.o
    altcp_tls_mbedtls.o
    http_client.o
    altcp_proxyconnect.o
    httpd.o
    fs.o
    lwiperf.o
    mdns.o
    mqtt.o
    netbiosns.o
    smtp.o
    snmp_threadsync.o
    snmp_scalar.o
    snmp_netconn.o
    snmp_mib2.o
    snmp_asn1.o
    snmp_pbuf_stream.o
    snmp_mib2_tcp.o
    snmp_mib2_system.o
    snmp_mib2_ip.o
    snmpv3_mbedtls.o
    snmp_mib2_snmp.o
    snmp_traps.o
    snmp_mib2_interfaces.o
    snmp_core.o
    snmp_msg.o
    snmpv3.o
    snmp_raw.o
    snmp_table.o
    snmp_mib2_icmp.o
    snmp_snmpv2_framework.o
    snmp_snmpv2_usm.o
    snmp_mib2_udp.o
    tftp_server.o
    #librws
    rws_common.o
    rws_error.o
    rws_frame.o
    rws_thread.o
    librws.o
    rws_socketpriv.o
    rws_list.o
    rws_memory.o
    rws_string.o
    rws_socketpub.o
  )
  objs=("${objs[@]/ecp.o}")
  objs=("${objs[@]/smtp.o}")

  name=net
  libprefix=${name}/lib/lib${name}-best2600w_aiot
  libs=(${libprefix}-0x1000000.a ${libprefix}-0x2000000.a)

  for lib in ${libs[@]}; do
    echo update lib: ${lib}
    echo ${ar} d -N 2 ${lib} ecp.o
    ${ar} d -N 2 ${lib} ecp.o

    echo ${ar} d -N 2 ${lib} smtp.o
    ${ar} d -N 2 ${lib} smtp.o

    echo ${ar} d ${lib} ${objs[@]}
    ${ar} d ${lib} ${objs[@]}
  done
}

function f_lib_services(){

  objs=(
    transq_msg.o app_tws_ibrt_trace.o 
    communication_svr.o 
    app_ibrt_middleware.o 
    cp_code.o cp_boot.o cp_ipc.o 
    ibrt_mgr_tws_fsm.o ibrt_mgr_mobile_device.o ibrt_mgr_evt_mediator.o ibrt_mgr_evt.o ibrt_mgr.o ibrt_mgr_mobile_thread.o ibrt_mgr_queues.o 
    app_custom_api.o app_custom_config.o 
    app_tws_ibrt.o app_tws_profile_sync.o app_tws_ibrt_mock.o app_vendor_cmd_evt.o app_tws_besaud.o app_tws_ibrt_conn.o cplus_support.o app_tws_ibrt_audio_analysis.o app_tws_ctrl_thread.o app_tws_ibrt_sm.o app_tws_ibrt_queue.o app_ibrt_voice_report.o app_ibrt_bt_profile_sync.o app_tws_ibrt_mobile_conn_sm.o app_tws_ibrt_tws_conn_sm.o app_tws_ibrt_audio_sync.o app_tws_ibrt_cmd_handler.o app_tws_ibrt_trace.ao 
    app_ibrt_rssi.o app_ibrt_ota_cmd.o app_ibrt_nvrecord.o app_ibrt_customif_ui.o app_fpga_test_proxy.o app_ibrt_ota_update.o app_ibrt_search_pair_ui.o app_ibrt_auto_test.o app_ibrt_tws_ext_cmd.o app_ibrt_auto_test_cmd_handle.o app_tws_ibrt_ui_test.o app_ibrt_ble_adv.o app_ibrt_keyboard.o app_ibrt_if.o app_ibrt_customif_cmd.o 
    hsm.o 
    nvrecord.o nvrecord_ble.o nvrecord_env.o list_ext.o nvrecord_fp_account_key.o nvrec_config.o 
    nvrecord_bt.o nvrecord_fp_account_key.o nvrecord_dma_config.o nvrecord_extension.o nvrecord_env.o nvrecord_ble.o 
    factory_section.o factory_efuse.o log_section.o coredump_section.o crash_dump_section.o customparam_section.o aud_section.o 
    sys_time.o 
    app_bap_bis_instance.o app_gaf.o app_gaf_event_callback.o app_bap_bc_scan_msg.o app_bap_uc_srv_msg.o app_bap_capa_cli_msg.o app_bap_capa_srv_msg.o app_bap_data_path_itf.o app_bap_stream.o app_bap.o app_bap_pid.o app_bap_bc_sink_msg.o app_bap_sync.o app_bap_bc_deleg_msg.o app_bap_bc_assist_msg.o app_bap_bc_src_msg.o app_bap_uc_cli_msg.o app_arc_vocs_msg.o app_arc_aics_msg.o app_arc.o app_arc_vcc_msg.o app_arc_vcs_msg.o app_arc_aicc_msg.o app_arc_mics_msg.o app_arc_vocc_msg.o app_arc_micc_msg.o app_acc_otc_msg.o app_acc_mcc_msg.o app_acc_mcs_msg.o app_acc_tbs_msg.o app_acc_ots_msg.o app_acc_tbc_msg.o app_acc.o app_capc_msg.o app_cap.o app_caps_msg.o app_atc_csisc_msg.o app_atc_csism_msg.o app_atc.o 
    datapathps.o datapathps_task.o hrps.o hogpbh.o hogpd.o bass.o diss.o diss_task.o prf_utils.o prf.o csisc.o csism.o otc.o ots.o ota.o gfps_provider.o gfps_provider_task.o buds_task.o buds.o 
    bleadapter_utils.o btadapter_ble_client_task.o btadapter_ble_client.o btadapter_ble_service.o btadapter_ble_service_task.o 
    retention_ram.o 
    app_ble_custom_api.o app_task.o app_ble_rx_handler.o app.o app_ble_param_config.o app_ble_mode_switch.o app_sec.o app_ble_evt_mediator.o app_ble_core.o app_ble_customif.o app_ble_uart.o app_dis.o app_ht.o app_datapath_server.o app_ble_cmd_handler.o app_ble_custom_cmd.o app_hrps.o app_hid.o app_batt.o app_ota.o app_gfps.o 
    data_dump_usb.o data_dump_tota.o audio_dump.o 
    norflash_api.o app_flash_api.o 
    audio_cfg.o audio_process.o audio_user_process.o audio_process_sync.o audio_process_vol.o audio_spectrum.o 
    audioflinger.o 
    hw_limiter_cfg.o hw_filter_codec_iir.o 
    voice_tx_aec.o voice_noise_est.o voice_algo.o voice_dev.o voice_algo_cp.o 
    app_overlay.o 
    resources.o 
    ddbif_bes.o osif_rtx.o 
    pool_api.o multi_heap.o heap_api.o 
    crc16.o crc32.o 
    app_a2dp_codecs.o a2dp_codec_ldac.o a2dp_codec_sbc.o a2dp_codec_aac.o a2dp_codec_scalable.o a2dp_codec_opus.o a2dp_codec_lhdc.o codec_lhdc.o 
    besmain.o app_ring_merge.o app_bt_stream_pcm_player.o app_hfp.o app_bt_hiecho.o app_spp.o app_fp_rfcomm.o app_noise_estimation.o app_rfcomm_mgr.o app_a2dp.o app_pbap.o app_bt_cmd.o app_btgatt.o app_btmap_sms.o app_keyhandle.o app_media_player.o app_bt_hid.o app_bqb_new_profile.o app_bt_stream.o app_bt.o app_bt_func.o app_bt_media_manager.o app_sec.o audio_prompt_sbc.o app_dip.o 
    audio_policy.o 
    dirent.o lfs_adapt_sd_card.o lfs_adapt_flash.o lfs_adapt_ram.o lfs_adapt.o partition_conf.o flash.o lfs_util.o lfs_file_ops.o lfs.o 
    at_spp.o cmd_basic.o # console.o at.o at_printf.o console_parse.o
  )
  name=services
  libprefix=${name}/lib/lib${name}-best2600w_aiot
  libs=(${libprefix}-0x1000000.a ${libprefix}-0x2000000.a)

  for lib in ${libs[@]}; do
  echo update lib: ${lib}
  echo ${ar} d ${lib} ${objs[@]}
  ${ar} d ${lib} ${objs[@]}
  done
}

function f_lib_ble_profiles(){

  objs=(
    datapathps_task.o datapathps.o
  )
  name=ble_profile
  libprefix=services/ble_profiles_v2/lib/lib${name}-best2600w_aiot
  libs=(${libprefix}-0x1000000.a ${libprefix}-0x2000000.a)

  for lib in ${libs[@]}; do
  echo update lib: ${lib}
  echo ${ar} d ${lib} ${objs[@]}
  ${ar} d ${lib} ${objs[@]}
  done
}

f_lib_net

f_lib_services

f_lib_ble_profiles

