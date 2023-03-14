#!/bin/bash
# Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# == area config a7 liteos bin begin == #
OPT_LITEOS_A7=" \
OPUS_CODEC_SUPPORT=0\
"
# == area config a7 liteos bin end   == #

# == area config liteos main bin begin == #
OPT_LITEOS_MAIN="              \
LIBC_INIT_AFTER_OS_INIT=1      \
LPLAYER_FRAMEWORK_EN=1         \
NET_LIBCURL_SUPPORT=1          \
NET_MBEDTLS_SUPPORT=1          \
HTTP_MEDIA_SUPPORT_HTTPS=1     \
MP3_CODEC_MINIMP3_DISENABLE=0  \
HISTREAMER_RENDER_SUPPORT=1    \
CONFIG_DISPLAY_A064=1          \
"
# == area config liteos main bin end   == #

# == area config liteos cp bin begin == #
OPT_LITEOS_CP=" \
CP_OS_HEAP_SIZE=0x50000 \
"
# == area config liteos cp bin end   == #


# == area config liteos main mini bin begin == #
OPT_LITEOS_MAIN_MINI_SE=" \
"
# == area config liteos main mini bin end   == #

