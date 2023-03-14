/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "hdf_log.h"
#include "hdmi_core.h"
#include "securec.h"

#define HDF_LOG_TAG hdmi_cec_c

#define HDMI_CEC_PHY_ADDR_PHASE(addr) \
    ((addr) >> 12), ((addr) >> 8) & 0xf, ((addr) >> 4) & 0xf, (addr) & 0xf

typedef void (*HdmiCecHandleMsgFunc)(struct HdmiCntlr *cntlr, struct HdmiCecMsg *oldMsg, struct HdmiCecMsg *newMsg);

struct HdmiCecHandleMsgFuncMap {
    uint8_t opcode;
    HdmiCecHandleMsgFunc func;
};

struct HdmiCecMsgLenInfo g_cecMsg[] = {
    { HDMI_CEC_OPCODE_ACTIVE_SOURCE, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_ACTIVE_SOURCE_MSG_PARAM_LEN),
      HDMI_CEC_MSG_BROADCAST },
    { HDMI_CEC_OPCODE_IMAGE_VIEW_ON, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_TEXT_VIEW_ON, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_INACTIVE_SOURCE, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_INACTIVE_SOURCE_MSG_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_REQUEST_ACTIVE_SOURCE, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_BROADCAST },
    { HDMI_CEC_OPCODE_ROUTING_CHANGE, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_ROUTING_CHANGE_MSG_PARAM_LEN),
      HDMI_CEC_MSG_BROADCAST },
    { HDMI_CEC_OPCODE_ROUTING_INFORMATION, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_ROUTING_INFORMATIO_MSG_PARAM_LEN),
      HDMI_CEC_MSG_BROADCAST },
    { HDMI_CEC_OPCODE_SET_STREAM_PATH, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SET_STREAM_PATH_MSG_PARAM_LEN),
      HDMI_CEC_MSG_BROADCAST },
    { HDMI_CEC_OPCODE_STANDBY, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_RECORD_OFF, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_RECORD_ON, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_SOURCE_TYPE_LEN), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_RECORD_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_STATUS_MSG_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_RECORD_TV_SCREEN, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_CLEAR_ANALOGUE_TIMER, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_ANALOGUE_TIMER_INFO_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_CLEAR_DIGITAL_TIMER, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DIGITAL_TIMER_INFO_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_CLEAR_EXTERNAL_TIMER, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_EXTERNAL_TIMER_INFO_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SET_ANALOGUE_TIMER, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_ANALOGUE_TIMER_INFO_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SET_DIGITAL_TIMER, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DIGITAL_TIMER_INFO_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SET_EXTERNAL_TIMER, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_EXTERNAL_TIMER_INFO_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SET_TIMER_PROGRAM_TITLE, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_TIMER_CLEARED_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_TIMER_CLEARED_STATUS_MSG_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_TIMER_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_TIMER_STATUS_DATA_MIN_LEN), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_CEC_VERSION, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_CEC_VERSION_MSG_PARAM_LEN), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_GET_CEC_VERSION, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_GET_MENU_LANGUAGE, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REPORT_PHYSICAL_ADDRESS_MSG_PARAM_LEN),
      HDMI_CEC_MSG_BROADCAST },
    { HDMI_CEC_OPCODE_SET_MENU_LANGUAGE, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SET_MENU_LANGUAGE_MSG_PARAM_LEN),
      HDMI_CEC_MSG_BROADCAST },
    { HDMI_CEC_OPCODE_REPORT_FEATURES, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REPORT_FEATURES_MSG_PARAM_MIN_LEN),
      HDMI_CEC_MSG_BROADCAST },
    { HDMI_CEC_OPCODE_GIVE_FEATURES, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_DECK_CONTROL, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DECK_CONTROL_MSG_PARAM_LEN), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_DECK_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DECK_STATUS_MSG_PARAM_LEN), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_GIVE_DECK_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_GIVE_DECK_STATUS_MSG_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_PLAY, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_PLAY_MSG_PARAM_LEN), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_GIVE_TUNER_DEVICE_STATU_MSG_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SELECT_ANALOGUE_SERVICE, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SELECT_ANALOGUE_SERVICE_MSG_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SELECT_DIGITAL_SERVICE, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SELECT_DIGITAL_SERVICE_MSG_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_TUNER_DEVICE_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_TUNER_DEVICE_STATUS_MSG_ANA_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_TUNER_STEP_DECREMENT, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_TUNER_STEP_INCREMENT, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_DEVICE_VENDOR_ID, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DEVICE_VENDOR_ID_MSG_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_GIVE_DEVICE_VENDOR_ID, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_VENDOR_COMMAND, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_VENDOR_COMMAND_WITH_ID, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_VENDOR_ID_LEN), HDMI_CEC_MSG_ALL },
    { HDMI_CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_ALL },
    { HDMI_CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_ALL },
    { HDMI_CEC_OPCODE_SET_OSD_STRING, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DISPLAY_CONTROL_LEN), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_GIVE_OSD_NAME, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SET_OSD_NAME, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_MENU_REQUEST, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_MENU_REQUEST_MSG_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_MENU_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_MENU_STATUS_MSG_PARAM_LEN), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_USER_CONTROL_PRESSED, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_UI_COMMAND_LEN), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_USER_CONTROL_RELEASED, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_GIVE_DEVICE_POWER_STATUS, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_REPORT_POWER_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REPORT_POWER_STATUS_MSG_PARA_LEN),
      HDMI_CEC_MSG_DIRECTED_OR_BROADCAST_2_0 },
    { HDMI_CEC_OPCODE_FEATURE_ABORT, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_FEATURE_ABORT_MSG_PARA_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_ABORT, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_GIVE_AUDIO_STATUS, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_REPORT_AUDIO_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REPORT_AUDIO_STATUSMSG_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_REPORT_SHORT_AUDIO_DESCRIPTOR, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_REQUEST_SHORT_AUDIO_DESCRIPTOR, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SET_SYSTEM_AUDIO_MODE, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SYSTEM_AUDIO_STATUS_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SYSTEM_AUDIO_MODE_REQUEST_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SYSTEM_AUDIO_STATUS_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_SET_AUDIO_RATE, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SET_AUDIO_RATE_PARAM_LEN),
      HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_INITIATE_ARC, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_REPORT_ARC_INITIATED, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_REPORT_ARC_TERMINATION, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_REQUEST_ARC_INITIATION, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_REQUEST_ARC_TERMINATION, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_TERMINATE_ARC, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_DIRECTED },
    { HDMI_CEC_OPCODE_CDC_MESSAGE, HDMI_CEC_GET_MSG_LEN(0), HDMI_CEC_MSG_BROADCAST },
    { HDMI_CEC_OPCODE_REQUEST_CURRENT_LATENCY, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REQUEST_CURRENT_LATENCY_MSG_LEN),
      HDMI_CEC_MSG_BROADCAST },
    { HDMI_CEC_OPCODE_REPORT_CURRENT_LATENCY, HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REPORT_CURRENT_LATENCY_MSG_PARAM_MIN_LEN),
      HDMI_CEC_MSG_BROADCAST }
};

static struct HdmiCecMsgLenInfo *HdmiCecGetMsgLenInfo(uint8_t opcode)
{
    uint32_t i, len;

    len = sizeof(g_cecMsg) / sizeof(g_cecMsg[0]);
    for (i = 0; i < len; i++) {
        if (opcode == g_cecMsg[i].opcode) {
            return (&g_cecMsg[i]);
        }
    }
    return NULL;
}

static bool HdmiCecCheckTimerStatusMsgLen(struct HdmiCecMsg *msg)
{
    /* Progremmed Info or Not Progremmed Error Info. */
    uint8_t info = (msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] & 0xf);

    /* Progremmed Indicator Check. */
    if ((msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] & 0x10) > 0) {
        if (((info == HDMI_CEC_PROGRAMMED_INFO_NOT_ENOUGH_SPAC) ||
            (info == HDMI_CEC_PROGRAMMED_INFO_MIGHT_NOT_BE_ENOUGH_SPACE)) &&
            (msg->len < HDMI_CEC_GET_MSG_LEN(HDMI_CEC_TIMER_STATUS_DATA_MAX_LEN))) {
            HDF_LOGD("check Timer_Status(Indicator) Msg fail.");
            return false;
        }
    } else if (info == HDMI_CEC_NOT_PROG_ERR_DUPLICATE) {
        if (msg->len < HDMI_CEC_GET_MSG_LEN(HDMI_CEC_TIMER_STATUS_DATA_MAX_LEN)) {
            HDF_LOGD("check Timer_Status Msg fail.");
            return false;
        }
    }
    return true;
}

static bool HdmiCecCheckRecordOnMsgLen(struct HdmiCecMsg *msg)
{
    switch (msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]) {
        case HDMI_CEC_RECORD_SRC_OWN:
            break;
        case HDMI_CEC_RECORD_SRC_DIGITAL:
            if (msg->len < HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_ON_DIGITAL_MSG_PARAM_LEN)) {
                HDF_LOGD("check Record_On(DIGITAL) Msg fail.");
                return false;
            }
            break;
        case HDMI_CEC_RECORD_SRC_ANALOG:
            if (msg->len < HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_ON_ANALOG_MSG_PARAM_LEN)) {
                HDF_LOGD("check Record_On(ANALOG) Msg fail.");
                return false;
            }
            break;
        case HDMI_CEC_RECORD_SRC_EXT_PLUG:
            if (msg->len < HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_ON_EXT_PLUG_MSG_PARAM_LEN)) {
                HDF_LOGD("check Record_On(EXT_PLUG) Msg fail.");
                return false;
            }
            break;
        case HDMI_CEC_RECORD_SRC_EXT_PHY_ADDR:
            if (msg->len < HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_ON_EXT_PHY_ADDR_MSG_PARAM_LEN)) {
                HDF_LOGD("check Record_On(EXT_PHY_ADDR) Msg fail.");
                return false;
            }
            break;
        default:
            break;
    }
    return true;
}

static bool HdmiCecCheckSomeSpecialfMsgLen(struct HdmiCecMsg *msg, uint8_t opcode)
{
    bool ret = true;

    switch (opcode) {
        case HDMI_CEC_OPCODE_TIMER_STATUS:
            ret = HdmiCecCheckTimerStatusMsgLen(msg);
            break;
        case HDMI_CEC_OPCODE_RECORD_ON:
            ret = HdmiCecCheckRecordOnMsgLen(msg);
            break;
        default:
            break;
    }
    return ret;
}

static bool HdmiCecCheckMsgLen(struct HdmiCec *cec, struct HdmiCecMsg *msg, uint8_t opcode)
{
    struct HdmiCecMsgLenInfo *info = NULL;

    info = HdmiCecGetMsgLenInfo(opcode);
    if (info == NULL) {
        HDF_LOGD("have no this cec msg");
        return false;
    }

    if (msg->len < info->minLen) {
        HDF_LOGD("this cec msg is invalid, opcode = 0x%x, len = %d.", opcode, msg->len);
        return false;
    }
    if (HdmiCecIsBroadcastMsg(msg) == true &&
        (info->type & HDMI_CEC_MSG_BROADCAST) == 0) {
        HDF_LOGD("this cec msg,check broadcast msg fail.");
        return false;
    }
    if (HdmiCecIsBroadcastMsg(msg) == false &&
        (info->type & HDMI_CEC_MSG_BROADCAST) > 0) {
        HDF_LOGD("this cec msg,check directed msg fail.");
        return false;
    }
    if (HdmiCecIsBroadcastMsg(msg) == true &&
        (info->type & HDMI_CEC_MSG_BROADCAST_1_4) > 0 &&
        cec->info.cecVersion < HDMI_CEC_VERSION_2_0) {
        HDF_LOGD("this cec msg,check broadcast msg version fail.");
        return false;
    }
    return HdmiCecCheckSomeSpecialfMsgLen(msg, opcode);
}

/* message encoding/decoding functions. */
void HdmiCecEncodingActiveSourceMsg(struct HdmiCecMsg *msg, uint16_t phyAddr)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_ACTIVE_SOURCE_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_ACTIVE_SOURCE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (phyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (phyAddr & HDMI_ONE_BYTE_MARK);
}

void HdmiCecEncodingImageViewOnMsg(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_IMAGE_VIEW_ON;
}

void HdmiCecEncodingTextViewOnMsg(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_TEXT_VIEW_ON;
}

void HdmiCecEncodingInactiveSourceMsg(struct HdmiCecMsg *msg, uint16_t phyAddr)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_INACTIVE_SOURCE_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_INACTIVE_SOURCE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (phyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (phyAddr & HDMI_ONE_BYTE_MARK);
}

void HdmiCecEncodingRequestActiveSourceMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REQUEST_ACTIVE_SOURCE;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_ACTIVE_SOURCE;
    }
}

void HdmiCecEncodingRoutingChangeMsg(struct HdmiCecMsg *msg, uint16_t orgAddr, uint16_t newAddr, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_ROUTING_CHANGE_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_ROUTING_CHANGE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (orgAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (orgAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = (newAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (newAddr & HDMI_ONE_BYTE_MARK);
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_ROUTING_INFORMATION;
    }
}

void HdmiCecEncodingRoutingInfomationMsg(struct HdmiCecMsg *msg, uint16_t phyAddr)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_ROUTING_INFORMATIO_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_ROUTING_INFORMATION;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (phyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (phyAddr & HDMI_ONE_BYTE_MARK);
}

void HdmiCecEncodingSetStreamPathMsg(struct HdmiCecMsg *msg, uint16_t phyAddr)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SET_STREAM_PATH_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SET_STREAM_PATH;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (phyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (phyAddr & HDMI_ONE_BYTE_MARK);
}

void HdmiCecEncodingStandbyMsg(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_STANDBY;
}

void HdmiCecEncodingRecordOffMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_RECORD_OFF;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_RECORD_STATUS;
    }
}

static void HdmiCecEncodingRecordOnOwn(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_SOURCE_TYPE_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_RECORD_ON;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = HDMI_CEC_RECORD_SRC_OWN;
}

static void HdmiCecEncodingDigitalServiceId(uint8_t *data, uint8_t len, struct HdmiCecDigitalServiceId *digital)
{
    if (len < HDMI_CEC_DIGITAL_SERVICE_ID_LEN) {
        return;
    }

    data[DATA_ZEROTH_OFFSET_ELEMENT] = (digital->method << HDMI_CEC_DIGITAL_SERVICE_ID_METHOD_SHIFT) |
                                       (digital->system);
    if (digital->method == HDMI_CEC_SERVICE_ID_METHOD_BY_CHANNEL) {
        data[DATA_FIRST_OFFSET_ELEMENT] =
            (digital->systemData.channel.format << HDMI_CEC_CHANNEL_NUMBER_FORMAT_SHIFT) |
            (digital->systemData.channel.major >> HDMI_ONE_BYTE_SHIFT);
        data[DATA_SECOND_OFFSET_ELEMENT] = digital->systemData.channel.major & HDMI_ONE_BYTE_MARK;
        data[DATA_THIRD_OFFSET_ELEMENT] = digital->systemData.channel.minor >> HDMI_ONE_BYTE_SHIFT;
        data[DATA_FORTH_OFFSET_ELEMENT] = digital->systemData.channel.minor & HDMI_ONE_BYTE_MARK;
        return;
    }
    switch (digital->system) {
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ARIB_GEN:
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ARIB_BS:
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ARIB_CS:
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ARIB_T:
            data[DATA_FIRST_OFFSET_ELEMENT] = digital->systemData.arib.transportId >> HDMI_ONE_BYTE_SHIFT;
            data[DATA_SECOND_OFFSET_ELEMENT] = digital->systemData.arib.transportId & HDMI_ONE_BYTE_MARK;
            data[DATA_THIRD_OFFSET_ELEMENT] = digital->systemData.arib.serviceId >> HDMI_ONE_BYTE_SHIFT;
            data[DATA_FORTH_OFFSET_ELEMENT] = digital->systemData.arib.serviceId & HDMI_ONE_BYTE_MARK;
            data[DATA_FIFTH_OFFSET_ELEMENT] = digital->systemData.arib.orgNetworkId >> HDMI_ONE_BYTE_SHIFT;
            data[DATA_SIXTH_OFFSET_ELEMENT] = digital->systemData.arib.orgNetworkId & HDMI_ONE_BYTE_MARK;
            break;
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ATSC_GEN:
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ATSC_CABLE:
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ATSC_SATELLITE:
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ATSC_TERRESTRIAL:
            data[DATA_FIRST_OFFSET_ELEMENT] = digital->systemData.atsc.transportId >> HDMI_ONE_BYTE_SHIFT;
            data[DATA_SECOND_OFFSET_ELEMENT] = digital->systemData.atsc.transportId & HDMI_ONE_BYTE_MARK;
            data[DATA_THIRD_OFFSET_ELEMENT] = digital->systemData.atsc.programNumber >> HDMI_ONE_BYTE_SHIFT;
            data[DATA_FORTH_OFFSET_ELEMENT] = digital->systemData.atsc.programNumber & HDMI_ONE_BYTE_MARK;
            break;
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_DVB_GEN:
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_DVB_C:
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_DVB_S:
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_DVB_S2:
        case HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_DVB_T:
            data[DATA_FIRST_OFFSET_ELEMENT] = digital->systemData.dvb.transportId >> HDMI_ONE_BYTE_SHIFT;
            data[DATA_SECOND_OFFSET_ELEMENT] = digital->systemData.dvb.transportId & HDMI_ONE_BYTE_MARK;
            data[DATA_THIRD_OFFSET_ELEMENT] = digital->systemData.dvb.serviceId >> HDMI_ONE_BYTE_SHIFT;
            data[DATA_FORTH_OFFSET_ELEMENT] = digital->systemData.dvb.serviceId & HDMI_ONE_BYTE_MARK;
            data[DATA_FIFTH_OFFSET_ELEMENT] = digital->systemData.dvb.orgNetworkId >> HDMI_ONE_BYTE_SHIFT;
            data[DATA_SIXTH_OFFSET_ELEMENT] = digital->systemData.dvb.orgNetworkId & HDMI_ONE_BYTE_MARK;
            break;
        default:
            HDF_LOGE("digital system 0x%x is invalid", digital->system);
    }
}

static void HdmiCecEncodingRecordOnDigital(struct HdmiCecMsg *msg, struct HdmiCecDigitalServiceId *digital)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_ON_DIGITAL_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_RECORD_ON;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = HDMI_CEC_RECORD_SRC_DIGITAL;
    HdmiCecEncodingDigitalServiceId(&(msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT]),
                                    HDMI_CEC_DIGITAL_SERVICE_ID_LEN,
                                    digital);
}

static void HdmiCecEncodingRecordOnAnalog(struct HdmiCecMsg *msg,
    uint8_t anaBcastType, uint16_t anaFreq, uint8_t bcstSystem)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_ON_ANALOG_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_RECORD_ON;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = HDMI_CEC_RECORD_SRC_ANALOG;
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = anaBcastType;
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = (anaFreq >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (anaFreq & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_SIXTH_ELEMENT] = bcstSystem;
}

static void HdmiCecEncodingRecordOnExtPlug(struct HdmiCecMsg *msg, uint8_t extPlug)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_ON_EXT_PLUG_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_RECORD_ON;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = HDMI_CEC_RECORD_SRC_EXT_PLUG;
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = extPlug;
}

static void HdmiCecEncodingRecordOnExtPhyAddr(struct HdmiCecMsg *msg, uint16_t extPhyAddr)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_ON_EXT_PHY_ADDR_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_RECORD_ON;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = HDMI_CEC_RECORD_SRC_EXT_PHY_ADDR;
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (extPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = (extPhyAddr & HDMI_ONE_BYTE_MARK);
}

void HdmiCecEncodingRecordOnMsg(struct HdmiCecMsg *msg, struct HdmiCecRecordSource *src, bool response)
{
    switch (src->type) {
        case HDMI_CEC_RECORD_SRC_OWN:
            HdmiCecEncodingRecordOnOwn(msg);
            break;
        case HDMI_CEC_RECORD_SRC_DIGITAL:
            HdmiCecEncodingRecordOnDigital(msg, &(src->data.id));
            break;
        case HDMI_CEC_RECORD_SRC_ANALOG:
            HdmiCecEncodingRecordOnAnalog(msg, src->data.analog.anaBcastType,
                src->data.analog.anaFreq, src->data.analog.bcstSystem);
            break;
        case HDMI_CEC_RECORD_SRC_EXT_PLUG:
            HdmiCecEncodingRecordOnExtPlug(msg, src->data.extPlug);
            break;
        case HDMI_CEC_RECORD_SRC_EXT_PHY_ADDR:
            HdmiCecEncodingRecordOnExtPhyAddr(msg, src->data.extPhyAddr);
            break;
        default:
            HDF_LOGE("cec record on: type %d unknow", src->type);
    }

    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_RECORD_STATUS;
    }
}

void HdmiCecEncodingRecordStatusMsg(struct HdmiCecMsg *msg, uint8_t recordStatusInfo)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_RECORD_STATUS_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_RECORD_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = recordStatusInfo;
}

void HdmiCecEncodingRecordTvScreenMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_RECORD_TV_SCREEN;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_RECORD_ON;
    }
}

static void HdmiCecEncodingCommTimerInfo(uint8_t *data, uint8_t len, struct HdmiCecCommTimerInfo *info)
{
    if (len < HDMI_CEC_COMM_TIMER_INFO_LEN) {
        return;
    }
    data[DATA_ZEROTH_OFFSET_ELEMENT] = info->data;
    data[DATA_FIRST_OFFSET_ELEMENT] = info->month;
    data[DATA_SECOND_OFFSET_ELEMENT] = HDMI_CEC_BCD_FORMAT_TIME(info->startHour);
    data[DATA_THIRD_OFFSET_ELEMENT] = HDMI_CEC_BCD_FORMAT_TIME(info->startMinute);
    data[DATA_FORTH_OFFSET_ELEMENT] = HDMI_CEC_BCD_FORMAT_TIME(info->durationHour);
    data[DATA_FIFTH_OFFSET_ELEMENT] = HDMI_CEC_BCD_FORMAT_TIME(info->durationMinute);
    data[DATA_SIXTH_OFFSET_ELEMENT] = info->recordingSeq;
}

static void HdmiCecEncodingAnalogueTimerInfo(uint8_t *data, uint8_t len, struct HdmiCecAnalogueTimerInfo *info)
{
    if (len < HDMI_CEC_ANALOGUE_TIMER_INFO_LEN) {
        return;
    }
    HdmiCecEncodingCommTimerInfo(data, HDMI_CEC_COMM_TIMER_INFO_LEN, &(info->commInfo));
    data[DATA_SEVENTH_OFFSET_ELEMENT] = info->anaBcastType;
    data[DATA_EIGHTH_OFFSET_ELEMENT] = (info->anaFreq >> HDMI_ONE_BYTE_SHIFT);
    data[DATA_NINTH_OFFSET_ELEMENT] = (info->anaFreq & HDMI_ONE_BYTE_MARK);
    data[DATA_TENTH_OFFSET_ELEMENT] = info->bcstSystem;
}

void HdmiCecEncodingClearAnalogueTimerMsg(struct HdmiCecMsg *msg, struct HdmiCecAnalogueTimerInfo *info, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_ANALOGUE_TIMER_INFO_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CLEAR_ANALOGUE_TIMER;
    HdmiCecEncodingAnalogueTimerInfo(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]),
                                     HDMI_CEC_ANALOGUE_TIMER_INFO_LEN,
                                     info);
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_TIMER_CLEARED_STATUS;
    }
}

static void HdmiCecEncodingDigitalTimerInfo(uint8_t *data, uint8_t len, struct HdmiCecDigitalTimerInfo *info)
{
    if (len < HDMI_CEC_DIGITAL_TIMER_INFO_LEN) {
        return;
    }
    HdmiCecEncodingCommTimerInfo(data, HDMI_CEC_COMM_TIMER_INFO_LEN, &(info->commInfo));
    HdmiCecEncodingDigitalServiceId(&data[DATA_SEVENTH_OFFSET_ELEMENT],
                                    len - HDMI_CEC_COMM_TIMER_INFO_LEN,
                                    &(info->id));
}

void HdmiCecEncodingClearDigitalTimerMsg(struct HdmiCecMsg *msg, struct HdmiCecDigitalTimerInfo *info, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DIGITAL_TIMER_INFO_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CLEAR_DIGITAL_TIMER;
    HdmiCecEncodingDigitalTimerInfo(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]),
                                    HDMI_CEC_DIGITAL_TIMER_INFO_LEN,
                                    info);
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_TIMER_CLEARED_STATUS;
    }
}

static void HdmiCecEncodingExternalTimerInfo(uint8_t *data, uint8_t len, struct HdmiCecExternalTimerInfo *info)
{
    if (len < HDMI_CEC_EXTERNAL_TIMER_INFO_LEN) {
        return;
    }
    HdmiCecEncodingCommTimerInfo(data, HDMI_CEC_COMM_TIMER_INFO_LEN, &(info->commInfo));
    data[DATA_SEVENTH_OFFSET_ELEMENT] = info->extSrcSpec;
    data[DATA_EIGHTH_OFFSET_ELEMENT] = info->extPlug;
    data[DATA_NINTH_OFFSET_ELEMENT] = (info->extPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    data[DATA_TENTH_OFFSET_ELEMENT] = (info->extPhyAddr & HDMI_ONE_BYTE_MARK);
}

void HdmiCecEncodingClearExternalTimerMsg(struct HdmiCecMsg *msg, struct HdmiCecExternalTimerInfo *info, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_EXTERNAL_TIMER_INFO_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CLEAR_EXTERNAL_TIMER;
    HdmiCecEncodingExternalTimerInfo(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]),
                                     HDMI_CEC_EXTERNAL_TIMER_INFO_LEN,
                                     info);
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_TIMER_CLEARED_STATUS;
    }
}

void HdmiCecEncodingSetAnalogueTimerMsg(struct HdmiCecMsg *msg, struct HdmiCecAnalogueTimerInfo *info, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_ANALOGUE_TIMER_INFO_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SET_ANALOGUE_TIMER;
    HdmiCecEncodingAnalogueTimerInfo(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]),
                                     HDMI_CEC_ANALOGUE_TIMER_INFO_LEN,
                                     info);
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_TIMER_STATUS;
    }
}

void HdmiCecEncodingSetDigitalTimerMsg(struct HdmiCecMsg *msg, struct HdmiCecDigitalTimerInfo *info, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DIGITAL_TIMER_INFO_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SET_DIGITAL_TIMER;
    HdmiCecEncodingDigitalTimerInfo(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]),
                                    HDMI_CEC_DIGITAL_TIMER_INFO_LEN,
                                    info);
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_TIMER_STATUS;
    }
}

void HdmiCecEncodingSetExternalTimerMsg(struct HdmiCecMsg *msg, struct HdmiCecExternalTimerInfo *info, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_EXTERNAL_TIMER_INFO_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SET_EXTERNAL_TIMER;
    HdmiCecEncodingExternalTimerInfo(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]),
                                     HDMI_CEC_EXTERNAL_TIMER_INFO_LEN,
                                     info);
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_TIMER_STATUS;
    }
}

void HdmiCecEncodingSetTimerProgramTitleMsg(struct HdmiCecMsg *msg, uint8_t *title, uint32_t len)
{
    uint32_t length;

    if (title == NULL || len < HDMI_CEC_PROGRAM_TITLE_STR_MIN_LEN) {
        HDF_LOGE("encoding set timer program title, input param invalid.");
        return;
    }
    length = ((len <= HDMI_CEC_PROGRAM_TITLE_STR_MAX_LEN) ? len : HDMI_CEC_PROGRAM_TITLE_STR_MAX_LEN);
    msg->len = HDMI_CEC_GET_MSG_LEN(length);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SET_TIMER_PROGRAM_TITLE;
    if (memcpy_s(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]), (msg->len - HDMI_CEC_MSG_DATA_SECOND_ELEMENT),
        title, length) != EOK) {
        HDF_LOGE("encoding set timer program title, memcpy_s fail.");
    }
}

void HdmiCecEncodingTimerClearedStatusMsg(struct HdmiCecMsg *msg, uint8_t status)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_TIMER_CLEARED_STATUS_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_TIMER_CLEARED_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = status;
}

void HdmiCecEncodingTimerStatusMsg(struct HdmiCecMsg *msg, struct HdmiCecTimerStatusData *status)
{
    bool duration = false;

    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_TIMER_STATUS_DATA_MIN_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_TIMER_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] |= (status->timerOverlap << HDMI_CEC_TIMER_OVERLAP_WARNING_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] |= (status->mediaInfo << HDMI_CEC_MEDIA_INFO_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] |= (status->progInfo.indicator << HDMI_CEC_PROG_IND_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] |= (status->progInfo.info);
    if (status->progInfo.indicator > 0) {
        /* Progremmed Info */
        if (status->progInfo.info == HDMI_CEC_PROGRAMMED_INFO_NOT_ENOUGH_SPAC ||
            status->progInfo.info == HDMI_CEC_PROGRAMMED_INFO_MIGHT_NOT_BE_ENOUGH_SPACE) {
            duration = true;
        }
    } else {
        /* Not Progremmed Error Info */
        if (status->progInfo.info == HDMI_CEC_NOT_PROG_ERR_DUPLICATE) {
            duration = true;
        }
    }
    if (duration == true) {
        msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_TIMER_STATUS_DATA_MAX_LEN);
        msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = HDMI_CEC_BCD_FORMAT_TIME(status->progInfo.durationHour);
        msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = HDMI_CEC_BCD_FORMAT_TIME(status->progInfo.durationMinute);
    }
}

void HdmiCecEncodingCecVersionMsg(struct HdmiCecMsg *msg, uint8_t version)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_CEC_VERSION_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CEC_VERSION;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = version;
}

void HdmiCecEncodingGetCecVersionMsg(struct HdmiCecMsg *msg, bool response)
{
    if (msg == NULL) {
        HDF_LOGE("%s: msg is null", __func__);
        return;
    }

    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GET_CEC_VERSION;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_CEC_VERSION;
    }
}

void HdmiCecEncodingGetPhyAddressMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS;
    }
}

void HdmiCecEncodingGetMenuLanguageMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GET_MENU_LANGUAGE;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_SET_MENU_LANGUAGE;
    }
}

void HdmiCecEncodingReportPhyAddressMsg(struct HdmiCecMsg *msg, uint16_t phyAddr, uint8_t deviceType)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REPORT_PHYSICAL_ADDRESS_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (phyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (phyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = deviceType;
}

void HdmiCecEncodingSetMenuLanguageMsg(struct HdmiCecMsg *msg, uint8_t *language, uint32_t len)
{
    if (language == NULL || len != HDMI_CEC_SET_MENU_LANGUAGE_MSG_PARAM_LEN) {
        HDF_LOGE("encoding set menu language, input param invalid.");
        return;
    }

    msg->len = HDMI_CEC_GET_MSG_LEN(len);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SET_MENU_LANGUAGE;
    if (memcpy_s(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]), (msg->len - HDMI_CEC_MSG_DATA_SECOND_ELEMENT),
        language, len) != EOK) {
        HDF_LOGE("encoding set menu language, memcpy_s fail.");
    }
}

void HdmiCecEncodingReportFeaturesMsg(struct HdmiCecMsg *msg, struct HdmiCecInfo *info)
{
    uint32_t i;

    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REPORT_FEATURES;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = info->cecVersion;
    msg->len++;
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = info->allDeviceType;
    msg->len++;
    /* fill RC Profile. */
    for (i = 0; i < HDMI_CEC_RC_PROFILE_MAX_NUM; i++) {
        msg->data[msg->len] = info->rcProfile[i];
        msg->len++;
        if ((info->rcProfile[i] & HDMI_CEC_RC_PROFILE_EXTERNSION_MARK) == 0) {
            break;
        }
    }
    /* fill Device Features. */
    for (i = 0; i < HDMI_CEC_DEVICE_FEATURES_MAX_NUM; i++) {
        msg->data[msg->len] = info->devFeatures[i];
        msg->len++;
        if ((info->devFeatures[i] & HDMI_CEC_DEVICE_FEATURES_EXTERNSION_MARK) == 0) {
            break;
        }
    }
}

void HdmiCecEncodingGiveFeaturesMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GIVE_FEATURES;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_SET_MENU_LANGUAGE;
    }
}


void HdmiCecEncodingDeckControlMsg(struct HdmiCecMsg *msg, uint8_t mode)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DECK_CONTROL_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_DECK_CONTROL;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = mode;
}

void HdmiCecEncodingDeckStatusMsg(struct HdmiCecMsg *msg, uint8_t info)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DECK_STATUS_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_DECK_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = info;
}

void HdmiCecEncodingGiveDeckStatusMsg(struct HdmiCecMsg *msg, uint8_t statusReq, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_GIVE_DECK_STATUS_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GIVE_DECK_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = statusReq;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_DECK_STATUS;
    }
}

void HdmiCecEncodingPlayMsg(struct HdmiCecMsg *msg, uint8_t playMode)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_PLAY_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_PLAY;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = playMode;
}

void HdmiCecEncodingGiveTunerDeviceStatusMsg(struct HdmiCecMsg *msg, uint8_t statusReq, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_GIVE_TUNER_DEVICE_STATU_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = statusReq;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_TUNER_DEVICE_STATUS;
    }
}

void HdmiCecEncodingSelectAnalogueServiceMsg(struct HdmiCecMsg *msg,
    uint8_t anaBcastType, uint16_t anaFreq, uint8_t bcstSystem)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SELECT_ANALOGUE_SERVICE_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SELECT_ANALOGUE_SERVICE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = anaBcastType;
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (anaFreq >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = (anaFreq & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = bcstSystem;
}

void HdmiCecEncodingSelectDigitalServiceMsg(struct HdmiCecMsg *msg, struct HdmiCecDigitalServiceId *digital)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SELECT_DIGITAL_SERVICE_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SELECT_DIGITAL_SERVICE;
    HdmiCecEncodingDigitalServiceId(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]),
                                    HDMI_CEC_SELECT_DIGITAL_SERVICE_MSG_PARAM_LEN,
                                    digital);
}

void HdmiCecEncodingTunerDeviceStatusMsg(struct HdmiCecMsg *msg, struct HdmiCecTunerDeviceInfo *info)
{
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_TUNER_DEVICE_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (info->recordingFlag << HDMI_CEC_RECORDING_FALG_SHIFT) |
        (info->dispInfo);
    if (info->isAnalogService == true) {
        msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_TUNER_DEVICE_STATUS_MSG_ANA_PARAM_LEN);
        msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = info->data.analog.anaBcastType;
        msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = (info->data.analog.anaFreq >> HDMI_ONE_BYTE_SHIFT);
        msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (info->data.analog.anaFreq & HDMI_ONE_BYTE_MARK);
        msg->data[HDMI_CEC_MSG_DATA_SIXTH_ELEMENT] = info->data.analog.bcstSystem;
    } else {
        msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_TUNER_DEVICE_STATUS_MSG_DIG_PARAM_LEN);
        HdmiCecEncodingDigitalServiceId(&(msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT]),
                                        HDMI_CEC_SELECT_DIGITAL_SERVICE_MSG_PARAM_LEN,
                                        &(info->data.digital));
    }
}

void HdmiCecEncodingTunerStepDecrementMsg(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_TUNER_STEP_DECREMENT;
}

void HdmiCecEncodingTunerStepIncrementMsg(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_TUNER_STEP_INCREMENT;
}

void HdmiCecEncodingDeviceVendorIdMsg(struct HdmiCecMsg *msg, uint32_t devVendorId)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_DEVICE_VENDOR_ID_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_DEVICE_VENDOR_ID;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (devVendorId >> HDMI_TWO_BYTES_SHIFT) & HDMI_ONE_BYTE_MARK;
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (devVendorId >> HDMI_ONE_BYTE_SHIFT) & HDMI_ONE_BYTE_MARK;
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = (devVendorId & HDMI_ONE_BYTE_MARK);
}

void HdmiCecEncodingGiveDeviceVendorIdMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GIVE_DEVICE_VENDOR_ID;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_DEVICE_VENDOR_ID;
    }
}

void HdmiCecEncodingVendorCommandMsg(struct HdmiCecMsg *msg, uint8_t *data, uint32_t len)
{
    uint32_t length;

    if (data == NULL || len == 0) {
        HDF_LOGE("encoding vendor cmd, input param invalid.");
        return;
    }
    length = (len > HDMI_CEC_VENDOR_SPECIFIC_DATA_MAX_LEN) ? HDMI_CEC_VENDOR_SPECIFIC_DATA_MAX_LEN : len;
    msg->len = HDMI_CEC_GET_MSG_LEN(length);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_VENDOR_COMMAND;
    if (memcpy_s(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]), (msg->len - HDMI_CEC_MSG_MIN_LEN), data, length)
        != EOK) {
        HDF_LOGE("encoding vendor cmd, memcpy_s fail.");
    }
}

void HdmiCecEncodingVendorCommandWithIdMsg(struct HdmiCecMsg *msg, uint32_t vendorId, uint8_t *data, uint32_t len)
{
    uint32_t length;

    if (data == NULL || len == 0) {
        HDF_LOGE("encoding vendor cmd with id, input param invalid.");
        return;
    }
    length = (len > HDMI_CEC_VENDOR_SPECIFIC_DATA_WITH_ID_MAX_LEN) ?
        HDMI_CEC_VENDOR_SPECIFIC_DATA_WITH_ID_MAX_LEN : len;
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_VENDOR_ID_LEN + length);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_VENDOR_COMMAND_WITH_ID;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (vendorId >> HDMI_TWO_BYTES_SHIFT) & HDMI_ONE_BYTE_MARK;
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (vendorId >> HDMI_ONE_BYTE_SHIFT) & HDMI_ONE_BYTE_MARK;
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = (vendorId & HDMI_ONE_BYTE_MARK);
    if (memcpy_s(&(msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT]), (msg->len - HDMI_CEC_MSG_DATA_FIFTH_ELEMENT),
        data, length) != EOK) {
        HDF_LOGE("encoding vendor cmd with id, memcpy_s fail.");
    }
}

void HdmiCecEncodingVendorRemoteButtonDownMsg(struct HdmiCecMsg *msg, uint8_t *rcCode, uint32_t len)
{
    uint32_t length;

    if (rcCode == NULL || len == 0) {
        HDF_LOGE("encoding vendor remote button down, input param invalid.");
        return;
    }
    length = (len > HDMI_CEC_VENDOR_SPECIFIC_RC_CODE_MAX_LEN) ? HDMI_CEC_VENDOR_SPECIFIC_RC_CODE_MAX_LEN : len;
    msg->len = HDMI_CEC_GET_MSG_LEN(length);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN;
    if (memcpy_s(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]), (msg->len - HDMI_CEC_MSG_DATA_SECOND_ELEMENT),
        rcCode, length) != EOK) {
        HDF_LOGE("encoding vendor remote button down, memcpy_s fail.");
    }
}

void HdmiCecEncodingVendorRemoteButtonUpMsg(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP;
}

void HdmiCecEncodingSetOsdStringMsg(struct HdmiCecMsg *msg, uint8_t dispControl, uint8_t *str, uint32_t len)
{
    uint32_t length;

    if (str == NULL || len == 0) {
        HDF_LOGE("encoding set OSD string, input param invalid.");
        return;
    }
    length = (len > HDMI_CEC_OSD_STRING_MAX_LEN) ? HDMI_CEC_OSD_STRING_MAX_LEN : len;
    msg->len = HDMI_CEC_GET_MSG_LEN(length + HDMI_CEC_DISPLAY_CONTROL_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SET_OSD_STRING;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = dispControl;
    if (memcpy_s(&(msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT]), (msg->len - HDMI_CEC_MSG_DATA_THIRD_ELEMENT),
        str, length) != EOK) {
        HDF_LOGE("encoding set OSD string, memcpy_s fail.");
    }
}

void HdmiCecEncodingGiveOsdNameMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GIVE_OSD_NAME;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_SET_OSD_NAME;
    }
}

void HdmiCecEncodingSetOsdNameMsg(struct HdmiCecMsg *msg, uint8_t *name, uint32_t len)
{
    uint32_t length;

    if (name == NULL || len == 0) {
        HDF_LOGE("encoding set OSD name, input param invalid.");
        return;
    }
    length = (len > HDMI_CEC_OSD_NAME_MAX_LEN) ? HDMI_CEC_OSD_NAME_MAX_LEN : len;
    msg->len = HDMI_CEC_GET_MSG_LEN(length);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SET_OSD_NAME;
    if (memcpy_s(&(msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT]), (msg->len - HDMI_CEC_MSG_DATA_SECOND_ELEMENT),
        name, length) != EOK) {
        HDF_LOGE("encoding set OSD name, memcpy_s fail.");
    }
}

void HdmiCecEncodingMenuRequestMsg(struct HdmiCecMsg *msg, uint8_t menuReq, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_MENU_REQUEST_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_MENU_REQUEST;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = menuReq;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_MENU_STATUS;
    }
}

void HdmiCecEncodingMenuStatusMsg(struct HdmiCecMsg *msg, uint8_t menuStatus)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_MENU_STATUS_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_MENU_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = menuStatus;
}

void HdmiCecEncodingUserControlPrtessedMsg(struct HdmiCecMsg *msg, struct HdmiCecUiCmd *cmd)
{
    if (cmd == NULL) {
        return;
    }
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_UI_COMMAND_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_USER_CONTROL_PRESSED;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = cmd->cmdType;
    if (cmd->hasAddOp == false) {
        return;
    }
    switch (cmd->cmdType) {
        case HDMI_CEC_UI_CMD_SELECT_BROADCAST_TYPE:
        case HDMI_CEC_UI_CMD_SELECT_SOUND_PRESENTATION:
        case HDMI_CEC_UI_CMD_PLAY_FUNCTION:
        case HDMI_CEC_UI_CMD_SELECT_MEDIA_FUNCTION:
        case HDMI_CEC_UI_CMD_SELECT_AV_INPUT_FUNCTION:
        case HDMI_CEC_UI_CMD_SELECT_AUDIO_INPUT_FUNCTION:
            (msg->len)++;
            /* The additional operand is one byte for all these UI commands */
            msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = cmd->addOperands.uiBroadcastType;
            break;
        case HDMI_CEC_UI_CMD_TUNE_FUNCTION:
            msg->len += HDMI_CEC_CHANNEL_IDENTIFIER_LEN;
            msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (cmd->addOperands.channel.format <<
                                                          HDMI_CEC_CHANNEL_NUMBER_FORMAT_SHIFT) |
                                                         (cmd->addOperands.channel.major >> HDMI_ONE_BYTE_SHIFT);
            msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = (cmd->addOperands.channel.major & HDMI_ONE_BYTE_MARK);
            msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (cmd->addOperands.channel.minor >> HDMI_ONE_BYTE_SHIFT);
            msg->data[HDMI_CEC_MSG_DATA_SIXTH_ELEMENT] = (cmd->addOperands.channel.minor & HDMI_ONE_BYTE_MARK);
            break;
        default:
            HDF_LOGI("UI type %d have no additional operands.", cmd->cmdType);
            break;
    }
}

void HdmiCecEncodingUserControlReleasedMsg(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_USER_CONTROL_RELEASED;
}

void HdmiCecEncodingGiveDevicePowerStatusMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GIVE_DEVICE_POWER_STATUS;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_REPORT_POWER_STATUS;
    }
}

void HdmiCecEncodingReportDevicePowerStatusMsg(struct HdmiCecMsg *msg, uint8_t powerStatus)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REPORT_POWER_STATUS_MSG_PARA_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REPORT_POWER_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = powerStatus;
}

void HdmiCecEncodingFeatureAbortMsg(struct HdmiCecMsg *msg, uint8_t opcode, uint8_t reason)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_FEATURE_ABORT_MSG_PARA_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_FEATURE_ABORT;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = opcode;
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = reason;
}

void HdmiCecEncodingAbortMsg(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_ABORT;
}

void HdmiCecEncodingGiveAudioStatusMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GIVE_AUDIO_STATUS;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_REPORT_AUDIO_STATUS;
    }
}

void HdmiCecEncodingGiveSystemAudioModeMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS;
    }
}

void HdmiCecEncodingReportAudioStatusMsg(struct HdmiCecMsg *msg, uint8_t audioMuteStatus, uint8_t audioVolumeStatus)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REPORT_AUDIO_STATUSMSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REPORT_AUDIO_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (audioMuteStatus << HDMI_CEC_AUDIO_MUTE_STATUS_SHIFT) |
                                                  (audioVolumeStatus & HDMI_CEC_AUDIO_VOLUME_STATUS_MARK);
}

void HdmiCecEncodingRequestShortAudioDescriptorMsg(struct HdmiCecMsg *msg,
    uint8_t *id, uint8_t *code, uint32_t len, bool response)
{
    uint32_t num, i;

    if (id == NULL || code == NULL || len == 0) {
        HDF_LOGE("encoding request short audio descriptor, input param invalid.");
        return;
    }
    num = (len > HDMI_CEC_MAX_AUDIO_FORMAT_NUM) ? HDMI_CEC_MAX_AUDIO_FORMAT_NUM : len;
    msg->len = HDMI_CEC_GET_MSG_LEN(num * HDMI_CEC_AUDIO_FORMAT_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REQUEST_SHORT_AUDIO_DESCRIPTOR;
    for (i = 0; i < num; i++) {
        msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT + i] = (id[i] << HDMI_CEC_AUDIO_FORMAT_ID_SHIFT) |
            (code[i] & HDMI_CEC_AUDIO_FORMAT_CODE_MARK);
    }
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_REPORT_SHORT_AUDIO_DESCRIPTOR;
    }
}

void HdmiCecEncodingReportShortAudioDescriptorMsg(struct HdmiCecMsg *msg, uint32_t *descriptor, uint32_t len)
{
    uint32_t num, i;

    if (descriptor == NULL || len == 0) {
        HDF_LOGE("encoding report short audio descriptor, input param invalid.");
        return;
    }
    num = (len > HDMI_CEC_MAX_SHORT_AUDIO_DESCRIPTOR_NUM) ? HDMI_CEC_MAX_SHORT_AUDIO_DESCRIPTOR_NUM : len;
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SHORT_AUDIO_DESCRIPTOR_LEN * num);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REPORT_SHORT_AUDIO_DESCRIPTOR;
    for (i = 0; i < num; i++) {
        msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT + i * HDMI_CEC_SHORT_AUDIO_DESCRIPTOR_LEN] =
            (descriptor[i] >> HDMI_TWO_BYTES_SHIFT) & HDMI_ONE_BYTE_MARK;
        msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT + i * HDMI_CEC_SHORT_AUDIO_DESCRIPTOR_LEN] =
            (descriptor[i] >> HDMI_ONE_BYTE_SHIFT) & HDMI_ONE_BYTE_MARK;
        msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT + i * HDMI_CEC_SHORT_AUDIO_DESCRIPTOR_LEN] =
            descriptor[i] & HDMI_ONE_BYTE_MARK;
    }
}

void HdmiCecEncodingSetSystemAudioModeMsg(struct HdmiCecMsg *msg, uint8_t status)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SYSTEM_AUDIO_STATUS_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SET_SYSTEM_AUDIO_MODE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = status;
}

void HdmiCecEncodingSystemAudioModeRequestMsg(struct HdmiCecMsg *msg, uint16_t phyAddr, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SYSTEM_AUDIO_MODE_REQUEST_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (phyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (phyAddr & HDMI_ONE_BYTE_MARK);
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_SET_SYSTEM_AUDIO_MODE;
    }
}

void HdmiCecEncodingSystemAudioModeStatusMsg(struct HdmiCecMsg *msg, uint8_t status)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SYSTEM_AUDIO_STATUS_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = status;
}

void HdmiCecEncodingSetAudioRateMsg(struct HdmiCecMsg *msg, uint8_t rate)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_SET_AUDIO_RATE_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_SET_AUDIO_RATE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = rate;
}

void HdmiCecEncodingInitiateArcMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_INITIATE_ARC;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_REPORT_ARC_INITIATED;
    }
}

void HdmiCecEncodingReportArcInitiatedMsg(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REPORT_ARC_INITIATED;
}

void HdmiCecEncodingReportArcTerminationMsg(struct HdmiCecMsg *msg)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REPORT_ARC_TERMINATION;
}

void HdmiCecEncodingRequestArcInitiationMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REQUEST_ARC_INITIATION;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_INITIATE_ARC;
    }
}

void HdmiCecEncodingRequestArcTerminationMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REQUEST_ARC_TERMINATION;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_TERMINATE_ARC;
    }
}

void HdmiCecEncodingTerminateArcMsg(struct HdmiCecMsg *msg, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_TERMINATE_ARC;
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_REPORT_ARC_TERMINATION;
    }
}

void HdmiCecEncodingRequestCurrentLatencyMsg(struct HdmiCecMsg *msg, uint16_t phyAddr, bool response)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REQUEST_CURRENT_LATENCY_MSG_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REQUEST_CURRENT_LATENCY;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (phyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (phyAddr & HDMI_ONE_BYTE_MARK);
    if (response == true) {
        msg->rspMsg = HDMI_CEC_OPCODE_REPORT_CURRENT_LATENCY;
    }
}

void HdmiCecEncodingReportCurrentLatencyMsg(struct HdmiCecMsg *msg,
    uint16_t phyAddr, struct HdmiCecLatencyInfo *info)
{
    msg->len = HDMI_CEC_GET_MSG_LEN(HDMI_CEC_REPORT_CURRENT_LATENCY_MSG_PARAM_MIN_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_REPORT_CURRENT_LATENCY;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (phyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (phyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = info->videoLatency;
    msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (info->lowLatencyMode << HDMI_CEC_LOW_LATENCY_MODE_SHIFT) |
                                                 (info->audioOutputCompensated);
    /* Operand[Audio Output Delay] is only present when [Audio Output Compensated] is 3. */
    if (info->audioOutputCompensated == HDMI_CEC_AUDIO_OUTPUT_COMPENSATED_PARTIAL_DELAY) {
        msg->len++;
        msg->data[HDMI_CEC_MSG_DATA_SIXTH_ELEMENT] = info->audioOutputDelay;
    }
}

/* CDC Message Encoding. */
void HdmiCdcEncodingHecInquireStateMsg(struct HdmiCecMsg *msg, uint16_t initiatorPhyAddr,
    uint16_t phyAddr1, uint16_t phyAddr2, bool response)
{
    msg->len = HDMI_CDC_GET_MSG_LEN(HDMI_CDC_HEC_INQUIRE_STATE_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CDC_MESSAGE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (initiatorPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (initiatorPhyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = HDMI_CDC_HEC_INQUIRE_STATE;
    msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (phyAddr1 >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_SIXTH_ELEMENT] = (phyAddr1 & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_SEVENTH_ELEMENT] = (phyAddr2 >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_EIGHTH_ELEMENT] = (phyAddr2 & HDMI_ONE_BYTE_MARK);
    if (response == true) {
        msg->rspMsg = HDMI_CDC_HEC_REPORT_STATE;
    }
}

void HdmiCdcEncodingHecReportStateMsg(struct HdmiCecMsg *msg, uint16_t initiatorPhyAddr,
    uint16_t phyAddr, struct HdmiCdcHecState *state)
{
    msg->len = HDMI_CDC_GET_MSG_LEN(HDMI_CDC_HEC_REPORT_STATE_MSG_PARAM_MIN_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CDC_MESSAGE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (initiatorPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (initiatorPhyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = HDMI_CDC_HEC_REPORT_STATE;
    msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (phyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_SIXTH_ELEMENT] = (phyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_SEVENTH_ELEMENT] = (state->hecFuncState << HDMI_CDC_HEC_FUNC_STATE_SHIFT) |
                                                   (state->hostFuncState << HDMI_CDC_HOST_FUNC_STATE_SHIFT) |
                                                   (state->encFuncState << HDMI_CDC_ENC_FUNC_STATE_SHIFT) |
                                                   (state->cdcErrCode);
    if (state->haveHecField == true) {
        msg->len = HDMI_CDC_GET_MSG_LEN(HDMI_CDC_HEC_REPORT_STATE_MSG_PARAM_MAX_LEN);
        msg->data[HDMI_CEC_MSG_DATA_EIGHTH_ELEMENT] = (state->hecField >> HDMI_ONE_BYTE_SHIFT);
        msg->data[DATA_NINTH_OFFSET_ELEMENT] = (state->hecField & HDMI_ONE_BYTE_MARK);
    }
}

void HdmiCdcEncodingHecSetStateAdjacentMsg(struct HdmiCecMsg *msg, uint16_t initiatorPhyAddr,
    uint16_t phyAddr, uint8_t hecSetState, bool response)
{
    msg->len = HDMI_CDC_GET_MSG_LEN(HDMI_CDC_HEC_SET_STATE_ADJACENT_MSG_PARAM_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CDC_MESSAGE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (initiatorPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (initiatorPhyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = HDMI_CDC_HEC_SET_STATE_ADJACENT;
    msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (phyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_SIXTH_ELEMENT] = (phyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_SEVENTH_ELEMENT] = hecSetState;
    if (response == true) {
        msg->rspMsg = HDMI_CDC_HEC_REPORT_STATE;
    }
}

void HdmiCdcEncodingHecSetStateMsg(struct HdmiCecMsg *msg, uint16_t initiatorPhyAddr,
    struct HemiCdcHecStateInfo *info, bool response)
{
    uint32_t i;

    msg->len = HDMI_CDC_GET_MSG_LEN(HDMI_CDC_HEC_SET_STATE_MSG_MIN_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CDC_MESSAGE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (initiatorPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (initiatorPhyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = HDMI_CDC_HEC_SET_STATE;
    msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (info->phyAddr1 >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_SIXTH_ELEMENT] = (info->phyAddr1 & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_SEVENTH_ELEMENT] = (info->phyAddr2 >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_EIGHTH_ELEMENT] = (info->phyAddr2 & HDMI_ONE_BYTE_MARK);
    msg->data[DATA_NINTH_OFFSET_ELEMENT] = info->hecSetState;
    for (i = 0; (i < info->phyAddrLen) && (i < HDMI_CDC_HEC_SET_STATE_MSG_OPTIONAL_PA_MAX_NUM); i++) {
        if (info->phyAddr[i] == HDMI_CEC_INVALID_PHY_ADDR) {
            break;
        }
        msg->data[msg->len] = (info->phyAddr[i] >> HDMI_ONE_BYTE_SHIFT);
        (msg->len)++;
        msg->data[msg->len] = (info->phyAddr[i] & HDMI_ONE_BYTE_MARK);
        (msg->len)++;
    }
    if (response == true) {
        msg->rspMsg = HDMI_CDC_HEC_REPORT_STATE;
    }
}

void HdmiCdcEncodingHecRequestDeactivationMsg(struct HdmiCecMsg *msg, uint16_t initiatorPhyAddr,
    uint16_t *phyAddr, uint32_t len, bool response)
{
    uint32_t i;

    if (phyAddr == NULL || len != HDMI_CDC_HEC_REQUEST_DEACTIVATION_MSG_PHY_ADDR_NUM) {
        return;
    }
    msg->len = HDMI_CDC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CDC_MESSAGE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (initiatorPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (initiatorPhyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = HDMI_CDC_HEC_REQUEST_DEACTIVATION;
    for (i = 0; i < len; i++) {
        msg->data[msg->len] = (phyAddr[i] >> HDMI_ONE_BYTE_SHIFT);
        (msg->len)++;
        msg->data[msg->len] = (phyAddr[i] & HDMI_ONE_BYTE_MARK);
        (msg->len)++;
    }
    if (response == true) {
        msg->rspMsg = HDMI_CDC_HEC_REPORT_STATE;
    }
}

void HdmiCdcEncodingHecNotifyAliveMsg(struct HdmiCecMsg *msg, uint16_t initiatorPhyAddr)
{
    msg->len = HDMI_CDC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CDC_MESSAGE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (initiatorPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (initiatorPhyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = HDMI_CDC_HEC_NOTIFY_ALIVE;
}

void HdmiCdcEncodingHecDiscoverMsg(struct HdmiCecMsg *msg, uint16_t initiatorPhyAddr, bool response)
{
    msg->len = HDMI_CDC_GET_MSG_LEN(0);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CDC_MESSAGE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (initiatorPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (initiatorPhyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = HDMI_CDC_HEC_DISCOVER;
    if (response == true) {
        msg->rspMsg = HDMI_CDC_HEC_REPORT_STATE;
    }
}

void HdmiCdcEncodingHpdSetStateMsg(struct HdmiCecMsg *msg, uint16_t initiatorPhyAddr,
    uint8_t portNum, uint8_t hpdState, bool response)
{
    msg->len = HDMI_CDC_GET_MSG_LEN(HDMI_CDC_HPD_SET_STATE_MSG_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CDC_MESSAGE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (initiatorPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (initiatorPhyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = HDMI_CDC_HPD_SET_STATE;
    msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (portNum << HDMI_CDC_INPUT_PORT_NUMBER_SHIFT) | hpdState;
    if (response == true) {
        msg->rspMsg = CEC_MSG_CDC_HPD_REPORT_STATE;
    }
}

void HdmiCdcEncodingHpdReportStateMsg(struct HdmiCecMsg *msg, uint16_t initiatorPhyAddr,
    uint8_t hpdState, uint8_t errCode)
{
    msg->len = HDMI_CDC_GET_MSG_LEN(HDMI_CDC_HPD_REPORT_STATE_MSG_LEN);
    msg->data[HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT] |= HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST;
    msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT] = HDMI_CEC_OPCODE_CDC_MESSAGE;
    msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] = (initiatorPhyAddr >> HDMI_ONE_BYTE_SHIFT);
    msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT] = (initiatorPhyAddr & HDMI_ONE_BYTE_MARK);
    msg->data[HDMI_CEC_MSG_DATA_FORTH_ELEMENT] = CEC_MSG_CDC_HPD_REPORT_STATE;
    msg->data[HDMI_CEC_MSG_DATA_FIFTH_ELEMENT] = (hpdState << HDMI_CDC_INPUT_PORT_NUMBER_SHIFT) | errCode;
}

static int32_t HdmiCecSendMsg(struct HdmiCntlr *cntlr, struct HdmiCecMsg *msg)
{
    int32_t ret;

    if (cntlr->ops == NULL || cntlr->ops->cecMsgSend == NULL) {
        return HDF_ERR_NOT_SUPPORT;
    }

    if (msg->response == true) {
        msg->timeout = HDMI_CEC_WAIT_RESPONSE_MSG_MAX_TIME;
    }

    HdmiCntlrLock(cntlr);
    ret = cntlr->ops->cecMsgSend(cntlr, msg);
    HdmiCntlrUnlock(cntlr);
    return ret;
}

static bool HdmiCecMsgIgnore(uint8_t opcode, bool unregistered, bool broadcast)
{
    switch (opcode) {
        case HDMI_CEC_OPCODE_GET_CEC_VERSION:
        case HDMI_CEC_OPCODE_ABORT:
        case HDMI_CEC_OPCODE_GIVE_DEVICE_POWER_STATUS:
        case HDMI_CEC_OPCODE_GIVE_OSD_NAME:
            /* Ignore if initiator is Unregistered, because these messages should reply with a directed message. */
            if (unregistered == true) {
                return true;
            }
        /* fallthrough */
        case HDMI_CEC_OPCODE_GIVE_DEVICE_VENDOR_ID:
        case HDMI_CEC_OPCODE_GIVE_FEATURES:
        case HDMI_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:
            /* Ignore if addressing is wrong */
            if (broadcast == true) {
                return true;
            }
            break;
        case HDMI_CEC_OPCODE_USER_CONTROL_PRESSED:
        case HDMI_CEC_OPCODE_USER_CONTROL_RELEASED:
            if (unregistered == true || broadcast == true) {
                return true;
            }
            break;
        case HDMI_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS:
            if (broadcast == false) {
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

static void HdmiCecHandleReportPhyAddressMsg(struct HdmiCntlr *cntlr,
    struct HdmiCecMsg *msg, struct HdmiCecMsg *txMsg)
{
    uint16_t phyAddr;

    phyAddr = ((msg->data[HDMI_CEC_MSG_DATA_SECOND_ELEMENT] << HDMI_ONE_BYTE_SHIFT) |
               msg->data[HDMI_CEC_MSG_DATA_THIRD_ELEMENT]);
    (void)cntlr;
    (void)txMsg;

    HDF_LOGD("reported phy address is  %x.%x.%x.%x, logical address is  %d",
        HDMI_CEC_PHY_ADDR_PHASE(phyAddr), HdmiCecGetMsgInitiator(msg));
}

static void HdmiCecHandleUserControlPrtessedMsg(struct HdmiCntlr *cntlr,
    struct HdmiCecMsg *msg, struct HdmiCecMsg *txMsg)
{
    (void)msg;
    (void)txMsg;

    /* Not support CEC Remote Control. */
    if (cntlr->cap.baseCap.bits.cecRc == 0) {
        return;
    }
    HDF_LOGD("Now User Control Prtessed not support.");
}

static void HdmiCecHandleUserControlReleasedMsg(struct HdmiCntlr *cntlr,
    struct HdmiCecMsg *msg, struct HdmiCecMsg *txMsg)
{
    (void)msg;
    (void)txMsg;

    /* Not support CEC Remote Control. */
    if (cntlr->cap.baseCap.bits.cecRc == 0) {
        return;
    }
    HDF_LOGD("Now User Control Released not support.");
}

static void HdmiCecHandleGetCecVersionMsg(struct HdmiCntlr *cntlr,
    struct HdmiCecMsg *msg, struct HdmiCecMsg *txMsg)
{
    HdmiCecEncodingCecVersionMsg(txMsg, cntlr->cec->info.cecVersion);
    if (HdmiCecSendMsg(cntlr, txMsg) != HDF_SUCCESS) {
        HDF_LOGE("get cec version msg send fail");
    }
}

static void HdmiCecHandleGivePhyAddressMsg(struct HdmiCntlr *cntlr,
    struct HdmiCecMsg *msg, struct HdmiCecMsg *txMsg)
{
    /* Ignore for CEC switches using addr 15. */
    if ((cntlr->cec->info.primaryDeviceType == HDMI_CEC_DEVICE_TYPE_PURE_CEC_SWITCH) &&
        (HdmiCecGetMsgDestination(msg) == HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST)) {
        return;
    }

    HdmiCecEncodingReportPhyAddressMsg(txMsg, cntlr->cec->info.phyAddr, cntlr->cec->info.primaryDeviceType);
    if (HdmiCecSendMsg(cntlr, txMsg) != HDF_SUCCESS) {
        HDF_LOGE("give phy address msg send fail");
    }
}

static void HdmiCecHandleGiveDeviceVendorIdMsg(struct HdmiCntlr *cntlr,
    struct HdmiCecMsg *msg, struct HdmiCecMsg *txMsg)
{
    if (cntlr->cec->info.vendorId == HDMI_CEC_VENDOR_ID_UNKNOWN) {
        HdmiCecEncodingFeatureAbortMsg(txMsg,
                                       msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT],
                                       HDMI_CEC_ABORT_UNRECOGNIZED_OPCODE);
        if (HdmiCecSendMsg(cntlr, txMsg) != HDF_SUCCESS) {
            HDF_LOGE("feature abort msg send fail");
        }
        return;
    }
    HdmiCecEncodingDeviceVendorIdMsg(txMsg, cntlr->cec->info.vendorId);
    if (HdmiCecSendMsg(cntlr, txMsg) != HDF_SUCCESS) {
        HDF_LOGE("give device vendor id msg send fail");
    }
}

static void HdmiCecHandleAbortMsg(struct HdmiCntlr *cntlr,
    struct HdmiCecMsg *msg, struct HdmiCecMsg *txMsg)
{
    /* Ignore for CEC switches. */
    if (cntlr->cec->info.primaryDeviceType == HDMI_CEC_DEVICE_TYPE_PURE_CEC_SWITCH) {
        return;
    }
    HdmiCecEncodingFeatureAbortMsg(txMsg, msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT], HDMI_CEC_ABORT_REFUSED);
    if (HdmiCecSendMsg(cntlr, txMsg) != HDF_SUCCESS) {
        HDF_LOGE("feature abort msg send fail");
    }
}

static void HdmiCecHandleGiveOsdNameMsg(struct HdmiCntlr *cntlr,
    struct HdmiCecMsg *msg, struct HdmiCecMsg *txMsg)
{
    if (cntlr->cec->info.osdName[0] == 0) {
        HdmiCecEncodingFeatureAbortMsg(txMsg,
                                       msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT],
                                       HDMI_CEC_ABORT_UNRECOGNIZED_OPCODE);
        if (HdmiCecSendMsg(cntlr, txMsg) != HDF_SUCCESS) {
            HDF_LOGE("feature abort msg send fail");
        }
        return;
    }

    HdmiCecEncodingSetOsdNameMsg(txMsg, cntlr->cec->info.osdName, cntlr->cec->info.osdNameLen);
    if (HdmiCecSendMsg(cntlr, txMsg) != HDF_SUCCESS) {
        HDF_LOGE("set osd name msg send fail");
    }
}

static void HdmiCecHandleGiveFeaturesMsg(struct HdmiCntlr *cntlr,
    struct HdmiCecMsg *msg, struct HdmiCecMsg *txMsg)
{
    if (cntlr->cec->info.cecVersion < HDMI_CEC_VERSION_2_0) {
        HdmiCecEncodingFeatureAbortMsg(txMsg,
                                       msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT],
                                       HDMI_CEC_ABORT_UNRECOGNIZED_OPCODE);
        if (HdmiCecSendMsg(cntlr, txMsg) != HDF_SUCCESS) {
            HDF_LOGE("feature abort msg send fail");
        }
        return;
    }

    HdmiCecEncodingReportFeaturesMsg(txMsg, &(cntlr->cec->info));
    if (HdmiCecSendMsg(cntlr, txMsg) != HDF_SUCCESS) {
        HDF_LOGE("report feature msg send fail");
    }
}

static void HdmiCecMsgDefaultHandle(struct HdmiCntlr *cntlr,
    struct HdmiCecMsg *msg, struct HdmiCecMsg *txMsg)
{
    bool broadcast = HdmiCecIsBroadcastMsg(msg);
    uint8_t opcode = msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT];
    bool isResponse = (cntlr->cec->info.isWaitingResponse == true && cntlr->cec->info.response == opcode);

    if (broadcast == true || isResponse == true) {
        return;
    }
    if (opcode != HDMI_CEC_OPCODE_FEATURE_ABORT) {
        return;
    }

    HdmiCecEncodingFeatureAbortMsg(txMsg, opcode, HDMI_CEC_ABORT_UNRECOGNIZED_OPCODE);
    if (HdmiCecSendMsg(cntlr, txMsg) != HDF_SUCCESS) {
        HDF_LOGE("feature abort msg send fail");
    }
}

static void HdmiCecMsgHandle(struct HdmiCntlr *cntlr, struct HdmiCecMsg *msg,
    struct HdmiCecMsg *txMsg, uint8_t opcode)
{
    uint32_t i, len;

    struct HdmiCecHandleMsgFuncMap funcMap[] = {
        { HDMI_CEC_OPCODE_GET_CEC_VERSION, HdmiCecHandleGetCecVersionMsg },
        { HDMI_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS, HdmiCecHandleReportPhyAddressMsg },
        { HDMI_CEC_OPCODE_USER_CONTROL_PRESSED, HdmiCecHandleUserControlPrtessedMsg },
        { HDMI_CEC_OPCODE_USER_CONTROL_RELEASED, HdmiCecHandleUserControlReleasedMsg },
        { HDMI_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS, HdmiCecHandleGivePhyAddressMsg },
        { HDMI_CEC_OPCODE_GIVE_DEVICE_VENDOR_ID, HdmiCecHandleGiveDeviceVendorIdMsg },
        { HDMI_CEC_OPCODE_ABORT, HdmiCecHandleAbortMsg },
        { HDMI_CEC_OPCODE_GIVE_OSD_NAME, HdmiCecHandleGiveOsdNameMsg },
        { HDMI_CEC_OPCODE_GIVE_FEATURES, HdmiCecHandleGiveFeaturesMsg }
    };

    len = sizeof(funcMap) / sizeof(funcMap[0]);
    for (i = 0; i < len; i++) {
        if (opcode == funcMap[i].opcode) {
            funcMap[i].func(cntlr, msg, txMsg);
            return;
        }
    }
    HdmiCecMsgDefaultHandle(cntlr, msg, txMsg);
}

static void HdmiCecUpdateResponseFlag(struct HdmiCec *cec, uint8_t opcode)
{
    /* receive response msg. */
    if (cec->info.isWaitingResponse == true && cec->info.response == opcode) {
        cec->info.isWaitingResponse = false;
        cec->info.response = 0;
    }
}

static void HdmiCecReceivedMsgHandle(struct HdmiCntlr *cntlr, struct HdmiCecMsg *msg)
{
    uint8_t opcode = msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT];
    bool broadcast = HdmiCecIsBroadcastMsg(msg);
    bool unregistered = (HdmiCecGetMsgInitiator(msg) == HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST);
    struct HdmiCecMsg txMsg = {0};

    if (HdmiCecMsgIgnore(opcode, unregistered, broadcast) == true) {
        HdmiCecUpdateResponseFlag(cntlr->cec, opcode);
        return;
    }

    HdmiCecFillMsgHeader(&txMsg, msg);
    HdmiCecMsgHandle(cntlr, msg, &txMsg, opcode);
    HdmiCecUpdateResponseFlag(cntlr->cec, opcode);
}

int32_t HdmiCecReceivedMsg(struct HdmiCec *cec, struct HdmiCecMsg *msg)
{
    struct HdmiCntlr *cntlr = NULL;
    uint8_t opcode, initiator, destination;

    if (cec == NULL || cec->priv == NULL || msg == NULL) {
        HDF_LOGE("cec receive msg, input param invalid.");
        return HDF_ERR_INVALID_PARAM;
    }

    cntlr = (struct HdmiCntlr *)cec->priv;
    if (cntlr->cap.baseCap.bits.cec == 0) {
        HDF_LOGD("not support cec.");
        return HDF_ERR_NOT_SUPPORT;
    }

    if (msg->len < HDMI_CEC_MSG_MIN_LEN) {
        HDF_LOGE("cec receive msg, len is error.");
        return HDF_ERR_INVALID_PARAM;
    }

    initiator = HdmiCecGetMsgInitiator(msg);
    destination = HdmiCecGetMsgDestination(msg);
    opcode = msg->data[HDMI_CEC_MSG_DATA_FIRST_ELEMENT];
    /* Check if this message is for us. */
    if ((initiator != HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST) &&
        (HdmiCecLogAddrValid(cntlr->cec, initiator) == true)) {
        return HDF_ERR_INVALID_PARAM;
    }
    if ((HdmiCecIsBroadcastMsg(msg) == false) &&
        (HdmiCecLogAddrValid(cec, destination) == false)) {
        HDF_LOGD("this cec msg is not for us!");
        return HDF_ERR_INVALID_PARAM;
    }

    if (cntlr->cec->info.logAddrMask == 0) {
        return HDF_ERR_NOT_SUPPORT;
    }

    /* CDC Only devices should ignore non-CDC messages. */
    if ((HdmiCecIsCdcOnlyDevice(cec) == true) &&
        (opcode != HDMI_CEC_OPCODE_CDC_MESSAGE)) {
        HDF_LOGD("this cec msg is not cdc msg.");
        return HDF_ERR_NOT_SUPPORT;
    }
    if (HdmiCecCheckMsgLen(cec, msg, opcode) == false) {
        return HDF_ERR_INVALID_PARAM;
    }

    HdmiCecReceivedMsgHandle(cntlr, msg);
    return HDF_SUCCESS;
}
