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
import { AsyncCallback, Callback } from "./basic";

/**
 * This module provides the capability to subscribe to sensor data.
 *
 * @since 8
 * @syscap SystemCapability.Sensors.Sensor
 * @import import sensor from '@ohos.sensor'
 * @permission ohos.permission.ACCELEROMETER, ohos.permission.GYROSCOPE,
 *             ohos.permission.ACTIVITY_MOTION, ohos.permission.HEALTH_DATA
 */
declare namespace sensor {
    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_ACCELEROMETER}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @permission ohos.permission.ACCELEROMETER
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_ACCELEROMETER, callback: Callback<AccelerometerResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_ACCELEROMETER_UNCALIBRATED}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @permission ohos.permission.ACCELEROMETER
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_ACCELEROMETER_UNCALIBRATED, callback: Callback<AccelerometerUncalibratedResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_AMBIENT_LIGHT}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_AMBIENT_LIGHT, callback: Callback<LightResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_AMBIENT_TEMPERATURE}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_AMBIENT_TEMPERATURE, callback: Callback<AmbientTemperatureResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_BAROMETER}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_BAROMETER, callback: Callback<BarometerResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_GRAVITY}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_GRAVITY, callback: Callback<GravityResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_GYROSCOPE}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.GYROSCOPE
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_GYROSCOPE, callback: Callback<GyroscopeResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.GYROSCOPE
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED, callback: Callback<GyroscopeUncalibratedResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_HALL}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_HALL, callback: Callback<HallResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_HEART_RATE}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.HEALTH_DATA
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_HEART_RATE, callback: Callback<HeartRateResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_HUMIDITY}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_HUMIDITY, callback: Callback<HumidityResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_LINEAR_ACCELERATION}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACCELEROMETER
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_LINEAR_ACCELERATION, callback: Callback<LinearAccelerometerResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD, callback: Callback<MagneticFieldResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD_UNCALIBRATED}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD_UNCALIBRATED, callback: Callback<MagneticFieldUncalibratedResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_ORIENTATION}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_ORIENTATION, callback: Callback<OrientationResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_PEDOMETER}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACTIVITY_MOTION
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_PEDOMETER, callback: Callback<PedometerResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_PEDOMETER_DETECTION}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACTIVITY_MOTION
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_PEDOMETER_DETECTION, callback: Callback<PedometerDetectionResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_PROXIMITY}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_PROXIMITY, callback: Callback<ProximityResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_ROTATION_VECTOR}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_ROTATION_VECTOR, callback: Callback<RotationVectorResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_SIGNIFICANT_MOTION}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_SIGNIFICANT_MOTION, callback: Callback<SignificantMotionResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data, If the API is called multiple times, the last call takes effect.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_WEAR_DETECTION}.
     * @param options Optional parameters specifying the interval at which sensor data is reported, {@code Options}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function on(type: SensorType.SENSOR_TYPE_ID_WEAR_DETECTION, callback: Callback<WearDetectionResponse>,
        options?: Options): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_ACCELEROMETER}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACCELEROMETER
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_ACCELEROMETER, callback: Callback<AccelerometerResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_ACCELEROMETER_UNCALIBRATED}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACCELEROMETER
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_ACCELEROMETER_UNCALIBRATED, callback: Callback<AccelerometerUncalibratedResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_AMBIENT_LIGHT}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_AMBIENT_LIGHT, callback: Callback<LightResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_AMBIENT_TEMPERATURE}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_AMBIENT_TEMPERATURE, callback: Callback<AmbientTemperatureResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_BAROMETER}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_BAROMETER, callback: Callback<BarometerResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_GRAVITY}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_GRAVITY, callback: Callback<GravityResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_GYROSCOPE}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.GYROSCOPE
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_GYROSCOPE, callback: Callback<GyroscopeResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.GYROSCOPE
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED, callback: Callback<GyroscopeUncalibratedResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_HALL}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_HALL, callback: Callback<HallResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_HEART_RATE}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.HEART_RATE
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_HEART_RATE, callback: Callback<HeartRateResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_HUMIDITY}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_HUMIDITY, callback: Callback<HumidityResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_LINEAR_ACCELERATION}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACCELERATION
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_LINEAR_ACCELERATION, callback: Callback<LinearAccelerometerResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD, callback: Callback<MagneticFieldResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD_UNCALIBRATED}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD_UNCALIBRATED, callback: Callback<MagneticFieldUncalibratedResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_ORIENTATION}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_ORIENTATION, callback: Callback<OrientationResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_PEDOMETER}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACTIVITY_MOTION
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_PEDOMETER, callback: Callback<PedometerResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_PEDOMETER_DETECTION}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACTIVITY_MOTION
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_PEDOMETER_DETECTION, callback: Callback<PedometerDetectionResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_PROXIMITY}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_PROXIMITY, callback: Callback<ProximityResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_ROTATION_VECTOR}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_ROTATION_VECTOR, callback: Callback<RotationVectorResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_SIGNIFICANT_MOTION}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_SIGNIFICANT_MOTION, callback: Callback<SignificantMotionResponse>): void;

    /**
     * Subscribe to sensor data once.
     * @param type Indicate the sensor type to listen for, {@code SensorType.SENSOR_TYPE_ID_WEAR_DETECTION}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function once(type: SensorType.SENSOR_TYPE_ID_WEAR_DETECTION, callback: Callback<WearDetectionResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_ACCELEROMETER}.
     * @permission ohos.permission.ACCELEROMETER
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
     function off(type: SensorType.SENSOR_TYPE_ID_ACCELEROMETER, callback?: Callback<AccelerometerResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_ACCELEROMETER_UNCALIBRATED}.
     * @permission ohos.permission.ACCELEROMETER
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_ACCELEROMETER_UNCALIBRATED,
        callback?: Callback<AccelerometerUncalibratedResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_AMBIENT_LIGHT}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_AMBIENT_LIGHT, callback?: Callback<LightResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_AMBIENT_TEMPERATURE}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_AMBIENT_TEMPERATURE, callback?: Callback<AmbientTemperatureResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_BAROMETER}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_BAROMETER, callback?: Callback<BarometerResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_GRAVITY}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_GRAVITY, callback?: Callback<GravityResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_GYROSCOPE}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.GYROSCOPE
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_GYROSCOPE, callback?: Callback<GyroscopeResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.GYROSCOPE
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED, callback?: Callback<GyroscopeUncalibratedResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_HALL}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_HALL, callback?: Callback<HallResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_HEART_RATE}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.HEALTH_DATA
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_HEART_RATE, callback?: Callback<HeartRateResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_HUMIDITY}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_HUMIDITY, callback?: Callback<HumidityResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_LINEAR_ACCELERATION}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACCELEROMETER
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_LINEAR_ACCELERATION, callback?: Callback<LinearAccelerometerResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD, callback?: Callback<MagneticFieldResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD_UNCALIBRATED}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_MAGNETIC_FIELD_UNCALIBRATED, callback?: Callback<MagneticFieldUncalibratedResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_ORIENTATION}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_ORIENTATION, callback?: Callback<OrientationResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_PEDOMETER}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACTIVITY_MOTION
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_PEDOMETER, callback?: Callback<PedometerResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_PEDOMETER_DETECTION}.
     * @syscap SystemCapability.Sensors.Sensor
     * @permission ohos.permission.ACTIVITY_MOTION
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_PEDOMETER_DETECTION, callback?: Callback<PedometerDetectionResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_PROXIMITY}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_PROXIMITY, callback?: Callback<ProximityResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_ROTATION_VECTOR}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_ROTATION_VECTOR, callback?: Callback<RotationVectorResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_SIGNIFICANT_MOTION}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_SIGNIFICANT_MOTION, callback?: Callback<SignificantMotionResponse>): void;

    /**
     * Unsubscribe to sensor data.
     * @param type Indicate the sensor type to unsubscribe, {@code SensorType.SENSOR_TYPE_ID_WEAR_DETECTION}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function off(type: SensorType.SENSOR_TYPE_ID_WEAR_DETECTION, callback?: Callback<WearDetectionResponse>): void;

    /**
     * Indicates geographic location.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface LocationOptions {
        latitude: number;
        longitude: number;
        altitude: number;
    }

    /**
     * Indicates geomagnetic field data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface GeomagneticResponse {
        x: number;
        y: number;
        z: number;
        geomagneticDip: number;
        deflectionAngle: number;
        levelIntensity: number;
        totalIntensity: number;
    }

   /**
     * Implements the calculation of the geomagnetic field at a specific location on Earth.
     *
     * @param LocationOptions Indicates geographic location, {@code LocationOptions}.
     * @param timeMillis Indicates the time at which the magnetic declination is to be obtained, in milliseconds
     * since the Unix epoch.
     * @return Returns the geomagnetic field data, {@code GeomagneticResponse}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function getGeomagneticField(locationOptions: LocationOptions, timeMillis: number, callback: AsyncCallback<GeomagneticResponse>): void;
    function getGeomagneticField(locationOptions: LocationOptions, timeMillis: number): Promise<GeomagneticResponse>;

   /**
     * Obtains the altitude at which the device is located based on the current atmospheric pressure.
     *
     * @param seaPressure Indicates the sea level pressure, in hPa.
     * @param currentPressure Indicates the atmospheric pressure measured by the barometer, in hPa.
     * @return Returns the altitude in meters at which the device is located.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function getAltitude(seaPressure: number, currentPressure: number, callback: AsyncCallback<number>): void;
    function getAltitude(seaPressure: number, currentPressure: number): Promise<number>;

    /**
     * Computes the geomagnetic inclination angle in radians from the inclination matrix.
     *
     * @param inclinationMatrix Indicates the inclination matrix.
     * @return Returns the geomagnetic inclination angle in radians.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function getGeomagneticDip(inclinationMatrix: Array<number>, callback: AsyncCallback<number>): void;
    function getGeomagneticDip(inclinationMatrix: Array<number>): Promise<number>;

    /**
     * Get the angle change between two rotation matrices
     *
     * @param currentRotationMatrix Indicates the current rotation matrix.
     * @param preRotationMatrix Indicates the current rotation matrix.
     * @return Returns the array of number(z, x and y) in which the angle variety.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function getAngleModify(currentRotationMatrix: Array<number>, preRotationMatrix: Array<number>,
        callback: AsyncCallback<Array<number>>): void;
    function getAngleModify(currentRotationMatrix: Array<number>, preRotationMatrix: Array<number>): Promise<Array<number>>;

    /**
     * Convert rotation vector to rotation matrix.
     *
     * @param rotationVector Indicates the rotation vector.
     * @return Returns the rotation matrix.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function createRotationMatrix(rotationVector: Array<number>, callback: AsyncCallback<Array<number>>): void;
    function createRotationMatrix(rotationVector: Array<number>): Promise<Array<number>>;

    /**
     * Indicates the axis of the new coordinate system that coincides with the XY axis of the
     * original coordinate system.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface CoordinatesOptions {
        /** Indicates the axis of the new coordinate system that coincides with the X axis of
         * the original coordinate system. 
         */
        x: number;
        /** Indicates the axis of the new coordinate system that coincides with the Z axis of
         * the original coordinate system. 
         */
        y: number;
    }

    /**
     * Rotate the provided rotation matrix so that it can be represented in a different way
     * Coordinate System
     * @param inRotationVector Indicates the rotation matrix to be transformed.
     * @param coordinates Indicates coordinate system guidance, {@code CoordinatesOptions}.
     * @return Returns the transformed rotation matrix.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function transformCoordinateSystem(inRotationVector: Array<number>, coordinates: CoordinatesOptions,
        callback: AsyncCallback<Array<number>>): void;
    function transformCoordinateSystem(inRotationVector: Array<number>, coordinates: CoordinatesOptions): Promise<Array<number>>;

    /**
     * convert a rotation vector to a normalized quaternion.
     *
     * @param rotationVector Indicates the rotation vector.
     * @return Returns the normalized quaternion.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function createQuaternion(rotationVector: Array<number>, callback: AsyncCallback<Array<number>>): void;
    function createQuaternion(rotationVector: Array<number>): Promise<Array<number>>;

    /**
     * Computes the device's orientation based on the rotation matrix.
     *
     * @param rotationMatrix Indicates the rotation matrix.
     * @return Returns the array is the angle of rotation around the z, x, y axis.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function getDirection(rotationMatrix: Array<number>, callback: AsyncCallback<Array<number>>): void;
    function getDirection(rotationMatrix: Array<number>): Promise<Array<number>>;

    /**
     * Indicates the response of rotation matrix.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface RotationMatrixResponse {
        rotation: Array<number>;
        inclination: Array<number>
    }

    /**
     * Calculate rotation matrix based on gravity vector and geomagnetic vector.
     *
     * @param gravity Indicates the gravity vector.
     * @param geomagnetic Indicates the geomagnetic vector.
     * @return Returns the rotation matrix, {@code RotationMatrixResponse}.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    function createRotationMatrix(gravity: Array<number>, geomagnetic: Array<number>, callback: AsyncCallback<RotationMatrixResponse>): void;
    function createRotationMatrix(gravity: Array<number>, geomagnetic: Array<number>,): Promise<RotationMatrixResponse>;

    /**
     * Subscribe to the sensor's optional parameters.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface Options {
        interval?: number; /**< Sensor event reporting event interval */
    }

    /**
     * The type of sensor.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    enum SensorType {
        SENSOR_TYPE_ID_ACCELEROMETER = 1,          /**< Acceleration sensor */
        SENSOR_TYPE_ID_GYROSCOPE = 2,              /**< Gyroscope sensor */
        SENSOR_TYPE_ID_AMBIENT_LIGHT = 5,          /**< Ambient light sensor */
        SENSOR_TYPE_ID_MAGNETIC_FIELD = 6,         /**< Magnetic field sensor */
        SENSOR_TYPE_ID_BAROMETER = 8,              /**< Barometric pressure sensor */
        SENSOR_TYPE_ID_HALL = 10,                  /**< Hall effect sensor */
        SENSOR_TYPE_ID_PROXIMITY = 12,             /**< Proximity sensor */
        SENSOR_TYPE_ID_HUMIDITY = 13,              /**< Humidity sensor */
        SENSOR_TYPE_ID_ORIENTATION = 256,          /**< Orientation sensor */
        SENSOR_TYPE_ID_GRAVITY = 257,              /**< Gravity sensor */
        SENSOR_TYPE_ID_LINEAR_ACCELERATION = 258,  /**< Linear acceleration sensor */
        SENSOR_TYPE_ID_ROTATION_VECTOR = 259,      /**< Rotation vector sensor */
        SENSOR_TYPE_ID_AMBIENT_TEMPERATURE = 260,  /**< Ambient temperature sensor */
        SENSOR_TYPE_ID_MAGNETIC_FIELD_UNCALIBRATED = 261,  /**< Uncalibrated magnetic field sensor */
        SENSOR_TYPE_ID_GYROSCOPE_UNCALIBRATED = 263,  /**< Uncalibrated gyroscope sensor */
        SENSOR_TYPE_ID_SIGNIFICANT_MOTION = 264,    /**< Significant motion sensor */
        SENSOR_TYPE_ID_PEDOMETER_DETECTION = 265,   /**< Pedometer detection sensor */
        SENSOR_TYPE_ID_PEDOMETER = 266,             /**< Pedometer sensor */
        SENSOR_TYPE_ID_HEART_RATE = 278,            /**< Heart rate sensor */
        SENSOR_TYPE_ID_WEAR_DETECTION = 280,        /**< Wear detection sensor */
        SENSOR_TYPE_ID_ACCELEROMETER_UNCALIBRATED = 281,   /**< Uncalibrated acceleration sensor */
    }

    /**
     * The basic data structure of the sensor event.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface Response {
        timestamp: number; /**< The timestamp of the reported sensor data */
    }

    /**
     * Acceleration sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface AccelerometerResponse extends Response {
        x: number; /**< Acceleration x-axis component */
        y: number; /**< Acceleration y-axis component */
        z: number; /**< Acceleration z-axis component */
    }

    /**
     * Linear acceleration sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface LinearAccelerometerResponse extends Response{
        x: number; /**< Linear acceleration x-axis component */
        y: number; /**< Linear acceleration y-axis component */
        z: number; /**< Linear acceleration z-axis component */
    }

    /**
     * Acceleration uncalibrated sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface AccelerometerUncalibratedResponse extends Response {
        x: number; /**< Acceleration uncalibrated x-axis component */
        y: number; /**< Acceleration uncalibrated y-axis component */
        z: number; /**< Acceleration uncalibrated z-axis component */
        biasX: number; /**< Acceleration uncalibrated x-axis offset */
        biasY: number; /**< Acceleration uncalibrated y-axis offset */
        biasZ: number; /**< Acceleration uncalibrated z-axis offset */
    }

    /**
     * Gravity sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface GravityResponse extends Response {
        x: number; /**< Gravity x-axis component */
        y: number; /**< Gravity y-axis component */
        z: number; /**< Gravity z-axis component */
    }

    /**
     * Orientation sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface OrientationResponse extends Response {
        alpha: number; /**< The device rotates at an angle around the Z axis */
        beta: number; /**< The device rotates at an angle around the X axis */
        gamma: number; /**< The device rotates at an angle around the Y axis */
    }

    /**
     * Rotation vector sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface RotationVectorResponse extends Response {
        x: number; /**< Rotation vector x-axis component */
        y: number; /**< Rotation vector y-axis component */
        z: number; /**< Rotation vector z-axis component */
        w: number; /**< Scalar quantity */
    }

    /**
     * Gyroscope sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface GyroscopeResponse extends Response {
        x: number; /**< Gyroscope x-axis component */
        y: number; /**< Gyroscope y-axis component */
        z: number; /**< Gyroscope z-axis component */
    }

    /**
     * Gyroscope uncalibrated sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface GyroscopeUncalibratedResponse extends Response {
        x: number; /**< Gyroscope uncalibrated x-axis component */
        y: number; /**< Gyroscope uncalibrated y-axis component */
        z: number; /**< Gyroscope uncalibrated z-axis component */
        biasX: number; /**< Gyroscope uncalibrated x-axis offset */
        biasY: number; /**< Gyroscope uncalibrated y-axis offset */
        biasZ: number; /**< Gyroscope uncalibrated z-axis offset */
    }

    /**
     * Significant motion sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface SignificantMotionResponse extends Response {
        scalar: number; /**< The degree of significant motion */
    }

    /**
     * Proximity sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface ProximityResponse extends Response {
        distance: number; /**< Indicates the degree of proximity, event 0 indicates proximity, while 1 indicates distance */
    }

    /**
     * Light sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface LightResponse extends Response {
        intensity: number; /**< Indicates light intensity, in lux */
    }

    /**
     * Hall sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface HallResponse extends Response {
        status: number; /**< Indicates hall status, event 0 indicates open, while 1 indicates close*/
    }

    /**
     * Magnetic field sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface MagneticFieldResponse extends Response {
        x: number; /**< Magnetic field x-axis component */
        y: number; /**< Magnetic field y-axis component */
        z: number; /**< Magnetic field z-axis component */
    }

    /**
     * Magnetic field uncalibrated sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface MagneticFieldUncalibratedResponse extends Response {
        x: number; /**< Magnetic field uncalibrated x-axis component */
        y: number; /**< Magnetic field uncalibrated y-axis component */
        z: number; /**< Magnetic field uncalibrated z-axis component */
        biasX: number; /**< Magnetic field uncalibrated x-axis offset */
        biasY: number; /**< Magnetic field uncalibrated y-axis offset */
        biasZ: number; /**< Magnetic field uncalibrated z-axis offset */
    }

    /**
     * Pedometer sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface PedometerResponse extends Response {
        steps: number; /**< Indicates the number of steps */
    }

    /**
     * Humidity sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface HumidityResponse extends Response {
        humidity: number; /**< Indicates the number of humidity */
    }

    /**
     * Pedometer detection sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface PedometerDetectionResponse extends Response {
        scalar: number; /**< Indicates the pedometer detection status, 1 indicates that a walking action has occurred, and 0 indicates that no movement has occurred */
    }

    /**
     * Ambient temperature sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface AmbientTemperatureResponse extends Response {
        temperature: number; /**< Indicates ambient temperature, in celsius */
    }

    /**
     * Barometer sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface BarometerResponse extends Response {
        pressure: number; /**< Indicates the number of barometer, in pascal */
    }

    /**
     * Heart rate sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface HeartRateResponse extends Response {
        heartRate: number; /**< Indicates the number of heart rate */
    }

    /**
     * Wear detection sensor event data.
     * @syscap SystemCapability.Sensors.Sensor
     * @since 8
     */
    interface WearDetectionResponse extends Response {
        value: number; /**< Indicates the status of wear detection, 1 for wearing, 0 for wearing not */
    }
 }

 export default sensor;