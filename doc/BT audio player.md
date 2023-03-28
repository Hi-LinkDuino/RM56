# BT audio player #

BES2600WM SDK supports Bluetooth audio play function. This document introduces the usage of BT sink.

##### Initialize BT

```c
void bes_bt_init(void)
```

该函数初始化BES蓝牙基本功能，包括controller、驱动、profiles和app。

This function initialize BES BT basic functions including controller/driver/profiles and app.  This function  encapsulates all the initialized functions and shouldn't be adjusted by customers.

##### Set access mode

```c
bt_status_t btif_me_set_accessible_mode(btif_accessible_mode_t mode, const btif_access_mode_info_t * info);
```

The function has two parameters:

```c
typedef uint8_t btif_accessible_mode_t;

typedef struct {
    uint16_t inqInterval;            /* Inquiry scan interval */
    uint16_t inqWindow;              /* Inquiry scan Window */
    uint16_t pageInterval;           /* Page scan interval */
    uint16_t pageWindow;             /* Page scan window */
} btif_access_mode_info_t;
```

mode can be following values：

```c
#define BTIF_BAM_NOT_ACCESSIBLE     0x00    /* Non-discoverable or connectable */
#define BTIF_BAM_GENERAL_ACCESSIBLE 0x03    /* General discoverable and connectable */
#define BTIF_BAM_LIMITED_ACCESSIBLE 0x13    /* Limited discoverable and connectable */
#define BTIF_BAM_CONNECTABLE_ONLY   0x02    /* Connectable but not discoverable */
#define BTIF_BAM_DISCOVERABLE_ONLY  0x01    /* Discoverable but not connectable */
#define BTIF_BT_DEFAULT_ACCESS_MODE_PAIR       BTIF_BAM_GENERAL_ACCESSIBLE
#define BTIF_BAM_INVALID_ACCESS_MODE    0xFF
```

info value is recommend as following：

```c
const btif_access_mode_info_t info = { BTIF_BT_DEFAULT_INQ_SCAN_INTERVAL,
                                           BTIF_BT_DEFAULT_INQ_SCAN_WINDOW,
                                           BTIF_BT_DEFAULT_PAGE_SCAN_INTERVAL,
                                           BTIF_BT_DEFAULT_PAGE_SCAN_WINDOW
                                         };
```

The returned value is uint32_t，can be following values：

```c
enum _bt_status {
    BT_STS_SUCCESS = 0,
    BT_STS_FAILED = 1,
    BT_STS_PENDING = 2,
    BT_STS_BUSY = 11,
    BT_STS_NO_RESOURCES = 12,
    BT_STS_NOT_FOUND = 13,
    BT_STS_DEVICE_NOT_FOUND = 14,
    BT_STS_CONNECTION_FAILED = 15,
    BT_STS_TIMEOUT = 16,
    BT_STS_NO_CONNECTION = 17,
    BT_STS_INVALID_PARM = 18,
    BT_STS_IN_PROGRESS = 19,
    BT_STS_RESTRICTED = 20,
    BT_STS_INVALID_TYPE = 21,
    BT_STS_HCI_INIT_ERR = 22,
    BT_STS_NOT_SUPPORTED = 23,
    BT_STS_IN_USE = 5,
    BT_STS_SDP_CONT_STATE = 24,
    BT_STS_CONTINUE =24,
    BT_STS_CANCELLED = 25,

    /* The last defined status code */
    BT_STS_LAST_CODE = BT_STS_CANCELLED,

}
```

初始化蓝牙和设置访问模式成功后，可以在BT source端搜索到sink设备并连接，播放音乐。

After initializing BT and setting access mode, the BT source(e.g. mobile phone) can search and connect the sink device and play audio by BT.

##### BT sink audio control

```c
void app_bt_af_stream_start(void);
```

Starting to play BT sink audio stream play. When the source device connects to sink device and begins to play, the sink device will start audio stream play automatically. 

```c
void app_bt_af_stream_stop(void);
```

Stopping playing BT sink audio stream. Notice: When called this function, BT sink device still is receiving the  air data from source device, but don't decode and play. 

```c
void a2dp_handleKey(uint8_t a2dp_key);
```

The function encapsulates many bif calling and is easy to control a2dp function by caller.  It controls sink device by AVRCP profile. The key's value is following'：

```c
#define AVRCP_KEY_NULL                  0
#define AVRCP_KEY_STOP                  1
#define AVRCP_KEY_PLAY                  2
#define AVRCP_KEY_PAUSE                 3
#define AVRCP_KEY_FORWARD               4
#define AVRCP_KEY_BACKWARD              5
#define AVRCP_KEY_VOLUME_UP             6
#define AVRCP_KEY_VOLUME_DOWN           7
```

The volume configure is in the file *config\best2600w_aiot\tgt_hardware.c*

```c
const struct CODEC_DAC_VOL_T codec_dac_vol[TGT_VOLUME_LEVEL_QTY] = {

... ...

}
```

```c
void hfp_handle_key(uint8_t hfp_key);
```

The function encapsulates many bif calling and is easy to control HFP function by caller.  It controls sink device by HFP profile. The key's value is following'：

```c
#define HFP_KEY_ANSWER_CALL             8
#define HFP_KEY_HANGUP_CALL             9
#define HFP_KEY_REDIAL_LAST_CALL        10
#define HFP_KEY_CHANGE_TO_PHONE         11
#define HFP_KEY_ADD_TO_EARPHONE         12
#define HFP_KEY_MUTE                    13
#define HFP_KEY_CLEAR_MUTE              14
//3way calls oper
#define HFP_KEY_THREEWAY_HOLD_AND_ANSWER              15
#define HFP_KEY_THREEWAY_HANGUP_AND_ANSWER             16
#define HFP_KEY_THREEWAY_HOLD_REL_INCOMING            17
#define HFP_KEY_THREEWAY_HOLD_ADD_HELD_CALL           18

#define HFP_KEY_DUAL_HF_HANGUP_ANOTHER      19
#define HFP_KEY_DUAL_HF_HANGUP_CURR_ANSWER_ANOTHER  20
#define HFP_KEY_DUAL_HF_HOLD_CURR_ANSWER_ANOTHER    21
#define HFP_KEY_DUAL_HF_CHANGETOPHONE_ANSWER_ANOTHER  22
#define HFP_KEY_DUAL_HF_CHANGETOPHONE_ANOTHER_ADDTOEARPHONE 23
#define HFP_KEY_DUAL_HF_HANGUP_ANOTHER_ADDTOEARPHONE 24
#define HFP_KEY_DUAL_HF_CHANGETOPHONE_ANSWER_CURR 25
```

