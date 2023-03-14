/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

/**
 * @addtogroup wifiaware
 * @{
 *
 * @brief Enables devices to interact with neighboring devices without connecting to a Wi-Fi network.
 *
 *
 * Using the capabilities provided by this module, a Wi-Fi device can quickly discover and connect to other devices. \n
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file wifiaware.h
 *
 * @brief Defines functions of the Wi-Fi Aware module.
 *
 * You can use the functions to: \n
 * <ul>
 *   <li>Initialize and deinitialize the module.</li>
 *   <li>Start and stop the Wi-Fi Aware subscription service.</li>
 *   <li>Send data to and receive data from a peer device.</li>
 *   <li>Enable and disable short-distance transmission.</li>
 * </ul>
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef WIFI_AWARE_H
#define WIFI_AWARE_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Indicates that short-distance transmission is enabled.
 *
 */
#define WIFIAWARE_LOW_POWER_SWITCH_ON       1
/**
 * @brief Indicates that short-distance transmission is disabled.
 *
 */
#define WIFIAWARE_LOW_POWER_SWITCH_OFF      0

/**
 * @brief Indicates a successful operation.
 *
 */
#define WIFIAWARE_SUCCESS                   0
/**
 * @brief Indicates an operation failure.
 *
 */
#define WIFIAWARE_FAIL                      (-1)

/**
 * @brief Indicates the default channel of Wi-Fi Aware, which is channel 6.
 *
 */
#define WIFIAWARE_DEFAULT_CHANNEL           6

/**
 * @brief Enumerates Neighbor Awareness Networking (NAN) synchronization modes.
 *
 */
enum NanSyncMode {
    /** Huawei private NAN synchronization */
    NAN_SYNC_MODE_PRIVATE,
    /** Standard NAN synchronization */
    NAN_SYNC_MODE_STANDARD,
    /** Both Huawei private and standard NAN synchronization */
    NAN_SYNC_MODE_BOTH,
    /** BUTT value, which represents the total number of enumerated values (excluding the BUTT value itself) */
    NAN_SYNC_MODE_BUTT
};

/**
 * @brief Defines the type of the callback that is invoked when the subscription service receives a message.
 *
 * This callback only processes time-efficient transactions. To process time-consuming transactions, you need to create
 * specific tasks.
 *
 * @param macAddr Indicates the MAC address of the device that sends the message.
 * @param peerHandle Indicates the instance ID of the device that sends the message.
 * @param localHandle Indicates the instance ID of the device that receives the message.
 * @param msg Indicates the message received.
 * @param len Indicates the length of the message received.
 * @return Returns {@link WIFIAWARE_SUCCESS} if the callback is invoked successfully; returns {@link WIFIAWARE_FAIL}
 * otherwise.
 * @since 1.0
 * @version 1.0
 */
typedef int (*RecvCallback)(unsigned char* macAddr, unsigned char peerHandle, unsigned char localHandle,
    unsigned char* msg, unsigned char len);

/**
 * @brief Initializes the Wi-Fi Aware module.
 *
 * Before using this function, ensure that the access point (AP) interface is enabled on the device and channel 6 is
 * used. Otherwise, calling this function will fail. \n
 * You can enable the AP interface using the corresponding function or AT command. \n
 *
 * @return Returns {@link WIFIAWARE_SUCCESS} if the Wi-Fi Aware module is initialized; returns {@link WIFIAWARE_FAIL}
 * otherwise.
 * @since 1.0
 * @version 1.0
 */
int InitNAN(void);

/**
 * @brief Deinitializes the Wi-Fi Aware module.
 *
 * This function takes effect regardless of whether the module has been initialized. \n
 *
 * @return Returns {@link WIFIAWARE_SUCCESS} if the Wi-Fi Aware module is deinitialized; returns {@link WIFIAWARE_FAIL}
 * otherwise.
 * @since 1.0
 * @version 1.0
 */
int DeinitNAN(void);

/**
 * @brief Starts the Wi-Fi Aware subscription service.
 *
 *
 * Before using this function, call {@link InitNAN} to initialize the Wi-Fi Aware module. \n
 *
 * @param svcName Indicates the service name. It is a string and cannot be <b>NULL</b>.
 * @param localHandle Indicates the instance ID of the local device associated with the service name. The value ranges
 * from 1 to 255. It is used for identify authentication during device interaction.
 * @param recvCB Indicates the callback invoked when the Wi-Fi Aware subscription service receives a message.
 * @return Returns {@link WIFIAWARE_SUCCESS} if the Wi-Fi Aware subscription service is started;
 * returns {@link WIFIAWARE_FAIL} otherwise.
 * @since 1.0
 * @version 1.0
 */
int SubscribeService(const char* svcName, unsigned char localHandle, RecvCallback recvCB);

/**
 * @brief Sends user data to the peer device.
 *
 *
 * This is an asynchronous function. The return value only indicates whether the user data has been sent to
 * the hardware driver. \n
 * As this function does not check whether the MAC address and instance ID are valid, the caller should
 * ensure their validity. \n
 *
 * @param macAddr Indicates the MAC address of the peer device. It contains 6 bytes.
 * @param peerHandle Indicates the instance ID of the peer device. It contains 1 to 255 bytes.
 * @param localHandle Indicates the instance ID of the local device specified when starting the Wi-Fi Aware
 * subscription service.
 * @param msg Indicates the user data to send. It cannot exceed 255 bytes.
 * @param len Indicates the length of the user data to send.
 * @return Returns {@link WIFIAWARE_SUCCESS} if the user data is sent to the hardware driver;
 * returns {@link WIFIAWARE_FAIL} otherwise.
 * @since 1.0
 * @version 1.0
 */
int SendData(unsigned char* macAddr, unsigned char peerHandle, unsigned char localHandle,
    unsigned char* msg, int len);

/**
 * @brief Stops the Wi-Fi Aware subscription service.
 *
 *
 * Before using this function, call {@link InitNAN} to initialize the Wi-Fi Aware module. \n
 * This function takes effect regardless of whether the Wi-Fi Aware subscription service has been started. \n
 *
 * @param localHandle Indicates the instance ID of the local device specified when starting the Wi-Fi Aware
 * subscription service.
 * @return Returns {@link WIFIAWARE_SUCCESS} if the Wi-Fi Aware subscription service is stopped;
 * returns {@link WIFIAWARE_FAIL} otherwise.
 * @since 1.0
 * @version 1.0
 */
int StopSubscribe(unsigned char localHandle);

/**
 * @brief Sets whether to enable short-distance transmission.
 *
 * Short-distance transmission is determined by {@link WIFIAWARE_LOW_POWER_SWITCH_ON}
 * and {@link WIFIAWARE_LOW_POWER_SWITCH_OFF}. \n
 * When short-distance transmission is enabled, messages can be sent to devices
 * within a distance of 30 cm from the local device. \n
 * Before using this function, ensure that the AP interface is enabled. \n
 *
 * @param powerSwitch Specifies whether to enable short-distance transmission. The value can be
 * {@link WIFIAWARE_LOW_POWER_SWITCH_ON} or {@link WIFIAWARE_LOW_POWER_SWITCH_OFF}.
 * @return Returns {@link WIFIAWARE_SUCCESS} if short-distance transmission is set;
 * returns {@link WIFIAWARE_FAIL} otherwise.
 * @since 1.0
 * @version 1.0
 */
int SetLowPower(int powerSwitch);

/**
 * @brief Sets the transmit power in short-distance transmission mode.
 *
 * If you call {@link SetLowPower} with <b>powerSwitch</b> set to {@link WIFIAWARE_LOW_POWER_SWITCH_ON},the Wi-Fi Aware
 * module enters the short-distance transmission mode and sends data at the transmit power set by this function. \n
 * Before using this function, ensure that the AP has been started. \n
 *
 * @param power Indicates the transmit power to set. The recommended value range is -70 dB to -42 dB, and the default
 * value is <b>-61 dB</b>.
 * @return Returns {@link WIFIAWARE_SUCCESS} if the transmit power is set; returns {@link WIFIAWARE_FAIL} otherwise.
 * @since 2.2
 * @version 1.0
 */
int SetPower(signed char value);

/**
 * @brief Sets whether the AP can be discovered.
 *
 * By default, the AP can be discovered.
 * Before using this function, ensure that the AP has been started. \n
 *
 * @param enable Specifies whether the AP can be discovered. The value <b>true</b> indicates that the AP can be
 * discovered, and <b>false</b> indicates the opposite.
 * @return Returns {@link WIFIAWARE_SUCCESS} if the setting is successful; returns {@link WIFIAWARE_FAIL} otherwise.
 * @since 2.2
 * @version 1.0
 */
int NanBeaconSwitch(unsigned char enable);

/**
 * @brief Sets the number of NAN packet retransmissions in short-distance transmission mode.
 *
 * When you call {@link SendData} to send packets, the Wi-Fi Aware module will retransmit the packets based on the
 * number set using this function once packet transmission fails. \n
 * Before using this function, ensure that the AP has been started. \n
 *
 * @param retries Indicates the number of NAN packet retransmissions to set. The recommended value range is 0 to 200,
 * and the default value is <b>200</b>.
 * @return Returns {@link WIFIAWARE_SUCCESS} if the number of NAN packet retransmissions is set;
 * returns {@link WIFIAWARE_FAIL} otherwise.
 * @since 2.2
 * @version 1.0
 */
int NanSetRetryTimes(unsigned int retries);

/**
 * @brief Obtains the NAN synchronization mode supported by the Wi-Fi Aware module.
 *
 * @return Returns the NAN synchronization mode, as enumerated in {@link NanSyncMode}.
 * @since 2.2
 * @version 1.0
 */
int NanGetSyncMode(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
/** @} */