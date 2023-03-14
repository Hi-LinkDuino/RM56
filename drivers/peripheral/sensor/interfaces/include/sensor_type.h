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
 * @version 1.0
 */

/**
 * @file sensor_type.h
 *
 * @brief Defines the data used by the sensor module, including the sensor types, sensor information,
 * and reported sensor data.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef SENSOR_TYPE_H
#define SENSOR_TYPE_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SENSOR_NAME_MAX_LEN       32 /**< Maximum length of the sensor name */
#define SENSOR_VERSION_MAX_LEN    16 /**< Maximum length of the sensor version */

/**
 * @brief Enumerates return values of the sensor module.
 *
 * @since 2.2
 */
enum SensorStatus {
    SENSOR_SUCCESS           = 0,    /**< The operation is successful. */
    SENSOR_FAILURE           = -1,   /**< The operation failed. */
    SENSOR_NOT_SUPPORT       = -2,   /**< The operation is not supported. */
    SENSOR_INVALID_PARAM     = -3,   /**< The sensor parameter is invalid. */
    SENSOR_INVALID_SERVICE   = -4,   /**< The sensor service is invalid. */
    SENSOR_NULL_PTR          = -5,   /**< The pointer is null. */
};

/**
 * @brief Enumerates sensor types.
 *
 * @since 2.2
 */
enum SensorTypeTag {
    SENSOR_TYPE_NONE                = 0,   /**< None, for testing only */
    SENSOR_TYPE_ACCELEROMETER       = 1,   /**< Acceleration sensor */
    SENSOR_TYPE_GYROSCOPE           = 2,   /**< Gyroscope sensor */
    SENSOR_TYPE_PHOTOPLETHYSMOGRAPH = 3,   /**< Photoplethysmography sensor */
    SENSOR_TYPE_ELECTROCARDIOGRAPH  = 4,   /**< Electrocardiogram (ECG) sensor */
    SENSOR_TYPE_AMBIENT_LIGHT       = 5,   /**< Ambient light sensor */
    SENSOR_TYPE_MAGNETIC_FIELD      = 6,   /**< Magnetic field sensor */
    SENSOR_TYPE_CAPACITIVE          = 7,   /**< Capacitive sensor */
    SENSOR_TYPE_BAROMETER           = 8,   /**< Barometric pressure sensor */
    SENSOR_TYPE_TEMPERATURE         = 9,   /**< Temperature sensor */
    SENSOR_TYPE_HALL                = 10,  /**< Hall effect sensor */
    SENSOR_TYPE_GESTURE             = 11,  /**< Gesture sensor */
    SENSOR_TYPE_PROXIMITY           = 12,  /**< Proximity sensor */
    SENSOR_TYPE_HUMIDITY            = 13,  /**< Humidity sensor */
    SENSOR_TYPE_MEDICAL_BEGIN       = 128, /**< The begin of medical sensorId enumeration value range */
    SENSOR_TYPE_MEDICAL_END         = 160, /**< The end of medical sensorId enumeration value range */
    SENSOR_TYPE_PHYSICAL_MAX        = 255, /**< Maximum type of a physical sensor */
    SENSOR_TYPE_ORIENTATION         = 256, /**< Orientation sensor */
    SENSOR_TYPE_GRAVITY             = 257, /**< Gravity sensor */
    SENSOR_TYPE_LINEAR_ACCELERATION = 258, /**< Linear acceleration sensor */
    SENSOR_TYPE_ROTATION_VECTOR     = 259, /**< Rotation vector sensor */
    SENSOR_TYPE_AMBIENT_TEMPERATURE = 260, /**< Ambient temperature sensor */
    SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED = 261,   /**< Uncalibrated magnetic field sensor */
    SENSOR_TYPE_GAME_ROTATION_VECTOR        = 262,   /**< Game rotation vector sensor */
    SENSOR_TYPE_GYROSCOPE_UNCALIBRATED      = 263,   /**< Uncalibrated gyroscope sensor */
    SENSOR_TYPE_SIGNIFICANT_MOTION          = 264,   /**< Significant motion sensor */
    SENSOR_TYPE_PEDOMETER_DETECTION         = 265,   /**< Pedometer detection sensor */
    SENSOR_TYPE_PEDOMETER                   = 266,   /**< Pedometer sensor */
    SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR = 277,   /**< Geomagnetic rotation vector sensor */
    SENSOR_TYPE_HEART_RATE                  = 278,   /**< Heart rate sensor */
    SENSOR_TYPE_DEVICE_ORIENTATION          = 279,   /**< Device orientation sensor */
    SENSOR_TYPE_WEAR_DETECTION              = 280,   /**< Wear detection sensor */
    SENSOR_TYPE_ACCELEROMETER_UNCALIBRATED  = 281,   /**< Uncalibrated acceleration sensor */
    SENSOR_TYPE_MAX,                                 /**< Maximum number of sensor types */
};

/**
 * @brief Enumerates accuracy types of sensors.
 *
 * @since 2.2
 */
enum SensorAccuracyType {
    SENSOR_NO_ACCURACY     = 0,  /**< No accuracy */
    SENSOR_LOW_ACCURACY    = 1,  /**< Low accuracy */
    SENSOR_MEDIUM_ACCURACY = 2,  /**< Medium accuracy */
    SENSOR_HIGH_ACCURACY   = 3,  /**< High accuracy */
    SENSOR_MAX_ACCURACY,         /**< Maximum accuracy */
};

/**
 * @brief Enumerates measurement ranges of sensors.
 *
 * @since 2.2
 */
enum SensorRangeType {
    SENSOR_RANGE_LEVEL1 = 0, /**< Measurement range level 1 */
    SENSOR_RANGE_LEVEL2 = 1, /**< Measurement range level 2 */
    SENSOR_RANGE_LEVEL3 = 2, /**< Measurement range level 3 */
    SENSOR_RANGE_LEVEL_MAX,  /**< Maximum measurement range level */
};

/**
 * @brief Enumerates data reporting modes of sensors.
 *
 * @since 2.2
 */
enum SensorModeType {
    SENSOR_MODE_DEFAULT   = 0, /**< Default data reporting mode */
    SENSOR_MODE_REALTIME  = 1, /**< Real-time data reporting mode to report a group of data each time */
    SENSOR_MODE_ON_CHANGE = 2, /**< Real-time data reporting mode to report data upon status changes */
    SENSOR_MODE_ONE_SHOT  = 3, /**< Real-time data reporting mode to report data only once */
    SENSOR_MODE_FIFO_MODE = 4, /**< FIFO-based data reporting mode to report data based on the configured cache size */
    SENSOR_MODE_MAX,           /**< Maximum sensor data reporting mode */
};

/**
 * @brief Enumerates hardware service group for sensors
 *
 * @since 2.2
 */
enum SensorGroupType {
    TRADITIONAL_SENSOR_TYPE = 0, /**< traditional sensor type, the sensorId enumeration value range is 128-160 */
    MEDICAL_SENSOR_TYPE = 1,  /**< medical sensor type, the sensorId enumeration value range is not within 128-160 */
    SENSOR_GROUP_TYPE_MAX,          /**< Maximum sensor type*/
};

/**
 * @brief Defines basic sensor information.
 *
 * Information about a sensor includes the sensor name, vendor, firmware version, hardware version, sensor type ID,
 * sensor ID, maximum measurement range, accuracy, and power.
 *
 * @since 2.2
 */
struct SensorInformation {
    char sensorName[SENSOR_NAME_MAX_LEN]; /**< Sensor name */
    char vendorName[SENSOR_NAME_MAX_LEN]; /**< Sensor vendor */
    char firmwareVersion[SENSOR_VERSION_MAX_LEN]; /**< Sensor firmware version */
    char hardwareVersion[SENSOR_VERSION_MAX_LEN]; /**< Sensor hardware version */
    int32_t sensorTypeId; /**< Sensor type ID (described in {@link SensorTypeTag}) */
    int32_t sensorId;     /**< Sensor ID, defined by the sensor driver developer */
    float maxRange;       /**< Maximum measurement range of the sensor */
    float accuracy;       /**< Sensor accuracy */
    float power;          /**< Sensor power */
};

/**
 * @brief Defines the data reported by the sensor.
 *
 * The reported sensor data includes the sensor ID, sensor algorithm version, data generation time,
 * data options (such as the measurement range and accuracy), data reporting mode, data address, and data length.
 *
 * @since 2.2
 */
struct SensorEvents {
    int32_t sensorId;  /**< Sensor ID */
    int32_t version;   /**< Sensor algorithm version */
    int64_t timestamp; /**< Time when sensor data was generated */
    uint32_t option;   /**< Sensor data options, including the measurement range and accuracy */
    int32_t mode;      /**< Sensor data reporting mode */
    uint8_t *data;     /**< Sensor data address */
    uint32_t dataLen;  /**< Sensor data length */
};

/**
 * @brief Defines the callback for reporting sensor data. This callback needs to be registered when
 * a sensor user subscribes to sensor data. Only after the sensor is enabled, the sensor data subscriber can receive
 * sensor data. For details, see {@link SensorInterface}.
 *
 * @since 2.2
 */
typedef int32_t (*RecordDataCallback)(const struct SensorEvents*);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* SENSOR_TYPE_H */
/** @} */
