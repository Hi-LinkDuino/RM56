/**
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
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


export const AUTO_ACCEPT_NUMBER = '10010';
export const AUTO_ACCEPT_NUMBER2 = '10010';
export const PHONE_NUMBER_AREA_LAND = '089800000000';
export const PHONE_NUMBER_LENGTH_8 = '86459751';

export const PHONE_NUMBER_LENGTH_11 = '10000000001';
export const PHONE_NUMBER_VOICE_MAIL = '12599';
export const PHONE_NUMBER_LONG = '1234567890123456789012345678901234567890123' +
    '4567890123456789012345678901234567890123456789012345678901';
export const EMERGENCY_NUMBER = '112';
export const RIGHT_PASSWORD = '000001';
export const ERROR_PASSWORD = '1234567890123456789012345678901234567890123' +
'4567890123456789012345678901234567890123456789012345678901';
export const CALL_EMERGENCY = 2;
export const PHONE_LIST = ['10000000001'];
export const PHONE_LIST2 = ['10000000001', '10000000002'];
export const NULL_PHONE_NUMBER = '';
export const SPECIA_CHAR_NUMBER = '"=#$%^&*()+';
export const PHONE_NUMBER_CHAR = 'abcdefghijk';
export const BOUNDARY_NUMBER_INT = 2147483649;
export const MINUS_BOUNDARY_NUMBER_INT = -2147483649;
export const MINUS_VALUE = -100;
export const TEST_RELY_NUMBER = 20; // test count for Reliability
export const TEST_PERF_COUNT = 20; // test count for Perfomace
export const PERF_GOAL_TIME = TEST_PERF_COUNT * 0.5;

export const CALL_STATUS_ACTIVE = 0;
export const CALL_STATUS_HOLDING = 1;
export const CALL_STATUS_DIALING = 2;
export const CALL_STATUS_ALERTING = 3;
export const CALL_STATUS_INCOMING = 4;
export const CALL_STATUS_WAITING = 5;
export const CALL_STATUS_DISCONNECTED = 6;
export const CALL_STATUS_DISCONNECTING = 7;
export const CALL_STATUS_IDLE = 8;

export const TEL_CONFERENCE_IDLE = 0;
export const TEL_CONFERENCE_ACTIVE = 1;
export const TEL_CONFERENCE_DISCONNECTING = 2;
export const TEL_CONFERENCE_DISCONNECTED = 3;

export const DEVICE_EARPIECE = 0;
export const DEVICE_SPEAKER = 1;
export const DEVICE_WIRED_HEADSET = 2;
export const DEVICE_BLUETOOTH_SCO = 3;
export const DEVICE_INVALID = -1;
export const DEVICE_INVALID4 = 4;

export const RESTRICTION_MODE_DEACTIVATION = 0;
export const RESTRICTION_MODE_ACTIVATION = 1;
export const RESTRICTION_MODE_INVALID = -1;
export const RESTRICTION_MODE_INVALID2 = 2;

export const RESTRICTION_DISABLE = 0;
export const RESTRICTION_ENABLE = 1;

export const RESTRICTION_TYPE_ALL_INCOMING = 0;
export const RESTRICTION_TYPE_ALL_OUTGOING = 1;
export const RESTRICTION_TYPE_INTERNATIONAL = 2;
export const RESTRICTION_TYPE_INTERNATIONAL_EXCLUDING_HOME = 3;
export const RESTRICTION_TYPE_ROAMING_INCOMING = 4;
export const RESTRICTION_TYPE_INCOMING_SERVICES = 7;
export const RESTRICTION_TYPE_OUTGOING_SERVICES = 6;
export const RESTRICTION_TYPE_ALL_CALLS = 5;
export const RESTRICTION_TYPE_INVALID = -1;
export const RESTRICTION_TYPE_INVALID2 = 8;

export const DEFAULT_SLOT_ID = 0;
export const SLOT_ID1 = 1;
export const SLOT_ID_INVALID = -1;
export const SLOT_ID_INVALID_2 = 2;
export const CALL_ID_NOT_EXIST = 999;
export const VIDEO_STATE_AUDIO = 0;
export const CALL_TYPE_CS = 0;
export const CALL_TYPE_IMS = 1;

export const EVENT_START_DTMF_SUCCESS = 3;
export const EVENT_STOP_DTMF_SUCCESS = 5;

export const CALL_STATE_UNKNOWN = -1;
export const CALL_STATE_IDLE = 0;
export const CALL_STATE_RINGING = 1;
export const CALL_STATE_OFFHOOK = 2;

export const TIMEOUT_LENTH = 1000;

// call error code
export const CALL_MANAGER_NUMBER_NULL_NOTICE = '83951616';

export const TRANSFER_DISABLE = 0; // tran is close
export const TRANSFER_ENABLE = 1; // tran is open

export const CALL_TRANSFER_ENABLE = 0;
export const CALL_TRANSFER_DISABLE = 1;
export const CALL_TRANSFER_REGISTRATION = 3; // open tran function
export const CALL_TRANSFER_ERASURE = 4; // close tran function
export const CALL_TRANSFER_INVALID = -1;
export const CALL_TRANSFER_INVALID2 = 5;
export const CALL_TRANSFER_INVALID3 = 2;

export const TRANSFER_TYPE_BUSY = 1; // tran busy
export const TRANSFER_TYPE_NO_REPLY = 2;// no response tran
export const TRANSFER_TYPE_NOT_REACHABLE = 3; // untouchable（no signal/shut down） tran
export const TRANSFER_TYPE_UNCONDITIONAL = 0; // unconditional tran
export const TRANSFER_TYPE_INVALID = -1;
export const TRANSFER_TYPE_INVALID4 = 4;

export const REACH_TIMES = 10;
export const AFTER_HANUP_TIMES = 5;

export const CARMER_ID_NOT_EXIT = 'xtsautotest123456';
export const CARMER_ID_SPACE = '';
export const IMAGE_PNG_PATH = '/system/data/callManager.png';
export const IMAGE_JPG_PATH = '/system/data/callManager.jpg';
export const IMAGE_BMP_PATH = '/system/data/callManager.bmp';
export const IMAGE_WEBP_PATH = '/system/data/callManager.webp';
export const IMAGE_LOCAL_ERROR_PATH = '/system/data/callManager/notexistpath.png';
export const IMAGE_SPACE_PATH = '';
export const SENT_STRING_0 = '0';
export const SENT_STRING_1 = '1';
export const SENT_STRING_2 = '2';
export const SENT_STRING_3 = '3';
export const SENT_STRING_4 = '4';
export const SENT_STRING_5 = '5';
export const SENT_STRING_6 = '6';
export const SENT_STRING_7 = '7';
export const SENT_STRING_8 = '8';
export const SENT_STRING_9 = '9';
export const SENT_STRING_ERROR_10 = '10';
export const SENT_STRING_A = 'A';
export const SENT_STRING_Z = 'Z';
export const SENT_STRING_C = 'C';
export const SENT_STRING_D = 'D';
export const SENT_STRING_E = 'E';
export const SENT_STRING_LOW_A = 'a';
export const SENT_STRING_LOW_C = 'c';
export const SENT_STRING_LOW_B = 'b';
export const SENT_STRING_LOW_D = 'd';
export const SENT_STRING_STAR = '*';
export const SENT_STRING_ADD = '+';
export const SENT_STRING_MINUS = '-';
export const SENT_STRING_JIN = '#';
export const CALL_MODE_CS = 0;
export const CALL_MODE_IMS = 3;
export const MEDIA_TYPE_VOICE = 0;
export const MEDIA_TYPE_VIDEO = 1;
export const MEDIA_TYPE_ERROR = -1;
export const MEDIA_TYPE_ERROR2 = 2;
export const DIAL_SCENCE_CALL_NORMAL = 0;
export const DIAL_SCENCE_CALL_PRIVILEGED = 1;
export const DIAL_SCENCE_CALL_CALL_EMERGENCY = 2;
export const DIAL_SCENCE_ERROR = -1;
export const DIAL_SCENCE_ERROR2 = 3;
export const DIAL_CARRIER_TYPE = 0;
export const DIAL_VOICE_MAIL_TYPE = 1;
export const DIAL_OTT_TYPE = 2;
export const DIAL_TYPE_ERROR = -1;
export const DIAL_TYPE_ERROR3 = 3;

export const POS_700 = 700;
export const POS_10 = 10;
export const POS_Z_0 = 0;
export const POS_Z_1 = 1;
export const POS_Z_ERROR = -1;
export const POS_Z_ERROR2 = 2;
export const POS_LENGTH_300 = 300;
export const POS_LENGTH_600 = 600;

export const ZOOM_RATIO_5_0 = 5.0;
export const ZOOM_RATIO_15_0 = 15.0;
export const ZOOM_RATIO_0_0 = 0.0;
export const ZOOM_RATIO_10_1 = 10.1;
export const ZOOM_RATIO_MINUS_1_0 = -1.0;

export const ROTATION_0 = 0;
export const ROTATION_90 = 90;
export const ROTATION_60 = 60;
export const ROTATION_180 = 180;
export const ROTATION_270 = 270;
export const ROTATION_360 = 360;
export const ROTATION_MINUS_1 = -1;

export const COMMAND_CALL_WAITTING_OPEN = '*43#';
export const COMMAND_CALL_WAITTING_CLOSE = '#43#';
export const COMMAND_CALL_WAITTING_QUERY = '*#43#';
export const COMMAND_CALL_TRANS_OPEN = '*21*10000000001#';
export const COMMAND_CALL_TRANS_OPEN2 = '**21*10000000001*10#';
export const COMMAND_CALL_TRANS_RESET = '##21#';
export const COMMAND_CALL_ERROR = '**21*10000000001******10#';
export const COMMAND_CALL_ERROR2 = '*ABCDEF123456789#';
export const COMMAND_CALL_ERROR3 = '#ABCDEF123456789#';
export const COMMAND_CALL_ERROR4 = '*#ABCDE123456789#';
export const COMMAND_CALL_ERROR5 = '**ABCDE123456789#';
export const COMMAND_CALL_ERROR6 = '##ABCDEF123456789#';

export const CALL_WAITING_DISABLE = 0;
export const CALL_WAITING_ENABLE = 1;

export const EVENT_SEND_USSD_SUCCESS = 17;
export const EVENT_SEND_USSD_FAILED = 18;
export const EVENT_SEND_MEDIA_MODE_UPDATE = 19;

export const RTT_MSG = 'Ab666&#';

export const CALL_MODE_AUDIO_ONLY = 0;
export const CALL_MODE_SEND_RECEIVE = 3;
export const CALL_MODE_SEND_ONLY = 1;
export const CALL_MODE_RECEIVE_ONLY = 2;
export const CALL_MODE_VIDEO_PAUSED = 4;
export const CALL_MODE_INVALID_MINUS_1 = -1;
export const CALL_MODE_INVALID_5 = 5;

