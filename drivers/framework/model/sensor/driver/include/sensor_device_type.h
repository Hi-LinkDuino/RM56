/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef SENSOR_DEVICE_TYPE_H
#define SENSOR_DEVICE_TYPE_H

#include "hdf_base.h"

#define SENSOR_INFO_NAME_MAX_LEN      32
#define SENSOR_INFO_VERSION_MAX_LEN   16

enum SensorAccuracyMode {
    SENSOR_ACCURACY_NO     = 0,
    SENSOR_ACCURACY_LOW    = 1,
    SENSOR_ACCURACY_MEDIUM = 2,
    SENSOR_ACCURACY_HIGH   = 3,
    SENSOR_ACCURACY_MAX,
};

enum SensorRangeLevel {
    SENSOR_RANGE_LEVEL_1 = 0,
    SENSOR_RANGE_LEVEL_2 = 1,
    SENSOR_RANGE_LEVEL_3 = 2,
};

enum SensorWorkMode {
    SENSOR_WORK_MODE_DEFAULT   = 0,
    SENSOR_WORK_MODE_REALTIME  = 1,
    SENSOR_WORK_MODE_ON_CHANGE = 2,
    SENSOR_WORK_MODE_ONE_SHOT  = 3,
    SENSOR_WORK_MODE_FIFO      = 4,
    SENSOR_WORK_MODE_MAX,
};

enum SensorTag {
    SENSOR_TAG_NONE                = 0,   /**< No sensor type for sensor test */
    SENSOR_TAG_ACCELEROMETER       = 1,   /**< Acceleration sensor */
    SENSOR_TAG_GYROSCOPE           = 2,   /**< Gyroscope sensor */
    SENSOR_TAG_PHOTOPLETHYSMOGRAPH = 3,   /**< Photoplethysmography sensor */
    SENSOR_TAG_ELECTROCARDIOGRAPH  = 4,   /**< Electrocardiogram sensor */
    SENSOR_TAG_AMBIENT_LIGHT       = 5,   /**< Ambient light sensor */
    SENSOR_TAG_MAGNETIC_FIELD      = 6,   /**< Magnetic field sensor */
    SENSOR_TAG_CAPACITIVE          = 7,   /**< Capacitance sensor */
    SENSOR_TAG_BAROMETER           = 8,   /**< Barometric pressure sensor */
    SENSOR_TAG_TEMPERATURE         = 9,   /**< Temperature sensor */
    SENSOR_TAG_HALL                = 10,  /**< Hall effect sensor */
    SENSOR_TAG_GESTURE             = 11,  /**< Gesture sensor */
    SENSOR_TAG_PROXIMITY           = 12,  /**< Proximity sensor */
    SENSOR_TAG_HUMIDITY            = 13,  /**< Humidity sensor */
    SENSOR_TAG_PHYSICAL_MAX        = 255, /**< Maximum type of a physical sensor */
    SENSOR_TAG_ORIENTATION         = 256, /**< Orientation sensor */
    SENSOR_TAG_GRAVITY             = 257, /**< Gravity sensor */
    SENSOR_TAG_LINEAR_ACCELERATION = 258, /**< Linear acceleration sensor */
    SENSOR_TAG_ROTATION_VECTOR     = 259, /**< Rotation vector sensor */
    SENSOR_TAG_AMBIENT_TEMPERATURE = 260, /**< Ambient temperature sensor */
    SENSOR_TAG_MAGNETIC_FIELD_UNCALIBRATED = 261,   /**< Uncalibrated magnetic field sensor */
    SENSOR_TAG_GAME_ROTATION_VECTOR        = 262,   /**< Game rotation vector sensor */
    SENSOR_TAG_GYROSCOPE_UNCALIBRATED      = 263,   /**< Uncalibrated gyroscope sensor */
    SENSOR_TAG_SIGNIFICANT_MOTION          = 264,   /**< Significant motion sensor */
    SENSOR_TAG_PEDOMETER_DETECTION         = 265,   /**< Pedometer detection sensor */
    SENSOR_TAG_PEDOMETER                   = 266,   /**< Pedometer sensor */
    SENSOR_TAG_GEOMAGNETIC_ROTATION_VECTOR = 277,   /**< Geomagnetic rotation vector sensor */
    SENSOR_TAG_HEART_RATE                  = 278,   /**< Heart rate sensor */
    SENSOR_TAG_DEVICE_ORIENTATION          = 279,   /**< Device orientation sensor */
    SENSOR_TAG_WEAR_DETECTION              = 280,   /**< Wear detection sensor */
    SENSOR_TAG_ACCELEROMETER_UNCALIBRATED  = 281,   /**< Uncalibrated acceleration sensor */
    SENSOR_TAG_MAX                         = 0xFFF, /**< Maximum number of sensor types */
};

/**
 * @brief Defines the basic description of a sensor.
 *
 * The basic description of the sensor includes its vendor, version, type, ID,
 * measurement range, accuracy, and power consumption.
 */
struct SensorBasicInfo {
    char sensorName[SENSOR_INFO_NAME_MAX_LEN]; /**< Sensor name */
    char vendorName[SENSOR_INFO_NAME_MAX_LEN]; /**< Sensor vendor */
    char firmwareVersion[SENSOR_INFO_VERSION_MAX_LEN]; /**< Sensor firmware version */
    char hardwareVersion[SENSOR_INFO_VERSION_MAX_LEN]; /**< Sensor hardware version */
    int32_t sensorTypeId; /**< Sensor type ID (described in {@link SensorTypeTag}) */
    int32_t sensorId;     /**< Sensor ID, defined by the sensor driver developer */
    int32_t maxRange;     /**< Maximum measurement range of the sensor */
    int32_t accuracy;     /**< Sensor accuracy */
    int32_t power;        /**< Sensor power */
};

struct SensorReportEvent {
    int32_t sensorId;  /**< Sensor ID */
    int32_t version;   /**< Sensor algorithm version */
    uint64_t timestamp; /**< Time when sensor data was generated */
    uint32_t option;   /**< Sensor data options, including the measurement range and accuracy */
    int32_t mode;      /**< Sensor data reporting mode */
    uint8_t *data;     /**< Sensor data address */
    uint32_t dataLen;  /**< Sensor data length */
};

#endif /* SENSOR_DEVICE_TYPE_H */
