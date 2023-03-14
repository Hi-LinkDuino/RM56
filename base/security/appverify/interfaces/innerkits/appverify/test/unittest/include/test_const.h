/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef HAPVERIFY_TEST_CONST_H
#define HAPVERIFY_TEST_CONST_H

#include <string>

namespace OHOS {
namespace Security {
namespace Verify {
#define protected public
#define private   public

constexpr char TEST_HAPBYTEBUFFER_CHAR_DATA = 0xff;
constexpr unsigned short TEST_HAPBYTEBUFFER_UINT16_DATA = 0xffff;
constexpr unsigned int TEST_HAPBYTEBUFFER_UINT32_DATA = 0xffffffff;
constexpr int TEST_HAPBYTEBUFFER_INT32_DATA = 0xffffffff;
constexpr long long TEST_HAPBYTEBUFFER_INT64_DATA = 0xffffffffffffffff;
constexpr int TEST_HAPBYTEBUFFER_INT32_DATA_2 = 100000;
constexpr int TEST_HAPBYTEBUFFER_INVALID_INDEX = -1;

constexpr int TEST_RANDOMREAD_LENGTH = 150000;
constexpr int TEST_HAPBYTEBUFFER_LENGTH = 15;
constexpr int TEST_HAPBYTEBUFFER_LENGTH_2 = 8;
constexpr int TEST_HAPBYTEBUFFER_POSITION = 10;

constexpr int TEST_HAPBYTEBUFFER_UINT16_LENGTH = 2;
constexpr int TEST_HAPBYTEBUFFER_INT64_LENGTH = 8;

constexpr int TEST_FILE_BLOCK_LENGTH = 50;
constexpr int TEST_FILE_BLOCK_COUNT = 3;

constexpr int TEST_FILE_CD_OFFSET_POSITION = 6;

constexpr int TEST_HAP_SIGN_VERSION = 1;

constexpr int TEST_MAX_COMMENT_SIZE = 4;
constexpr int TEST_INVALID_MAX_COMMENT_SIZE = -1;

constexpr int TEST_ZIP_EOCD_SIZE = 22;
constexpr int TEST_ZIP_ECD_OFFSET_FIELD_OFFSET = 16;
constexpr int TEST_ZIP_ECD_SIZE_FIELD_OFFSET = 12;

constexpr int TEST_NULL_NID = 118;
constexpr int TEST_SHA256_NID = 672;
constexpr int TEST_SHA384_NID = 673;
constexpr int TEST_SHA512_NID = 674;

constexpr int TEST_MINI_HAP_FILE_LENGTH = 2619;

constexpr int TEST_SIGN_VERSION = 2;
constexpr int TEST_CERT_CHAIN_LEN = 3;

constexpr int TEST_FILE_NUM = 2;
constexpr int TEST_PFOFILE_PKCS7_DATA_INDEX = 5110;
constexpr int TEST_APP_PKCS7_DATA_INDEX = 7000;

const std::string TEST_ANS_TIME = "000000000000Z";

constexpr char miniHapFile[] = {
    80, 75, 3, 4, 10, 0, 0, 8, 0, 0, 0, 0, 33, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 18, 0,
    49, 46, 116, 120, 116, 85, 84, 9, 0, 6, 49, 40, 41, 94, 49, 40, 41, 94, -2, -54, 0, 0, 0, 0, 0,
    0, 32, -117, 9, 0, 0, 48, -126, 9, -121, 6, 9, 42, -122, 72, -122, -9, 13, 1, 7, 2, -96, -126, 9,
    120, 48, -126, 9, 116, 2, 1, 1, 49, 15, 48, 13, 6, 9, 96, -122, 72, 1, 101, 3, 4, 2, 1, 5, 0, 48,
    -126, 3, -6, 6, 9, 42, -122, 72, -122, -9, 13, 1, 7, 1, -96, -126, 3, -21, 4, -126, 3, -25, 44, 0,
    0, 0, 40, 0, 0, 0, 1, 1, 0, 0, 32, 0, 0, 0, -9, 111, -2, 4, -40, -83, -50, -26, 50, -78, -26, 18,
    101, 6, -30, -100, -26, -96, 17, -84, 15, 112, 74, -114, -67, 67, -81, -49, 19, -28, -110, 48, -77,
    3, 0, 0, -81, 3, 0, 0, 48, -126, 3, -85, 48, -126, 2, -109, -96, 3, 2, 1, 2, 2, 20, 122, 93, -87,
    110, 73, 90, -84, 41, -27, -29, -93, 93, 59, 56, -105, -122, -20, -75, 10, -99, 48, 13, 6, 9, 42,
    -122, 72, -122, -9, 13, 1, 1, 11, 5, 0, 48, 101, 49, 11, 48, 9, 6, 3, 85, 4, 6, 19, 2, 67, 78, 49,
    17, 48, 15, 6, 3, 85, 4, 8, 12, 8, 83, 104, 101, 110, 122, 104, 101, 110, 49, 17, 48, 15, 6, 3, 85,
    4, 7, 12, 8, 83, 104, 101, 110, 122, 104, 101, 110, 49, 15, 48, 13, 6, 3, 85, 4, 10, 12, 6, 72, 117,
    97, 119, 101, 105, 49, 15, 48, 13, 6, 3, 85, 4, 11, 12, 6, 72, 117, 97, 119, 101, 105, 49, 14, 48, 12,
    6, 3, 85, 4, 3, 12, 5, 67, 104, 105, 110, 97, 48, 30, 23, 13, 50, 48, 48, 49, 49, 48, 48, 55, 51, 54,
    53, 57, 90, 23, 13, 52, 55, 48, 53, 50, 56, 48, 55, 51, 54, 53, 57, 90, 48, 101, 49, 11, 48, 9, 6, 3,
    85, 4, 6, 19, 2, 67, 78, 49, 17, 48, 15, 6, 3, 85, 4, 8, 12, 8, 83, 104, 101, 110, 122, 104, 101, 110,
    49, 17, 48, 15, 6, 3, 85, 4, 7, 12, 8, 83, 104, 101, 110, 122, 104, 101, 110, 49, 15, 48, 13, 6, 3, 85,
    4, 10, 12, 6, 72, 117, 97, 119, 101, 105, 49, 15, 48, 13, 6, 3, 85, 4, 11, 12, 6, 72, 117, 97, 119, 101,
    105, 49, 14, 48, 12, 6, 3, 85, 4, 3, 12, 5, 67, 104, 105, 110, 97, 48, -126, 1, 34, 48, 13, 6, 9, 42,
    -122, 72, -122, -9, 13, 1, 1, 1, 5, 0, 3, -126, 1, 15, 0, 48, -126, 1, 10, 2, -126, 1, 1, 0, -17, -107,
    -66, -22, 68, -42, -13, 116, 108, -23, 84, -30, -19, 127, -45, -40, -60, -67, -53, -33, -114, 26, -11,
    104, 46, 103, 105, -78, -42, -32, 89, -70, 87, 67, 32, 81, 35, 61, -121, 114, -83, 71, 8, -100, -35, 82,
    -91, -3, 44, 95, -55, -118, 118, 19, 1, -50, 35, -3, -123, 29, 103, 54, -16, -3, 36, -67, 54, 51, 2, 107,
    112, -9, -23, 40, -117, -84, -72, -7, 112, -92, -127, 97, 122, -86, -126, 93, -37, -55, 17, 35, 118, -110,
    -40, 27, -123, 2, 123, 102, 39, 117, -48, -71, 103, -84, -105, -81, 28, -86, 118, -95, 63, 59, 107, 4, -71,
    -30, 83, 39, 105, -112, -35, -15, 112, -52, -58, -118, -114, 127, -54, -125, 46, 70, -95, -70, -11, 125, 49,
    116, -54, 34, 93, 34, 113, 119, -33, -26, -21, 13, 69, -69, -33, -54, 87, 95, 62, -64, -125, -108, 4, -80,
    -86, -11, 103, -121, 97, 122, -114, -88, -83, 59, 119, 56, 22, 72, 35, -67, -67, -83, -60, 112, 105, 117,
    50, -19, 16, -53, 83, 103, -110, -107, 45, 5, 17, -28, -97, -90, 59, 117, -88, 47, -86, -32, 17, -96, 111,
    88, -103, -60, -55, 20, -80, 44, 64, -48, 104, 94, -118, 49, 72, -68, 42, 2, -9, 87, 17, -127, -43, -82, 85,
    70, 13, -98, 24, -55, -90, -51, 103, -65, -58, -97, 52, -71, -85, -3, -118, -99, -84, 110, 2, 110, 56, -18,
    -21, -109, -65, 67, 2, 3, 1, 0, 1, -93, 83, 48, 81, 48, 29, 6, 3, 85, 29, 14, 4, 22, 4, 20, 49, 59, -102,
    -88, 25, -106, 97, 76, -15, -86, -88, -67, -85, -88, -36, 8, -122, 3, 36, -51, 48, 31, 6, 3, 85, 29, 35,
    4, 24, 48, 22, -128, 20, 49, 59, -102, -88, 25, -106, 97, 76, -15, -86, -88, -67, -85, -88, -36, 8, -122,
    3, 36, -51, 48, 15, 6, 3, 85, 29, 19, 1, 1, -1, 4, 5, 48, 3, 1, 1, -1, 48, 13, 6, 9, 42, -122, 72, -122,
    -9, 13, 1, 1, 11, 5, 0, 3, -126, 1, 1, 0, -33, 97, 20, 104, -18, -45, -47, -109, 81, -55, -69, 34, -119,
    -4, -78, 9, -101, 7, -42, 107, 41, -65, -115, -64, -89, -38, 24, 103, -119, 22, -38, 60, 4, -2, 36, -94,
    103, 84, 89, 115, -93, -67, -2, -41, 124, 53, 6, 37, 116, -30, 49, -78, 83, -110, 50, 26, 120, -58, -11,
    91, -119, -121, 87, 99, 74, -71, 23, -113, -63, -14, -68, -44, 17, -109, 24, -94, -26, -60, 32, -8, 77, 29,
    113, 42, -94, -9, -92, -71, -57, -41, 24, -62, 2, -3, 53, -98, -99, -32, -1, 120, -17, 26, 32, -2, -59, 85,
    -22, 23, -79, 9, -72, 116, 67, 55, 118, 12, 7, 79, -83, -103, -66, 123, -55, 5, -29, -53, 92, -95, 99, 4, -23,
    26, -121, -22, 69, 89, 7, -109, -54, -76, -59, 0, 54, -24, -58, -42, 14, -51, -121, -14, 97, 68, -70, -122, -30,
    -113, 43, -102, -34, 96, 38, -8, -14, 65, 83, -124, -104, 27, 69, -42, -69, -30, -108, -76, -1, -47, -117, -2,
    108, 9, 61, 79, 59, 67, -18, -126, -117, 36, -14, -102, 23, 55, -27, 97, -47, -24, 16, 102, 60, 88, 47, -59, 63,
    -57, 46, -77, -112, -88, -93, -60, -120, 101, -3, 117, -102, -113, -10, -61, -69, -122, 6, 96, -78, 28, -111, 1,
    -88, -60, -123, -39, 28, -28, 40, -28, -59, -97, 77, -45, -9, -113, 120, 84, 58, 105, 24, -35, -113, -97, 115,
    -107, -3, 121, 56, -60, 75, -60, -96, -126, 3, -81, 48, -126, 3, -85, 48, -126, 2, -109, -96, 3, 2, 1, 2, 2,
    20, 122, 93, -87, 110, 73, 90, -84, 41, -27, -29, -93, 93, 59, 56, -105, -122, -20, -75, 10, -99, 48, 13, 6,
    9, 42, -122, 72, -122, -9, 13, 1, 1, 11, 5, 0, 48, 101, 49, 11, 48, 9, 6, 3, 85, 4, 6, 19, 2, 67, 78, 49, 17,
    48, 15, 6, 3, 85, 4, 8, 12, 8, 83, 104, 101, 110, 122, 104, 101, 110, 49, 17, 48, 15, 6, 3, 85, 4, 7, 12, 8,
    83, 104, 101, 110, 122, 104, 101, 110, 49, 15, 48, 13, 6, 3, 85, 4, 10, 12, 6, 72, 117, 97, 119, 101, 105, 49,
    15, 48, 13, 6, 3, 85, 4, 11, 12, 6, 72, 117, 97, 119, 101, 105, 49, 14, 48, 12, 6, 3, 85, 4, 3, 12, 5, 67, 104,
    105, 110, 97, 48, 30, 23, 13, 50, 48, 48, 49, 49, 48, 48, 55, 51, 54, 53, 57, 90, 23, 13, 52, 55, 48, 53, 50,
    56, 48, 55, 51, 54, 53, 57, 90, 48, 101, 49, 11, 48, 9, 6, 3, 85, 4, 6, 19, 2, 67, 78, 49, 17, 48, 15, 6, 3,
    85, 4, 8, 12, 8, 83, 104, 101, 110, 122, 104, 101, 110, 49, 17, 48, 15, 6, 3, 85, 4, 7, 12, 8, 83, 104, 101,
    110, 122, 104, 101, 110, 49, 15, 48, 13, 6, 3, 85, 4, 10, 12, 6, 72, 117, 97, 119, 101, 105, 49, 15, 48, 13,
    6, 3, 85, 4, 11, 12, 6, 72, 117, 97, 119, 101, 105, 49, 14, 48, 12, 6, 3, 85, 4, 3, 12, 5, 67, 104, 105, 110,
    97, 48, -126, 1, 34, 48, 13, 6, 9, 42, -122, 72, -122, -9, 13, 1, 1, 1, 5, 0, 3, -126, 1, 15, 0, 48, -126, 1,
    10, 2, -126, 1, 1, 0, -17, -107, -66, -22, 68, -42, -13, 116, 108, -23, 84, -30, -19, 127, -45, -40, -60, -67,
    -53, -33, -114, 26, -11, 104, 46, 103, 105, -78, -42, -32, 89, -70, 87, 67, 32, 81, 35, 61, -121, 114, -83, 71,
    8, -100, -35, 82, -91, -3, 44, 95, -55, -118, 118, 19, 1, -50, 35, -3, -123, 29, 103, 54, -16, -3, 36, -67, 54,
    51, 2, 107, 112, -9, -23, 40, -117, -84, -72, -7, 112, -92, -127, 97, 122, -86, -126, 93, -37, -55, 17, 35, 118,
    -110, -40, 27, -123, 2, 123, 102, 39, 117, -48, -71, 103, -84, -105, -81, 28, -86, 118, -95, 63, 59, 107, 4,
    -71, -30, 83, 39, 105, -112, -35, -15, 112, -52, -58, -118, -114, 127, -54, -125, 46, 70, -95, -70, -11, 125,
    49, 116, -54, 34, 93, 34, 113, 119, -33, -26, -21, 13, 69, -69, -33, -54, 87, 95, 62, -64, -125, -108, 4, -80,
    -86, -11, 103, -121, 97, 122, -114, -88, -83, 59, 119, 56, 22, 72, 35, -67, -67, -83, -60, 112, 105, 117, 50,
    -19, 16, -53, 83, 103, -110, -107, 45, 5, 17, -28, -97, -90, 59, 117, -88, 47, -86, -32, 17, -96, 111, 88,
    -103, -60, -55, 20, -80, 44, 64, -48, 104, 94, -118, 49, 72, -68, 42, 2, -9, 87, 17, -127, -43, -82, 85, 70,
    13, -98, 24, -55, -90, -51, 103, -65, -58, -97, 52, -71, -85, -3, -118, -99, -84, 110, 2, 110, 56, -18, -21,
    -109, -65, 67, 2, 3, 1, 0, 1, -93, 83, 48, 81, 48, 29, 6, 3, 85, 29, 14, 4, 22, 4, 20, 49, 59, -102, -88, 25,
    -106, 97, 76, -15, -86, -88, -67, -85, -88, -36, 8, -122, 3, 36, -51, 48, 31, 6, 3, 85, 29, 35, 4, 24, 48, 22,
    -128, 20, 49, 59, -102, -88, 25, -106, 97, 76, -15, -86, -88, -67, -85, -88, -36, 8, -122, 3, 36, -51, 48, 15,
    6, 3, 85, 29, 19, 1, 1, -1, 4, 5, 48, 3, 1, 1, -1, 48, 13, 6, 9, 42, -122, 72, -122, -9, 13, 1, 1, 11, 5, 0, 3,
    -126, 1, 1, 0, -33, 97, 20, 104, -18, -45, -47, -109, 81, -55, -69, 34, -119, -4, -78, 9, -101, 7, -42, 107, 41,
    -65, -115, -64, -89, -38, 24, 103, -119, 22, -38, 60, 4, -2, 36, -94, 103, 84, 89, 115, -93, -67, -2, -41, 124,
    53, 6, 37, 116, -30, 49, -78, 83, -110, 50, 26, 120, -58, -11, 91, -119, -121, 87, 99, 74, -71, 23, -113, -63,
    -14, -68, -44, 17, -109, 24, -94, -26, -60, 32, -8, 77, 29, 113, 42, -94, -9, -92, -71, -57, -41, 24, -62, 2,
    -3, 53, -98, -99, -32, -1, 120, -17, 26, 32, -2, -59, 85, -22, 23, -79, 9, -72, 116, 67, 55, 118, 12, 7, 79,
    -83, -103, -66, 123, -55, 5, -29, -53, 92, -95, 99, 4, -23, 26, -121, -22, 69, 89, 7, -109, -54, -76, -59, 0,
    54, -24, -58, -42, 14, -51, -121, -14, 97, 68, -70, -122, -30, -113, 43, -102, -34, 96, 38, -8, -14, 65, 83,
    -124, -104, 27, 69, -42, -69, -30, -108, -76, -1, -47, -117, -2, 108, 9, 61, 79, 59, 67, -18, -126, -117, 36,
    -14, -102, 23, 55, -27, 97, -47, -24, 16, 102, 60, 88, 47, -59, 63, -57, 46, -77, -112, -88, -93, -60, -120,
    101, -3, 117, -102, -113, -10, -61, -69, -122, 6, 96, -78, 28, -111, 1, -88, -60, -123, -39, 28, -28, 40, -28,
    -59, -97, 77, -45, -9, -113, 120, 84, 58, 105, 24, -35, -113, -97, 115, -107, -3, 121, 56, -60, 75, -60, 49,
    -126, 1, -85, 48, -126, 1, -89, 2, 1, 1, 48, 125, 48, 101, 49, 11, 48, 9, 6, 3, 85, 4, 6, 19, 2, 67, 78, 49,
    17, 48, 15, 6, 3, 85, 4, 8, 19, 8, 83, 104, 101, 110, 122, 104, 101, 110, 49, 17, 48, 15, 6, 3, 85, 4, 7, 19,
    8, 83, 104, 101, 110, 122, 104, 101, 110, 49, 15, 48, 13, 6, 3, 85, 4, 10, 19, 6, 72, 117, 97, 119, 101, 105,
    49, 15, 48, 13, 6, 3, 85, 4, 11, 19, 6, 72, 117, 97, 119, 101, 105, 49, 14, 48, 12, 6, 3, 85, 4, 3, 19, 5, 67,
    104, 105, 110, 97, 2, 20, 122, 93, -87, 110, 73, 90, -84, 41, -27, -29, -93, 93, 59, 56, -105, -122, -20, -75,
    10, -99, 48, 13, 6, 9, 96, -122, 72, 1, 101, 3, 4, 2, 1, 5, 0, 48, 14, 6, 10, 42, -122, 72, -122, -9, 13, 1, 1,
    -113, 95, 5, 0, 4, -126, 1, 0, 10, -19, -16, -126, -10, -94, 59, 95, 83, 115, -33, 23, 97, -30, -91, -114, 59,
    93, 97, -15, -12, 60, 39, -53, 28, -25, -6, 32, -48, 120, -97, 64, 125, 23, -9, 53, 104, -122, 18, -5, 122, 126,
    19, -80, 53, 14, 70, 21, 63, -99, -76, -52, -2, 103, -114, 95, -21, -60, -49, -42, -81, 58, -16, -23, 19, 108,
    -30, -32, -12, -103, -107, 25, 33, 85, 61, 2, 30, 17, 0, 117, -63, -12, -112, 60, 12, 58, 114, 126, 24, -25,
    -123, 32, 19, 99, 54, -2, -78, -23, 126, 61, 46, 61, 4, -114, -36, 53, -1, -71, 22, -10, 127, 63, -10, -73,
    51, -74, -8, 80, -34, 63, 57, -17, 106, 100, -109, -27, -30, 46, 35, -85, -91, -20, -72, -128, 22, -97, 107,
    101, 21, 63, -36, 50, -69, 99, -10, -88, 52, 19, -115, 67, 53, 53, 30, -107, 122, -123, -118, -56, 80, -50,
    -58, 30, 110, -109, -14, 50, 18, 2, -46, 102, 64, 6, -71, 25, 71, -36, 95, -121, -7, -11, 90, 18, 20, 96,
    -6, -61, 59, 90, -1, 64, -111, 73, 82, 50, -61, 111, 41, -29, -102, 100, -100, -73, 42, -88, 21, -64, -122,
    8, 117, -65, 47, 86, -55, -37, -14, 20, 60, 91, -22, -101, 90, -18, 10, -101, -40, 64, -110, -120, 119, -7,
    -41, -109, 82, -2, -122, 72, 74, 44, 54, -48, -51, -52, -37, -56, -10, -8, 78, 64, -105, 59, -71, 54, -25,
    -9, 105, 29, -95, 0, 1, 0, 0, 0, -77, 9, 0, 0, 0, 0, 0, 0, 72, 65, 80, 32, 83, 105, 103, 32, 66, 108, 111,
    99, 107, 32, 52, 50, 1, 0, 0, 0, 80, 75, 1, 2, 10, 0, 10, 0, 0, 8, 0, 0, 0, 0, 33, 58, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 5, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 46, 116, 120, 116, 85, 84, 1, 0, 6,
    -2, -54, 0, 0, 0, 80, 75, 5, 6, 0, 0, 0, 0, 1, 0, 1, 0, 61, 0, 0, 0, -24, 9, 0, 0, 0, 0, -33, -26, -21, 13, 69,
    -69, -33, -54, 87, 95, 62, -64, -125, -108, 4, -80, -86, -11, 103, -121, 97, 122, -114, -88, -83, 59, 119, 56,
    22, 72, 35, -67, -67, -83, -60, 112, 105, 117, 50, -19, 16, -53, 83, 103, -110, -107, 45, 5, 17, -28, -97, -90,
    59, 117, -88, 47, -86, -32, 17, -96, 111, 88, -103, -60, -55, 20, -80, 44, 64, -48, 104, 94, -118, 49, 72, -68,
    42, 2, -9, 87, 17, -127, -43, -82, 85, 70, 13, -98, 24, -55, -90, -51, 103, -65, -58, -97, 52, -71, -85, -3,
    -118, -99, -84, 110, 2, 110, 56, -18, -21, -109, -65, 67, 2, 3, 1, 0, 1, -93, 83, 48, 81, 48, 29, 6, 3, 85, 29,
    14, 4, 22, 4, 20, 49, 59, -102, -88, 25, -106, 97, 76, -15, -86, -88, -67, -85, -88, -36, 8, -122, 3, 36, -51,
    48, 31, 6, 3, 85, 29, 35, 4, 24, 48, 22, -128, 20, 49, 59, -102, -88, 25, -106, 97, 76, -15, -86, -88, -67, -85,
    -88, -36, 8, -122, 3, 36, -51, 48, 15, 6, 3, 85, 29, 19, 1, 1, -1, 4, 5, 48, 3, 1, 1, -1, 48, 13, 6, 9, 42, -122,
    72, -122, -9, 13, 1, 1, 11, 5, 0, 3, -126, 1, 1, 0, -33, 97, 20, 104, -18, -45, -47, -109, 81, -55, -69, 34, -119,
    -4, -78, 9, -101, 7, -42, 107, 41, -65, -115, -64, -89, -38, 24, 103, -119, 22, -38, 60, 4, -2, 36, -94, 103, 84,
    89, 115, -93, -67, -2, -41, 124, 53, 6, 37, 116, -30, 49, -78, 83, -110, 50, 26, 120, -58, -11, 91, -119, -121,
    87, 99, 74, -71, 23, -113, -63, -14, -68, -44, 17, -109, 24, -94, -26, -60, 32, -8, 77, 29, 113, 42, -94, -9,
    -92, -71, -57, -41, 24, -62, 2, -3, 53, -98, -99, -32, -1, 120, -17, 26, 32, -2, -59, 85, -22, 23, -79, 9, -72,
    116, 67, 55, 118, 12, 7, 79, -83, -103, -66, 123, -55, 5, -29, -53, 92, -95, 99, 4, -23, 26, -121, -22, 69, 89,
    7, -109, -54, -76, -59, 0, 54, -24, -58, -42, 14, -51, -121, -14, 97, 68, -70, -122, -30, -113, 43, -102, -34,
    96, 38, -8, -14, 65, 83, -124, -104, 27, 69, -42, -69, -30, -108, -76, -1, -47, -117, -2, 108, 9, 61, 79, 59,
    67, -18, -126, -117, 36, -14, -102, 23, 55, -27, 97, -47, -24, 16, 102, 60, 88, 47, -59, 63, -57, 46, -77,
    -112, -88, -93, -60, -120, 101, -3, 117, -102, -113, -10, -61, -69, -122, 6, 96, -78, 28, -111, 1, -88, -60,
    -123, -39, 28
};

const std::string ROOTCERT = "-----BEGIN CERTIFICATE-----\n"
    "MIICGjCCAaGgAwIBAgIIShhpn519jNAwCgYIKoZIzj0EAwMwUzELMAkGA1UEBhMC\n"
    "Q04xDzANBgNVBAoMBkh1YXdlaTETMBEGA1UECwwKSHVhd2VpIENCRzEeMBwGA1UE\n"
    "AwwVSHVhd2VpIENCRyBSb290IENBIEcyMB4XDTIwMDMxNjAzMDQzOVoXDTQ5MDMx\n"
    "NjAzMDQzOVowUzELMAkGA1UEBhMCQ04xDzANBgNVBAoMBkh1YXdlaTETMBEGA1UE\n"
    "CwwKSHVhd2VpIENCRzEeMBwGA1UEAwwVSHVhd2VpIENCRyBSb290IENBIEcyMHYw\n"
    "EAYHKoZIzj0CAQYFK4EEACIDYgAEWidkGnDSOw3/HE2y2GHl+fpWBIa5S+IlnNrs\n"
    "GUvwC1I2QWvtqCHWmwFlFK95zKXiM8s9yV3VVXh7ivN8ZJO3SC5N1TCrvB2lpHMB\n"
    "wcz4DA0kgHCMm/wDec6kOHx1xvCRo0IwQDAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0T\n"
    "AQH/BAUwAwEB/zAdBgNVHQ4EFgQUo45a9Vq8cYwqaiVyfkiS4pLcIAAwCgYIKoZI\n"
    "zj0EAwMDZwAwZAIwMypeB7P0IbY7c6gpWcClhRznOJFj8uavrNu2PIoz9KIqr3jn\n"
    "BlBHJs0myI7ntYpEAjBbm8eDMZY5zq5iMZUC6H7UzYSix4Uy1YlsLVV738PtKP9h\n"
    "FTjgDHctXJlC5L7+ZDY=\n-----END CERTIFICATE-----\n";

const std::string SOFT_CA_CERT = "-----BEGIN CERTIFICATE-----\n"
    "MIIDADCCAoegAwIBAgIIJGDixWQS3MkwCgYIKoZIzj0EAwMwUzELMAkGA1UEBhMC\n"
    "Q04xDzANBgNVBAoMBkh1YXdlaTETMBEGA1UECwwKSHVhd2VpIENCRzEeMBwGA1UE\n"
    "AwwVSHVhd2VpIENCRyBSb290IENBIEcyMB4XDTIwMDMxNjEyMzIzOVoXDTQwMDMx\n"
    "NjEyMzIzOVowZDELMAkGA1UEBhMCQ04xDzANBgNVBAoMBkh1YXdlaTETMBEGA1UE\n"
    "CwwKSHVhd2VpIENCRzEvMC0GA1UEAwwmSHVhd2VpIENCRyBTb2Z0d2FyZSBTaWdu\n"
    "aW5nIFNlcnZpY2UgQ0EwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAASsEz7cwYkzFh9b\n"
    "xIwKfXx5qHGjl5WITy0teGnNWqv+jYCceeixHqErvK7YRn2hVPIqhRqKWeANHZUK\n"
    "G0qxi+NIpmSmQS8/63CLz1QAcxfv2Xl3/V82dF0v9lm16ehMsN+jggEVMIIBETAf\n"
    "BgNVHSMEGDAWgBSjjlr1WrxxjCpqJXJ+SJLiktwgADAdBgNVHQ4EFgQU+vX3viBW\n"
    "XV3U2m3xFBU8HQnbsjQwDwYDVR0TAQH/BAUwAwEB/zBGBgNVHSAEPzA9MDsGBFUd\n"
    "IAAwMzAxBggrBgEFBQcCARYlaHR0cDovL2Nwa2ktY2F3ZWIuaHVhd2VpLmNvbS9j\n"
    "cGtpL2NwczAOBgNVHQ8BAf8EBAMCAQYwZgYDVR0fBF8wXTBboFmgV4ZVaHR0cDov\n"
    "L2Nwa2ktY2F3ZWIuaHVhd2VpLmNvbS9jcGtpL3NlcnZsZXQvY3JsRmlsZURvd24u\n"
    "Y3JsP2NlcnR5cGU9MTAmL3Jvb3RfZzJfY3JsLmNybDAKBggqhkjOPQQDAwNnADBk\n"
    "AjBrAQQxUlNgqhYkcEm5eksnPxDkPJSY/qNd2BDgbvEydiLwPSvB7Z9lipxz8ikZ\n"
    "EeUCMGppWcaV//SIG1y5tEwthLwWeEaF613vUILWQLir8+CA3RZGsRBqtE8xSqfz\n"
    "yafLYQ==\n-----END CERTIFICATE-----\n";

const std::string UAT_SOFT_SIGN_CA = "-----BEGIN CERTIFICATE-----\n"
    "MIIDCjCCApGgAwIBAgIIWbEqGvOqT10wCgYIKoZIzj0EAwMwWDELMAkGA1UEBhMC\n"
    "Q04xDzANBgNVBAoMBkh1YXdlaTETMBEGA1UECwwKSHVhd2VpIENCRzEjMCEGA1UE\n"
    "AwwaSHVhd2VpIENCRyBSb290IENBIEcyIFRlc3QwHhcNMjAwMzEzMTE1ODI4WhcN\n"
    "NDAwMzEzMTE1ODI4WjBpMQswCQYDVQQGEwJDTjEPMA0GA1UECgwGSHVhd2VpMRMw\n"
    "EQYDVQQLDApIdWF3ZWkgQ0JHMTQwMgYDVQQDDCtIdWF3ZWkgQ0JHIFNvZnR3YXJl\n"
    "IFNpZ25pbmcgU2VydmljZSBDQSBUZXN0MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE\n"
    "ukPDS3s0TpYa/lANCTc7eX8fdGGjMPUbvso3TtlBvzdm0XDNTdVtZq3XVOfefgpE\n"
    "OaC/JSoXgiNHkeEQ4XSSm0d7MbeoYEyoEKWa1G2/SOQxbVNqKLexxlGMjMuOLdMb\n"
    "o4IBFTCCAREwHwYDVR0jBBgwFoAUaV3AWPH1POb9CkDxHWBz3Bx4xsUwHQYDVR0O\n"
    "BBYEFHu4R1Kn8cxYnvtV7OEtcQ4Hmi8mMA8GA1UdEwEB/wQFMAMBAf8wRgYDVR0g\n"
    "BD8wPTA7BgRVHSAAMDMwMQYIKwYBBQUHAgEWJWh0dHA6Ly9jcGtpLWNhd2ViLmh1\n"
    "YXdlaS5jb20vY3BraS9jcHMwDgYDVR0PAQH/BAQDAgEGMGYGA1UdHwRfMF0wW6BZ\n"
    "oFeGVWh0dHA6Ly9jcGtpLWNhd2ViLmh1YXdlaS5jb20vY3BraS9zZXJ2bGV0L2Ny\n"
    "bEZpbGVEb3duLmNybD9jZXJ0eXBlPTEwJi9yb290X2cyX2NybC5jcmwwCgYIKoZI\n"
    "zj0EAwMDZwAwZAIwF7PjIuOODhpDhzpw2cqV/xbLNJ5CExFJHxcy1D0bHljE5xTt\n"
    "csIN40Ma6aEi3MJQAjAHQLfAzZvMmreYwKnc2bHXlS68roSRvNTvrUKp3Lcp92nK\n"
    "MzieiyKHlWKEgrUjnKc=\n-----END CERTIFICATE-----\n";

const std::string ECC_TEST_KEY = "-----BEGIN EC PARAMETERS-----\n"
    "BggqhkjOPQMBBw==\n-----END EC PARAMETERS-----\n"
    "-----BEGIN EC PRIVATE KEY-----\n"
    "MHcCAQEEIPPNgGEGZA97B5g54d0D8kDBIOYS1LDldMGRf1aifZauoAoGCCqGSM49\n"
    "AwEHoUQDQgAE4BehGAUU+XDXxtlQ+7Ct4sILmYL7CligKCSWbRw0YsBp4F20626u\n"
    "8+T9xNlH7pGc5moba0EKy003FH1sVbtEtg==\n-----END EC PRIVATE KEY-----\n";

const std::string ECC_TEST_CERT = "-----BEGIN CERTIFICATE-----\n"
    "MIIBkjCCATgCCQCncuWaDED8pjAKBggqhkjOPQQDAjBQMQswCQYDVQQGEwJDTjEM\n"
    "MAoGA1UECgwDQ0JHMRwwGgYDVQQLDBNDQkcgSE9TIHNpZ25lZCBUZWFtMRUwEwYD\n"
    "VQQDDAxIT1MgdW5pdHRlc3QwIBcNMjAwNDI3MDQwNDQ2WhgPMjEyMDA0MDMwNDA0\n"
    "NDZaMFAxCzAJBgNVBAYTAkNOMQwwCgYDVQQKDANDQkcxHDAaBgNVBAsME0NCRyBI\n"
    "T1Mgc2lnbmVkIFRlYW0xFTATBgNVBAMMDEhPUyB1bml0dGVzdDBZMBMGByqGSM49\n"
    "AgEGCCqGSM49AwEHA0IABOAXoRgFFPlw18bZUPuwreLCC5mC+wpYoCgklm0cNGLA\n"
    "aeBdtOturvPk/cTZR+6RnOZqG2tBCstNNxR9bFW7RLYwCgYIKoZIzj0EAwIDSAAw\n"
    "RQIhALYbUacS0XrpPzVatHMpHnCmYgsi28hf1UTBNk+iPG/9AiBGh25CpFoDmGqb\n"
    "fz5ebYSvkSe1KNtVHUTprMBQeWaB0A==\n"
    "-----END CERTIFICATE-----\n";

const std::string APP_SIGN_TEST = "-----BEGIN CERTIFICATE-----\n"
    "MIIDHjCCAqWgAwIBAgIISwRVtFpC3o0wCgYIKoZIzj0EAwMwaTELMAkGA1UEBhMC\n"
    "Q04xDzANBgNVBAoMBkh1YXdlaTETMBEGA1UECwwKSHVhd2VpIENCRzE0MDIGA1UE\n"
    "AwwrSHVhd2VpIENCRyBTb2Z0d2FyZSBTaWduaW5nIFNlcnZpY2UgQ0EgVGVzdDAe\n"
    "Fw0yMDA0MjcxMjQyMjJaFw0yMzA0MjcxMjQyMjJaMGUxCzAJBgNVBAYTAkNOMRMw\n"
    "EQYDVQQKDApIdWF3ZWkgQ0JHMR0wGwYDVQQLDBRIT1MgRGV2ZWxvcG1lbnQgVGVh\n"
    "bTEiMCAGA1UEAwwZSE9TIEFwcGxpY2F0aW9uIFByb3Zpc2lvbjBZMBMGByqGSM49\n"
    "AgEGCCqGSM49AwEHA0IABNrFL0TBoPUBcwLCT+jzlDatUBcxv8l4/3xirQkSgPq8\n"
    "ZTsrVChJE51ZotHcDN8QOtLfEJZC92ev+hAqksltfDKjggE5MIIBNTAfBgNVHSME\n"
    "GDAWgBR7uEdSp/HMWJ77VezhLXEOB5ovJjAdBgNVHQ4EFgQU80TcTfc+VizmL+/V\n"
    "0fFAXwsh2vgwRgYDVR0gBD8wPTA7BgRVHSAAMDMwMQYIKwYBBQUHAgEWJWh0dHA6\n"
    "Ly9jcGtpLWNhd2ViLmh1YXdlaS5jb20vY3BraS9jcHMwDgYDVR0PAQH/BAQDAgeA\n"
    "MGwGA1UdHwRlMGMwYaBfoF2GW2h0dHA6Ly9jcGtpLWNhd2ViLmh1YXdlaS5jb20v\n"
    "Y3BraS9zZXJ2bGV0L2NybEZpbGVEb3duLmNybD9jZXJ0eXBlPTE0Ji9zb2Z0X3Np\n"
    "Z25fc3J2X2NybC5jcmwwEwYDVR0lBAwwCgYIKwYBBQUHAwMwGAYMKwYBBAGPWwKC\n"
    "eAECBAgwBgIBAQoBADAKBggqhkjOPQQDAwNnADBkAjAqTxz2Z9F1HaPVoBD2e3fL\n"
    "VXWASZx+YGFMpN0pyCGScDQr4zj+MXuNKewv13M+yaICMHCemT39agd/dHF+sorJ\n"
    "Gz6qDn/r0hmkeHoCzUwA0Dwq/417Re81BbZvHHv7yPO7jw==\n"
    "-----END CERTIFICATE-----\n";

const std::string BASE64_NOT_URL_SAFE_CERT =
    "MIIDLDCCArOgAwIBAgIIRBvbxEi6hzwwCgYIKoZIzj0EAwMwaTELMAkGA1UEBhMCQ04xDzANBgNVBAoMBkh1YXdlaTETMBEGA1UECwwK"
    "SHVhd2VpIENCRzE0MDIGA1UEAwwrSHVhd2VpIENCRyBTb2Z0d2FyZSBTaWduaW5nIFNlcnZpY2UgQ0EgVGVzdDAeFw0yMDA0MjkxMTUw"
    "NTlaFw0yMzA0MjkxMTUwNTlaMHMxCzAJBgNVBAYTAkNOMRMwEQYDVQQKDApIdWF3ZWkgQ0JHMR0wGwYDVQQLDBRIT1MgRGV2ZWxvcG1l"
    "bnQgVGVhbTEwMC4GA1UEAwwnSE9TIEFwcGxpY2F0aW9uIFByb3Zpc2lvbiBQcm9maWxlIERlYnVnMFkwEwYHKoZIzj0CAQYIKoZIzj0D"
    "AQcDQgAEcS/2lY4sb1EHegznTHYurPL4isgkq/AS4r8TqTnAkP2GI3N/e/+AX9MvdNQrCdNObSp/jXikxnU/Ng/m/pL0UaOCATkwggE1"
    "MB8GA1UdIwQYMBaAFHu4R1Kn8cxYnvtV7OEtcQ4Hmi8mMB0GA1UdDgQWBBSOm2cVJTbTlMJF+CJer6HVFCC1iDBGBgNVHSAEPzA9MDsG"
    "BFUdIAAwMzAxBggrBgEFBQcCARYlaHR0cDovL2Nwa2ktY2F3ZWIuaHVhd2VpLmNvbS9jcGtpL2NwczAOBgNVHQ8BAf8EBAMCB4AwbAYD"
    "VR0fBGUwYzBhoF+gXYZbaHR0cDovL2Nwa2ktY2F3ZWIuaHVhd2VpLmNvbS9jcGtpL3NlcnZsZXQvY3JsRmlsZURvd24uY3JsP2NlcnR5"
    "cGU9MTQmL3NvZnRfc2lnbl9zcnZfY3JsLmNybDATBgNVHSUEDDAKBggrBgEFBQcDAzAYBgwrBgEEAY9bAoJ4AQMECDAGAgEBCgEBMAoG"
    "CCqGSM49BAMDA2cAMGQCMBs+JL4mtRpANjBvrv4zPQr/dE9Fk+eS/rj38qwkMM2CyDdoo0akI8HQ+fNGJP6JKwIwJqNjupjVsQ60b2tQ"
    "AQSLisHmegx5s7USRD3dlpcUEGnepWMop1r8D4Ko8jHsKOX7";

const std::string BASE64_CRL =
    "MIIBLTCBswIBATAKBggqhkjOPQQDAzBTMQswCQYDVQQGEwJDTjEPMA0GA1UECgwGSHVhd2VpMRMwEQYDVQQLDApIdWF3ZWkgQ0JHMR4w"
    "HAYDVQQDDBVIdWF3ZWkgQ0JHIFJvb3QgQ0EgRzIXDTIwMDMyNjE2NDkwNVoXDTMwMDMyNTAxMzgzM1qgLzAtMB8GA1UdIwQYMBaAFKOO"
    "WvVavHGMKmolcn5IkuKS3CAAMAoGA1UdFAQDAgEAMAoGCCqGSM49BAMDA2kAMGYCMQCuQOz7IvwENw/Mf1ytoXT5AHiknrrOaw/dqTuZ"
    "RR0wcxP5IJqI1I3Bg5WNbUmTomMCMQDIs23wc9iLgvuDOmMQia4srWunydbbslpFNfV3SIIhtyWDIPVch7RTf/s92HqnrVw=";
}  // Verify
}  // Security
}  // OHOS
#endif  // HAPVERIFY_TEST_CONST_H
