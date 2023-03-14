/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "hcs_macro.h"
#include "hdf_macro_test.h"

#define HDF_LOG_TAG hcs_macro_cases

#define AUDIO_INFO_NODE_SMARTPA_ADDR 65536
#define AUDIO_INFO_NODE_SMARTPA_NUM 1
#define AUDIO_INFO_NODE_BUILDIN_PRIMARY_MIC_EXIST 1
#define AUDIO_INFO_NODE_BUILDIN_SECOND_MIC_EXIST 1
#define AUDIO_INFO_NODE_VOICE_LEVEL 256
#define AUDIO_INFO_NODE_U64_DATA 0x100000000
#define FP_NODE_U32_IDX_SIZE 3
#define FP_INFO_NODE_STR_LIST_SIZE 3
#define DATA_TEST_NODE_FOUR_DATA_SIZE 4
#define DATA_TEST_NODE_U8_DATA_SIZE 8
#define DATA_TEST_NODE_U16_DATA_SIZE 8
#define BOARD_ID_DATA_SIZE 2
#define MODEM_ID_DATA_SIZE 2
#define BOARD_ID_VALUE 8000
#define FP_ONE_IDX 0
#define FP_TWO_IDX 1
#define FP_THREE_IDX 2

#define CHECK_STRING_EQU(part1, part2) \
    if (strcmp(part1, part2)) { \
        HDF_LOGE("%s %s %d\n", part1, part2, __LINE__); \
        return HDF_FAILURE; \
    }

#define CHECK_INT_EQU(part1, part2) \
    if ((part1) != (part2)) { \
        HDF_LOGE("%d\n", __LINE__); \
        return HDF_FAILURE; \
    }

#define FP_CHILD_DEAL(node) \
    CHECK_STRING_EQU(HCS_PROP(node, status), g_fpData[index].child.status)

#define FP_DEAL(node) \
    do { \
        CHECK_STRING_EQU(HCS_PROP(node, product), g_fpData[index].product); \
        CHECK_STRING_EQU(HCS_PROP(node, chip), g_fpData[index].chip); \
        node##_foreach_child(FP_CHILD_DEAL); \
        CHECK_STRING_EQU(HCS_PROP(node, status), g_fpData[index].status); \
        index++; \
    } while (0)

#define FP_DEAL_(node) FP_DEAL(node)

#define FP_CHILD_DEAL_VARGS(node, fpArgs, idx) \
    CHECK_STRING_EQU(HCS_PROP(node, status), (fpArgs)[idx].child.status)

#define FP_DEAL_VARGS(node, fpArgs, idx) \
    do { \
        CHECK_STRING_EQU(HCS_PROP(node, product), (fpArgs)[(idx)].product); \
        CHECK_STRING_EQU(HCS_PROP(node, chip), (fpArgs)[(idx)].chip); \
        node##_foreach_child_vargs(FP_CHILD_DEAL_VARGS, fpArgs, idx); \
        CHECK_STRING_EQU(HCS_PROP(node, status), (fpArgs)[(idx)].status); \
        (idx)++; \
    } while (0)

#define FP_DEAL_VARGS_(node, fpArgs, idx) FP_DEAL_VARGS(node, fpArgs, idx)

struct OneChild {
    const char *status;
};
struct FingerPrint {
    const char *product;
    const char *chip;
    struct OneChild child;
    const char *status;
};

#define AUDIO_INFO_NODE HCS_NODE(HCS_ROOT, audio_info)
#define FP_INFO_NODE HCS_NODE(HCS_ROOT, fingerprint_info)
#define FP_INFO_ONE_NODE HCS_NODE(FP_INFO_NODE, fingerprint_one)
#define FP_INFO_TWO_NODE HCS_NODE(FP_INFO_NODE, fingerprint_two)
#define DATA_TYPE_TEST_NODE HCS_NODE(HCS_ROOT, data_type_test)
#define FP_INFO_THREE_NODE HCS_NODE(HCS_ROOT, fingerprint_three)
#define FP_INFO_AUDIO_NODE HCS_NODE(FP_INFO_NODE, audio_info)

static struct FingerPrint g_fpData[] = {
    { "test", "one", { "ok" }, "ok" },
    { "test", "two", { "ok" }, "disable" },
    { "test", "three", { "ok" }, "disable" }
};

static int TraversalInfoReadU32(void)
{
    const uint32_t infoReadU32[] = { 1, 256, 65536 };
    const uint32_t infoReadU32Gen[] = HCS_ARRAYS(HCS_NODE(FP_INFO_NODE, read_u32_index));
    int i;

    for (i = 0; i < HCS_ARRAYS_SIZE(HCS_NODE(FP_INFO_NODE, read_u32_index)); i++) {
        CHECK_INT_EQU(infoReadU32Gen[i], infoReadU32[i]);
    }

    return HDF_SUCCESS;
}

static int TraversalStringListNames(void)
{
    const char *stringListNames[] = {"first", "second", "third"};
    const char *stringListNamesGen[] = HCS_ARRAYS(HCS_NODE(FP_INFO_NODE, string_list_names));
    int i;

    for (i = 0; i < HCS_ARRAYS_SIZE(HCS_NODE(FP_INFO_NODE, string_list_names)); i++) {
        CHECK_STRING_EQU(stringListNamesGen[i], stringListNames[i]);
    }

    return HDF_SUCCESS;
}

static int TraversalReadFourDatas(void)
{
    const uint64_t readFourDatas[] = { 0x1, 0x100, 0x10000, 0x100000000 };
    const uint64_t readFourDatasGen[] = HCS_ARRAYS(HCS_NODE(DATA_TYPE_TEST_NODE, read_four_data_type));
    int i;

    for (i = 0; i < HCS_ARRAYS_SIZE(HCS_NODE(DATA_TYPE_TEST_NODE, read_four_data_type)); i++) {
        CHECK_INT_EQU(readFourDatasGen[i], readFourDatas[i]);
    }

    return HDF_SUCCESS;
}

static int TraversalTestU8ElemData(void)
{
    const uint8_t testU8ElemData[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    const uint8_t testU8ElemDataGen[] = HCS_ARRAYS(HCS_NODE(DATA_TYPE_TEST_NODE, test_u8_elem_data));
    int i;

    for (i = 0; i < HCS_ARRAYS_SIZE(HCS_NODE(DATA_TYPE_TEST_NODE, test_u8_elem_data)); i++) {
        CHECK_INT_EQU(testU8ElemDataGen[i], testU8ElemData[i]);
    }

    return HDF_SUCCESS;
}
static int TraversalTestU16ElemData(void)
{
    const uint16_t testU16ElemData[] = { 0, 1, 2, 3, 4, 5, 256, 257 };
    const uint16_t testU16ElemDataGen[] = HCS_ARRAYS(HCS_NODE(DATA_TYPE_TEST_NODE, test_u16_elem_data));
    int i;

    for (i = 0; i < HCS_ARRAYS_SIZE(HCS_NODE(DATA_TYPE_TEST_NODE, test_u16_elem_data)); i++) {
        CHECK_INT_EQU(testU16ElemDataGen[i], testU16ElemData[i]);
    }

    return HDF_SUCCESS;
}
static int TraversalBoardId(void)
{
    const uint32_t boardId[] = { 65536, 256 };
    const uint32_t boardIdGen[] = HCS_ARRAYS(HCS_NODE(HCS_ROOT, board_id));
    int i;

    for (i = 0; i < HCS_ARRAYS_SIZE(HCS_NODE(HCS_ROOT, board_id)); i++) {
        CHECK_INT_EQU(boardIdGen[i], boardId[i]);
    }

    return HDF_SUCCESS;
}
static int TraversalModemId(void)
{
    const uint32_t modemId[] = { 0xF6B40136, 0x0 };
    const uint32_t modemIdGen[] = HCS_ARRAYS(HCS_NODE(HCS_ROOT, modem_id));
    int i;

    for (i = 0; i < HCS_ARRAYS_SIZE(HCS_NODE(HCS_ROOT, modem_id)); i++) {
        CHECK_INT_EQU(modemIdGen[i], modemId[i]);
    }

    return HDF_SUCCESS;
}

static int TraversalAudio(void)
{
    CHECK_STRING_EQU(HCS_PROP(AUDIO_INFO_NODE, match_attr), "hw,hw_audio_info");
    CHECK_STRING_EQU(HCS_PROP(AUDIO_INFO_NODE, pa_identifier), "smartpakit");
    CHECK_INT_EQU(HCS_PROP(AUDIO_INFO_NODE, smartpa_addr), AUDIO_INFO_NODE_SMARTPA_ADDR);
    CHECK_INT_EQU(HCS_PROP(AUDIO_INFO_NODE, smartpa_num), AUDIO_INFO_NODE_SMARTPA_NUM);
    CHECK_INT_EQU(HCS_PROP(AUDIO_INFO_NODE, builtin_primary_mic_exist), AUDIO_INFO_NODE_BUILDIN_PRIMARY_MIC_EXIST);
    CHECK_INT_EQU(HCS_PROP(AUDIO_INFO_NODE, builtin_second_mic_exist), AUDIO_INFO_NODE_BUILDIN_SECOND_MIC_EXIST);
    CHECK_INT_EQU(HCS_PROP(AUDIO_INFO_NODE, voice_vol_level), AUDIO_INFO_NODE_VOICE_LEVEL);
    CHECK_STRING_EQU(HCS_PROP(AUDIO_INFO_NODE, cust_name), "audio_custom_v2");
    CHECK_STRING_EQU(HCS_PROP(AUDIO_INFO_NODE, dual_smartpa_delay), "true");
    CHECK_INT_EQU(HCS_PROP(AUDIO_INFO_NODE, read_u64data), AUDIO_INFO_NODE_U64_DATA);
    CHECK_STRING_EQU(HCS_PROP(AUDIO_INFO_NODE, status), "ok");

    return HDF_SUCCESS;
}

static int TraversalFPFingerInfo(void)
{
    int index = FP_ONE_IDX;
    FP_DEAL_(HCS_NODE(FP_INFO_NODE, finger_info));
    return HDF_SUCCESS;
}

static int TraversalFPAudio(void)
{
    CHECK_STRING_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, match_attr), "hw,hw_audio_info");
    CHECK_STRING_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, pa_identifier), "smartpakit");
    CHECK_INT_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, smartpa_addr), AUDIO_INFO_NODE_SMARTPA_ADDR);
    CHECK_INT_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, smartpa_num), AUDIO_INFO_NODE_SMARTPA_NUM);
    CHECK_INT_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, builtin_primary_mic_exist), AUDIO_INFO_NODE_BUILDIN_PRIMARY_MIC_EXIST);
    CHECK_INT_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, builtin_second_mic_exist), AUDIO_INFO_NODE_BUILDIN_SECOND_MIC_EXIST);
    CHECK_INT_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, voice_vol_level), AUDIO_INFO_NODE_VOICE_LEVEL);
    CHECK_STRING_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, cust_name), "audio_custom_v2");
    CHECK_STRING_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, dual_smartpa_delay), "true");
    CHECK_INT_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, read_u64data), AUDIO_INFO_NODE_U64_DATA);
    CHECK_STRING_EQU(HCS_PROP(FP_INFO_AUDIO_NODE, status), "ok");
    return HDF_SUCCESS;
}

static int TraversalFPOne(void)
{
    int index = FP_ONE_IDX;
    FP_DEAL_(HCS_NODE(FP_INFO_NODE, fingerprint_one));
    index = FP_ONE_IDX;
    FP_DEAL_VARGS_(HCS_NODE(FP_INFO_NODE, fingerprint_one), g_fpData, index);
    return HDF_SUCCESS;
}

static int TraversalFPTwo(void)
{
    int index = FP_TWO_IDX;
    FP_DEAL_(HCS_NODE(FP_INFO_NODE, fingerprint_two));
    index = FP_TWO_IDX;
    FP_DEAL_VARGS_(HCS_NODE(FP_INFO_NODE, fingerprint_two), g_fpData, index);
    return HDF_SUCCESS;
}

static int TraversalPringerprintThree(void)
{
    int index = FP_THREE_IDX;
    FP_DEAL_(HCS_NODE(HCS_ROOT, fingerprint_three));
    index = FP_THREE_IDX;
    FP_DEAL_VARGS_(HCS_NODE(HCS_ROOT, fingerprint_three), g_fpData, index);
    return HDF_SUCCESS;
}

static int TraversalFPInfo(void)
{
    CHECK_STRING_EQU(HCS_PROP(FP_INFO_NODE, match_attr), "hw,hw_fingerprint_info");
    CHECK_INT_EQU(TraversalInfoReadU32(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalFPFingerInfo(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalFPAudio(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalFPOne(), HDF_SUCCESS);
    CHECK_STRING_EQU(HCS_PROP(FP_INFO_NODE, dual_fingerprint), "false");
    CHECK_INT_EQU(TraversalFPTwo(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalStringListNames(), HDF_SUCCESS);
    CHECK_STRING_EQU(HCS_PROP(FP_INFO_NODE, status), "ok");

    return HDF_SUCCESS;
}

static int TraversalDataTypeTest(void)
{
    CHECK_STRING_EQU(HCS_PROP(DATA_TYPE_TEST_NODE, match_attr), "hw,data_type_test");
    CHECK_INT_EQU(TraversalReadFourDatas(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalTestU8ElemData(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalTestU16ElemData(), HDF_SUCCESS);
    return HDF_SUCCESS;
}

int HcsMacroGetOneNode(void)
{
    CHECK_STRING_EQU(HCS_PROP(AUDIO_INFO_NODE, match_attr), "hw,hw_audio_info");
    return HDF_SUCCESS;
}

int HcsMacroCheckNodeExists(void)
{
#if HCS_NODE_EXISTS(AUDIO_INFO_NODE)
    CHECK_STRING_EQU(HCS_PROP(AUDIO_INFO_NODE, match_attr), "hw,hw_audio_info");
    return HDF_SUCCESS;
#else
    return HDF_FAILURE;
#endif
}

int HcsMacroCheckNodeNotExists(void)
{
#if HCS_NODE_EXISTS(HCS_NODE(AUDIO_INFO_NODE, node_not_exists))
    return HDF_FAILURE;
#else
    return HDF_SUCCESS;
#endif
}

int HcsMacroCheckPropExists(void)
{
#if HCS_NODE_HAS_PROP(AUDIO_INFO_NODE, match_attr)
    CHECK_STRING_EQU(HCS_PROP(AUDIO_INFO_NODE, match_attr), "hw,hw_audio_info");
    return HDF_SUCCESS;
#else
    return HDF_FAILURE;
#endif
}

int HcsMacroCheckPropNotExists(void)
{
#if HCS_NODE_HAS_PROP(AUDIO_INFO_NODE, prop_not_exists)
    return HDF_FAILURE;
#else
    return HDF_SUCCESS;
#endif
}

int HcsMacroGetOneNodeProp(void)
{
    CHECK_STRING_EQU(HCS_PROP(AUDIO_INFO_NODE, pa_identifier), "smartpakit");
    return HDF_SUCCESS;
}

int HcsMacroTraversalOneNode(void)
{
    return TraversalAudio();
}

int HcsMacroGetArraySize(void)
{
    CHECK_INT_EQU(HCS_ARRAYS_SIZE(HCS_NODE(FP_INFO_NODE, read_u32_index)), FP_NODE_U32_IDX_SIZE);
    CHECK_INT_EQU(HCS_ARRAYS_SIZE(HCS_NODE(FP_INFO_NODE, string_list_names)), FP_INFO_NODE_STR_LIST_SIZE);
    CHECK_INT_EQU(HCS_ARRAYS_SIZE(HCS_NODE(DATA_TYPE_TEST_NODE, read_four_data_type)), DATA_TEST_NODE_FOUR_DATA_SIZE);
    CHECK_INT_EQU(HCS_ARRAYS_SIZE(HCS_NODE(DATA_TYPE_TEST_NODE, test_u8_elem_data)), DATA_TEST_NODE_U8_DATA_SIZE);
    CHECK_INT_EQU(HCS_ARRAYS_SIZE(HCS_NODE(DATA_TYPE_TEST_NODE, test_u16_elem_data)), DATA_TEST_NODE_U16_DATA_SIZE);
    CHECK_INT_EQU(HCS_ARRAYS_SIZE(HCS_NODE(HCS_ROOT, board_id)), BOARD_ID_DATA_SIZE);
    CHECK_INT_EQU(HCS_ARRAYS_SIZE(HCS_NODE(HCS_ROOT, modem_id)), MODEM_ID_DATA_SIZE);
    return HDF_SUCCESS;
}

int HcsMacroGetArrayElement(void)
{
    const uint32_t infoReadU32[] = { 1, 256, 65536 };
    const uint32_t infoReadU32Gen[] = HCS_ARRAYS(HCS_NODE(FP_INFO_NODE, read_u32_index));

    CHECK_INT_EQU(infoReadU32Gen[0], infoReadU32[0]);

    return HDF_SUCCESS;
}

int HcsMacroTraversalOneArray(void)
{
    return TraversalInfoReadU32();
}

int HcsMacroTraversalAllArray(void)
{
    CHECK_INT_EQU(TraversalInfoReadU32(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalStringListNames(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalReadFourDatas(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalTestU8ElemData(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalTestU16ElemData(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalBoardId(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalModemId(), HDF_SUCCESS);
    return HDF_SUCCESS;
}

int HcsMacroTraversalOneNodeChild(void)
{
    int index = 0;

    HCS_FOREACH_CHILD(FP_INFO_NODE, FP_DEAL);
    return HDF_SUCCESS;
}

int HcsMacroTraversalOneNodeChildVargs(void)
{
    int index = 0;

    HCS_FOREACH_CHILD_VARGS(FP_INFO_NODE, FP_DEAL_VARGS, g_fpData, index);
    return HDF_SUCCESS;
}

int HcsMacroGetOneFile(void)
{
    CHECK_STRING_EQU(HCS_PROP(HCS_ROOT, match_attr), "test,config");
    CHECK_INT_EQU(TraversalAudio(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalFPInfo(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalDataTypeTest(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalBoardId(), HDF_SUCCESS);
    CHECK_STRING_EQU(HCS_PROP(HCS_ROOT, support_Device), "TP LCD Sensor");
    CHECK_INT_EQU(TraversalPringerprintThree(), HDF_SUCCESS);
    CHECK_INT_EQU(TraversalModemId(), HDF_SUCCESS);
    CHECK_INT_EQU(HCS_PROP(HCS_ROOT, boardId), BOARD_ID_VALUE);

    return HDF_SUCCESS;
}
