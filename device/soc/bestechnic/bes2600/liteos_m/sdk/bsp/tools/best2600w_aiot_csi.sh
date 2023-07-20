#!/bin/bash

############# CAMERA TEST OPTIONS ###############
CSI_DSI_LOOP_V=0
no_clr=0
no_sleep=0
debug_port=1
################################################

A7_PROJECT_PARAM=dummy
MCU_PROJECT_PARAM=$A7_PROJECT_PARAM
MIC_SAMPLERATE_PARAM=16000
MIC_PERIOD_MS_PARAM=64
AQE_KWS_PARAM=0
ALI_KWS_PARAM=0
WEBRTC_VAD_PARAM=0
if [ "$AQE_KWS_PARAM$ALI_KWS_PARAM$WEBRTC_VAD_PARAM" != "000" ]; then
   MCU_PROJECT_PARAM=kws
   if [ $AQE_KWS_PARAM == 1 ]; then
      A7_PROJECT_PARAM=kws
      MIC_PERIOD_MS_PARAM=16
   else
      MIC_PERIOD_MS_PARAM=32
   fi
fi

MCU_LZMA_SRAM_PARAM=0
MCU_LZMA_PSRAM_PARAM=0
MCU_LZMA_CP_TEXT_SRAM=0
MCU_LZMA_CP_DATA_SRAM=0
MCU_LZMA_FAST_SRAM=0

####### each project need to config following toolchain path #########
TOOLCHAIN_9_2_1="/home/toolchain/9.2.1/gcc-arm-none-eabi-9-2019-q4-major/bin"
TOOLCHAIN_5_4_1="/home/toolchain/ali_5.4.1/gcc-arm-none-eabi/Linux64/bin"

####### FLASH TYPE & SECURE_TYPE #########
SECURE_TYPE=0
FLA_SIZE=0x1000000
CP_XIP=0
A7_DSP_V=1
BWIFI_DISABLE_V=1
PSRAM_XCCELA_MODE=0
RF_TX_CONTROL_IO_PARAM=16
PSRAMUHS_AP325608A=0
PSRAMULS_MODE=0
ALT_BOOT_FLASH=0
for arg in $*
do
   if [ $arg == "sign=1" ];then
      SECURE_TYPE=1
   fi
   if [ $arg == "cpxip=1" ];then
      CP_XIP=1
   fi
   if [ $arg == "CSI_DSI_LOOP=1" ];then
      CSI_DSI_LOOP_V=1
   fi
   if [ $arg == "AX4D=1" ];then
      PSRAM_XCCELA_MODE=0
      PSRAMUHS_AP325608A=0
      PSRAMULS_MODE=0
   fi
   if [ $arg == "AX4C=1" ];then
      PSRAMULS_MODE=1
   fi
   if [ $arg == "GX6D=1" ];then
      PSRAM_XCCELA_MODE=1
      RF_TX_CONTROL_IO_PARAM=100
      FLA_SIZE=0x2000000
   fi
   if [ $arg == "NX6D=1" ];then
      PSRAM_XCCELA_MODE=1
      RF_TX_CONTROL_IO_PARAM=100
   fi
   if [ $arg == "AX4F=1" ];then
      PSRAMUHS_AP325608A=1
      ALT_BOOT_FLASH=1
      RF_TX_CONTROL_IO_PARAM=100
   fi
   if [ $arg == "NX4F=1" ];then
      PSRAMUHS_AP325608A=1
   fi
   if [ $arg == "GX4F=1" ];then
      PSRAM_XCCELA_MODE=1
      RF_TX_CONTROL_IO_PARAM=100
      PSRAMUHS_AP325608A=1
      FLA_SIZE=0x2000000
   fi
done

RTOS_A_CODE_OFFSET=0
RTOS_B_CODE_OFFSET=0x6B8000
if [ $FLA_SIZE == "0x2000000" ];then
    RTOS_B_CODE_OFFSET=0xC38000
    echo "=====32m flash====="
fi

echo "FLA_SIZE=$FLA_SIZE"
echo "SECURE_TYPE=$SECURE_TYPE"
echo "RTOS_A_CODE_OFFSET=$RTOS_A_CODE_OFFSET"
echo "RTOS_B_CODE_OFFSET=$RTOS_B_CODE_OFFSET"
echo "ALT_BOOT_FLASH=$ALT_BOOT_FLASH"

FS_SIZE="0"
# config psram fs
PSRAMFS_OPT=" PSRAM_XCCELA_MODE=$PSRAM_XCCELA_MODE PSRAMULS_MODE=$PSRAMULS_MODE PSRAMUHS_AP325608A=$PSRAMUHS_AP325608A PSRAMFS_SIZE=${FS_SIZE} MCU_SLAVE_FS_EN=0 "

####### each project need to config project #########
if [ "$MCU_PROJECT_PARAM" == "kws" ]; then
   A7_PROJECT_PARAM=genie
   A7_DSP_BIN_NAME=best2600w_a7
else
   A7_DSP_BIN_NAME=a7_dsp
fi

M33_BIN_NAME=best2600w_aiot
M33_BIN_NAME_text_psram=best2600w_aiot_psram
M33_BIN_NAME_text_sram=best2600w_aiot_sram
M33_BIN_NAME_text_cpsram=best2600w_aiot_text_cpsram
M33_BIN_NAME_data_cpsram=best2600w_aiot_data_cpsram
M33_BIN_NAME_text_fasram=best2600w_aiot_fasram

CMCP_BIN_NAME="cmcp_$FLA_SIZE.bin"

BUILDOPT_M33="CHIP=best2003 A7_DSP_ENABLE=$A7_DSP_V DSP_AUDIO_TASK=1 DSP_CODEC_SUPPORT=0 SECURE_BOOT=$SECURE_TYPE \
              PSRAMUHS_ENABLE=1 PSRAM_ENABLE=1 SPEECH_MSG_ENABLE=1  \
              MIC_SAMPLERATE=$MIC_SAMPLERATE_PARAM MIC_PERIOD_MS=$MIC_PERIOD_MS_PARAM \
              SPEECH_MSG_TEST_CASE=0 SPEECH_MSG_TEST_RX=0 SPEECH_MSG_TEST_TX=0 SPEECH_MSG_TEST_TXRX_LOOP=0 \
              FLASH_SIZE=$FLA_SIZE ${PSRAMFS_OPT} FORCE_TO_USE_TARGET_LIB=1"

make_cmcp="make T=cmcp DEBUG_PORT=4 PSRAMCP_SIZE=0x000000 DUAL_BAND=1 \
           KERNEL=LITEOS_M  FLASH_SIZE=$FLA_SIZE \
           SECURE_BOOT=$SECURE_TYPE \
           RF_TX_CONTROL_IO=$RF_TX_CONTROL_IO_PARAM \
           WIFI_RF_OUT_FEM_SW=0 \
           WIFI_BT_COEX_FDD=0 \
           WIFI_BT_COEX_HYBRID=0 \
           WIFI_DRV_KEY_FRAME_FILTER=1 \
           CP_XIP=$CP_XIP ${PSRAMFS_OPT} FORCE_TO_USE_TARGET_LIB=1\
           -j8"

make_a7="make T=$A7_DSP_BIN_NAME \
         A7_UART=4 \
         CSI_DSI_LOOP=${CSI_DSI_LOOP_V} CSI_DSI_CONFIG=${CSI_DSI_LOOP_V} \
         $BUILDOPT_M33 A7_PROJECT=$A7_PROJECT_PARAM \
         KERNEL=LITEOS_A A7_CMSIS_TEST_CASE=0 LITTLEFS_ENABLE=0 A7_LITTLEFS_TEST=0 LFS_ON_RAM=0 \
         -j8 lst all"

make_best2600w="make T=$M33_BIN_NAME \
                KERNEL=FREERTOS \
                NO_SLEEP=${no_sleep} DEBUG_PORT=${debug_port} \
                CSI_DSI_LOOP=${CSI_DSI_LOOP_V} CSI_DSI_CONFIG=${CSI_DSI_LOOP_V} \
                AP_MODE_SUPPORT=0 \
                WPA3_SUPPORT=0 \
                IEEE80211W_SUPPORT=0 \
                SNIFFER_MODE_SUPPORT=0 \
                UART_CPUOCCPUT_TEST=0 \
                NET_JANSSON_SUPPORT=0 \
                NET_CJSON_SUPPORT=0 \
                UTILS_BES_KV=1 \
                RF_TX_CONTROL_IO=$RF_TX_CONTROL_IO_PARAM \
                WIFI_RF_OUT_FEM_SW=0 \
                PA_ENABLE_IO=21 \
                WIFI_BT_COEX_FDD=0 \
                WIFI_BT_COEX_HYBRID=0 \
                UART1_IOMUX_INDEX=32 \
                DCOMMS=1 DCOMMC=1 \
                NO_ADCKEY=1 BWIFI_DISABLE=$BWIFI_DISABLE_V \
                RF_CALIB_ONLY_BT=1 \
                NO_PWRKEY=0 APP_SLEEP_ENABLE=1 A7_HEART_BEAT_TIMEOUT=10000 \
                WIFI_ON_CP=0 WIFI_NO_IPC=0 WIFI_POWERSAVE=1 LWIP_NO_TIMER=0 \
                SLEEP_STATS_TRACE=1 DEBUG_SLEEP_USER=1 WAKEUP_SRC_STATS=1 DMA_DEBUG=1 \
                BT_RAMRUN=0 A2DP_LC3_ON=0 BLE_AUDIO_ENABLED=0 BIS_VOL_SYNC_ENABLED=0 INTERSYS_DEBUG=0\
                CP_BOOT=0 PSRAMCP_SIZE=0x000000 CP_XIP=$CP_XIP \
                FAST_TIMER_COMPENSATE=1 OTA_CODE_OFFSET=$RTOS_A_CODE_OFFSET \
                NET_MBEDTLS_TESTS_SUPPORT=1 MBEDTLS_FS_IO=1 \
                LITTLEFS_PARTITION_SUPPORT=1 A7_LITTLEFS_TEST=0 LFS_ON_RAM=0\
                MCU_LZMA_SRAM=$MCU_LZMA_SRAM_PARAM MCU_LZMA_PSRAM=$MCU_LZMA_PSRAM_PARAM \
                MCU_LZMA_CP_TEXT=$MCU_LZMA_CP_TEXT_SRAM MCU_LZMA_CP_DATA=$MCU_LZMA_CP_DATA_SRAM \
                MCU_LZMA_FASRAM=$MCU_LZMA_FAST_SRAM \
                ALT_BOOT_FLASH=$ALT_BOOT_FLASH $BUILDOPT_M33 DSP_BIN_NAME=$A7_DSP_BIN_NAME A7_PROJECT=$MCU_PROJECT_PARAM DUAL_BAND=1 -j8"


BUILDOPT_BOOT="ALT_BOOT_FLASH=$ALT_BOOT_FLASH PSRAM_XCCELA_MODE=$PSRAM_XCCELA_MODE PSRAMULS_MODE=$PSRAMULS_MODE PSRAMUHS_AP325608A=$PSRAMUHS_AP325608A"

make_boot1="make T=ota_boot1 REMAP_SUPPORT=1 FLASH_SIZE=$FLA_SIZE SECURE_BOOT=$SECURE_TYPE ${BUILDOPT_BOOT} \
            all lst -j8"

make_boot2a="make T=ota_boot2a REMAP_SUPPORT=1 FLASH_SIZE=$FLA_SIZE SECURE_BOOT=$SECURE_TYPE ${BUILDOPT_BOOT} \
            RTOS_A_CODE_OFFSET=$RTOS_A_CODE_OFFSET RTOS_B_CODE_OFFSET=$RTOS_B_CODE_OFFSET \
            all lst -j8"
################################ functions list begin #########################
BUILD_INFO_LIST=()
BUILD_INDEX=0;
GLOBAL_OPTS=""
RELEASE_SDK=0

SPACE=" "
EQUEL="="

# Set variables according to real environment
# on ubuntu
dev_env=`uname | sed 'y/LINUX/linux/'`
if [ "x$dev_env" == "xlinux" ]; then
    JOBS=`grep ^processor /proc/cpuinfo|wc -l`
else
    error "Unsupported Build System : $dev_env"
fi

error () {
    set +u
    echo "$0: error: $@" >&2
    exit 1
    set -u
}

build_cmd_echo="\n"
tmp_path=$PATH
function build_result()
{
   eval cmd="\${$1}"
   cmd=`echo $cmd`

   toolchain_path=`which arm-none-eabi-gcc`
   toolchain_info="\n=== build with toolchain [$toolchain_path] ===\n"
   PATH=$tmp_path
   if [ "x$2" != "x1" ];then
      echo -e "$toolchain_info"
      echo -e "\033[41m[build err]\033[0m ${cmd}"
      exit 1
   fi
   build_cmd_echo+=$toolchain_info
   build_cmd_echo+="${cmd}\n\n"
}

#pre-handle option of every target,for now GEN_LIB and SDK
function pre_handle_global_opt()
{
   # printf "pre_handle_global_opt $1 \n"
   GLOBAL_OPTS="$SPACE$1"
}

#pre-handle options of target
function pre_handle_opt()
{
   eval cmd="\${$1}"
   cmd=`echo $cmd`
   # printf "\n[$1] $cmd\n"

   for input_opt in $*
   do
      #strip first para
      if [ "x${1}" = "x${input_opt}" ] ;then
         continue
      fi
      # parse key value by =
      input_opt_key=${input_opt%%=*}

      temp_cmd=""
      flag_match=0
      for cmd_opt in $cmd
      do
         # parse key value by =
         cmd_opt_key=${cmd_opt%%=*}

         # cover same parameter by check key
         if [ "x${input_opt_key}" = "x${cmd_opt_key}" ] ;then
            printf "[$1][opt] ${cmd_opt} covered by ${input_opt}\n"
            temp_cmd+="$input_opt$SPACE"
            flag_match=1
            continue
         fi

         # keep current parameter not matched
         temp_cmd+="$cmd_opt$SPACE"
      done

      # handle no matched parameters
      if [ "x${flag_match}" = "x0" ] ;then
         printf "[$1][opt] target add ${input_opt}\n"
         temp_cmd+="$input_opt$SPACE"
      fi
      # printf "[temp_cmd] ${temp_cmd} \n"
      cmd=$temp_cmd
   done
   # printf "[cmd] ${cmd} \n"
   BUILD_INFO_LIST[BUILD_INDEX]="$1,${cmd}"
   printf "[GEN] ${BUILD_INFO_LIST[BUILD_INDEX]}\n"
   let BUILD_INDEX=BUILD_INDEX+1
}

function pre_handle_relase()
{
   # printf "pre_handle_relase\n "
   RELEASE_SDK=1
}

function build_cmd()
{
   eval cmd="\${$1}"
   build_target=`echo $cmd`
   # printf "$build_target \n"

   for ((index=0; index<$BUILD_INDEX; index++))
   do
      kv_cmd=${BUILD_INFO_LIST[index]}
      OLD_IFS="$IFS"
      IFS=","
      arr_cmd=(${kv_cmd})
      IFS="$OLD_IFS"
      if [ "x${arr_cmd[0]}" = "x$1" ] ;then
         build_target=${arr_cmd[1]}
         break;
      fi
   done
   build_target="$build_target$GLOBAL_OPTS"
   printf "\n[BUILD][$1] $build_target \n"

   ### config toolchain ###
   if [ "x$2" = "x" ] ;then
      if [ -d $M33_TOOLCHAIN ] ;then
         export PATH=$M33_TOOLCHAIN:$PATH
      fi
   else
      eval toolchain="\${$2}"
      # printf "\ntoolchain : $toolchain \n"
      if [ -d $toolchain ]; then
         export PATH=$toolchain:$PATH
      fi
   fi
   rm -rf out/log/bsp_build_$1.log
   mkdir -p out/log
   ${build_target} 2>&1 | tee out/log/bsp_build_$1.log
   cat out/log/bsp_build_$1.log | awk 'END {print}' | grep "Error" > /dev/null
   rc=$?
   # echo "rc = $rc"
   echo "${build_target}" >> out/log/bsp_build_$1.log
   build_result build_target $rc
}

function print_all_cmds()
{
   echo -e "\n~~~ build success! show all build command list ~~~\n"
   echo -e "${build_cmd_echo}"
   echo -e "~~~ build success! show all build command over  ~~~\n"
}

function realse_sdk()
{
   if [ "x$RELEASE_SDK" == "x1" ]; then
      relase_cmd="tools/relsw_$1 $1 WORKING $CMCP_BIN_NAME"
      echo -e "[realse_sdk] $relase_cmd\n"
      $relase_cmd
   fi
}

usage ()
{
cat<<EOF
Usage:

[       -c,     -a,     -m,    -s,       -l,        -x,         -y,         -z,     -h,        -r ]
[  --cmcp=,  --a7=, --m33=, --se=, --lmini=, --xboot1=, --yboot2a=, --zboot2b=, --help, --release ]

EOF
exit 1
}
################################ functions list end   #########################


# total_param=$@
GETOPT_ARGS=`getopt -o c:a:m:s:l:x:y:z:hr -al cmcp:,a7:,m33:,se:,lmini:,xboot1:,yboot2a:,zboot2b:,help,release -- "$@"`

eval set -- "$GETOPT_ARGS"

while [ -n "$1" ]
do
   # printf "1:$1 2:$2 total_param:$total_param\n"
    case "$1" in
        -c|--cmcp)      pre_handle_opt make_cmcp $2;              shift 2;;
        -a|--a7)        pre_handle_opt make_a7  $2;               shift 2;;
        -m|--m33)       pre_handle_opt make_best2600w $2;         shift 2;;
        -s|--se)        pre_handle_opt make_best2600w_se $2;      shift 2;;
        -l|--lmini)     pre_handle_opt make_best2600w_mini $2;    shift 2;;
        -x|--xboot1)    pre_handle_opt make_boot1 $2;             shift 2;;
        -y|--yboot2a)   pre_handle_opt make_boot2a $2;            shift 2;;
        -z|--zboot2b)   pre_handle_opt make_boot2b $2;            shift 2;;
        -h|--help)      usage;                                    shift 1;;
        -r|--realse)    pre_handle_relase;                        shift 1;;
        --)             pre_handle_global_opt $2;break;                  ;;
        *)  break ;;
    esac
done


# for fit build increate
BUILD_SDK=0
for arg in $*
do
   if [ $arg == "SDK=1" ];then
      BUILD_SDK=1
      echo "find SDK = 1"
   fi
done
if [ $arg == "SDK=1" ];then
   echo "~~~~~ not clear bsp, decide in app script ~~~~~"
else
   echo "~~~~~ clear bsp all now ~~~~~~"
   rm -rf out/cmcp
   rm -rf out/${A7_DSP_BIN_NAME}
   rm -rf out/${M33_BIN_NAME}
fi

# clear out/
rm -rf out/ota_boot1
rm -rf out/ota_boot2a
rm -rf out/ota_boot2b
rm -rf out/cmcp
rm -rf out/${A7_DSP_BIN_NAME}
rm -rf out/${M33_BIN_NAME}
rm -rf out/best2600w_aiot_se
rm -rf out/best2600w_aiot_mini
rm -rf out/best2600w_aiot_mini_se
rm -rf out/prebuild

mkdir -p out/prebuild

# make ota boot
# build_cmd make_boot1
# build_cmd make_boot2a
# build_cmd make_boot2b

if [ "x${A7_DSP_V}" == "x1" ] ;then
   # make_a7
   if [ "$MCU_PROJECT_PARAM" == "kws" ]; then
      make_a7="make T=best2600w_a7 SDK=1 VOICE_DETECTOR_EN=1 LIB_BIN_IN_SRC_DIR=1 KERNEL=RHINO A7_ALI_LIB=1 PSRAMUHS_ENABLE=1 CHIP=best2003 A7_DSP_ENABLE=1 DSP_AUDIO_TASK=1 DSP_CODEC_SUPPORT=0 A7_SRAM=0 A7_UART=4 PSRAMUHS_ENABLE=1 PSRAM_ENABLE=0 SPEECH_MSG_ENABLE=1 MCU_DSP=0 SPEECH_MSG_TEST_CASE=0 SPEECH_MSG_TEST_RX=0 SPEECH_MSG_TEST_TX=0 SPEECH_MSG_TEST_TXRX_LOOP=0 HW_MODULE=0 A7_HEARTBEAT_TIMEOUT=15000 A7_PROJECT=genie A7_CMSIS_TEST_CASE=0 -j8"
      build_cmd make_a7 TOOLCHAIN_5_4_1
   else
      build_cmd make_a7 TOOLCHAIN_9_2_1
   fi

   # compress dsp bin
   tools/lzma e out/$A7_DSP_BIN_NAME/$A7_DSP_BIN_NAME.bin out/$A7_DSP_BIN_NAME/$A7_DSP_BIN_NAME.bin.lzma
   mv out/$A7_DSP_BIN_NAME/$A7_DSP_BIN_NAME.bin.lzma out/$A7_DSP_BIN_NAME/$A7_DSP_BIN_NAME.bin

   # mklittlefs
   FS_BIN_NAME="littlefs"
   let FS_BIN_SIZE=${FS_SIZE}
   ./tools/mklittlefs -c apps/bes_test/fs -d 5 -b 4096 -s $FS_BIN_SIZE out/$A7_DSP_BIN_NAME/$FS_BIN_NAME.bin
   # compress fs bin
   tools/lzma e out/$A7_DSP_BIN_NAME/$FS_BIN_NAME.bin out/$A7_DSP_BIN_NAME/$FS_BIN_NAME.bin.lzma
   mv out/$A7_DSP_BIN_NAME/$FS_BIN_NAME.bin.lzma out/$A7_DSP_BIN_NAME/$FS_BIN_NAME.bin
fi

# if [ "x$BUILD_SDK" == "x1" ]; then
#    # use prebuild cp bin
#    echo "=============mkdir out for cmcp==============="
#    mkdir -p out/cmcp/
#   cp prebuild/$CMCP_BIN_NAME out/cmcp/cmcp.bin
#    ls out/cmcp/
#    echo "=============mkdir out for cmcp==============="
# else
#    # build cp
#    if [ "$MCU_PROJECT_PARAM" == "kws" ]; then
#       make_cmcp=$make_cmcp" CODEC_VAD_CFG_BUF_SIZE=0x8000 "
#    fi
#    build_cmd make_cmcp TOOLCHAIN_9_2_1
#    # compress cp bin
# if [ "x$CP_XIP" == "x0" ]; then
#    tools/lzma e out/cmcp/cmcp.bin out/cmcp/cmcp.bin.lzma
#    mv out/cmcp/cmcp.bin.lzma out/cmcp/cmcp.bin
# fi
# fi

# compress prebuild bins
# bt ramrun
cp platform/drivers/bt/best2003/best2003_ramrun_fw.bin out/prebuild/.
tools/lzma e out/prebuild/best2003_ramrun_fw.bin out/prebuild/best2003_ramrun_fw.lzma
mv out/prebuild/best2003_ramrun_fw.lzma out/prebuild/best2003_ramrun_fw.bin

# make_best2600w TrustZone App
# build_cmd make_best2600w_se

# make_best2600w TrustZone App
# build_cmd make_best2600w_mini_se

# make_best2600w_mini
# build_cmd make_best2600w_mini

# make_best2600w
if [ "$MCU_PROJECT_PARAM" == "kws" ]; then
   make_best2600w=$make_best2600w" MCU_DSP_SEPERATE=1 AQE_KWS=$AQE_KWS_PARAM ALI_KWS=$ALI_KWS_PARAM WEBRTC_VAD=$WEBRTC_VAD_PARAM VOICE_DETECTOR_EN=1 DEBUG_SLEEP_USER=1 WAKEUP_SRC_STATS=1 CODEC_VAD_CFG_BUF_SIZE=0x8000 "
fi

# compress m33 bin partly
if [ "$MCU_LZMA_SRAM_PARAM$MCU_LZMA_PSRAM_PARAM$MCU_LZMA_CP_TEXT_SRAM$MCU_LZMA_CP_DATA_SRAM$MCU_LZMA_FAST_SRAM" != "00000" ];then
   make_best2600w=$make_best2600w" MCU_BIN_NAME_SRAM=$M33_BIN_NAME_text_sram MCU_BIN_NAME_PSRAM=$M33_BIN_NAME_text_psram MCU_BIN_NAME_TEXT_CPSRAM=$M33_BIN_NAME_text_cpsram MCU_BIN_NAME_DATA_CPSRAM=$M33_BIN_NAME_data_cpsram MCU_BIN_NAME_FASRAM=$M33_BIN_NAME_text_fasram"
   # stub bin for 1st build
   mkdir out/$M33_BIN_NAME
   touch out/$M33_BIN_NAME/$M33_BIN_NAME_text_sram.bin
   touch out/$M33_BIN_NAME/$M33_BIN_NAME_text_psram.bin
   touch out/$M33_BIN_NAME/$M33_BIN_NAME_text_cpsram.bin
   touch out/$M33_BIN_NAME/$M33_BIN_NAME_data_cpsram.bin
   touch out/$M33_BIN_NAME/$M33_BIN_NAME_text_fasram.bin
   # 1st build
   build_cmd make_best2600w TOOLCHAIN_9_2_1

   # gen lzma bin
   if [ "$MCU_LZMA_SRAM_PARAM" == "1" ];then
      arm-none-eabi-objcopy -j .sram_text -j .sram_data -O binary out/$M33_BIN_NAME/$M33_BIN_NAME.elf out/$M33_BIN_NAME/$M33_BIN_NAME_text_sram.bin
      cp out/$M33_BIN_NAME/$M33_BIN_NAME_text_sram.bin out/$M33_BIN_NAME/$M33_BIN_NAME_text_sram.bin.bk
      tools/lzma e out/$M33_BIN_NAME/$M33_BIN_NAME_text_sram.bin out/$M33_BIN_NAME/$M33_BIN_NAME_text_sram.bin.lzma
      mv out/$M33_BIN_NAME/$M33_BIN_NAME_text_sram.bin.lzma out/$M33_BIN_NAME/$M33_BIN_NAME_text_sram.bin
   fi
   if [ "$MCU_LZMA_PSRAM_PARAM" == "1" ];then
      arm-none-eabi-objcopy -j .psram_text -j .psram_data -O binary out/$M33_BIN_NAME/$M33_BIN_NAME.elf out/$M33_BIN_NAME/$M33_BIN_NAME_text_psram.bin
      cp out/$M33_BIN_NAME/$M33_BIN_NAME_text_psram.bin out/$M33_BIN_NAME/$M33_BIN_NAME_text_psram.bin.bk
      tools/lzma e out/$M33_BIN_NAME/$M33_BIN_NAME_text_psram.bin out/$M33_BIN_NAME/$M33_BIN_NAME_text_psram.bin.lzma
      mv out/$M33_BIN_NAME/$M33_BIN_NAME_text_psram.bin.lzma out/$M33_BIN_NAME/$M33_BIN_NAME_text_psram.bin
   fi
   if [ "$MCU_LZMA_CP_TEXT_SRAM" == "1" ];then
      arm-none-eabi-objcopy -j .cp_text_sram -O binary out/$M33_BIN_NAME/$M33_BIN_NAME.elf out/$M33_BIN_NAME/$M33_BIN_NAME_text_cpsram.bin
      cp out/$M33_BIN_NAME/$M33_BIN_NAME_text_cpsram.bin out/$M33_BIN_NAME/$M33_BIN_NAME_text_cpsram.bin.bk
      tools/lzma e out/$M33_BIN_NAME/$M33_BIN_NAME_text_cpsram.bin out/$M33_BIN_NAME/$M33_BIN_NAME_text_cpsram.bin.lzma
      mv out/$M33_BIN_NAME/$M33_BIN_NAME_text_cpsram.bin.lzma out/$M33_BIN_NAME/$M33_BIN_NAME_text_cpsram.bin
   fi
   if [ "$MCU_LZMA_CP_DATA_SRAM" == "1" ];then
      arm-none-eabi-objcopy -j .cp_data_sram -O binary out/$M33_BIN_NAME/$M33_BIN_NAME.elf out/$M33_BIN_NAME/$M33_BIN_NAME_data_cpsram.bin
      cp out/$M33_BIN_NAME/$M33_BIN_NAME_data_cpsram.bin out/$M33_BIN_NAME/$M33_BIN_NAME_data_cpsram.bin.bk
      tools/lzma e out/$M33_BIN_NAME/$M33_BIN_NAME_data_cpsram.bin out/$M33_BIN_NAME/$M33_BIN_NAME_data_cpsram.bin.lzma
      mv out/$M33_BIN_NAME/$M33_BIN_NAME_data_cpsram.bin.lzma out/$M33_BIN_NAME/$M33_BIN_NAME_data_cpsram.bin
   fi
   if [ "$MCU_LZMA_FAST_SRAM" == "1" ];then
      arm-none-eabi-objcopy -j .fast_text_sram -O binary out/$M33_BIN_NAME/$M33_BIN_NAME.elf out/$M33_BIN_NAME/$M33_BIN_NAME_text_fasram.bin
      cp out/$M33_BIN_NAME/$M33_BIN_NAME_text_fasram.bin out/$M33_BIN_NAME/$M33_BIN_NAME_text_fasram.bin.bk
      tools/lzma e out/$M33_BIN_NAME/$M33_BIN_NAME_text_fasram.bin out/$M33_BIN_NAME/$M33_BIN_NAME_text_fasram.bin.lzma
      mv out/$M33_BIN_NAME/$M33_BIN_NAME_text_fasram.bin.lzma out/$M33_BIN_NAME/$M33_BIN_NAME_text_fasram.bin
   fi

   # pack bin into image
   rm -rf out/${M33_BIN_NAME}/platform/main/main_comp.o
   build_cmd make_best2600w TOOLCHAIN_9_2_1
else
   build_cmd make_best2600w TOOLCHAIN_9_2_1
fi

# sign_write_tool
FLASH_SEL=16m_flash
SECURE_SEL=nosec
if [ $FLA_SIZE == "0x2000000" ];then
   FLASH_SEL=32m_flash
fi
if [ $SECURE_TYPE == "1" ];then
   SECURE_SEL=sec
fi

# make ota boot
build_cmd make_boot1
build_cmd make_boot2a
# build_cmd make_boot2b

# print all make cmds
print_all_cmds


if [ -d "tools/sign_write_tool/" ];then
    #update ota bin
    echo -e "\n~~~ copy ota boot bin to sign_write_tool start ~~~\n"
    cp -f out/ota_boot1/ota_boot1.bin tools/sign_write_tool/prebuild/$FLASH_SEL/$SECURE_SEL/
    cp -f out/ota_boot2a/ota_boot2a.bin tools/sign_write_tool/prebuild/$FLASH_SEL/$SECURE_SEL/
    echo -e "ls tools/sign_write_tool/prebuild/$FLASH_SEL/$SECURE_SEL/"
    ls tools/sign_write_tool/prebuild/$FLASH_SEL/$SECURE_SEL/
    echo -e "\n~~~ copy ota boot bin to sign_write_tool over ~~~\n"

    cd tools/sign_write_tool/
    if [ -d "prebuild/$FLASH_SEL/$SECURE_SEL/" ];then
        cp -f prebuild/$FLASH_SEL/$SECURE_SEL/*.bin ./prebuild/
        cp -f prebuild/$FLASH_SEL/$SECURE_SEL/bes_sign_cfg.yaml ./release/auto_build_tool/
        cp -f prebuild/$FLASH_SEL/bes_dld_cfg_best2600w.yaml ./release/write_flash_gui/dld_cfg/
    fi

    cd release/auto_build_tool/
    python3 bes_genbin.py best2003 $FLASH_SEL

    cd ../../../..
    echo "$FLASH_SEL $SECURE_SEL"
    if [ -f "tools/sign_write_tool/release/write_flash_gui_best2600w_$FLASH_SEL.tar.gz" ];then
        mv tools/sign_write_tool/release/write_flash_gui_best2600w_$FLASH_SEL.tar.gz out/best2600w_aiot/
    fi
fi

# release script
realse_sdk best2600w_aiot_csi
