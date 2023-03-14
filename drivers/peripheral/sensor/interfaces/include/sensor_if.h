/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
 * @addtogroup Sensor
 * @{
 *
 * @brief Provides unified APIs for sensor services to access sensor drivers.
 *
 * A sensor service can obtain a sensor driver object or agent and then call APIs provided by this object or agent to
 * access different types of sensor devices based on the sensor IDs, thereby obtaining sensor information,
 * subscribing to or unsubscribing from sensor data, enabling or disabling a sensor,
 * setting the sensor data reporting mode, and setting sensor options such as the accuracy and measurement range.
 *
 * @since 2.2
 */

/**
 * @file sensor_if.h
 *
 * @brief Declares the APIs provided by the sensor module for obtaining sensor information, subscribing to or
 * unsubscribing from sensor data, enabling or disabling a sensor, setting the sensor data reporting mode,
 * and setting sensor options such as the accuracy and measurement range.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef SENSOR_IF_H
#define SENSOR_IF_H

#include "sensor_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Defines the functions for performing basic operations on sensors.
 *
 * The operations include obtaining sensor information, subscribing to or unsubscribing from sensor data,
 * enabling or disabling a sensor, setting the sensor data reporting mode, and setting sensor options such as
 * the accuracy and measurement range.
 */
struct SensorInterface {
    /**
     * @brief Obtains information about all sensors in the system.
     *
     * @param sensorInfo Indicates the double pointer to the information about all sensors in the system.
     * The information about a sensor generally includes the sensor name, sensor vendor, firmware version,
     * hardware version, sensor type ID, sensor ID, maximum measurement range, accuracy, and power. For details,
     * see {@link SensorInformation}.
     * @param count Indicates the pointer to the total number of sensors in the system.
     * @return Returns <b>0</b> if the information is obtained; returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*GetAllSensors)(struct SensorInformation **sensorInfo, int32_t *count);

    /**
     * @brief Enables the sensor available in the sensor list based on the specified sensor ID.
     * The subscriber can obtain the sensor data only after the sensor is enabled.
     *
     * @param sensorId Indicates the sensor ID. For details, see {@link SensorTypeTag}.
     * @return Returns <b>0</b> if the sensor is successfully enabled; returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*Enable)(int32_t sensorId);

    /**
     * @brief Disables an enabled sensor.
     *
     * @param sensorId Indicates the sensor ID. For details, see {@link SensorTypeTag}.
     * @return Returns <b>0</b> if the sensor is successfully disabled; returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*Disable)(int32_t sensorId);

    /**
     * @brief Sets the data sampling interval and data reporting interval for the specified sensor.
     *
     * @param sensorId Indicates the sensor ID. For details, see {@link SensorTypeTag}.
     * @param samplingInterval Indicates the sensor data sampling interval to set, in nanoseconds.
     * @param reportInterval Indicates the sensor data reporting interval, in nanoseconds.
     * @return Returns <b>0</b> if the setting is successful; returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*SetBatch)(int32_t sensorId, int64_t samplingInterval, int64_t reportInterval);

    /**
     * @brief Sets the data reporting mode for the specified sensor.
     *
     * @param sensorId Indicates the sensor ID. For details, see {@link SensorTypeTag}.
     * @param mode Indicates the data reporting mode to set. For details, see {@link SensorModeType}.
     * @return Returns <b>0</b> if the sensor data reporting mode is successfully set;
     * returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*SetMode)(int32_t sensorId, int32_t mode);

    /**
     * @brief Sets options for the specified sensor, including its measurement range and accuracy.
     *
     * @param sensorId Indicates the sensor ID. For details, see {@link SensorTypeTag}.
     * @param option Indicates the options to set, such as the measurement range and accuracy.
     * @return Returns <b>0</b> if the options are successfully set; returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*SetOption)(int32_t sensorId, uint32_t option);

    /**
     * @brief Registers the callback for reporting sensor data to the subscriber.
     *
     * @param groupId Indicates the sensor group ID.
     * The sensorId enumeration value range is 128-160, which means that the medical sensor service is subscribed.
     * It only needs to be subscribed once successfully, and there is no need to subscribe repeatedly.
     * The sensorId enumeration value range is not within 128-160, which means that the traditional sensor
     * is subscribed, and the subscription is successful once.
     * @param cb Indicates the callback to register. For details, see {@link RecordDataCallback}.
     * @return Returns <b>0</b> if the callback is successfully registered; returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*Register)(int32_t groupId, RecordDataCallback cb);

    /**
     * @brief Deregisters the callback for reporting sensor data.
     *
     * @param groupId Indicates the sensor group ID.
     * The sensorId enumeration value range is 128-160, which means that the medical sensor service is subscribed.
     * It only needs to cancel the subscription once successfully, and there is no need to
     * cancel the subscription repeatedly. The sensorId enumeration value range is not within 128-160,
     * which means that the traditional sensor is subscribed. You can cancel the subscription once successfully.
     * @param cb Indicates the callback to register. For details, see {@link RecordDataCallback}.
     * @return Returns <b>0</b> if the callback is successfully deregistered; returns a negative value otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t (*Unregister)(int32_t groupId, RecordDataCallback cb);
};

/**
 * @brief Creates a <b>SensorInterface</b> instance.
 *
 * @param sensorId Indicates the sensor ID. For details, see {@link SensorTypeTag}.
 * You can use the instance to obtain sensor information, subscribe to or unsubscribe from sensor data,
 * enable or disable a sensor, set the sensor data reporting mode, and set the sensor options such as the accuracy and
 * measurement range.
 * @param cb Indicates the callback to register. For details, see {@link RecordDataCallback}.
 * @return Returns a non-zero value if the instance is successfully created; returns <b>0</b> otherwise.
 *
 * @since 2.2
 * @version 1.0
 */
const struct SensorInterface *NewSensorInterfaceInstance(void);

/**
 * @brief Releases the <b>SensorInterface</b> instance.
 *
 * @return Returns <b>0</b> if the instance is successfully released; returns a negative value otherwise.
 *
 * @since 2.2
 * @version 1.0
 */
int32_t FreeSensorInterfaceInstance(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* SENSOR_IF_H */
/** @} */
