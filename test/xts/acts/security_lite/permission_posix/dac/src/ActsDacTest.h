/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef XTS_ACTS_SECURITY_LITE_PERMISSSION_POSIX_DAC_SRC_ACTSDACTEST_H
#define XTS_ACTS_SECURITY_LITE_PERMISSSION_POSIX_DAC_SRC_ACTSDACTEST_H

#include <cstddef>
#include <sys/stat.h>

constexpr int ABNORMALINT = -100;
constexpr int SINGLESIZE = 1;
constexpr unsigned int MAX_INT = 2147483647;

constexpr size_t SIZE1 = 1;
constexpr size_t SIZE2 = 2;
constexpr size_t SIZE3 = 3;
constexpr size_t SIZE253 = 253;
constexpr size_t SIZE254 = 254;
constexpr size_t SIZE255 = 255;
constexpr size_t MAX_SIZE = 65536;
constexpr size_t INVAILD_SIZE = 65537;

constexpr mode_t CHMOD000 = 0000;
constexpr mode_t CHMOD013 = 0013;
constexpr mode_t CHMOD016 = 0016;
constexpr mode_t CHMOD027 = 0027;
constexpr mode_t CHMOD031 = 0031;
constexpr mode_t CHMOD044 = 0044;
constexpr mode_t CHMOD045 = 0045;
constexpr mode_t CHMOD052 = 0052;
constexpr mode_t CHMOD063 = 0063;
constexpr mode_t CHMOD074 = 0074;
constexpr mode_t CHMOD076 = 0076;
constexpr mode_t CHMOD077 = 0077;
constexpr mode_t CHMOD105 = 0105;
constexpr mode_t CHMOD111 = 0111;
constexpr mode_t CHMOD116 = 0116;
constexpr mode_t CHMOD120 = 0120;
constexpr mode_t CHMOD123 = 0123;
constexpr mode_t CHMOD124 = 0124;
constexpr mode_t CHMOD132 = 0132;
constexpr mode_t CHMOD143 = 0143;
constexpr mode_t CHMOD151 = 0151;
constexpr mode_t CHMOD166 = 0166;
constexpr mode_t CHMOD167 = 0167;
constexpr mode_t CHMOD175 = 0175;
constexpr mode_t CHMOD203 = 0203;
constexpr mode_t CHMOD210 = 0210;
constexpr mode_t CHMOD222 = 0222;
constexpr mode_t CHMOD230 = 0230;
constexpr mode_t CHMOD235 = 0235;
constexpr mode_t CHMOD241 = 0241;
constexpr mode_t CHMOD242 = 0242;
constexpr mode_t CHMOD256 = 0256;
constexpr mode_t CHMOD257 = 0257;
constexpr mode_t CHMOD261 = 0261;
constexpr mode_t CHMOD274 = 0274;
constexpr mode_t CHMOD305 = 0305;
constexpr mode_t CHMOD306 = 0306;
constexpr mode_t CHMOD310 = 0310;
constexpr mode_t CHMOD312 = 0312;
constexpr mode_t CHMOD325 = 0325;
constexpr mode_t CHMOD333 = 0333;
constexpr mode_t CHMOD334 = 0334;
constexpr mode_t CHMOD342 = 0342;
constexpr mode_t CHMOD347 = 0347;
constexpr mode_t CHMOD354 = 0354;
constexpr mode_t CHMOD362 = 0362;
constexpr mode_t CHMOD371 = 0371;
constexpr mode_t CHMOD401 = 0401;
constexpr mode_t CHMOD406 = 0406;
constexpr mode_t CHMOD407 = 0407;
constexpr mode_t CHMOD415 = 0415;
constexpr mode_t CHMOD422 = 0422;
constexpr mode_t CHMOD430 = 0430;
constexpr mode_t CHMOD444 = 0444;
constexpr mode_t CHMOD446 = 0446;
constexpr mode_t CHMOD453 = 0453;
constexpr mode_t CHMOD456 = 0456;
constexpr mode_t CHMOD457 = 0457;
constexpr mode_t CHMOD460 = 0460;
constexpr mode_t CHMOD473 = 0473;
constexpr mode_t CHMOD507 = 0507;
constexpr mode_t CHMOD511 = 0511;
constexpr mode_t CHMOD521 = 0521;
constexpr mode_t CHMOD526 = 0526;
constexpr mode_t CHMOD536 = 0536;
constexpr mode_t CHMOD543 = 0543;
constexpr mode_t CHMOD555 = 0555;
constexpr mode_t CHMOD560 = 0560;
constexpr mode_t CHMOD562 = 0562;
constexpr mode_t CHMOD564 = 0564;
constexpr mode_t CHMOD570 = 0570;
constexpr mode_t CHMOD604 = 0604;
constexpr mode_t CHMOD611 = 0611;
constexpr mode_t CHMOD614 = 0614;
constexpr mode_t CHMOD623 = 0623;
constexpr mode_t CHMOD637 = 0637;
constexpr mode_t CHMOD640 = 0640;
constexpr mode_t CHMOD655 = 0655;
constexpr mode_t CHMOD657 = 0657;
constexpr mode_t CHMOD665 = 0665;
constexpr mode_t CHMOD666 = 0666;
constexpr mode_t CHMOD670 = 0670;
constexpr mode_t CHMOD671 = 0671;
constexpr mode_t CHMOD672 = 0672;
constexpr mode_t CHMOD700 = 0700;
constexpr mode_t CHMOD702 = 0702;
constexpr mode_t CHMOD703 = 0703;
constexpr mode_t CHMOD712 = 0712;
constexpr mode_t CHMOD716 = 0716;
constexpr mode_t CHMOD717 = 0717;
constexpr mode_t CHMOD724 = 0724;
constexpr mode_t CHMOD731 = 0731;
constexpr mode_t CHMOD743 = 0743;
constexpr mode_t CHMOD750 = 0750;
constexpr mode_t CHMOD765 = 0765;
constexpr mode_t CHMOD777 = 0777;

// Creating Folders and Files for the Test
#define DACDIR0                  "DACDIR0"                        // DIR0/
#define DACDIR0_DACFILE0         "DACDIR0_DACFILE0"               // ├── DIR0_FILE0
#define DACDIR0_DACFILE1         "DACDIR0_DACFILE1"               // ├── DIR0_FILE1
#define DACDIR0_DACDIR0          "DACDIR0_DACDIR0"                // ├── DIR0_DIR0/
#define DACDIR0_DACDIR0_DACDIR0  "DACDIR0_DACDIR0_DACDIR0"        // |   └── DIR0_DIR0_DIR0/
#define DACDIR0_DACDIR1          "DACDIR0_DACDIR1"                // └── DIR0_DIR1/
#define DACDIR1                  "DACDIR1"                        // DIR1/
#define DACDIR1_DACFILE0         "DACDIR1_DACFILE0"               // ├── DIR1_FILE0
#define DACDIR1_DACDIR0          "DACDIR1_DACDIR0"                // └── DIR1_DIR0/

#endif