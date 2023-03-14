#!/bin/bash


####### each project need to config following toolchain path #########
TOOLCHAIN_9_2_1="/home/toolchain/9.2.1/gcc-arm-none-eabi-9-2019-q4-major/bin"


####### each project need to config project #########

# Set variables according to real environment
# on ubuntu
# same time support how many guys build
let GUYS_BUILD_NUM=5
dev_env=`uname | sed 'y/LINUX/linux/'`
if [ "x$dev_env" == "xlinux" ]; then
    let JOBS=`grep ^processor /proc/cpuinfo|wc -l`
else
    error "Unsupported Build System : $dev_env"
fi
echo -e "machine support JOBS: ${JOBS}"
let JOBS=(JOBS+GUYS_BUILD_NUM-1)/GUYS_BUILD_NUM
echo -e "build in machine use JOBS: ${JOBS}"

# build cmd define
# A7_DSP_BIN_NAME=best2300wp_a7
A7_DSP_BIN_NAME=a7_dsp

#special_buid_src2lib_dir=("services/bt_if_enhanced" "services/bt_profiles_enhanced" "services/ibrt_core_v2" "services/ble_audio_app" "services/custom_api" "services/app_ibrt_v2")
special_buid_src2lib_dir=()

# config psram fs
PSRAMFS_OPT=" PSRAM_XCCELA_MODE=1 PSRAMFS_SIZE=0 MCU_SLAVE_FS_EN=0 "

####### each project need to config project #########
make_boot1="make T=ota_boot1 \
            BOOT2A_OFFSET=0x10000 \
            BOOT2B_OFFSET=0x20000 \
            BOOTINFO_OFFSET=0xFD0000 \
            all lst -j${JOBS}"

make_boot2a="make T=ota_boot2a \
            TZ_MAIN_OFFSET=0x80000 \
            TZ_MINI_OFFSET=0xB00000 \
            BOOTINFO_OFFSET=0xFD0000 \
            all lst -j${JOBS}"

make_boot2b="make T=ota_boot2b \
            TZ_MAIN_OFFSET=0x30000 \
            TZ_MINI_OFFSET=0x56000 \
            BOOTINFO_OFFSET=0xFD0000 \
            all lst -j${JOBS}"

A7_DSP_BIN_NAME=a7_dsp

BUILDOPT_M33="CHIP=best2003 A7_DSP_ENABLE=1 DSP_AUDIO_TASK=1 DSP_CODEC_SUPPORT=0 \
              PSRAMUHS_ENABLE=1 PSRAM_ENABLE=1 SPEECH_MSG_ENABLE=1 \
              SPEECH_MSG_TEST_CASE=0 SPEECH_MSG_TEST_RX=0 SPEECH_MSG_TEST_TX=0 SPEECH_MSG_TEST_TXRX_LOOP=0 A7_PROJECT=dummy\
               ${PSRAMFS_OPT} LLD_PRINT_ENABLE=0 "

make_cmcp="make T=cmcp DEBUG_PORT=4 PSRAMCP_SIZE=0x100000 DUAL_BAND=1 \
           RF_TX_CONTROL_IO=100 \
           WIFI_RF_OUT_FEM_SW=0 \
           WIFI_BT_COEX_FDD=0 \
           WIFI_BT_COEX_HYBRID=0 \
           -j${JOBS} ${PSRAMFS_OPT} \
           -j8 "

make_a7="make T=$A7_DSP_BIN_NAME \
         A7_UART=4 \
         $BUILDOPT_M33 PSRAMCP_SIZE=0x100000 \
         KERNEL=LITEOS_A A7_CMSIS_TEST_CASE=0 \
         OPUS_CODEC_SUPPORT=1 -j${JOBS} all lst"

make_best2600w="make T=best2600w_liteos \
                ARM_CMNS=0 \
                PSRAM_ENABLE=1 \
                NET_JANSSON_SUPPORT=0 \
                NET_CJSON_SUPPORT=0 \
                UTILS_BES_KV=1 \
                RF_TX_CONTROL_IO=100 \
                WIFI_RF_OUT_FEM_SW=0 \
                PA_ENABLE_IO=21 \
                UART1_IOMUX_INDEX=20 \
                WIFI_BT_COEX_FDD=0 \
                WIFI_BT_COEX_HYBRID=0 \
                CP_BOOT=1 LIBC_INIT_AFTER_OS_INIT=1 PSRAMCP_SIZE=0x100000 CP_XIP=$CP_XIP \
                FAST_TIMER_COMPENSATE=1 OTA_CODE_OFFSET=0 \
                NET_MBEDTLS_TESTS_SUPPORT=0 MBEDTLS_FS_IO=0 \
                USB_AUDIO_FS=0 LPLAYER_FRAMEWORK_EN=0 NET_FTP_CLIENT_SUPPORT=0 NET_LWIP_SUPPORT=0 LITTLEFS_ENABLE=0 \
                MP3_CODEC_MINIMP3_DISENABLE=1 \
                $BUILDOPT_M33 DSP_BIN_NAME=$A7_DSP_BIN_NAME -j${JOBS}"

# NO_LIBC=1 LIBC_ADAPT_EN=1 BOARD_OS_WRAP_MALLOC=1 NET_LIBCURL_SUPPORT=1


make_best2600w_mini="make T=best2600w_liteos_mini \
                ARM_CMNS=0 \
                PSRAM_ENABLE=1 \
                NET_JANSSON_SUPPORT=0 \
                NET_CJSON_SUPPORT=0 \
                UTILS_BES_KV=1 \
                RF_TX_CONTROL_IO=12 \
                WIFI_RF_OUT_FEM_SW=0 \
                PA_ENABLE_IO=21 \
                UART1_IOMUX_INDEX=20 \
                AT_CMD=0 LITTLEFS_ENABLE=0 \
                CP_BOOT=1 PSRAMCP_SIZE=0x100000 \
                FAST_TIMER_COMPENSATE=1 OTA_CODE_OFFSET=0xb00000 \
                -j${JOBS}"

make_best2600w_mini_se="make T=best2600w_liteos_mini_se all lst -j${JOBS}"

make_best2600w_se="make T=best2600w_liteos_se all lst -j${JOBS}"


################################ functions list begin #########################
BUILD_INFO_LIST=()
BUILD_INDEX=0;
GLOBAL_OPTS=""
RELEASE_SDK=0

SPACE=" "
EQUEL="="

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
      echo -e "\033[41m[build err]\033[0m ${cmd}\n"
      pwd_cmd=`pwd`
      project_root="$pwd_cmd"
      echo -e "\033[41m[build log]\033[0m $project_root/${3}\n"
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
   log_path="out/log/bsp_build_$1.log"
   rm -rf $log_path
   mkdir -p out/log
   ${build_target} 2>&1 | tee $log_path
   cat $log_path | awk 'END {print}' | grep "Error" > /dev/null
   rc=$?
   # echo "rc = $rc"
   echo "${build_target}" >> $log_path
   build_result build_target $rc $log_path
}

function pre_build_into_lib_cmd()
{
   eval cmd="\${$1}"
   # printf "$build_target \n"

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

   for ((index=0; index<${#special_buid_src2lib_dir[@]}; index++))
   do
      build_target=`echo $cmd`
      pack_lib_dir=${special_buid_src2lib_dir[index]}
      printf "[pack dir] ${pack_lib_dir} \n"
      build_target="$build_target $pack_lib_dir GEN_LIB=1"
      printf "\n[BUILD][$1] $build_target \n"
      ${build_target}
   done
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
      relase_cmd="tools/relsw_$1 $1 WORKING"
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
   mkdir -p out/ota_boot1/
   cp -rf prebuild/ota_boot1.bin out/ota_boot1/
   mkdir -p out/ota_boot2a/
   cp -rf prebuild/ota_boot2a.bin out/ota_boot2a/
else
   echo "~~~~~ clear bsp all now ~~~~~~"
   rm -rf out/cmcp
   rm -rf out/${A7_DSP_BIN_NAME}
   rm -rf out/best2600w_aiot
   # make ota boot
   rm -rf out/ota_boot1
   rm -rf out/ota_boot2a
   # rm -rf out/ota_boot2b
   build_cmd make_boot1
   build_cmd make_boot2a
   # build_cmd make_boot2b
   mkdir -p prebuild/
   cp -rf out/ota_boot1/ota_boot1.bin prebuild/
   cp -rf out/ota_boot2a/ota_boot2a.bin prebuild/
fi


# clear out/
rm -rf out/cmcp
rm -rf out/${A7_DSP_BIN_NAME}
rm -rf out/best2600w_liteos
rm -rf out/best2600w_liteos_se
rm -rf out/best2600w_liteos_mini
rm -rf out/best2600w_liteos_mini_se
rm -rf out/prebuild/
mkdir -p out/prebuild/


# make_a7
build_cmd make_a7 TOOLCHAIN_9_2_1

# compress dsp bin
tools/lzma e out/$A7_DSP_BIN_NAME/$A7_DSP_BIN_NAME.bin out/$A7_DSP_BIN_NAME/$A7_DSP_BIN_NAME.bin.lzma
mv out/$A7_DSP_BIN_NAME/$A7_DSP_BIN_NAME.bin.lzma out/$A7_DSP_BIN_NAME/$A7_DSP_BIN_NAME.bin

# mklittlefs
FS_BIN_NAME="littlefs"
./tools/mklittlefs -c apps/bes_test/fs -d 5 -b 4096 -s 1048576 out/$A7_DSP_BIN_NAME/$FS_BIN_NAME.bin
# compress fs bin
tools/lzma e out/$A7_DSP_BIN_NAME/$FS_BIN_NAME.bin out/$A7_DSP_BIN_NAME/$FS_BIN_NAME.bin.lzma
mv out/$A7_DSP_BIN_NAME/$FS_BIN_NAME.bin.lzma out/$A7_DSP_BIN_NAME/$FS_BIN_NAME.bin

if [ "x$BUILD_SDK" == "x1" ]; then
   # use prebuild cp bin
   echo "=============mkdir out for cmcp==============="
   mkdir -p out/cmcp/
   cp prebuild/cmcp.bin out/cmcp/cmcp.bin
   ls out/cmcp/
   echo "=============mkdir out for cmcp==============="
else
#build cp
   build_cmd make_cmcp
# compress cp bin
   tools/lzma e out/cmcp/cmcp.bin out/cmcp/cmcp.bin.lzma
   if [ "x$RELEASE_SDK" == "x1" ]; then
      mv out/cmcp/cmcp.bin.lzma prebuild/cmcp_16.bin

      make_cmcp_32="${make_cmcp} PSRAM_XCCELA_MODE=1"
      build_cmd make_cmcp_32
      tools/lzma e out/cmcp/cmcp.bin out/cmcp/cmcp.bin.lzma
      mv out/cmcp/cmcp.bin.lzma prebuild/cmcp_32.bin
   else
      mv out/cmcp/cmcp.bin.lzma out/cmcp/cmcp.bin
   fi
fi

# compress prebuild bins
# bt ramrun
cp platform/drivers/bt/best2003/best2003_ramrun_fw.bin out/prebuild/
tools/lzma e out/prebuild/best2003_ramrun_fw.bin out/prebuild/best2003_ramrun_fw.lzma
mv out/prebuild/best2003_ramrun_fw.lzma out/prebuild/best2003_ramrun_fw.bin

# make_best2600w TrustZone App
# build_cmd make_best2600w_se

# make_best2600w TrustZone App
# build_cmd make_best2600w_mini_se

# make_best2600w_mini
# pre_build_into_lib_cmd make_best2600w_mini
# build_cmd make_best2600w_mini

# make_best2600w
# pre_build_into_lib_cmd make_best2600w
build_cmd make_best2600w

# release script
realse_sdk best2600w_ohos

# print all make cmds
print_all_cmds

