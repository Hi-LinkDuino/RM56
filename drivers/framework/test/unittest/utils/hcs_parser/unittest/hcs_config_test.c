/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hcs_config_test.h"
#include "device_resource_if.h"
#include "hdf_log.h"

#define HDF_LOG_TAG hcs_config_test

#define INDEX_NUM_ZERO 0
#define INDEX_NUM_ONE 1
#define INDEX_NUM_TWO 2
#define INDEX_NUM_THREE 3
#define INDEX_NUM_FOUR 4
#define INDEX_NUM_FIVE 5
#define INDEX_NUM_SIX 6
#define INDEX_NUM_SEVEN 7
#define TEST_DATA_ZERO 0
#define TEST_DATA_ONE 1
#define TEST_DATA_TWO 2
#define TEST_DATA_THREE 3
#define TEST_DATA_FOUR 4
#define TEST_DATA_FIVE 5
#define TEST_DATA_SIX 6
#define TEST_DATA_SEVEN 7
#define TEST_DATA_256 256
#define TEST_DATA_257 257
#define DATA_TYPE_NUM_U32 3
#define DATA_TYPE_NUM_U64 4
#define U8_DATA 1
#define U16_DATA_OFFSET 8
#define U16_DATA (1 << U16_DATA_OFFSET)
#define U32_DATA_OFFSET 16
#define U32_DATA (1 << U32_DATA_OFFSET)
#define U64_DATA 0x100000000
#define DEFAULT_UINT8_MAX 0xFF
#define DEFAULT_UINT16_MAX 0xFFFF
#define DEFAULT_UINT32_MAX 0xFFFFFFFF
#define DEFAULT_UINT64_MAX 0xFFFFFFFFFFFFFFFF
#define BOARDID_LENGTH 2
#define HW_AUDIO_INFO "hw,hw_audio_info"
#define HW_FINGERPRINT_INFO "hw,hw_fingerprint_info"
#define INVALID_STRING "NULL"
#define READ_U32_INDEX "read_u32_index"
#define DUAL_FINGERPRINT "dual_fingerprint"
#define BOARD_ID "board_id"
#define STRING_LIST_NAMES "string_list_names"
#define AUDIO_INFO "audio_info"
#define FINGERPRINT_INFO "fingerprint_info"
#define INVALID_NODE "fingerprint"
#define FINGER_INFO "finger_info"
#define STRING_ATTR_VALUE "default"
#define READ_FOUR_DATA_TYPE "read_four_data_type"
#define HW_DATA_TYPE_TEST "hw,data_type_test"
#define SMARTPA_NUM "smartpa_num"
#define READ_U64DATA "read_u64data"
#define VOICE_VOL_LEVEL "voice_vol_level"
#define SMARTPA_ADDR "smartpa_addr"
#define DATA_TEST_ARRAY_LENGTH 8
#define TEST_U8_ELEM_DATA  "test_u8_elem_data"
#define TEST_U16_ELEM_DATA "test_u16_elem_data"
static const struct DeviceResourceNode *g_testRoot = NULL;
static const struct DeviceResourceIface *g_devResInstance = NULL;

static bool TestGetRootNode(void)
{
    g_devResInstance = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (g_devResInstance == NULL) {
        HDF_LOGE("%s failed, DeviceResourceGetIfaceInstance is error", __func__);
        return false;
    }

    g_testRoot = g_devResInstance->GetRootNode();
    if (g_testRoot == NULL) {
        HDF_LOGE("%s failed, GetDmRootNode failed, line: %d\n", __FUNCTION__, __LINE__);
        return false;
    }
    return true;
}

int HcsTestCreateDMHcsToTree(void)
{
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static bool TestHcsGetNodeByMatchAttrSuccess(void)
{
    const struct DeviceResourceNode *fingerPrintNode = NULL;
    const struct DeviceResourceNode *audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    if (audioNode == NULL) {
        HDF_LOGE("%s failed, HcsGetNodeByMatchAttr failed, line: %d\n", __FUNCTION__, __LINE__);
        return false;
    }
    fingerPrintNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_FINGERPRINT_INFO);
    if (fingerPrintNode == NULL) {
        HDF_LOGE("%s failed, HcsGetNodeByMatchAttr failed, line: %d\n", __FUNCTION__, __LINE__);
        return false;
    }
    return true;
}

int HcsTestGetNodeByMatchAttrSuccess(void)
{
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    if (!TestHcsGetNodeByMatchAttrSuccess()) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static bool TestHcsGetNodeByMatchAttrFailed(void)
{
    const struct DeviceResourceNode *nullNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, INVALID_STRING);
    if (nullNode != NULL) {
        HDF_LOGE("%s failed, HcsGetNodeByMatchAttr failed, line: %d\n", __FUNCTION__, __LINE__);
        return false;
    }
    nullNode = g_devResInstance->GetNodeByMatchAttr(NULL, INVALID_STRING);
    if (nullNode != NULL) {
        HDF_LOGE("%s failed, HcsGetNodeByMatchAttr failed, line: %d\n", __FUNCTION__, __LINE__);
        return false;
    }
    nullNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, NULL);
    if (nullNode != NULL) {
        HDF_LOGE("%s failed, HcsGetNodeByMatchAttr failed, line: %d\n", __FUNCTION__, __LINE__);
        return false;
    }
    return true;
}

int HcsTestGetNodeByMatchAttrFail(void)
{
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    if (!TestHcsGetNodeByMatchAttrFailed()) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static bool TestHcsAttrGetBoolSuccess(void)
{
    const struct DeviceResourceNode *fingerNode = NULL;
    const struct DeviceResourceNode *audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    bool primaryMic = g_devResInstance->GetBool(audioNode, "builtin_primary_mic_exist");
    bool dualFinger = NULL;
    if (!primaryMic) {
        HDF_LOGE("%s failed, HcsGetBool failed, line: %d\n", __FUNCTION__, __LINE__);
        return false;
    }
    fingerNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_FINGERPRINT_INFO);
    if (fingerNode == NULL) {
        HDF_LOGE("%s failed, HcsGetNodeByMatchAttr failed, line: %d\n", __FUNCTION__, __LINE__);
        return false;
    }
    dualFinger = g_devResInstance->GetBool(fingerNode, DUAL_FINGERPRINT);
    if (dualFinger) {
        HDF_LOGE("%s failed, HcsGetBool failed, line: %d\n", __FUNCTION__, __LINE__);
        return false;
    }
    return true;
}

int HcsTestGetBoolAttrValueSuccess(void)
{
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    if (!TestHcsAttrGetBoolSuccess()) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static bool TestHcsAttrGetBoolFailed(void)
{
    const struct DeviceResourceNode *fingerNode = NULL;
    const struct DeviceResourceNode *audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    bool testReadBool = NULL;
    if (audioNode == NULL) {
        HDF_LOGE("%s failed, HcsGetNodeByMatchAttr failed, line: %d", __FUNCTION__, __LINE__);
        return false;
    }
    testReadBool = g_devResInstance->GetBool(audioNode, INVALID_STRING);
    if (testReadBool) {
        HDF_LOGE("%s failed, HcsGetBool failed, line: %d", __FUNCTION__, __LINE__);
        return false;
    }
    fingerNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_FINGERPRINT_INFO);
    if (fingerNode == NULL) {
        HDF_LOGE("%s failed, HcsGetNodeByMatchAttr failed, line: %d", __FUNCTION__, __LINE__);
        return false;
    }
    testReadBool = g_devResInstance->GetBool(fingerNode, INVALID_STRING);
    if (testReadBool) {
        HDF_LOGE("%s failed, HcsGetBool failed. line: %d", __FUNCTION__, __LINE__);
        return false;
    }
    testReadBool = g_devResInstance->GetBool(NULL, INVALID_STRING);
    if (testReadBool) {
        HDF_LOGE("%s failed, HcsGetBool failed. line: %d", __FUNCTION__, __LINE__);
        return false;
    }
    testReadBool = g_devResInstance->GetBool(fingerNode, NULL);
    if (testReadBool) {
        HDF_LOGE("%s failed, HcsGetBool failed. line: %d", __FUNCTION__, __LINE__);
        return false;
    }
    return true;
}

int HcsTestGetBoolAttrValueFail(void)
{
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    if (!TestHcsAttrGetBoolFailed()) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint8AttrValueSuccess(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    uint8_t data;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    ret = g_devResInstance->GetUint8(audioNode, SMARTPA_NUM, &data, DEFAULT_UINT8_MAX);
    if ((ret != HDF_SUCCESS) || (data != U8_DATA)) {
        HDF_LOGE("%s failed, line: %d, ret = %d, data = %u", __FUNCTION__, __LINE__, ret, data);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint8AttrValueFail(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    uint8_t data;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    ret = g_devResInstance->GetUint8(audioNode, INVALID_STRING, &data, DEFAULT_UINT8_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT8_MAX)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint8(audioNode, READ_U64DATA, NULL, DEFAULT_UINT8_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT8_MAX)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint8(audioNode, VOICE_VOL_LEVEL, &data, DEFAULT_UINT8_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT8_MAX)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint8(audioNode, SMARTPA_ADDR, &data, DEFAULT_UINT8_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT8_MAX)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint8(audioNode, READ_U64DATA, &data, DEFAULT_UINT8_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT8_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint8ArrayElemSuccess(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint8_t data[DATA_TEST_ARRAY_LENGTH] = { 0 };
    // the test data is 0, 1, 2, 3, 4, 5, 6, 7.
    uint8_t testData[DATA_TEST_ARRAY_LENGTH] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    int32_t i;
    int32_t count;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    count = g_devResInstance->GetElemNum(dataType, TEST_U8_ELEM_DATA);
    if (count != DATA_TEST_ARRAY_LENGTH) {
        return HDF_FAILURE;
    }
    for (i = 0; i < count; i++) {
        int32_t ret = g_devResInstance->GetUint8ArrayElem(dataType, TEST_U8_ELEM_DATA, i, &data[i],
            DEFAULT_UINT8_MAX);
        if ((ret != HDF_SUCCESS) || (data[i] != testData[i])) {
            HDF_LOGE("%s failed, line: %d, ret = %d, i = %u, data = %u", __FUNCTION__, __LINE__, ret, i, data[i]);
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint8ArrayElemFail(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint8_t data1[DATA_TYPE_NUM_U64] = { 0 };
    uint8_t data;
    int32_t i;
    int32_t ret;
    int32_t count;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    // the number of 8 is invalid value.
    ret = g_devResInstance->GetUint8ArrayElem(dataType, TEST_U8_ELEM_DATA, 8, &data, DEFAULT_UINT8_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT8_MAX)) {
        HDF_LOGE("%s failed, line: %d, ret = %d, data = %x", __FUNCTION__, __LINE__, ret, data);
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    count = g_devResInstance->GetElemNum(dataType, READ_FOUR_DATA_TYPE);
    if (count != DATA_TYPE_NUM_U64) {
        return HDF_FAILURE;
    }
    for (i = 0; i < count; i++) {
        ret = g_devResInstance->GetUint8ArrayElem(dataType, READ_FOUR_DATA_TYPE, i, &data1[i], DEFAULT_UINT8_MAX);
    }
    // the 0~3 represents the location in array.
    if ((ret == HDF_SUCCESS) || (data1[INDEX_NUM_ZERO] != U8_DATA) || (data1[INDEX_NUM_ONE] != DEFAULT_UINT8_MAX) ||
        (data1[INDEX_NUM_TWO] != DEFAULT_UINT8_MAX) || (data1[INDEX_NUM_THREE] != DEFAULT_UINT8_MAX)) {
        HDF_LOGE("%s failed, line: %d, ret = %d, data = %x", __FUNCTION__, __LINE__, ret, data);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint8ArraySuccess(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint8_t data[DATA_TEST_ARRAY_LENGTH] = { 0 };
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    ret = g_devResInstance->GetUint8Array(dataType, TEST_U8_ELEM_DATA, data, DATA_TEST_ARRAY_LENGTH,
                                          DEFAULT_UINT8_MAX);
    // the 0~7 represents the location in array or the value in hcs file.
    if ((ret != HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != TEST_DATA_ZERO) || (data[INDEX_NUM_ONE] != TEST_DATA_ONE) ||
        (data[INDEX_NUM_TWO] != TEST_DATA_TWO) || (data[INDEX_NUM_THREE] != TEST_DATA_THREE) ||
        (data[INDEX_NUM_FOUR] != TEST_DATA_FOUR) ||
        (data[INDEX_NUM_FIVE] != TEST_DATA_FIVE) || (data[INDEX_NUM_SIX] != TEST_DATA_SIX) ||
        (data[INDEX_NUM_SEVEN] != TEST_DATA_SEVEN)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint8ArrayFail(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint8_t data[DATA_TYPE_NUM_U64 + 1] = { 0 };
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    ret = g_devResInstance->GetUint8Array(dataType, READ_FOUR_DATA_TYPE, data, 0, DEFAULT_UINT8_MAX);
    // the 0, 1, 2 represents the location in array, the 0 of second param is default value.
    if ((ret == HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != TEST_DATA_ZERO) || (data[INDEX_NUM_ONE] != TEST_DATA_ZERO) ||
        (data[INDEX_NUM_TWO] != TEST_DATA_ZERO)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint8Array(dataType, READ_FOUR_DATA_TYPE, data, DATA_TYPE_NUM_U64 + 1,
        DEFAULT_UINT8_MAX);
    // the 0, 1, 2, 3, 4 represents the location in array
    if ((ret == HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != U8_DATA) || (data[INDEX_NUM_ONE] != DEFAULT_UINT8_MAX) ||
        (data[INDEX_NUM_TWO] != DEFAULT_UINT8_MAX) ||
        (data[INDEX_NUM_THREE] != DEFAULT_UINT8_MAX) || (data[INDEX_NUM_FOUR] != DEFAULT_UINT8_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint16AttrValueSuccess(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    uint16_t data;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    ret = g_devResInstance->GetUint16(audioNode, SMARTPA_NUM, &data, DEFAULT_UINT16_MAX);
    if ((ret != HDF_SUCCESS) || (data != U8_DATA)) {
        HDF_LOGE("%s failed, line: %d, ret = %d, data = %u", __FUNCTION__, __LINE__, ret, data);
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint16(audioNode, VOICE_VOL_LEVEL, &data, DEFAULT_UINT16_MAX);
    if ((ret != HDF_SUCCESS) || (data != U16_DATA)) {
        HDF_LOGE("%s failed, line: %d, ret = %d, data = %u", __FUNCTION__, __LINE__, ret, data);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint16AttrValueFail(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    uint16_t data;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    ret = g_devResInstance->GetUint16(audioNode, INVALID_STRING, &data, DEFAULT_UINT16_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT16_MAX)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint16(audioNode, READ_U64DATA, NULL, DEFAULT_UINT16_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT16_MAX)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint16(audioNode, SMARTPA_ADDR, &data, DEFAULT_UINT16_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT16_MAX)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint16(audioNode, READ_U64DATA, &data, DEFAULT_UINT16_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT16_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint16ArrayElemSuccess(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    // the length of data is 8.
    uint16_t data[DATA_TEST_ARRAY_LENGTH] = { 0 };
    // the test data is 0, 1, 2, 3, 4, 5, 256, 257.
    uint16_t testData[DATA_TEST_ARRAY_LENGTH] = { TEST_DATA_ZERO, TEST_DATA_ONE, TEST_DATA_TWO, TEST_DATA_THREE,
                                                  TEST_DATA_FOUR, TEST_DATA_FIVE, TEST_DATA_256, TEST_DATA_257 };
    int32_t i;
    int32_t count;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    count = g_devResInstance->GetElemNum(dataType, TEST_U16_ELEM_DATA);
    if (count != DATA_TEST_ARRAY_LENGTH) {
        return HDF_FAILURE;
    }
    for (i = 0; i < count; i++) {
        int32_t ret = g_devResInstance->GetUint16ArrayElem(dataType, TEST_U16_ELEM_DATA, i, &data[i],
            DEFAULT_UINT16_MAX);
        if ((ret != HDF_SUCCESS) || (data[i] != testData[i])) {
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint16ArrayElemFail(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint16_t data1[DATA_TYPE_NUM_U64] = { 0 };
    uint16_t data;
    int32_t i;
    int32_t ret;
    int32_t count;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    ret = g_devResInstance->GetUint16ArrayElem(dataType, TEST_U16_ELEM_DATA, DATA_TEST_ARRAY_LENGTH, &data,
        DEFAULT_UINT16_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT16_MAX)) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    count = g_devResInstance->GetElemNum(dataType, READ_FOUR_DATA_TYPE);
    if (count != DATA_TYPE_NUM_U64) {
        return HDF_FAILURE;
    }
    for (i = 0; i < count; i++) {
        ret = g_devResInstance->GetUint16ArrayElem(dataType, READ_FOUR_DATA_TYPE, i, &data1[i], DEFAULT_UINT16_MAX);
    }
    // the 0~3 represents the location in array.
    if ((ret == HDF_SUCCESS) || (data1[INDEX_NUM_ZERO] != U8_DATA) || (data1[INDEX_NUM_ONE] != U16_DATA) ||
        (data1[INDEX_NUM_TWO] != DEFAULT_UINT16_MAX) || (data1[INDEX_NUM_THREE] != DEFAULT_UINT16_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint16ArraySuccess(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint16_t data[DATA_TEST_ARRAY_LENGTH] = { 0 };
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    ret = g_devResInstance->GetUint16Array(dataType, TEST_U16_ELEM_DATA, data, DATA_TEST_ARRAY_LENGTH,
        DEFAULT_UINT16_MAX);
    // the data[0~7] represents the location in array, the test data is 0, 1, 2, 3, 4, 5, 256, 257.
    if ((ret != HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != TEST_DATA_ZERO) || (data[INDEX_NUM_ONE] != TEST_DATA_ONE) ||
        (data[INDEX_NUM_TWO] != TEST_DATA_TWO) || (data[INDEX_NUM_THREE] != TEST_DATA_THREE) ||
        (data[INDEX_NUM_FOUR] != TEST_DATA_FOUR) ||
        (data[INDEX_NUM_FIVE] != TEST_DATA_FIVE) || (data[INDEX_NUM_SIX] != TEST_DATA_256) ||
        (data[INDEX_NUM_SEVEN] != TEST_DATA_257)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint16ArrayFail(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint16_t data[DATA_TYPE_NUM_U64 + 1] = { 0 };
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    ret = g_devResInstance->GetUint16Array(dataType, READ_FOUR_DATA_TYPE, data, 0, DEFAULT_UINT16_MAX);
    // the 0, 1, 2 represents the location in array, the 0 of second param is default value.
    if ((ret == HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != TEST_DATA_ZERO) || (data[INDEX_NUM_ONE] != TEST_DATA_ZERO) ||
        (data[INDEX_NUM_TWO] != TEST_DATA_ZERO)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint16Array(dataType, READ_FOUR_DATA_TYPE, data, DATA_TYPE_NUM_U64 + 1,
        DEFAULT_UINT16_MAX);
    // the 0, 1, 2, 3, 4 represents the location in array
    if ((ret == HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != U8_DATA) || (data[INDEX_NUM_ONE] != U16_DATA) ||
        (data[INDEX_NUM_TWO] != DEFAULT_UINT16_MAX) || (data[INDEX_NUM_THREE] != DEFAULT_UINT16_MAX) ||
        (data[INDEX_NUM_FOUR] != DEFAULT_UINT16_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint32AttrValueSuccess(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    uint32_t data;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    ret = g_devResInstance->GetUint32(audioNode, SMARTPA_NUM, &data, DEFAULT_UINT32_MAX);
    if ((ret != HDF_SUCCESS) || (data != U8_DATA)) {
        HDF_LOGE("%s failed, line: %d, ret = %d, data = %u", __FUNCTION__, __LINE__, ret, data);
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint32(audioNode, VOICE_VOL_LEVEL, &data, DEFAULT_UINT32_MAX);
    if ((ret != HDF_SUCCESS) || (data != U16_DATA)) {
        HDF_LOGE("%s failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint32(audioNode, SMARTPA_ADDR, &data, DEFAULT_UINT32_MAX);
    if ((ret != HDF_SUCCESS) || (data != U32_DATA)) {
        HDF_LOGE("%s failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint32AttrValueFail(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    uint32_t data;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    ret = g_devResInstance->GetUint32(audioNode, INVALID_STRING, &data, DEFAULT_UINT32_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT32_MAX)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint32(audioNode, SMARTPA_ADDR, NULL, DEFAULT_UINT32_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT32_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint32ArrayElemSuccess(void)
{
    const struct DeviceResourceNode *fingerNode = NULL;
    uint32_t data[DATA_TYPE_NUM_U32] = { 0 };
    uint32_t testData[DATA_TYPE_NUM_U32] = { U8_DATA, U16_DATA, U32_DATA };
    int32_t i;
    int32_t count;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    fingerNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_FINGERPRINT_INFO);
    count = g_devResInstance->GetElemNum(fingerNode, READ_U32_INDEX);
    if (count != DATA_TYPE_NUM_U32) {
        return HDF_FAILURE;
    }
    for (i = 0; i < count; i++) {
        int32_t ret = g_devResInstance->GetUint32ArrayElem(fingerNode, READ_U32_INDEX, i, &data[i],
                                                           DEFAULT_UINT32_MAX);
        if ((ret != HDF_SUCCESS) || (data[i] != testData[i])) {
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint32ArrayElemFail(void)
{
    const struct DeviceResourceNode *fingerNode = NULL;
    uint32_t data;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    fingerNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_FINGERPRINT_INFO);
    ret = g_devResInstance->GetUint32ArrayElem(fingerNode, READ_U32_INDEX, DATA_TYPE_NUM_U32, &data,
        DEFAULT_UINT32_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT32_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint32ArraySuccess(void)
{
    uint32_t data[BOARDID_LENGTH] = { 0 };
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint32Array(g_testRoot, BOARD_ID, data, BOARDID_LENGTH, DEFAULT_UINT32_MAX);
    // the 0, 1 represents the location in array.
    if ((ret != HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != U32_DATA) || (data[INDEX_NUM_ONE] != U16_DATA)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint32ArrayFail(void)
{
    uint32_t data[DATA_TYPE_NUM_U32] = { 0 };
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint32Array(g_testRoot, BOARD_ID, data, 0, DEFAULT_UINT32_MAX);
    // the 0, 1, 2 represents the location in array, the 0 of second param is default value.
    if ((ret == HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != TEST_DATA_ZERO) || (data[INDEX_NUM_ONE] != TEST_DATA_ZERO) ||
        (data[INDEX_NUM_TWO] != TEST_DATA_ZERO)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint32Array(g_testRoot, BOARD_ID, data, DATA_TYPE_NUM_U32, DEFAULT_UINT32_MAX);
    // the 0, 1, 2 represents the location in array
    if ((ret == HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != U32_DATA) || (data[INDEX_NUM_ONE] != U16_DATA) ||
        (data[INDEX_NUM_TWO] != DEFAULT_UINT32_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint64AttrValueSuccess(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    uint64_t data;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    ret = g_devResInstance->GetUint64(audioNode, SMARTPA_NUM, &data, DEFAULT_UINT32_MAX);
    if ((ret != HDF_SUCCESS) || (data != U8_DATA)) {
        HDF_LOGE("%s failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint64(audioNode, VOICE_VOL_LEVEL, &data, DEFAULT_UINT32_MAX);
    if ((ret != HDF_SUCCESS) || (data != U16_DATA)) {
        HDF_LOGE("%s failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint64(audioNode, SMARTPA_ADDR, &data, DEFAULT_UINT32_MAX);
    if ((ret != HDF_SUCCESS) || (data != U32_DATA)) {
        HDF_LOGE("%s failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint64(audioNode, READ_U64DATA, &data, DEFAULT_UINT64_MAX);
    if ((ret != HDF_SUCCESS) || (data != U64_DATA)) {
        HDF_LOGE("%s failed, line: %d", __FUNCTION__, __LINE__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint64AttrValueFail(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    uint64_t data;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    ret = g_devResInstance->GetUint64(audioNode, INVALID_STRING, &data, DEFAULT_UINT64_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT64_MAX)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint64(audioNode, READ_U64DATA, NULL, DEFAULT_UINT64_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT64_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint64ArrayElemSuccess(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint64_t data[DATA_TYPE_NUM_U64] = { 0 };
    uint64_t testData[DATA_TYPE_NUM_U64] = { U8_DATA, U16_DATA, U32_DATA, U64_DATA };
    int32_t i;
    int32_t count;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    count = g_devResInstance->GetElemNum(dataType, READ_FOUR_DATA_TYPE);
    if (count != DATA_TYPE_NUM_U64) {
        return HDF_FAILURE;
    }
    for (i = 0; i < count; i++) {
        int32_t ret = g_devResInstance->GetUint64ArrayElem(dataType, READ_FOUR_DATA_TYPE, i, &data[i],
            DEFAULT_UINT64_MAX);
        if ((ret != HDF_SUCCESS) || (data[i] != testData[i])) {
            return HDF_FAILURE;
        }
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint64ArrayElemFail(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint64_t data;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    ret = g_devResInstance->GetUint64ArrayElem(dataType, READ_FOUR_DATA_TYPE, DATA_TYPE_NUM_U64, &data,
        DEFAULT_UINT64_MAX);
    if ((ret == HDF_SUCCESS) || (data != DEFAULT_UINT64_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint64ArraySuccess(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint64_t data[DATA_TYPE_NUM_U64] = { 0 };
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    ret = g_devResInstance->GetUint64Array(dataType, READ_FOUR_DATA_TYPE, data, DATA_TYPE_NUM_U64,
        DEFAULT_UINT64_MAX);
    // the 0, 1, 2 represents the location in array.
    if ((ret != HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != U8_DATA) || (data[INDEX_NUM_ONE] != U16_DATA) ||
        (data[INDEX_NUM_TWO] != U32_DATA) || (data[INDEX_NUM_THREE] != U64_DATA)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetUint64ArrayFail(void)
{
    const struct DeviceResourceNode *dataType = NULL;
    uint64_t data[DATA_TYPE_NUM_U64 + 1] = { 0 };
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    dataType = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_DATA_TYPE_TEST);
    ret = g_devResInstance->GetUint64Array(dataType, READ_FOUR_DATA_TYPE, data, 0, DEFAULT_UINT64_MAX);
    // the 0, 1, 2 represents the location in array, the 0 of second param is default value.
    if ((ret == HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != TEST_DATA_ZERO) || (data[INDEX_NUM_ONE] != TEST_DATA_ZERO) ||
        (data[INDEX_NUM_TWO] != TEST_DATA_ZERO)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetUint64Array(dataType, READ_FOUR_DATA_TYPE, data, DATA_TYPE_NUM_U64 + 1,
        DEFAULT_UINT64_MAX);
    // the 0, 1, 2, 3, 4 represents the location in array
    if ((ret == HDF_SUCCESS) || (data[INDEX_NUM_ZERO] != U8_DATA) || (data[INDEX_NUM_ONE] != U16_DATA) ||
        (data[INDEX_NUM_TWO] != U32_DATA) || (data[INDEX_NUM_THREE] != U64_DATA) ||
        (data[INDEX_NUM_FOUR] != DEFAULT_UINT64_MAX)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetElemNumSuccess(void)
{
    const struct DeviceResourceNode *fingerNode = NULL;
    int32_t count;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    count = g_devResInstance->GetElemNum(g_testRoot, BOARD_ID);
    if (count != BOARDID_LENGTH) {
        return HDF_FAILURE;
    }
    fingerNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_FINGERPRINT_INFO);
    count = g_devResInstance->GetElemNum(fingerNode, READ_U32_INDEX);
    if (count != DATA_TYPE_NUM_U32) {
        return HDF_FAILURE;
    }
    count = g_devResInstance->GetElemNum(fingerNode, STRING_LIST_NAMES);
    if (count != DATA_TYPE_NUM_U32) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetElemNumFail(void)
{
    const struct DeviceResourceNode *fingerNode = NULL;
    int32_t count;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    fingerNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_FINGERPRINT_INFO);
    // the dual_fingerprint attr do not array, so the count is HDF_FAILURE.
    count = g_devResInstance->GetElemNum(fingerNode, DUAL_FINGERPRINT);
    if (count != HDF_FAILURE) {
        return HDF_FAILURE;
    }
    // the NULL attr is not exist, so the count is HDF_FAILURE.
    count = g_devResInstance->GetElemNum(fingerNode, INVALID_STRING);
    if (count != HDF_FAILURE) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetChildNodeSuccess(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    const struct DeviceResourceNode *fingerprintNode = NULL;
    const struct DeviceResourceNode *oneNode = NULL;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetChildNode(g_testRoot, AUDIO_INFO);
    if (audioNode == NULL) {
        return HDF_FAILURE;
    }
    fingerprintNode = g_devResInstance->GetChildNode(g_testRoot, FINGERPRINT_INFO);
    if (fingerprintNode == NULL) {
        return HDF_FAILURE;
    }
    oneNode = g_devResInstance->GetChildNode(fingerprintNode, "fingerprint_one");
    if (oneNode == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetChildNodeFail(void)
{
    const struct DeviceResourceNode *twoNode = NULL;
    const struct DeviceResourceNode *nullNode = NULL;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    twoNode = g_devResInstance->GetChildNode(g_testRoot, "fingerprint_two");
    if (twoNode != NULL) {
        return HDF_FAILURE;
    }
    nullNode = g_devResInstance->GetChildNode(g_testRoot, INVALID_NODE);
    if (nullNode != NULL) {
        return HDF_FAILURE;
    }
    nullNode = g_devResInstance->GetChildNode(NULL, INVALID_NODE);
    if (nullNode != NULL) {
        return HDF_FAILURE;
    }
    nullNode = g_devResInstance->GetChildNode(g_testRoot, NULL);
    if (nullNode != NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestTraverseAttrInNodeSuccess(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    struct DeviceResourceAttr *pp = NULL;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetChildNode(g_testRoot, AUDIO_INFO);
    if (audioNode == NULL) {
        return HDF_FAILURE;
    }
    DEV_RES_NODE_FOR_EACH_ATTR(audioNode, pp) {
        if ((pp == NULL) || (pp->name == NULL)) {
            break;
        }
        if (strncmp(pp->name, "dual_smartpa_delay", strlen("dual_smartpa_delay")) != 0) {
            continue;
        } else {
            break;
        }
    }
    if (pp == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestTraverseAttrInNodeFail(void)
{
    const struct DeviceResourceNode *fingerprintNode = NULL;
    struct DeviceResourceAttr *pp = NULL;
    const struct DeviceResourceNode *childNode = NULL;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    fingerprintNode = g_devResInstance->GetChildNode(g_testRoot, FINGERPRINT_INFO);
    if (fingerprintNode == NULL) {
        return HDF_FAILURE;
    }
    DEV_RES_NODE_FOR_EACH_ATTR(fingerprintNode, pp) {
        if ((pp == NULL) || (pp->name == NULL)) {
            break;
        }
        if (strncmp(pp->name, "dual-fingerprint", strlen("dual-fingerprint")) != 0) {
            continue;
        } else {
            break;
        }
    }
    if (pp != NULL) {
        return HDF_FAILURE;
    }

    DEV_RES_NODE_FOR_EACH_CHILD_NODE(fingerprintNode, childNode) {
        if ((childNode == NULL) || (childNode->name == NULL)) {
            break;
        }
        if ((strcmp(childNode->name, "fingerprint_one") == 0) || (strcmp(childNode->name, "fingerprint_two") == 0)) {
            HDF_LOGE("%s: childNode->name is %s", __FUNCTION__, childNode->name);
            continue;
        } else {
            HDF_LOGE("%s: failed, childNode->name is %s", __FUNCTION__, childNode->name);
            return HDF_FAILURE;
        }
    }

    return HDF_SUCCESS;
}

int HcsTestGetStringSuccess(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    const char *type = NULL;
    int32_t readString;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    readString = g_devResInstance->GetString(audioNode, "cust_name", &type, NULL);
    if ((readString != HDF_SUCCESS) || (type == NULL) || (strcmp(type, "audio_custom_v2") != HDF_SUCCESS)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetStringFail(void)
{
    const struct DeviceResourceNode *audioNode = NULL;
    const char *type = NULL;
    int32_t testReadString;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    audioNode = g_devResInstance->GetNodeByMatchAttr(g_testRoot, HW_AUDIO_INFO);
    testReadString = g_devResInstance->GetString(audioNode, INVALID_STRING, &type, STRING_ATTR_VALUE);
    if ((testReadString == HDF_SUCCESS) || (type == NULL) || (strcmp(type, STRING_ATTR_VALUE) != HDF_SUCCESS)) {
        return HDF_FAILURE;
    }
    testReadString = g_devResInstance->GetString(audioNode, INVALID_STRING, NULL, STRING_ATTR_VALUE);
    if ((testReadString == HDF_SUCCESS) || (type == NULL) || (strcmp(type, STRING_ATTR_VALUE) != HDF_SUCCESS)) {
        return HDF_FAILURE;
    }
    testReadString = g_devResInstance->GetString(audioNode, INVALID_STRING, &type, NULL);
    if ((testReadString == HDF_SUCCESS) || (type != NULL)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetStringArrayElemSuccess(void)
{
    const char *rawDataDts = NULL;
    const struct DeviceResourceNode *fingerprintNode = NULL;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    fingerprintNode = g_devResInstance->GetChildNode(g_testRoot, FINGERPRINT_INFO);
    // the third param(0) is the location in string_list_names array.
    ret = g_devResInstance->GetStringArrayElem(fingerprintNode, STRING_LIST_NAMES, 0, &rawDataDts, NULL);
    if ((ret != HDF_SUCCESS) || (strcmp(rawDataDts, "first") != HDF_SUCCESS)) {
        return HDF_FAILURE;
    }
    // the third param(1) is the location in string_list_names array.
    ret = g_devResInstance->GetStringArrayElem(fingerprintNode, STRING_LIST_NAMES, 1, &rawDataDts, NULL);
    if ((ret != HDF_SUCCESS) || (strcmp(rawDataDts, "second") != HDF_SUCCESS)) {
        return HDF_FAILURE;
    }
    // the third param(2) is the location in string_list_names array.
    ret = g_devResInstance->GetStringArrayElem(fingerprintNode, STRING_LIST_NAMES, 2, &rawDataDts, NULL);
    if ((ret != HDF_SUCCESS) || (strcmp(rawDataDts, "third") != HDF_SUCCESS)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetStringArrayElemFail(void)
{
    const char *rawDataDts = NULL;
    const struct DeviceResourceNode *fingerprintNode = NULL;
    int32_t ret;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    fingerprintNode = g_devResInstance->GetChildNode(g_testRoot, FINGERPRINT_INFO);
    // the third param(3) is the location in string_list_names array.
    ret = g_devResInstance->GetStringArrayElem(fingerprintNode, STRING_LIST_NAMES, 3, &rawDataDts, STRING_ATTR_VALUE);
    if ((ret == HDF_SUCCESS) || (strcmp(rawDataDts, STRING_ATTR_VALUE) != HDF_SUCCESS)) {
        return HDF_FAILURE;
    }

    // the third param(1) is the location in string_list_names array.
    ret = g_devResInstance->GetStringArrayElem(fingerprintNode, READ_U32_INDEX, 1, &rawDataDts, STRING_ATTR_VALUE);
    if ((ret == HDF_SUCCESS) || (strcmp(rawDataDts, STRING_ATTR_VALUE) != HDF_SUCCESS)) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetStringArrayElem(fingerprintNode, STRING_LIST_NAMES, 1, NULL, STRING_ATTR_VALUE);
    if (ret == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetStringArrayElem(fingerprintNode, STRING_LIST_NAMES, 1, &rawDataDts, NULL);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetStringArrayElem(fingerprintNode, READ_U32_INDEX, 1, NULL, STRING_ATTR_VALUE);
    if (ret == HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetStringArrayElem(fingerprintNode, READ_U32_INDEX, 1, &rawDataDts, NULL);
    if ((ret == HDF_SUCCESS) || (rawDataDts != NULL)) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetNodeAttrRefSuccess(void)
{
    const struct DeviceResourceNode *fingerprintNode = NULL;
    const struct DeviceResourceNode *ret = NULL;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    fingerprintNode = g_devResInstance->GetChildNode(g_testRoot, FINGERPRINT_INFO);
    ret = g_devResInstance->GetNodeByRefAttr(fingerprintNode, FINGER_INFO);
    if (ret == NULL) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetNodeByRefAttr(fingerprintNode, AUDIO_INFO);
    if (ret == NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int HcsTestGetNodeAttrRefFail(void)
{
    const struct DeviceResourceNode *fingerprintNode = NULL;
    const struct DeviceResourceNode *ret = NULL;
    if (!TestGetRootNode()) {
        return HDF_FAILURE;
    }
    fingerprintNode = g_devResInstance->GetChildNode(g_testRoot, FINGERPRINT_INFO);
    ret = g_devResInstance->GetNodeByRefAttr(fingerprintNode, DUAL_FINGERPRINT);
    if (ret != NULL) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetNodeByRefAttr(NULL, DUAL_FINGERPRINT);
    if (ret != NULL) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetNodeByRefAttr(fingerprintNode, NULL);
    if (ret != NULL) {
        return HDF_FAILURE;
    }
    ret = g_devResInstance->GetNodeByRefAttr(NULL, FINGER_INFO);
    if (ret != NULL) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}