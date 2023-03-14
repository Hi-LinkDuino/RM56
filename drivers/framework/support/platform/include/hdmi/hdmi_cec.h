/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HDMI_CEC_H
#define HDMI_CEC_H

#include "hdf_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * CEC(Consumer Electronics Control).
 * CEC is a protocol that provides high-level control functions between all of the various audiovisual products
 * in a user's environment.
 */

/*
 * One Touch Play Feature/Routing Control Feature.
 * <Active Source> Used by a new source to indicate that it has started to transmit a stream OR
 * used in response to a <Request Active Source>.
 * Param: [Physical Address], 2 bytes.
 * Broadcast message.
 */
#define HDMI_CEC_OPCODE_ACTIVE_SOURCE 0x82

#define HDMI_CEC_ACTIVE_SOURCE_MSG_PARAM_LEN 2

/*
 * One Touch Play Feature.
 * <Image View On> Sent by a source device to the TV whenever it enters the
 * active state(alternatively it may send <Text View On>).
 * Param: None.
 */
#define HDMI_CEC_OPCODE_IMAGE_VIEW_ON 0x04

/*
 * One Touch Play Feature.
 * <Text View On> As <Image View On>, but should also remove any text, menus and PIP windows from the TV's display.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_TEXT_VIEW_ON 0x0D

/*
 * Routing Control Feature.
 * <Inactive Source> Used by currently active source to inform the TV that it has no video to be presented to the user,
 * or is going into the Standby state as the result of a local user command on the device.
 * Param: [Physical Address], 2 bytes.
 */
#define HDMI_CEC_OPCODE_INACTIVE_SOURCE 0x9D

#define HDMI_CEC_INACTIVE_SOURCE_MSG_PARAM_LEN 2

/*
 * Routing Control Feature.
 * <Request Active Source> Used by a new device to discover the status of the system.
 * Param: None.
 * Broadcast message.
 */
#define HDMI_CEC_OPCODE_REQUEST_ACTIVE_SOURCE 0x85

/*
 * Routing Control Feature.
 * <Routing Change> Sent By CEC Switch when it is manually switched to inform all other devices on the network that
 * the active route below the switch has changed.
 * Param: [Original(Physical) Address], 2 bytes. [New(Physical) Address], 2 bytes.
 * Broadcast message.
 */
#define HDMI_CEC_OPCODE_ROUTING_CHANGE 0x80

#define HDMI_CEC_ROUTING_CHANGE_MSG_PARAM_LEN 4

/*
 * Routing Control Feature.
 * <Routing Infomation> Sent By a CEC Switch to indicate the active route below the switch.
 * Param: [Physical Address], 2 bytes.
 * Broadcast message.
 */
#define HDMI_CEC_OPCODE_ROUTING_INFORMATION 0x81

#define HDMI_CEC_ROUTING_INFORMATIO_MSG_PARAM_LEN 2

/*
 * Routing Control Feature.
 * <Set Stream Path> Used by the TV to request a streaming path from the specified Physical Address.
 * Param: [Physical Address], 2 bytes.
 * Broadcast message.
 */
#define HDMI_CEC_OPCODE_SET_STREAM_PATH 0x86

#define HDMI_CEC_SET_STREAM_PATH_MSG_PARAM_LEN 2

/*
 * Routing Control Feature.
 * <Standby> Switches one or all devices into the Standby state.
 * Can be used as a broadcast message or be addressed to a specific device.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_STANDBY 0x36

/*
 * One Touch Record Feature.
 * <Record Off> Request a device stop a recording.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_RECORD_OFF 0x0B

/*
 * One Touch Record Feature.
 * <Record On> Attempt to record the specified source.
 * Param: [Record Source], 1~8 bytes.
 */
#define HDMI_CEC_OPCODE_RECORD_ON 0x09

/* Record Source Type, 1 byte. */
#define HDMI_CEC_RECORD_SOURCE_TYPE_LEN 1
enum HdmiCecRecordSourceType {
    HDMI_CEC_RECORD_SRC_OWN = 1,
    HDMI_CEC_RECORD_SRC_DIGITAL = 2,
    HDMI_CEC_RECORD_SRC_ANALOG = 3,
    HDMI_CEC_RECORD_SRC_EXT_PLUG = 4,
    HDMI_CEC_RECORD_SRC_EXT_PHY_ADDR = 5,
};

/*
 * Digital Service Identification. 7 bytes.
 * [Service Identification Method] 1 bit
 * [Digital Broadcast System] 7 bits
 * [Service Identification] 6 bytes
 */
#define HDMI_CEC_DIGITAL_SERVICE_ID_LEN 7
#define HDMI_CEC_SERVICE_ID_METHOD_BY_DIGITAL_IDS 0
#define HDMI_CEC_SERVICE_ID_METHOD_BY_CHANNEL 1

enum HdmiCecDigitalBroadcastSystem {
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ARIB_GEN = 0x00,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ATSC_GEN = 0x01,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_DVB_GEN = 0x02,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ARIB_BS = 0x08,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ARIB_CS = 0x09,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ARIB_T = 0x0A,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ATSC_CABLE = 0x10,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ATSC_SATELLITE = 0x11,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_ATSC_TERRESTRIAL = 0x12,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_DVB_C = 0x18,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_DVB_S = 0x19,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_DVB_S2 = 0x1A,
    HDMI_CEC_DIG_SERVICE_BCAST_SYSTEM_DVB_T = 0x1B,
};

/*
 * Service Identification, 6 bytes.
 * [ARIB Data] | [ATSC Data] | [DVB Data] | [Channel Data]
 * Channel Data, 6 bytes.
 * [Channel Identifier] 4 bytes, [Reserved] 2 bytes.
 * Channel Identifier, 4 bytes.
 * [Channel Number Format] 6 bits.
 * [Major Channel Number] 10 bits.
 * [Minor Channel Number] 16 bits
 */
#define HDMI_CEC_CHANNEL_NUMBER_FORMAT_1_PART 0x01
#define HDMI_CEC_CHANNEL_NUMBER_FORMAT_2_PART 0x02
#define HDMI_CEC_CHANNEL_NUMBER_FORMAT_SHIFT 2

struct HdmiCecDigitalServiceAribData {
    uint16_t transportId;
    uint16_t serviceId;
    uint16_t orgNetworkId;
};

struct HdmiCecDigitalServiceAtscData {
    uint16_t transportId;
    uint16_t programNumber;
};

struct HdmiCecDigitalServiceDvbData {
    uint16_t transportId;
    uint16_t serviceId;
    uint16_t orgNetworkId;
};

#define HDMI_CEC_CHANNEL_IDENTIFIER_LEN 4
struct HdmiCecChannelIdentifier {
    uint8_t format;
    uint16_t major;
    uint16_t minor;
};

#define HDMI_CEC_DIGITAL_SERVICE_ID_METHOD_SHIFT 7

struct HdmiCecDigitalServiceId {
    uint8_t method;
    uint8_t system;
    union {
        struct HdmiCecDigitalServiceAribData arib;
        struct HdmiCecDigitalServiceAtscData atsc;
        struct HdmiCecDigitalServiceDvbData dvb;
        struct HdmiCecChannelIdentifier channel;
    } systemData;
};

/*
 * Record Source.
 * {[Record Source Type]} | {[Record Source Type] [Digital Service Identification]} |
 * {[Record Source Type] [Analogue Broadcast Type] [Analogue Frequency] [Broadcast System]} |
 * {[Record Source Type] [External Plug]} | {[Record Source Type] [External Physical Address]}
 */
#define HDMI_CEC_ANALOGUE_BROADCAST_TYPE_LEN 1
#define HDMI_CEC_ANALOGUE_FREQ_LEN 2
#define HDMI_CEC_BROADCAST_SYSTEM_LEN 1
#define HDMI_CEC_EXT_PLUG_LEN 1
#define HDMI_CEC_EXT_PHY_ADDRESS_LEN 2
#define HDMI_CEC_RECORD_ON_OWN_MSG_PARAM_LEN HDMI_CEC_RECORD_SOURCE_TYPE_LEN
#define HDMI_CEC_RECORD_ON_DIGITAL_MSG_PARAM_LEN (HDMI_CEC_RECORD_SOURCE_TYPE_LEN + HDMI_CEC_DIGITAL_SERVICE_ID_LEN)
#define HDMI_CEC_RECORD_ON_ANALOG_MSG_PARAM_LEN (HDMI_CEC_RECORD_SOURCE_TYPE_LEN + \
    HDMI_CEC_ANALOGUE_BROADCAST_TYPE_LEN + HDMI_CEC_ANALOGUE_FREQ_LEN + HDMI_CEC_BROADCAST_SYSTEM_LEN)
#define HDMI_CEC_RECORD_ON_EXT_PLUG_MSG_PARAM_LEN (HDMI_CEC_RECORD_SOURCE_TYPE_LEN + HDMI_CEC_EXT_PLUG_LEN)
#define HDMI_CEC_RECORD_ON_EXT_PHY_ADDR_MSG_PARAM_LEN (HDMI_CEC_RECORD_SOURCE_TYPE_LEN + HDMI_CEC_EXT_PHY_ADDRESS_LEN)

struct HdmiCecRecordSource {
    uint8_t type;
    union {
        struct HdmiCecDigitalServiceId id;
        struct {
            uint8_t anaBcastType;
            uint16_t anaFreq;
            uint8_t bcstSystem;
        } analog;
        uint8_t extPlug;
        uint16_t extPhyAddr;
    } data;
};

/*
 * One Touch Record Feature.
 * <Record Status> Used by a Recording Device to inform the Initiator of the message <Record On> about its status.
 * Param: [Record Status Info], 1 byte.
 */
#define HDMI_CEC_OPCODE_RECORD_STATUS 0x0A

#define HDMI_CEC_RECORD_STATUS_MSG_PARAM_LEN 1

/* Record Status Info */
enum HdmiCecRecordStatusInfo {
    HDMI_CEC_RECORD_STATUS_CUR_SRC = 0x01,
    HDMI_CEC_RECORD_STATUS_DIG_SERVICE = 0x02,
    HDMI_CEC_RECORD_STATUS_ANA_SERVICE = 0x03,
    HDMI_CEC_RECORD_STATUS_EXT_INPUT = 0x04,
    HDMI_CEC_RECORD_STATUS_NO_DIG_SERVICE = 0x05,
    HDMI_CEC_RECORD_STATUS_NO_ANA_SERVICE = 0x06,
    HDMI_CEC_RECORD_STATUS_NO_SERVICE = 0x07,
    HDMI_CEC_RECORD_STATUS_INVALID_EXT_PLUG = 0x09,
    HDMI_CEC_RECORD_STATUS_INVALID_EXT_PHYS_ADDR = 0x0A,
    HDMI_CEC_RECORD_STATUS_UNSUP_CA = 0x0B,
    HDMI_CEC_RECORD_STATUS_NO_CA_ENTITLEMENTS = 0x0C,
    HDMI_CEC_RECORD_STATUS_NO_COPY_SRC = 0x0D,
    HDMI_CEC_RECORD_STATUS_NO_MORE_COPIES = 0x0E,
    HDMI_CEC_RECORD_STATUS_NO_MEDIA = 0x10,
    HDMI_CEC_RECORD_STATUS_PLAYING = 0x11,
    HDMI_CEC_RECORD_STATUS_ALREADY_RECORDING = 0x12,
    HDMI_CEC_RECORD_STATUS_MEDIA_PROTECTED = 0x13,
    HDMI_CEC_RECORD_STATUS_NO_SRC_SIGNAL = 0x14,
    HDMI_CEC_RECORD_STATUS_MEDIA_PROBLEM = 0x15,
    HDMI_CEC_RECORD_STATUS_NO_SPACE = 0x16,
    HDMI_CEC_RECORD_STATUS_PARENTAL_LOCK = 0x17,
    HDMI_CEC_RECORD_STATUS_TERMINATED_OK = 0x1A,
    HDMI_CEC_RECORD_STATUS_ALREADY_TERMINATED = 0x1B,
    HDMI_CEC_RECORD_STATUS_OTHER = 0x1F,
};

/*
 * One Touch Record Feature.
 * <Record TV Screen> Request by The Recording Device to record the presently displayed source.
 */
#define HDMI_CEC_OPCODE_RECORD_TV_SCREEN 0x0F

/*
 * Timer Programming Feature.
 * <Clear Analogue Timer> Used to clear an Analogue timer block of a device.
 * Param: see <Set Analogue Timer> param.
 */
#define HDMI_CEC_OPCODE_CLEAR_ANALOGUE_TIMER 0x33

/*
 * Timer Programming Feature.
 * <Clear Digital Timer> Used to clear a Digital timer block of a device.
 * Param: see <Set Digital Timer> param.
 */
#define HDMI_CEC_OPCODE_CLEAR_DIGITAL_TIMER 0x99

/*
 * Timer Programming Feature.
 * <Clear External Timer> Used to clear a External timer block of a device.
 * Param: see <Set External Timer> param.
 */
#define HDMI_CEC_OPCODE_CLEAR_EXTERNAL_TIMER 0xA1

/*
 * Timer Programming Feature.
 * <Set Analogue Timer> Used to set a single timer block on an Analogue Recording device.
 * Param: [Day of Month], 1 byte; [Month of Year], 1 byte; [Start Time], 2 byte; [Duration], 2 byte;
 * [Recording Sequence], 1 byte; [Analogue Broadcast Type], 1 byte; [Analogue Frequency], 2 byte;
 * [Broadcast System], 1 byte.
 */
#define HDMI_CEC_OPCODE_SET_ANALOGUE_TIMER 0x34

/* Analogue Broadcast Type, 1 byte. */
enum HdmiCecAnalogueBroadcastType {
    HDMI_CEC_ANALOGUE_BROADCAST_CABLE = 0,
    HDMI_CEC_ANALOGUE_BROADCAST_SATELITE = 1,
    HDMI_CEC_ANALOGUE_BROADCAST_TERRESTRIAL = 2,
};

/* Recording Sequence, 8 bits. */
enum HdmiCecRecordingSequence {
    HDMI_CEC_RECORDING_SEQ_ONCE_ONLY = 0x00,
    HDMI_CEC_RECORDING_SEQ_SUNDAY = 0x01,
    HDMI_CEC_RECORDING_SEQ_MONDAY = 0x02,
    HDMI_CEC_RECORDING_SEQ_TUESDAY = 0x04,
    HDMI_CEC_RECORDING_SEQ_WEDNESDAY = 0x08,
    HDMI_CEC_RECORDING_SEQ_THURSDAY = 0x10,
    HDMI_CEC_RECORDING_SEQ_FRIDAY = 0x20,
    HDMI_CEC_RECORDING_SEQ_SATERDAY = 0x40,
};

#define HDMI_CEC_ANALOGUE_TIMER_INFO_LEN 11
/* Hour and Minute are in BCD format. */
#define HDMI_CEC_BCD_FORMAT_TIME(time) ((((time) / 10) << 4) | ((time) % 10))

#define HDMI_CEC_COMM_TIMER_INFO_LEN 7
struct HdmiCecCommTimerInfo {
    uint8_t data;
    uint8_t month;
    uint8_t startHour;
    uint8_t startMinute;
    uint8_t durationHour;
    uint8_t durationMinute;
    uint8_t recordingSeq;
};

struct HdmiCecAnalogueTimerInfo {
    struct HdmiCecCommTimerInfo commInfo;
    uint8_t anaBcastType;
    uint16_t anaFreq;
    uint8_t bcstSystem;
};

/*
 * Timer Programming Feature.
 * <Set Digital Timer> Used to set a single timer block on a Digital Recording device.
 * Param: [Day of Month], 1 byte; [Month of Year], 1 byte; [Start Time], 2 byte; [Duration], 2 byte;
 * [Recording Sequence], 1 byte; [Digtial Service Identification], 7 byte.
 */
#define HDMI_CEC_OPCODE_SET_DIGITAL_TIMER 0x97

#define HDMI_CEC_DIGITAL_TIMER_INFO_LEN 14
struct HdmiCecDigitalTimerInfo {
    struct HdmiCecCommTimerInfo commInfo;
    struct HdmiCecDigitalServiceId id;
};

/*
 * Timer Programming Feature.
 * <Set External Timer> Used to set a single timer block to record from an External device.
 * Param: [Day of Month], 1 byte; [Month of Year], 1 byte; [Start Time], 2 byte; [Duration], 2 byte;
 * [Recording Sequence], 1 byte; [External Source Specifier], 1 byte; [External Plug], 1 byte;
 * [External Physical address], 2 byte.
 */
#define HDMI_CEC_OPCODE_SET_EXTERNAL_TIMER 0xA2

#define HDMI_CEC_EXTERNAL_TIMER_INFO_LEN 11

/* External Source Specifier */
#define HDMI_CEC_EXT_PLUG 4
#define HDMI_CEC_EXT_PHYS_ADDR 5

struct HdmiCecExternalTimerInfo {
    struct HdmiCecCommTimerInfo commInfo;
    uint8_t extSrcSpec;
    uint8_t extPlug;
    uint16_t extPhyAddr;
};

/*
 * Timer Programming Feature.
 * <Set Timer Program Title> Used to set the name of a program associate with a timer block.
 * Sent directly after sending a <Set Analogue Timer> or <Set Digital Timer> message.
 * The name is then associated with that timer block.
 * Param: [Program Title String], 1~14 bytes.
 */
#define HDMI_CEC_OPCODE_SET_TIMER_PROGRAM_TITLE 0x67

#define HDMI_CEC_PROGRAM_TITLE_STR_MIN_LEN 1
#define HDMI_CEC_PROGRAM_TITLE_STR_MAX_LEN 14

/*
 * Timer Programming Feature.
 * <Timer Clear Status> Used to give the status of a <Clear Analogue Timer>,
 * <Clear Digital Timer> or <Clear External Timer> message.
 * Param: [Timer Cleared Status Data], 1 byte.
 */
#define HDMI_CEC_OPCODE_TIMER_CLEARED_STATUS 0x43

#define HDMI_CEC_TIMER_CLEARED_STATUS_MSG_PARAM_LEN 1

/* Timer Cleared Status Data */
enum HdmiCecTimerClearedStatus {
    HDMI_CEC_TIMER_CLR_STAT_NO_CLEARED_RECORDING = 0x00,
    HDMI_CEC_TIMER_CLR_STAT_NO_CLEARED_NO_MATCHING = 0x01,
    HDMI_CEC_TIMER_CLR_STAT_NO_CLEARED_NO_INFO = 0x02,
    HDMI_CEC_TIMER_CLR_STAT_CLEARED = 0x80,
};

/*
 * Timer Programming Feature.
 * <Timer Status> Used to send timer status to the Initiator of a <Set Timer> message.
 * Param: [Timer Status Data], 1 byte or 3 bytes.
 */
#define HDMI_CEC_OPCODE_TIMER_STATUS 0x35

#define HDMI_CEC_TIMER_STATUS_DATA_MIN_LEN 1
#define HDMI_CEC_TIMER_STATUS_DATA_MAX_LEN 3

/*
 * Timer Status Data. 1 byte or 3 bytes.
 * [Timer overlap warning] 1bit.
 * [Media Info] 2 bits.
 * [Timer Progremmed Info] 5 bits or 21 bits.
 */
#define HDMI_CEC_TIMER_OVERLAP_WARNING_NO_OVERLAP 0
#define HDMI_CEC_TIMER_OVERLAP_WARNING_TIMER_BLOCK_OVERLAP 1
#define HDMI_CEC_TIMER_OVERLAP_WARNING_SHIFT 7

#define HDMI_CEC_MEDIA_INFO_SHIFT 5
enum HdmiCecMediaInfo {
    HDMI_CEC_MEDIA_PRESENT_NOT_POTECTED = 0,
    HDMI_CEC_MEDIA_PRESENT_POTECTED = 1,
    HDMI_CEC_MEDIA_NOT_PRESENT = 2,
    HDMI_CEC_MEDIA_BUTT,
};

/*
 * Timer Progremmed Info, 5 bits or 21 bits.
 * [Progremmed Indicator], 1 bit.
 * {[Progremmed Info], 4 bits. [Duration Available], 2bytes.} |
 * {[Not Progremmed Error Info], 4 bits. [Duration Available], 2bytes.}
 */
/* Progremmed Indicator, 1 bit */
#define HDMI_CEC_PROG_IND_NOT_PROGRAMMED 0
#define HDMI_CEC_PROG_IND_PROGRAMMED 1
#define HDMI_CEC_PROG_IND_SHIFT 4

/* Progremmed Info, 4 bits */
enum HdmiCecProgremmedInfo {
    HDMI_CEC_PROGRAMMED_INFO_ENOUGH_SPAC = 0x08,
    HDMI_CEC_PROGRAMMED_INFO_NOT_ENOUGH_SPAC = 0x09,
    HDMI_CEC_PROGRAMMED_INFO_NO_MEDIA_INFO = 0x0A,
    HDMI_CEC_PROGRAMMED_INFO_MIGHT_NOT_BE_ENOUGH_SPACE = 0x0B,
};

/* Not Progremmed Error Info */
enum HdmiCecNotProgremmedErrorInfo {
    HDMI_CEC_NOT_PROG_ERR_NO_FREE_TIMER = 0x01,
    HDMI_CEC_NOT_PROG_ERR_DATE_OUT_OF_RANGE = 0x02,
    HDMI_CEC_NOT_PROG_ERR_RECORDING_SEQ_ERROR = 0x03,
    HDMI_CEC_NOT_PROG_ERR_INVALID_EXT_PLUG = 0x04,
    HDMI_CEC_NOT_PROG_ERR_INVALID_EXT_PHYS_ADDR = 0x05,
    HDMI_CEC_NOT_PROG_ERR_CA_UNSUPP = 0x06,
    HDMI_CEC_NOT_PROG_ERR_INSUF_CA_ENTITLEMENTS = 0x07,
    HDMI_CEC_NOT_PROG_ERR_RESOLUTION_UNSUPP = 0x08,
    HDMI_CEC_NOT_PROG_ERR_PARENTAL_LOCK = 0x09,
    HDMI_CEC_NOT_PROG_ERR_CLOCK_FAILURE = 0x0A,
    HDMI_CEC_NOT_PROG_ERR_DUPLICATE = 0x0E,
};

struct HdmiCecTimerProgremmedInfo {
    uint8_t indicator;
    uint8_t info;       /* Progremmed Info/Not Progremmed Error Info */
    uint8_t durationHour;
    uint8_t durationMinute;
};

struct HdmiCecTimerStatusData {
    uint8_t timerOverlap;
    uint8_t mediaInfo;
    struct HdmiCecTimerProgremmedInfo progInfo;
};

/*
 * System Information Feature/Vendor Specific Commands Feature.
 * <CEC Version> Used to indicate the version number of the CEC Specification which was used to design the device,
 * in response to a <Get CEC Version>.
 * Param: [CEC Version], 1 byte.
 */
#define HDMI_CEC_OPCODE_CEC_VERSION 0x9E

#define HDMI_CEC_CEC_VERSION_MSG_PARAM_LEN 1

/* CEC Version */
#define HDMI_CEC_VERSION_1_3A 4
#define HDMI_CEC_VERSION_1_4 5
#define HDMI_CEC_VERSION_2_0 6

/*
 * System Information Feature/Vendor Specific Commands Feature.
 * <Get CEC Version> Used by a device to enquire which version number of the CEC Specification
 * was used to design the Follwer device.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_GET_CEC_VERSION 0x9F

/*
 * System Information Feature.
 * <Give Physical Address> A request to a device return its Physical Address.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_GIVE_PHYSICAL_ADDRESS 0x83

/*
 * System Information Feature.
 * <Get Menu Language> Sent by a device capable of character generation(for OSD and Menus) to a TV in order to
 * discover the currently selected Menu language on the TV.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_GET_MENU_LANGUAGE 0x91

/*
 * System Information Feature.
 * <Report Physical Address> Used to inform all other devices of the mapping between
 * physical and Logical address of the Initiator.
 * Param: [Physical Address], 2 bytes. [Device Type], 1 byte.
 * Broadcast message.
 */
#define HDMI_CEC_OPCODE_REPORT_PHYSICAL_ADDRESS 0x84

#define HDMI_CEC_REPORT_PHYSICAL_ADDRESS_MSG_PARAM_LEN 3

/*
 * Device Type/Primary Device Type.
 * The HDMI 1.4b operand [Device Type] has been extended as [Primary Device Type].
 */
enum HdmiCecDeviceType {
    HDMI_CEC_DEVICE_TYPE_TV = 0,
    HDMI_CEC_DEVICE_TYPE_RECORDING_DEVICE = 1,
    HDMI_CEC_DEVICE_TYPE_RESERVED = 2,
    HDMI_CEC_DEVICE_TYPE_TUNER = 3,
    HDMI_CEC_DEVICE_TYPE_PLAYBACK_DEVICE = 4,
    HDMI_CEC_DEVICE_TYPE_AUDIO_SYSTEM = 5,
    HDMI_CEC_DEVICE_TYPE_PURE_CEC_SWITCH = 6,
    HDMI_CEC_DEVICE_TYPE_PROCESSOR = 7,
};

/*
 * System Information Feature.
 * <Set Menu Language> Used by a TV to indicate its currently selected menu language.
 * Param: [Language], 3 bytes.
 * Broadcast message.
 */
#define HDMI_CEC_OPCODE_SET_MENU_LANGUAGE 0x32

#define HDMI_CEC_SET_MENU_LANGUAGE_MSG_PARAM_LEN 3

/*
 * System Information Feature.
 * <Report Features> Used by a device to announce its version, features and device type.
 * Param: [CEC Version], 1 byte. [All Device Types], 1 byte. [RC Profile], 1..N bytes. [Device Features], 1..N bytes.
 */
#define HDMI_CEC_OPCODE_REPORT_FEATURES 0xA6

#define HDMI_CEC_REPORT_FEATURES_MSG_PARAM_MIN_LEN 4

/* All Device Types */
enum HdmiCecAllDevType {
    HDMI_CEC_ALL_DEV_TYPE_CEC_SWITCH = 0x04,
    HDMI_CEC_ALL_DEV_TYPE_AUDIO_SYSTEM = 0x08,
    HDMI_CEC_ALL_DEV_TYPE_PLAYBACK = 0x10,
    HDMI_CEC_ALL_DEV_TYPE_TUNER = 0x20,
    HDMI_CEC_ALL_DEV_TYPE_RECORD = 0x40,
    HDMI_CEC_ALL_DEV_TYPE_TV = 0x80,
};

/*
 * RC Profile, 1..N bytes.
 * [RC Profile 1]; or [RC Profile 1]..[RC Profile N](where N >= 2)
 * RC Profile 1, 1 byte.
 * [RC Profile Externsion], bit 7
 * if bit6 is 0, [RC Profile TV] in bit5-0; if bit6 is 1, [RC Profile Source] in bit5-0.
 */
#define HDMI_CEC_RC_PROFILE_MAX_NUM 6
#define HDMI_CEC_RC_PROFILE_EXTERNSION_MARK (1 << 7)
#define HDMI_CEC_RC_PROFILE_TV_OR_SOURCE_MARK (1 << 6)

/* RC Profile Source, 6 bits. */
#define HDMI_CEC_RC_PROFILE_SOURCE_ROOT_MENU (1 << 4)
#define HDMI_CEC_RC_PROFILE_SOURCE_SETUP_MENU (1 << 3)
#define HDMI_CEC_RC_PROFILE_SOURCE_CONTENTS_MENU (1 << 2)
#define HDMI_CEC_RC_PROFILE_SOURCE_MEDIA_TOP_MENU (1 << 1)
#define HDMI_CEC_RC_PROFILE_SOURCE_MEDIA_CONTEXT_SENSITIVE_MENU (1 << 0)

/*
 * RC Profile TV, 6 bits.
 * [RC Profile ID], Bit3-0.
 */
enum HdmiCecRcProfileId {
    HDMI_CEC_RC_PROFILE_NONE = 0x00,
    HDMI_CEC_RC_PROFILE_1 = 0x02,
    HDMI_CEC_RC_PROFILE_2 = 0x06,
    HDMI_CEC_RC_PROFILE_3 = 0x0A,
    HDMI_CEC_RC_PROFILE_4 = 0x0E,
    HDMI_CEC_RC_PROFILE_BUTT,
};

/*
 * Device Features, 1..N bytes.
 * [Device Features 1], 1 byte.
 */
#define HDMI_CEC_DEVICE_FEATURES_MAX_NUM 6
#define HDMI_CEC_DEVICE_FEATURES_EXTERNSION_MARK (1 << 7)  /* 0 the last; 1 continues. */
#define HDMI_CEC_DEVICE_FEATURES_TV_SUPP_RECORD_TV_SCREEN (1 << 6)
#define HDMI_CEC_DEVICE_FEATURES_TV_SUPP_SET_OSD_STRING (1 << 5)
#define HDMI_CEC_DEVICE_FEATURES_DECK_SUPP_CONTROL (1 << 4)
#define HDMI_CEC_DEVICE_FEATURES_SRC_SUPP_SET_AUDIO_RATE (1 << 3)
#define HDMI_CEC_DEVICE_FEATURES_SINK_SUPP_ARC_TX (1 << 2)
#define HDMI_CEC_DEVICE_FEATURES_SRC_SUPP_ARC_RX (1 << 1)

/*
 * System Information Feature.
 * <Give Features> Used by a device to request another device's features.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_GIVE_FEATURES 0xA5

/*
 * Deck Control Feature.
 * <Deck Control> Used to control a device's media functions.
 * Param: [Deck Control Mode], 1 byte.
 */
#define HDMI_CEC_OPCODE_DECK_CONTROL 0x42

#define HDMI_CEC_DECK_CONTROL_MSG_PARAM_LEN 1

/* Deck Control Mode */
enum HdmiCecDeckControlMode {
    HDMI_CEC_DECK_CONTROL_MODE_SKIP_FWD = 1,
    HDMI_CEC_DECK_CONTROL_MODE_SKIP_REV = 2,
    HDMI_CEC_DECK_CONTROL_MODE_STOP = 3,
    HDMI_CEC_DECK_CONTROL_MODE_EJECT = 4,
};

/*
 * Deck Control Feature.
 * <Deck Status> Used to provide a deck's status to the indicate of the <Give Deck Status> message.
 * Param: [Deck Info], 1 byte.
 */
#define HDMI_CEC_OPCODE_DECK_STATUS 0x1B

#define HDMI_CEC_DECK_STATUS_MSG_PARAM_LEN 1

/* Deck Info. Indicate the current status of a tape or disk deck. */
enum HdmiCecDeckInfo {
    HDMI_CEC_DECK_PLAY = 0x11,
    HDMI_CEC_DECK_RECORD = 0x12,
    HDMI_CEC_DECK_PLAY_REVERSE = 0x13,
    HDMI_CEC_DECK_STILL = 0x14,
    HDMI_CEC_DECK_SLOW = 0x15,
    HDMI_CEC_DECK_SLOW_REVERSE = 0x16,
    HDMI_CEC_DECK_FAST_FORWARD = 0x17,
    HDMI_CEC_DECK_FAST_REVERSE = 0x18,
    HDMI_CEC_DECK_NO_MEDIA = 0x19,
    HDMI_CEC_DECK_STOP = 0x1A,
    HDMI_CEC_DECK_SKIP_FORWARD = 0x1B,
    HDMI_CEC_DECK_SKIP_REVERSE = 0x1C,
    HDMI_CEC_DECK_INDEX_SEARCH_FORWARD = 0x1D,
    HDMI_CEC_DECK_INDEX_SEARCH_REVERSE = 0x1E,
    HDMI_CEC_DECK_OTHER_STATUS = 0x1F,
};

/*
 * Deck Control Feature.
 * <Give Deck Status> Used to request the status of a device,
 * regardless of whether or not it is the current active source.
 * Param: [Status Request], 1 byte.
 */
#define HDMI_CEC_OPCODE_GIVE_DECK_STATUS 0x1A

#define HDMI_CEC_GIVE_DECK_STATUS_MSG_PARAM_LEN 1

/* Status Request */
enum HdmiCecStatusRequest {
    HDMI_CEC_STATUS_REQUEST_ON = 1,
    HDMI_CEC_STATUS_REQUEST_OFF = 2,
    HDMI_CEC_STATUS_REQUEST_ONCE = 3,
};

/*
 * Deck Control Feature.
 * <Play> Used to control the playback behavior of a source device.
 * Param: [Play Mode], 1 byte.
 */
#define HDMI_CEC_OPCODE_PLAY 0x41

#define HDMI_CEC_PLAY_MSG_PARAM_LEN 1

/* Play Mode */
enum HdmiCecPlayMode {
    HDMI_CEC_PLAY_MODE_PLAY_FAST_FWD_MIN = 0x05,
    HDMI_CEC_PLAY_MODE_PLAY_FAST_FWD_MED = 0x06,
    HDMI_CEC_PLAY_MODE_PLAY_FAST_FWD_MAX = 0x07,
    HDMI_CEC_PLAY_MODE_PLAY_FAST_REV_MIN = 0x09,
    HDMI_CEC_PLAY_MODE_PLAY_FAST_REV_MED = 0x0A,
    HDMI_CEC_PLAY_MODE_PLAY_FAST_REV_MAX = 0x0B,
    HDMI_CEC_PLAY_MODE_PLAY_SLOW_FWD_MIN = 0x15,
    HDMI_CEC_PLAY_MODE_PLAY_SLOW_FWD_MED = 0x16,
    HDMI_CEC_PLAY_MODE_PLAY_SLOW_FWD_MAX = 0x17,
    HDMI_CEC_PLAY_MODE_PLAY_SLOW_REV_MIN = 0x19,
    HDMI_CEC_PLAY_MODE_PLAY_SLOW_REV_MED = 0x1A,
    HDMI_CEC_PLAY_MODE_PLAY_SLOW_REV_MAX = 0x1B,
    HDMI_CEC_PLAY_MODE_PLAY_REV = 0x20,
    HDMI_CEC_PLAY_MODE_PLAY_FWD = 0x24,
    HDMI_CEC_PLAY_MODE_PLAY_STILL = 0x25,
};

/*
 * Tuner Control Feature.
 * <Give Tuner Device Status> Used to request the status of a tuner device.
 * Param: [Status Request], 1 byte.
 */
#define HDMI_CEC_OPCODE_GIVE_TUNER_DEVICE_STATUS 0x08

#define HDMI_CEC_GIVE_TUNER_DEVICE_STATU_MSG_PARAM_LEN 1

/*
 * Tuner Control Feature.
 * <Select Analogue Service> Directly selects an Analogue TV service.
 * Param: [Analogue Broadcast Type], 1 byte. [Analogue Frequency], 2 bytes. [Broadcast System], 1 byte.
 */
#define HDMI_CEC_OPCODE_SELECT_ANALOGUE_SERVICE 0x92

#define HDMI_CEC_SELECT_ANALOGUE_SERVICE_MSG_PARAM_LEN 4

/*
 * Tuner Control Feature.
 * <Select Digital Service> Directly selects a Digital TV, Radio or Data Briadcast service.
 * Param: [Digital Service Identification], 7 bytes.
 */
#define HDMI_CEC_OPCODE_SELECT_DIGITAL_SERVICE 0x93

#define HDMI_CEC_SELECT_DIGITAL_SERVICE_MSG_PARAM_LEN HDMI_CEC_DIGITAL_SERVICE_ID_LEN

/*
 * Tuner Control Feature.
 * <Tuner Device Status> Used by a tuner device to provide its status to the initiator of
 * the <Give Tuner Device Status> message.
 * Param: [Tuner Device Info], 5 bytes(analogue service), 8 bytes(digital service).
 */
#define HDMI_CEC_OPCODE_TUNER_DEVICE_STATUS 0x07

#define HDMI_CEC_TUNER_DEVICE_STATUS_MSG_ANA_PARAM_LEN 5
#define HDMI_CEC_TUNER_DEVICE_STATUS_MSG_DIG_PARAM_LEN 8

/*
 * Tuner Device Info, 5 bytes(analogue service), 8 bytes(digital service).
 * [Recording Flag], 1 bit. [Tuner Display Info], 7 bits.
 * {[Analogue Broadcast Type], 1 byte. [Analogue Frequency], 2 bytes. [Broadcast System], 1 byte.} |
 * {[Digital Service Identification], 7 bytes}.
 */
#define HDMI_CEC_RECORDING_FALG_NOT_USED 0
#define HDMI_CEC_RECORDING_FALG_USED 1
#define HDMI_CEC_RECORDING_FALG_SHIFT 7

enum HdmiCecTunerDisplayInfo {
    HDMI_CEC_TUNER_DISP_DIGITAL = 0,
    HDMI_CEC_TUNER_DISP_NOT_DISP = 1,
    HDMI_CEC_TUNER_DISP_ANALOGUE = 2,
};

struct HdmiCecTunerDeviceInfo {
    uint8_t recordingFlag;
    uint8_t dispInfo;
    bool isAnalogService;
    union {
        struct HdmiCecDigitalServiceId digital;
        struct {
            uint8_t anaBcastType;
            uint16_t anaFreq;
            uint8_t bcstSystem;
        } analog;
    } data;
};

/*
 * Tuner Control Feature.
 * <Tuner Step Decrement> Used to tune to next lowest service in a tuner's service list. Can used for PIP.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_TUNER_STEP_DECREMENT 0x06

/*
 * Tuner Control Feature.
 * <Tuner Step Increment> Used to tune to next highest service in a tuner's service list. Can used for PIP.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_TUNER_STEP_INCREMENT 0x05

/*
 * Vendor Specific Commands Feature.
 * <Device Vendor ID> Reports the vendor ID of this device.
 * Param: [Vendor ID], 3 bytes.
 * Broadcast message.
 */
#define HDMI_CEC_OPCODE_DEVICE_VENDOR_ID 0x87

#define HDMI_CEC_DEVICE_VENDOR_ID_MSG_PARAM_LEN 3

/*
 * Indicates the vendor ID should be disabled.
 */
#define HDMI_CEC_VENDOR_ID_UNKNOWN 0xFFFFFFFF

/*
 * Vendor Specific Commands Feature.
 * <Give Device Vendor ID> Requests the Vendor ID from a device.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_GIVE_DEVICE_VENDOR_ID 0x8C

/*
 * Vendor Specific Commands Feature.
 * <Vendor Command> Allows vendor specific commands to be sent between two devices.
 * Param: [Vendor Specific Data], <= 14 bytes.
 */
#define HDMI_CEC_OPCODE_VENDOR_COMMAND 0x89

#define HDMI_CEC_VENDOR_SPECIFIC_DATA_MAX_LEN 14

/*
 * Vendor Specific Commands Feature.
 * <Vendor Command With ID> Allows vendor specific commands to be sent between two devices or broadcast.
 * Param: [Vendor ID], 3 bytes. [Vendor Specific Data], <= 11 bytes.
 * Broadcast/Directly addressed message.
 */
#define HDMI_CEC_OPCODE_VENDOR_COMMAND_WITH_ID 0xA0

#define HDMI_CEC_VENDOR_ID_LEN 3
#define HDMI_CEC_VENDOR_SPECIFIC_DATA_WITH_ID_MAX_LEN 11

/*
 * Vendor Specific Commands Feature.
 * <Vendor Remote Button Down> Indicates that a remote control button has been depressed.
 * Param: [Vendor Specific RC Code], <= 14 bytes.
 * Broadcast/Directly addressed message.
 */
#define HDMI_CEC_OPCODE_VENDOR_REMOTE_BUTTON_DOWN 0x8A

#define HDMI_CEC_VENDOR_SPECIFIC_RC_CODE_MAX_LEN 14

/*
 * Vendor Specific Commands Feature.
 * <Vendor Remote Button Up> Indicates that a remote control button(the last button pressed indicated
 * by the Vendor Remote Button Down message) has been released.
 * Param: None.
 * Broadcast/Directly addressed message.
 */
#define HDMI_CEC_OPCODE_VENDOR_REMOTE_BUTTON_UP 0x8B

/*
 * OSD Display Feature.
 * <Set OSD String> Used to send a text massage to output on a TV.
 * OSD(on-screen display).
 * Param: [Display Control], 1 byte. [OSD String], 1-13 bits.
 */
#define HDMI_CEC_OPCODE_SET_OSD_STRING 0x64

#define HDMI_CEC_DISPLAY_CONTROL_LEN 1
#define HDMI_CEC_OSD_STRING_MAX_LEN 13

/* Display Control */
enum HdmiCecDisplayControl {
    HDMI_CEC_DISPLAY_FOR_DEFAULT_TIME = 0x00,
    HDMI_CEC_DISPLAY_UNTIL_CLEARED = 0x40,
    HDMI_CEC_DISPLAY_CLEAR_PREVIOUS_MESSAGE = 0x80,
    HDMI_CEC_DISPLAY_BUTT,
};

/*
 * OSD Display Feature.
 * <Give OSD Name> Used to request the preferred OSD name of a device for use in menus associated with that device.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_GIVE_OSD_NAME 0x46

/*
 * OSD Display Feature.
 * <Set OSD Name> Used to set the preferred OSD name of a device for use in menus associated with that device.
 * Param: [OSD Name], 1-14 bytes.
 */
#define HDMI_CEC_OPCODE_SET_OSD_NAME 0x47

#define HDMI_CEC_OSD_NAME_MAX_LEN 14

/*
 * Device Menu Control Feature.
 * <Menu Request> A request from the TV for a device to show/remove a menu or to
 * query if a device is currently showing a menu.
 * Param: [Menu Request Type], 1 byte.
 */
#define HDMI_CEC_OPCODE_MENU_REQUEST 0x8D

#define HDMI_CEC_MENU_REQUEST_MSG_PARAM_LEN 1

/* Menu Request Type */
enum HdmiCecMenuRequestType {
    HDMI_CEC_MENU_REQUEST_ACTIVE = 0,
    HDMI_CEC_MENU_REQUEST_DEACTIVE = 1,
    HDMI_CEC_MENU_REQUEST_QUERY = 2,
};

/*
 * Device Menu Control Feature.
 * <Menu Status> Used to indicate to the TV that the device is showing/has removed a menu and
 * requests the remote control keys to be passed throuh.
 * Param: [Menu State], 1 byte.
 */
#define HDMI_CEC_OPCODE_MENU_STATUS 0x8E

#define HDMI_CEC_MENU_STATUS_MSG_PARAM_LEN 1

/* Menu State */
#define HDMI_CEC_MENU_STATE_ACTIVE 0
#define HDMI_CEC_MENU_STATE_DEACTIVE 1

/*
 * Device Menu Control Feature/Remote Contro Passthrough Feature/System Audio Control Feature.
 * <User Control Pressed> Used to indicate that the user pressed a remote control button or switched from
 * one remote control button to another. Can also be used as a command that is not directly initiated by the user.
 * Param: [UI Command], 1 byte.
 * Note that some [UI Command] messages also have further operands. See CEC Table 6 and CEC Table 7.
 */
#define HDMI_CEC_OPCODE_USER_CONTROL_PRESSED 0x44

#define HDMI_CEC_UI_COMMAND_LEN 1

/* UI Command */
enum HdmiCecUiCmdType {
    HDMI_CEC_UI_CMD_SELECT = 0x00,
    HDMI_CEC_UI_CMD_UP = 0x01,
    HDMI_CEC_UI_CMD_DOWN = 0x02,
    HDMI_CEC_UI_CMD_LEFT = 0x03,
    HDMI_CEC_UI_CMD_RIGHT = 0x04,
    HDMI_CEC_UI_CMD_RIGHT_UP = 0x05,
    HDMI_CEC_UI_CMD_RIGHT_DOWN = 0x06,
    HDMI_CEC_UI_CMD_LEFT_UP = 0x07,
    HDMI_CEC_UI_CMD_LEFT_DOWN = 0x08,
    HDMI_CEC_UI_CMD_DEVICE_ROOT_MENU = 0x09,
    HDMI_CEC_UI_CMD_DEVICE_SETUP_MENU = 0x0a,
    HDMI_CEC_UI_CMD_CONTENTS_MENU = 0x0b,
    HDMI_CEC_UI_CMD_FAVORITE_MENU = 0x0c,
    HDMI_CEC_UI_CMD_EXIT = 0x0d,
    HDMI_CEC_UI_CMD_MEDIA_TOP_MENU = 0x10,
    HDMI_CEC_UI_CMD_MEDIA_CONTEXT_SENSITIVE_MENU = 0x11,
    HDMI_CEC_UI_CMD_NUMBER_ENTRY_MODE = 0x1d,
    HDMI_CEC_UI_CMD_NUMBER_11 = 0x1e,
    HDMI_CEC_UI_CMD_NUMBER_12 = 0x1f,
    HDMI_CEC_UI_CMD_NUMBER_0_OR_NUMBER_10 = 0x20,
    HDMI_CEC_UI_CMD_NUMBER_1 = 0x21,
    HDMI_CEC_UI_CMD_NUMBER_2 = 0x22,
    HDMI_CEC_UI_CMD_NUMBER_3 = 0x23,
    HDMI_CEC_UI_CMD_NUMBER_4 = 0x24,
    HDMI_CEC_UI_CMD_NUMBER_5 = 0x25,
    HDMI_CEC_UI_CMD_NUMBER_6 = 0x26,
    HDMI_CEC_UI_CMD_NUMBER_7 = 0x27,
    HDMI_CEC_UI_CMD_NUMBER_8 = 0x28,
    HDMI_CEC_UI_CMD_NUMBER_9 = 0x29,
    HDMI_CEC_UI_CMD_DOT = 0x2a,
    HDMI_CEC_UI_CMD_ENTER = 0x2b,
    HDMI_CEC_UI_CMD_CLEAR = 0x2c,
    HDMI_CEC_UI_CMD_NEXT_FAVORITE = 0x2f,
    HDMI_CEC_UI_CMD_CHANNEL_UP = 0x30,
    HDMI_CEC_UI_CMD_CHANNEL_DOWN = 0x31,
    HDMI_CEC_UI_CMD_PREVIOUS_CHANNEL = 0x32,
    HDMI_CEC_UI_CMD_SOUND_SELECT = 0x33,
    HDMI_CEC_UI_CMD_INPUT_SELECT = 0x34,
    HDMI_CEC_UI_CMD_DISPLAY_INFORMATION = 0x35,
    HDMI_CEC_UI_CMD_HELP = 0x36,
    HDMI_CEC_UI_CMD_PAGE_UP = 0x37,
    HDMI_CEC_UI_CMD_PAGE_DOWN = 0x38,
    HDMI_CEC_UI_CMD_POWER = 0x40,
    HDMI_CEC_UI_CMD_VOLUME_UP = 0x41,
    HDMI_CEC_UI_CMD_VOLUME_DOWN = 0x42,
    HDMI_CEC_UI_CMD_MUTE = 0x43,
    HDMI_CEC_UI_CMD_PLAY = 0x44,
    HDMI_CEC_UI_CMD_STOP = 0x45,
    HDMI_CEC_UI_CMD_PAUSE = 0x46,
    HDMI_CEC_UI_CMD_RECORD = 0x47,
    HDMI_CEC_UI_CMD_REWIND = 0x48,
    HDMI_CEC_UI_CMD_FAST_FORWARD = 0x49,
    HDMI_CEC_UI_CMD_EJECT = 0x4a,
    HDMI_CEC_UI_CMD_SKIP_FORWARD = 0x4b,
    HDMI_CEC_UI_CMD_SKIP_BACKWARD = 0x4c,
    HDMI_CEC_UI_CMD_STOP_RECORD = 0x4d,
    HDMI_CEC_UI_CMD_PAUSE_RECORD = 0x4e,
    HDMI_CEC_UI_CMD_ANGLE = 0x50,
    HDMI_CEC_UI_CMD_SUB_PICTURE = 0x51,
    HDMI_CEC_UI_CMD_VIDEO_ON_DEMAND = 0x52,
    HDMI_CEC_UI_CMD_ELECTRONIC_PROGRAM_GUIDE = 0x53,
    HDMI_CEC_UI_CMD_TIMER_PROGRAMMING = 0x54,
    HDMI_CEC_UI_CMD_INITIAL_CONFIGURATION = 0x55,
    HDMI_CEC_UI_CMD_SELECT_BROADCAST_TYPE = 0x56,          /* Additional Operands: [UI Broadcast Type] */
    HDMI_CEC_UI_CMD_SELECT_SOUND_PRESENTATION = 0x57,      /* Additional Operands: [UI Sound Presentation Control] */
    HDMI_CEC_UI_CMD_AUDIO_DESCRIPTION = 0x58,
    HDMI_CEC_UI_CMD_INTERNET = 0x59,
    HDMI_CEC_UI_CMD_3D_MODE = 0x5a,
    HDMI_CEC_UI_CMD_PLAY_FUNCTION = 0x60,                  /* Additional Operands: [Play Mode] */
    HDMI_CEC_UI_CMD_PAUSE_PLAY_FUNCTION = 0x61,
    HDMI_CEC_UI_CMD_RECORD_FUNCTION = 0x62,
    HDMI_CEC_UI_CMD_PAUSE_RECORD_FUNCTION = 0x63,
    HDMI_CEC_UI_CMD_STOP_FUNCTION = 0x64,
    HDMI_CEC_UI_CMD_MUTE_FUNCTION = 0x65,
    HDMI_CEC_UI_CMD_RESTORE_VOLUME_FUNCTION = 0x66,
    HDMI_CEC_UI_CMD_TUNE_FUNCTION = 0x67,                  /* Additional Operands: [Channel Identifier] */
    HDMI_CEC_UI_CMD_SELECT_MEDIA_FUNCTION = 0x68,          /* Additional Operands: [UI Function Media] */
    HDMI_CEC_UI_CMD_SELECT_AV_INPUT_FUNCTION = 0x69,       /* Additional Operands: [UI Function Select A/V Input] */
    HDMI_CEC_UI_CMD_SELECT_AUDIO_INPUT_FUNCTION = 0x6a,    /* Additional Operands: [UI Function Select Audio Input] */
    HDMI_CEC_UI_CMD_POWER_TOGGLE_FUNCTION = 0x6b,
    HDMI_CEC_UI_CMD_POWER_OFF_FUNCTION = 0x6c,
    HDMI_CEC_UI_CMD_POWER_ON_FUNCTION = 0x6d,
    HDMI_CEC_UI_CMD_F1_BLUE = 0x71,
    HDMI_CEC_UI_CMD_F2_RED = 0x72,
    HDMI_CEC_UI_CMD_F3_GREEN = 0x73,
    HDMI_CEC_UI_CMD_F4_YELLOW = 0x74,
    HDMI_CEC_UI_CMD_F5 = 0x75,
    HDMI_CEC_UI_CMD_DATA = 0x76,
};

enum HdmiCecUiBroadcastType {
    HDMI_CEC_UI_BROADCAST_TYPE_TOGGLE_ALL = 0x00,
    HDMI_CEC_UI_BROADCAST_TYPE_TOGGLE_DIG_ANA = 0x01,
    HDMI_CEC_UI_BROADCAST_TYPE_ANA = 0x10,
    HDMI_CEC_UI_BROADCAST_TYPE_ANA_TERRESTRIAL = 0x20,
    HDMI_CEC_UI_BROADCAST_TYPE_ANA_CABLE = 0x30,
    HDMI_CEC_UI_BROADCAST_TYPE_ANA_SATELLITE = 0x40,
    HDMI_CEC_UI_BROADCAST_TYPE_DIG = 0x50,
    HDMI_CEC_UI_BROADCAST_TYPE_DIG_TERRESTRIAL = 0x60,
    HDMI_CEC_UI_BROADCAST_TYPE_DIG_CABLE = 0x70,
    HDMI_CEC_UI_BROADCAST_TYPE_DIG_SATELLITE = 0x80,
    HDMI_CEC_UI_BROADCAST_TYPE_DIG_COM_SATELLITE = 0x90,
    HDMI_CEC_UI_BROADCAST_TYPE_DIG_COM_SATELLITE2 = 0x91,
    HDMI_CEC_UI_BROADCAST_TYPE_IP = 0xA0,
};

/* UI Sound Presentation Control */
enum HdmiCecUiSoundPresCtl {
    HDMI_CEC_UI_SOUND_PRES_CTL_DUAL_MONO = 0x20,
    HDMI_CEC_UI_SOUND_PRES_CTL_KARAOKE = 0x30,
    HDMI_CEC_UI_SOUND_PRES_CTL_DOWNMIX = 0x80,
    HDMI_CEC_UI_SOUND_PRES_CTL_REVERB_PROCESS = 0x90,
    HDMI_CEC_UI_SOUND_PRES_CTL_EQUALIZER = 0xA0,
    HDMI_CEC_UI_SOUND_PRES_CTL_BASS_STEP_UP = 0xB1,
    HDMI_CEC_UI_SOUND_PRES_CTL_BASS_NEUTRAL = 0xB2,
    HDMI_CEC_UI_SOUND_PRES_CTL_BASS_STEP_DOWN = 0xB3,
    HDMI_CEC_UI_SOUND_PRES_CTL_TREBLE_STEP_UP = 0xC1,
    HDMI_CEC_UI_SOUND_PRES_CTL_TREBLE_NEUTRAL = 0xC2,
    HDMI_CEC_UI_SOUND_PRES_CTL_TREBLE_STEP_DOWN = 0xC3,
};

struct HdmiCecUiCmd {
    uint8_t cmdType;
    bool hasAddOp;
    union {
        uint8_t uiBroadcastType;
        uint8_t uiSoundPresCtl;
        uint8_t playMode;
        uint8_t uiFunctionMedia;
        uint8_t uiFunctionSelectAvInput;
        uint8_t uiFunctionSelectAudioInput;
        struct HdmiCecChannelIdentifier channel;
    } addOperands;
};

/*
 * Device Menu Control Feature/Remote Contro Passthrough Feature/System Audio Control Feature.
 * <User Control Released> Indicates that user released a remote control button(the last one indicated by the
 * <User Control Pressed> message). Also used after a command that is not directly initiated by the user.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_USER_CONTROL_RELEASED 0x45

/*
 * Remote Contro Passthrough Feature.
 * <Give Device Power Status> Used to determine the current power status of a target device.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_GIVE_DEVICE_POWER_STATUS 0x8F

/*
 * Remote Contro Passthrough Feature.
 * <Report Power Status> Used to inform a requesting device of the current power status.
 * Param: [Power Status], 1 byte.
 * Broadcast(Hdmi Spec2.0)/Directly addressed message.
 */
#define HDMI_CEC_OPCODE_REPORT_POWER_STATUS 0x90

#define HDMI_CEC_REPORT_POWER_STATUS_MSG_PARA_LEN 1

/* Power Status */
enum HdmiCecPowerStatus {
    HDMI_CEC_POWER_STATUS_ON = 0x00,
    HDMI_CEC_POWER_STATUS_STANDBY = 0x01,
    HDMI_CEC_POWER_STATUS_IN_TRANSITION_STANDBY_TO_ON = 0x02,
    HDMI_CEC_POWER_STATUS_IN_TRANSITION_ON_TO_STANDBY = 0x03,
};

/*
 * General Protocol Messages.
 * <Feature Abort> Used as a response to indicate that the device does not support the requested message type,
 * or that is cannot execute it at the present time.
 * Param: [Feature Opcode], 1 byte. [Abort Reason], 1 byte.
 */
#define HDMI_CEC_OPCODE_FEATURE_ABORT 0x00

#define HDMI_CEC_FEATURE_ABORT_MSG_PARA_LEN 2

/* Abort Reason */
enum HdmiCecAbortReason {
    HDMI_CEC_ABORT_UNRECOGNIZED_OPCODE = 0,
    HDMI_CEC_ABORT_NOT_IN_CORRECT_MODE_TO_RESPOND = 1,
    HDMI_CEC_ABORT_CANNOT_PROVIDE_SOURCE = 2,
    HDMI_CEC_ABORT_INVALID_OPERAND = 3,
    HDMI_CEC_ABORT_REFUSED = 4,
    HDMI_CEC_ABORT_UNABLE_TO_DETERMINE = 5,
};

/*
 * General Protocol Messages.
 * <Abort> This message is reversed for testing purposes.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_ABORT 0xFF

/*
 * System Audio Control Feature.
 * <Give Audio Status> Requests an Amplifier to send its volume and mute status.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_GIVE_AUDIO_STATUS 0x71

/*
 * System Audio Control Feature.
 * <Give System Audio Mode Status> Requests the status of the System Audio Mode.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_GIVE_SYSTEM_AUDIO_MODE_STATUS 0x7D

/*
 * System Audio Control Feature.
 * <Report Audio Status> Reports an Amplifier's volume and mute status.
 * Param: [Audio Status], 1 byte.
 */
#define HDMI_CEC_OPCODE_REPORT_AUDIO_STATUS 0x7A

#define HDMI_CEC_REPORT_AUDIO_STATUSMSG_PARAM_LEN 1

/*
 * Audio Status, 1 byte.
 * [Audio Mute Status], Bit 7; [Audio Volume Status], Bits 6-0.
 */
#define HDMI_CEC_AUDIO_MUTE_STATUS_MARK 0x80
#define HDMI_CEC_AUDIO_MUTE_STATUS_SHIFT 7
#define HDMI_CEC_AUDIO_VOLUME_STATUS_MARK 0x7F
/* Audio Mute Status */
#define HDMI_CEC_AUDIO_MUTE_STATUS_OFF 0
#define HDMI_CEC_AUDIO_MUTE_STATUS_ON 1
/* Audio Volume Status, 0%~100% */
#define HDMI_CEC_AUDIO_VOLUME_STATUS_MIN_VAL 0x00
#define HDMI_CEC_AUDIO_VOLUME_STATUS_MAX_VAL 0x64
#define HDMI_CEC_AUDIO_VOLUME_STATUS_UNKNOWN 0x7F

/*
 * System Audio Control Feature.
 * <Report Short Audio Descriptor> Report Audio Capability.
 * Param: [Short Audio Descriptor], 3 bytes. Up to 4 Short Audio Descriptor identifying supported audio formats.
 */
#define HDMI_CEC_OPCODE_REPORT_SHORT_AUDIO_DESCRIPTOR 0xA3

#define HDMI_CEC_SHORT_AUDIO_DESCRIPTOR_LEN 3
#define HDMI_CEC_MAX_SHORT_AUDIO_DESCRIPTOR_NUM 4

/*
 * System Audio Control Feature.
 * <Request Short Audio Descriptor> Report Audio Capability.
 * Param: [Audio Format ID], 2 bits. [Audio Format Code], 6 bits. Up to 4 Audio Format ID and Audio Format Code.
 */
#define HDMI_CEC_OPCODE_REQUEST_SHORT_AUDIO_DESCRIPTOR 0xA4

#define HDMI_CEC_AUDIO_FORMAT_LEN 1
#define HDMI_CEC_MAX_AUDIO_FORMAT_NUM 4
#define HDMI_CEC_AUDIO_FORMAT_ID_SHIFT 6
#define HDMI_CEC_AUDIO_FORMAT_CODE_MARK 0x3F

/*
 * System Audio Control Feature.
 * <Set System Audio Mode> Turns the System Audio Mode On or Off.
 * Param: [System Audio Status], 1 byte.
 * Broadcast/Directly addressed message.
 */
#define HDMI_CEC_OPCODE_SET_SYSTEM_AUDIO_MODE 0x72

#define HDMI_CEC_SYSTEM_AUDIO_STATUS_LEN 1

/* System Audio Status */
#define HDMI_CEC_SYSTEM_AUDIO_STATUS_OFF 0
#define HDMI_CEC_SYSTEM_AUDIO_STATUS_ON 1

/*
 * System Audio Control Feature.
 * <System Audio Mode Request> A device implementing System Audio Control and which has volume control
 * RC buttons(e.g. TV or STB) requests to use System Audio Mode to the Amplifier.
 # Param: [Physical Address], 2 bytes.
 */
#define HDMI_CEC_OPCODE_SYSTEM_AUDIO_MODE_REQUEST 0x70

#define HDMI_CEC_SYSTEM_AUDIO_MODE_REQUEST_PARAM_LEN 2

/*
 * System Audio Control Feature.
 * <System Audio Mode Status> Reports the current status of the System Audio Mode.
 * Param: [System Audio Status], 1 byte.
 */
#define HDMI_CEC_OPCODE_SYSTEM_AUDIO_MODE_STATUS 0x7E

/*
 * Audio Rate Control Feature.
 * <Set Audio Rate> Used to control audio rate from Source Device.
 * Param: [Audio Rate], 1 byte.
 */
#define HDMI_CEC_OPCODE_SET_AUDIO_RATE 0x9A

#define HDMI_CEC_SET_AUDIO_RATE_PARAM_LEN 1

/* Audio Rate */
enum HdmiCecAudioRate {
    HDMI_CEC_AUDIO_RATE_OFF = 0,         /* Rate Control Off */
    HDMI_CEC_AUDIO_RATE_WIDE_STD = 1,    /* Standard Rate: 100% rate */
    HDMI_CEC_AUDIO_RATE_WIDE_FAST = 2,   /* Fast Rate: Max 101% rate */
    HDMI_CEC_AUDIO_RATE_WIDE_SLOW = 3,   /* Slow Rate: Min 99% rate */
    HDMI_CEC_AUDIO_RATE_NARROW_STD = 4,  /* Standard Rate: 100.0% rate */
    HDMI_CEC_AUDIO_RATE_NARROW_FAST = 5, /* Fast Rate: Max 100.1% rate */
    HDMI_CEC_AUDIO_RATE_NARROW_SLOW = 6, /* Slow Rate: Min 99.9% rate */
};

/*
 * Audio Return Channel Control Feature.
 * <Initiate ARC> Used by an ARC RX device to active the ARC functionality in an ARC TX device.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_INITIATE_ARC 0xC0

/*
 * Audio Return Channel Control Feature.
 * <Report ARC Initiated> Used by an ARC RX device to indicate that its ARC functionality has been activated.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_REPORT_ARC_INITIATED 0xC1

/*
 * Audio Return Channel Control Feature.
 * <Report ARC Termination> Used by an ARC RX device to indicate that its ARC functionality has been deactivated.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_REPORT_ARC_TERMINATION 0xC2

/*
 * Audio Return Channel Control Feature.
 * <Request ARC Initiation> Used by an ARC TX device to request an ARC RX device to
 * active the ARC functionality in the ARC TX device.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_REQUEST_ARC_INITIATION 0xC3

/*
 * Audio Return Channel Control Feature.
 * <Request ARC Termination> Used by an ARC TX device to request an ARC RX device to
 * deactive the ARC functionality in the ARC TX device.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_REQUEST_ARC_TERMINATION 0xC4

/*
 * Audio Return Channel Control Feature.
 * <Terminate ARC> Used by an ARC TX device to deactive the ARC functionality in the ARC TX device.
 * Param: None.
 */
#define HDMI_CEC_OPCODE_TERMINATE_ARC 0xC5

/*
 * Capability Discovery and Control Feature.
 * <CDC Message> Used for Capability Discovery and Control.
 * Broadcast message.
 */
#define HDMI_CEC_OPCODE_CDC_MESSAGE 0xF8

/*
 * Dynamic Auto Lispync Feature.
 * <Request Current Latency> Used by Amplifier(or other device) to request current latency values.
 * Param: [Physical Address], 2 byte.
 */
#define HDMI_CEC_OPCODE_REQUEST_CURRENT_LATENCY 0xA7

#define HDMI_CEC_REQUEST_CURRENT_LATENCY_MSG_LEN 2

/*
 * Dynamic Auto Lispync Feature.
 * <Report Current Latency> Used by TV(or other Initiator) to report updates of latency.
 * Param: [Physical Address], 2 byte. [Video Latency], 1 byte. [Latency Flags], 1 byte.
 * [Audio Output Delay], 1 byte, optional.
 */
#define HDMI_CEC_OPCODE_REPORT_CURRENT_LATENCY 0xA8

#define HDMI_CEC_REPORT_CURRENT_LATENCY_MSG_PARAM_MAX_LEN 5
#define HDMI_CEC_REPORT_CURRENT_LATENCY_MSG_PARAM_MIN_LEN 4

/*
 * Latency Flags, 1 byte.
 * Bit 7..3 reversed; Bit 2 [Low Latency Mode]; Bit 1-0 [Audio Output Compensated].
 */
#define HDMI_CEC_LOW_LATENCY_MODE_SHIFT 2
#define HDMI_CEC_NORMAL_LATENCY_MODE 0
#define HDMI_CEC_LOW_LATENCY_MODE 1

#define HDMI_CEC_AUDIO_OUTPUT_COMPENSATED_NA 0
#define HDMI_CEC_AUDIO_OUTPUT_COMPENSATED_DELAY 1
#define HDMI_CEC_AUDIO_OUTPUT_COMPENSATED_NO_DELAY 2
#define HDMI_CEC_AUDIO_OUTPUT_COMPENSATED_PARTIAL_DELAY 3

struct HdmiCecLatencyInfo {
    uint8_t videoLatency;
    uint8_t lowLatencyMode;
    uint8_t audioOutputCompensated;
    uint8_t audioOutputDelay;
};


/* Broadcast System, 0~31. See hdmi spec1.4, CEC Table 31. */
enum HdmiCecBroadcastSystem {
    HDMI_CEC_BROADCAST_SYSTEM_PAL_BG = 0,
    HDMI_CEC_BROADCAST_SYSTEM_SECAM_LQ = 1,
    HDMI_CEC_BROADCAST_SYSTEM_PAL_M = 2,
    HDMI_CEC_BROADCAST_SYSTEM_NTSC_M = 3,
    HDMI_CEC_BROADCAST_SYSTEM_PAL_I = 4,
    HDMI_CEC_BROADCAST_SYSTEM_SECAM_DK = 5,
    HDMI_CEC_BROADCAST_SYSTEM_SECAM_BG = 6,
    HDMI_CEC_BROADCAST_SYSTEM_SECAM_L = 7,
    HDMI_CEC_BROADCAST_SYSTEM_PAL_DK = 8,
    HDMI_CEC_BROADCAST_SYSTEM_OTHER = 31,
};

/* CDC(Capability Discovery and Control) Message */

/*
 * HDMI Ethernet Channel Feature.
 * <CDC_HEC_InquireState> Used to inquire the state of an HDMI Ethernet Channel.
 * Param: [Physical Address], 2 byte. [Physical Address], 2 byte.
 */
#define HDMI_CDC_HEC_INQUIRE_STATE 0x00

#define HDMI_CDC_HEC_INQUIRE_STATE_MSG_PARAM_LEN 4

/*
 * HDMI Ethernet Channel Feature.
 * <CDC_HEC_ReportState> Used to report capabilities and states of a device that supports functionality.
 * Param: [Physical Address], 2 bytes. [HEC State], 1 byte. [HEC Support Field], 2 bytes.
 * [HEC Activation Field], 2 bytes.
 */
#define HDMI_CDC_HEC_REPORT_STATE 0x01

#define HDMI_CDC_HEC_REPORT_STATE_MSG_PARAM_MAX_LEN 5
#define HDMI_CDC_HEC_REPORT_STATE_MSG_PARAM_MIN_LEN 3

/*
 * HEC State, 1 byte
 * [HEC Functionality State], 2 bits.
 * [Host Functionality State], 2 bits.
 * [ENC Functionality State], 2 bits.
 * [CDC Error Code], 2 bits.
 */
#define HDMI_CDC_HEC_FUNC_STATE_SHIFT 6
#define HDMI_CDC_HOST_FUNC_STATE_SHIFT 4
#define HDMI_CDC_ENC_FUNC_STATE_SHIFT 2

enum HdmiCdcHecFuncState {
    HDMI_CDC_HEC_FUNC_STATE_NOT_SUPPORTED = 0,
    HDMI_CDC_HEC_FUNC_STATE_INACTIVE = 1,
    HDMI_CDC_HEC_FUNC_STATE_ACTIVE = 2,
    HDMI_CDC_HEC_FUNC_STATE_ACTIVATION_FIELD = 3,
};

enum HdmiCdcHostFuncState {
    HDMI_CDC_HOST_HEC_STATE_NOT_SUPPORTED = 0,
    HDMI_CDC_HOST_HEC_STATE_NOT_INACTIVE = 1,
    HDMI_CDC_HOST_HEC_STATE_NOT_ACTIVE = 2,
};

enum HdmiCdcEncFuncState {
    HDMI_CDC_ENC_FUNC_STATE_EXT_CON_NOT_SUPPORTED = 0,
    HDMI_CDC_ENC_FUNC_STATE_EXT_CON_INACTIVE = 1,
    HDMI_CDC_ENC_FUNC_STATE_EXT_CON_ACTIVE = 2,
};

enum HdmiCdcErrCode {
    HDMI_CDC_ERR_CODE_NO_ERROR = 0,
    HDMI_CDC_ERR_CODE_CAP_UNSUPPORTED = 1,
    HDMI_CDC_ERR_CODE_WRONG_STATE = 2,
    HDMI_CDC_ERR_CODE_OTHER = 3,
};

/*
 * HEC Support Field, 2 bytes.
 * [HEC Support], Bit 14. {14[HEC Support]}, Bit 13-0.
 */
#define HDMI_CDC_HEC_SUPPORT_MAKR (1 << 14)
#define HDMI_CDC_HEC_NOT_SUPPORT 0
#define HDMI_CDC_HEC_SUPPORT 1

/*
 * HEC Activation Field, 2 bytes.
 * [HEC Activation], Bit 14. {14[HEC Activation]}, Bit 13-0.
 */
#define HDMI_CDC_HEC_ACTIVATION_MAKR (1 << 14)
#define HDMI_CDC_HEC_INACTIVE 0
#define HDMI_CDC_HEC_ACTIVE 1

struct HdmiCdcHecState {
    uint8_t hecFuncState;
    uint8_t hostFuncState;
    uint8_t encFuncState;
    uint8_t cdcErrCode;
    bool haveHecField;
    uint16_t hecField;
};

/*
 * HDMI Ethernet Channel Feature.
 * <CDC_HEC_SetStateAdjacent> Used to active or de-active an HDMI Ethernet Channel to an adjacent device.
 * Param: [Physical Address], 2 bytes. [HEC Set State], 1 byte.
 */
#define HDMI_CDC_HEC_SET_STATE_ADJACENT 0x02

#define HDMI_CDC_HEC_SET_STATE_ADJACENT_MSG_PARAM_LEN 3

/* HEC Set State, 1 byte. */
#define HDMI_CDC_DEACTIVATE_HEC 0
#define HDMI_CDC_ACTIVATE_HEC 1

/*
 * HDMI Ethernet Channel Feature.
 * <CDC_HEC_SetState> Used to active or de-active an HDMI Ethernet Channel to an adjacent device.
 * Param: [Physical Address], 2 bytes. [Physical Address], 2 bytes. [HEC Set State], 1 byte.
 * [Physical Address], 2 bytes. [Physical Address], 2 bytes. [Physical Address], 2 bytes.
 * The last three [PA] parameters shall not be incorporated if only one HEC shall activated.
 */
#define HDMI_CDC_HEC_SET_STATE 0x03

#define HDMI_CDC_HEC_SET_STATE_MSG_MIN_LEN 5
#define HDMI_CDC_HEC_SET_STATE_MSG_OPTIONAL_PA_MAX_NUM 3

struct HemiCdcHecStateInfo {
    uint16_t phyAddr1;
    uint16_t phyAddr2;
    uint8_t hecSetState;
    uint16_t phyAddr[HDMI_CDC_HEC_SET_STATE_MSG_OPTIONAL_PA_MAX_NUM];
    uint32_t phyAddrLen;
};

/*
 * HDMI Ethernet Channel Feature.
 * <CDC_HEC_RequestDeactivation> Used to request the deactivation of an HDMI Ethernet Channel.
 * Param: [Physical Address], 2 bytes. [Physical Address], 2 bytes. [Physical Address], 2 bytes.
 */
#define HDMI_CDC_HEC_REQUEST_DEACTIVATION 0x04

#define HDMI_CDC_HEC_REQUEST_DEACTIVATION_MSG_PHY_ADDR_NUM 3
#define HDMI_CDC_HEC_REQUEST_DEACTIVATION_MSG_PARAM_LEN 6

/*
 * HDMI Ethernet Channel Feature.
 * <CDC_HEC_NotifyAlive> Used to keep alive HDMI Ethernet Channels active.
 */
#define HDMI_CDC_HEC_NOTIFY_ALIVE 0x05

/*
 * HDMI Ethernet Channel Feature.
 * <CDC_HEC_Discover> Used to discover all HEC capabilities of devices within the HDMI network.
 */
#define HDMI_CDC_HEC_DISCOVER 0x06

/*
 * CDC HPD Feature.
 * <CDC_HPD_SetState> Used to substitute physical HPD pin signaling.
 * Param: [Input port number], 4 bits. [HPD State], 4 bits.
 */
#define HDMI_CDC_HPD_SET_STATE 0x10

#define HDMI_CDC_HPD_SET_STATE_MSG_LEN 1
#define HDMI_CDC_INPUT_PORT_NUMBER_SHIFT 4

enum HdmiCdcHpdState {
    HDMI_CDC_HDP_STATE_CP_EDID_DISABLE = 0,
    HDMI_CDC_HDP_STATE_CP_EDID_ENABLE = 1,
    HDMI_CDC_HDP_STATE_CP_EDID_DISABLE_ENABLE = 2,
    HDMI_CDC_HDP_STATE_EDID_DISABLE = 3,
    HDMI_CDC_HDP_STATE_EDID_ENABLE = 4,
    HDMI_CDC_HDP_STATE_EDID_DISABLE_ENABLE = 5,
    HDMI_CDC_HDP_STATE_BUTT,
};

/*
 * CDC HPD Feature.
 * <CDC_HPD_ReportState> Used to report the state of a device using the CDC HPD signaling.
 * Param: [HPD State], 4 bits. [CDC_HPD_Error_Code], 4 bits.
 */
#define CEC_MSG_CDC_HPD_REPORT_STATE 0x11

#define HDMI_CDC_HPD_REPORT_STATE_MSG_LEN 1
#define HDMI_CDC_HPD_STATE_SHIFT 4

enum HdmiCdcHpdErrCode {
    HDMI_CDC_HPD_NO_ERR = 0,
    HDMI_CDC_HPD_ERR_INITIATOR_NOT_CAPABLE = 1,
    HDMI_CDC_HPD_ERR_INITIATOR_WRONG_STATE = 2,
    HDMI_CDC_HPD_ERR_OTHER = 3,
    HDMI_CDC_HPD_NO_ERR_NO_VIDEO_STREAM = 4,
    HDMI_CDC_HPD_ERR_BUTT,
};

/*
 * CEC message frame.
 *
 *                 1 block           1 block          0-14 blocks
 * Start_Bit  CEC_Header_Block  CEC_Opcode_Block   CEC_Operand_Blocks
 *
 * CDC message frame.
 *
 *                 1 block           1 block                 2 blocks           1 block     0~11 blocks
 * Start_Bit  CEC_Header_Block  CEC_Opcode_Block(0xF8)  Initiator_Phy_Address  CDC_Opcode  CDC_Parameters
 *
 * Header Block.
 *          high 4 bits                      low 4 bits
 * Initiator Logic Address field    Destination Logic Address field
 *
 */
#define HDMI_CEC_MSG_MAX_LEN 20  /* Make sure it has enough space. */
#define HDMI_CEC_HEADER_LEN 1
#define HDMI_CEC_OPCODE_LEN 1
#define HDMI_CEC_MSG_MIN_LEN (HDMI_CEC_HEADER_LEN + HDMI_CEC_OPCODE_LEN)
#define HDMI_POLLING_MSG_LEN 1
#define HDMI_CEC_INITIATOR_PHY_ADDRESS_LEN 2
#define HDMI_CEC_GET_MSG_LEN(len) (HDMI_CEC_HEADER_LEN + HDMI_CEC_OPCODE_LEN + (len))
#define HDMI_CDC_OPCODE_LEN 1
#define HDMI_CDC_GET_MSG_LEN(len) \
    (HDMI_CEC_HEADER_LEN + HDMI_CEC_OPCODE_LEN + HDMI_CEC_INITIATOR_PHY_ADDRESS_LEN + HDMI_CDC_OPCODE_LEN + (len))

#define HDMI_CEC_HEADER_BLOCK_INITIATOR_MARK 0xF0
#define HDMI_CEC_HEADER_BLOCK_DESTINATION_MARK 0x0F
#define HDMI_CEC_HEADER_BLOCK_INITIATOR_SHIFT 4

#define HDMI_CEC_INVALID_PHY_ADDR 0xFFFF
#define HDMI_CEC_INVALID_LOGICAL_ADDR 0xFF

enum HdmiCecLogicalAddr {
    HDMI_CEC_LOG_ADDR_TV = 0,
    HDMI_CEC_LOG_ADDR_RECORDING_DEVICE_1 = 1,
    HDMI_CEC_LOG_ADDR_RECORDING_DEVICE_2 = 2,
    HDMI_CEC_LOG_ADDR_TUNER_1 = 3,
    HDMI_CEC_LOG_ADDR_PLAYBACK_DEVICE_1 = 4,
    HDMI_CEC_LOG_ADDR_AUDIO_SYSTEM = 5,
    HDMI_CEC_LOG_ADDR_TUNER_2 = 6,
    HDMI_CEC_LOG_ADDR_TUNER_3 = 7,
    HDMI_CEC_LOG_ADDR_PLAYBACK_DEVICE_2 = 8,
    HDMI_CEC_LOG_ADDR_RECORDING_DEVICE_3 = 9,
    HDMI_CEC_LOG_ADDR_TUNER_4 = 10,
    HDMI_CEC_LOG_ADDR_PLAYBACK_DEVICE_3 = 11,
    HDMI_CEC_LOG_ADDR_BACKUP_1 = 12,
    HDMI_CEC_LOG_ADDR_BACKUP_2 = 13,
    HDMI_CEC_LOG_ADDR_SPECIFIC_USE = 14,
    HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST = 15, /*
                                                       * Unregistered(as initiator address);
                                                       * Broadcast(as destination address).
                                                       */
    HDMI_CEC_LOG_ADDR_BUTT,
};

enum HdmiCecLogicalAddrMark {
    HDMI_CEC_LOG_ADDR_MASK_TV = (1 << HDMI_CEC_LOG_ADDR_TV),
    HDMI_CEC_LOG_ADDR_MASK_RECORD = ((1 << HDMI_CEC_LOG_ADDR_RECORDING_DEVICE_1) |
        (1 << HDMI_CEC_LOG_ADDR_RECORDING_DEVICE_2) | (1 << HDMI_CEC_LOG_ADDR_RECORDING_DEVICE_3)),
    HDMI_CEC_LOG_ADDR_MASK_TUNER = ((1 << HDMI_CEC_LOG_ADDR_TUNER_1) | (1 << HDMI_CEC_LOG_ADDR_TUNER_2) |
        (1 << HDMI_CEC_LOG_ADDR_TUNER_3) | (1 << HDMI_CEC_LOG_ADDR_TUNER_4)),
    HDMI_CEC_LOG_ADDR_MASK_PLAYBACK = ((1 << HDMI_CEC_LOG_ADDR_PLAYBACK_DEVICE_1) |
        (1 << HDMI_CEC_LOG_ADDR_PLAYBACK_DEVICE_2) | (1 << HDMI_CEC_LOG_ADDR_PLAYBACK_DEVICE_3)),
    HDMI_CEC_LOG_ADDR_MASK_AUDIOSYSTEM = (1 << HDMI_CEC_LOG_ADDR_AUDIO_SYSTEM),
    HDMI_CEC_LOG_ADDR_MASK_BACKUP = ((1 << HDMI_CEC_LOG_ADDR_BACKUP_1) | (1 << HDMI_CEC_LOG_ADDR_BACKUP_2)),
    HDMI_CEC_LOG_ADDR_MASK_SPECIFIC = (1 << HDMI_CEC_LOG_ADDR_SPECIFIC_USE),
    HDMI_CEC_LOG_ADDR_MASK_UNREGISTERED = (1 << HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST),
};

enum HdmiCecLogicalAddrType {
    HDMI_CEC_LOG_ADDR_TYPE_TV = 0,
    HDMI_CEC_LOG_ADDR_TYPE_RECORD = 1,
    HDMI_CEC_LOG_ADDR_TYPE_TUNER = 2,
    HDMI_CEC_LOG_ADDR_TYPE_PLAYBACK = 3,
    HDMI_CEC_LOG_ADDR_TYPE_AUDIOSYSTEM = 4,
    HDMI_CEC_LOG_ADDR_TYPE_SPECIFIC = 5,
    HDMI_CEC_LOG_ADDR_TYPE_UNREGISTERED = 6,
};

enum HdmiCecMsgType {
    HDMI_CEC_MSG_BROADCAST_1_4 = 0x01,
    HDMI_CEC_MSG_BROADCAST_2_0 = 0x02,
    HDMI_CEC_MSG_BROADCAST = 0x03,
    HDMI_CEC_MSG_DIRECTED = 0x04,
    HDMI_CEC_MSG_DIRECTED_OR_BROADCAST_1_4 = 0x05,
    HDMI_CEC_MSG_DIRECTED_OR_BROADCAST_2_0 = 0x06,
    HDMI_CEC_MSG_ALL = 0x07,
};

struct HdmiCecMsgLenInfo {
    uint8_t opcode;
    uint8_t minLen;
    uint8_t type;
};

#define HDMI_CEC_WAIT_RESPONSE_MSG_MAX_TIME 1000 /* 1s */
struct HdmiCecMsg {
    uint32_t len;
    uint8_t data[HDMI_CEC_MSG_MAX_LEN];
    uint8_t rspMsg;
    bool response;
    uint32_t timeout; /* Indicates the maximum time(in ms) for the peer to reply to a message. */
};

#define HDMI_CEC_MSG_DATA_ZEROTH_ELEMENT 0
#define HDMI_CEC_MSG_DATA_FIRST_ELEMENT 1
#define HDMI_CEC_MSG_DATA_SECOND_ELEMENT 2
#define HDMI_CEC_MSG_DATA_THIRD_ELEMENT 3
#define HDMI_CEC_MSG_DATA_FORTH_ELEMENT 4
#define HDMI_CEC_MSG_DATA_FIFTH_ELEMENT 5
#define HDMI_CEC_MSG_DATA_SIXTH_ELEMENT 6
#define HDMI_CEC_MSG_DATA_SEVENTH_ELEMENT 7
#define HDMI_CEC_MSG_DATA_EIGHTH_ELEMENT 8
#define HDMI_CEC_MSG_DATA_NINTH_ELEMENT 9

#define DATA_ZEROTH_OFFSET_ELEMENT 0
#define DATA_FIRST_OFFSET_ELEMENT 1
#define DATA_SECOND_OFFSET_ELEMENT 2
#define DATA_THIRD_OFFSET_ELEMENT 3
#define DATA_FORTH_OFFSET_ELEMENT 4
#define DATA_FIFTH_OFFSET_ELEMENT 5
#define DATA_SIXTH_OFFSET_ELEMENT 6
#define DATA_SEVENTH_OFFSET_ELEMENT 7
#define DATA_EIGHTH_OFFSET_ELEMENT 8
#define DATA_NINTH_OFFSET_ELEMENT 9
#define DATA_TENTH_OFFSET_ELEMENT 10

struct HdmiCecInfo {
    uint8_t logAddr;
    uint8_t logAddrType;
    uint16_t logAddrMask;
    uint8_t primaryDeviceType;
    uint8_t cecVersion;
    uint16_t phyAddr;
    uint32_t vendorId;
    uint32_t flags;
    uint8_t osdName[HDMI_CEC_OSD_NAME_MAX_LEN + 1];
    uint8_t osdNameLen;
    uint8_t allDeviceType;
    uint8_t rcProfile[HDMI_CEC_RC_PROFILE_MAX_NUM];
    uint8_t devFeatures[HDMI_CEC_DEVICE_FEATURES_MAX_NUM];
    bool isWaitingResponse;
    uint8_t response;
};

struct HdmiCec {
    struct HdmiCecInfo info;
    void *priv;
};

static inline bool HdmiCecIsBroadcastMsg(struct HdmiCecMsg *msg)
{
    if ((msg->data[0] & HDMI_CEC_HEADER_BLOCK_DESTINATION_MARK) ==
        HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST) {
        return true;
    }
    return false;
}

static inline uint8_t HdmiCecGetMsgInitiator(struct HdmiCecMsg *msg)
{
    return ((msg->data[0] & HDMI_CEC_HEADER_BLOCK_INITIATOR_MARK) >> HDMI_CEC_HEADER_BLOCK_INITIATOR_SHIFT);
}

static inline uint8_t HdmiCecGetMsgDestination(struct HdmiCecMsg *msg)
{
    return (msg->data[0] & HDMI_CEC_HEADER_BLOCK_DESTINATION_MARK);
}

static inline bool HdmiCecLogAddrValid(struct HdmiCec *cec, uint8_t logAddr)
{
    return (cec->info.logAddr & (1 << logAddr));
}

static inline bool HdmiCecIsCdcOnlyDevice(struct HdmiCec *cec)
{
    return ((cec->info.logAddr == HDMI_CEC_LOG_ADDR_UNREGISTERED_OR_BROADCAST) &&
        (cec->info.primaryDeviceType == HDMI_CEC_DEVICE_TYPE_PURE_CEC_SWITCH));
}

static inline void HdmiCecFillMsgHeader(struct HdmiCecMsg *destMsg, struct HdmiCecMsg *srcMsg)
{
    destMsg->data[0] = (HdmiCecGetMsgDestination(srcMsg) << HDMI_CEC_HEADER_BLOCK_INITIATOR_SHIFT) |
        HdmiCecGetMsgInitiator(srcMsg);
}

int32_t HdmiCecReceivedMsg(struct HdmiCec *cec, struct HdmiCecMsg *msg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* HDMI_CEC_H */
