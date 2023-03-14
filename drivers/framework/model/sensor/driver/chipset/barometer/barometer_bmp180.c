/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "barometer_bmp180.h"
#include <securec.h>
#include "osal_mem.h"
#include "osal_time.h"
#include "sensor_barometer_driver.h"
#include "sensor_config_controller.h"
#include "sensor_device_manager.h"
#include "sensor_platform_if.h"

#define HDF_LOG_TAG    hdf_sensor_barometer

static struct Bmp180DrvData *g_bmp180DrvData = NULL;

struct Bmp180DrvData *Bmp180GetDrvData(void)
{
    return g_bmp180DrvData;
}

/* IO config for int-pin and I2C-pin */
#define SENSOR_I2C6_DATA_REG_ADDR 0x114f004c
#define SENSOR_I2C6_CLK_REG_ADDR  0x114f0048
#define SENSOR_I2C_REG_CFG        0x403

static struct BarometerEepromData g_calibraData = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static int32_t ReadEepromRawData(struct SensorCfgData *data, uint8_t rfg[BAROMETER_EEPROM_SUM])
{
    int32_t ret;

    ret = ReadSensor(&data->busCfg, BMP180_AC1_MSB_ADDR, &rfg[BAROMETER_AC1_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_AC1_LSB_ADDR, &rfg[BAROMETER_AC1_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_AC2_MSB_ADDR, &rfg[BAROMETER_AC2_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_AC2_LSB_ADDR, &rfg[BAROMETER_AC2_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_AC3_MSB_ADDR, &rfg[BAROMETER_AC3_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_AC3_LSB_ADDR, &rfg[BAROMETER_AC3_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_AC4_MSB_ADDR, &rfg[BAROMETER_AC4_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_AC4_LSB_ADDR, &rfg[BAROMETER_AC4_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_AC5_MSB_ADDR, &rfg[BAROMETER_AC5_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_AC5_LSB_ADDR, &rfg[BAROMETER_AC5_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_AC6_MSB_ADDR, &rfg[BAROMETER_AC6_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");
    ret = ReadSensor(&data->busCfg, BMP180_AC6_LSB_ADDR, &rfg[BAROMETER_AC6_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_B1_MSB_ADDR, &rfg[BAROMETER_B1_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_B1_LSB_ADDR, &rfg[BAROMETER_B1_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_B2_MSB_ADDR, &rfg[BAROMETER_B2_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_B2_LSB_ADDR, &rfg[BAROMETER_B2_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_MB_MSB_ADDR, &rfg[BAROMETER_MB_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_MB_LSB_ADDR, &rfg[BAROMETER_MB_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_MC_MSB_ADDR, &rfg[BAROMETER_MC_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_MC_LSB_ADDR, &rfg[BAROMETER_MC_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_MD_MSB_ADDR, &rfg[BAROMETER_MD_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_MD_LSB_ADDR, &rfg[BAROMETER_MD_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    return ret;
}

static int32_t ReadEepromData(struct SensorCfgData *data, struct BarometerEepromData *g_calibraData)
{
    int32_t ret;
    uint8_t reg[BAROMETER_EEPROM_SUM];

    (void)memset_s(reg, sizeof(reg), 0, sizeof(reg));

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    ret = ReadEepromRawData(data, reg);
    if (ret != HDF_SUCCESS) {
    return HDF_FAILURE;
    }

    g_calibraData->ac1 = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_AC1_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_AC1_LSB]);
    g_calibraData->ac2 = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_AC2_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_AC2_LSB]);
    g_calibraData->ac3 = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_AC3_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_AC3_LSB]);
    g_calibraData->ac4 = (uint16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_AC4_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_AC4_LSB]);
    g_calibraData->ac5 = (uint16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_AC5_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_AC5_LSB]);
    g_calibraData->ac6 = (uint16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_AC6_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_AC6_LSB]);
    g_calibraData->b1 = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_B1_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_B1_LSB]);
    g_calibraData->b2 = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_B2_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_B2_LSB]);
    g_calibraData->mb = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_MB_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_MB_LSB]);
    g_calibraData->mc = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_MC_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_MC_LSB]);
    g_calibraData->md = (int16_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_MD_MSB], SENSOR_DATA_WIDTH_8_BIT) |
    reg[BAROMETER_MD_LSB]);

    return ret;
}

static int32_t ReadTempData(struct SensorCfgData *data,  struct BarometerRawData *Temp)
{
    int32_t ret;
    uint8_t status = 0;
    uint8_t reg[BAROMETER_TEM_SUM];
    uint8_t value[SENSOR_VALUE_BUTT];
    value[SENSOR_ADDR_INDEX] = BMP180_CONTROL_REG_ADDR;
    value[SENSOR_VALUE_INDEX] = BMP180_COVERT_TEMP;

    (void)memset_s(reg, sizeof(reg), 0, sizeof(reg));

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    ret = ReadSensor(&data->busCfg, BMP180_COVERT_PRES_3, &status, sizeof(uint8_t));
    if ((status & BMP180_STATUS_ADDR) == BMP180_STATUS_JUDGE) {
        WriteSensor(&data->busCfg, value, sizeof(value));
        OsalMDelay(DELAY_0);
        ret = ReadSensor(&data->busCfg, BMP180_OUT_MSB_ADDR, &reg[BAROMETER_TEM_MSB], sizeof(uint8_t));
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

        ret = ReadSensor(&data->busCfg, BMP180_OUT_LSB_ADDR, &reg[BAROMETER_TEM_LSB], sizeof(uint8_t));
        CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

        Temp->unpensateTemp = (int32_t)(SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_TEM_MSB], SENSOR_DATA_WIDTH_8_BIT) |
            reg[BAROMETER_TEM_LSB]);
    }
    return ret;
}

static int32_t ReadBarometerData(struct SensorCfgData *data, struct BarometerRawData *Barom)
{
    int32_t ret;
    uint8_t status = 0;
    uint8_t reg[BAROMETER_BAR_SUM];
    uint8_t value[SENSOR_VALUE_BUTT];
    value[SENSOR_ADDR_INDEX] = BMP180_CONTROL_REG_ADDR;
    value[SENSOR_VALUE_INDEX] = BMP180_COVERT_PRES_1;

    (void)memset_s(reg, sizeof(reg), 0, sizeof(reg));

    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);

    ret = ReadSensor(&data->busCfg, BMP180_COVERT_PRES_3, &status, sizeof(uint8_t));
    if ((status & BMP180_STATUS_ADDR) == BMP180_STATUS_JUDGE) {
    WriteSensor(&data->busCfg, value, sizeof(value));
    OsalMDelay(DELAY_1);
    ret = ReadSensor(&data->busCfg, BMP180_OUT_MSB_ADDR, &reg[BAROMETER_BAR_MSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_OUT_LSB_ADDR, &reg[BAROMETER_BAR_LSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    ret = ReadSensor(&data->busCfg, BMP180_OUT_XLSB_ADDR, &reg[BAROMETER_BAR_XLSB], sizeof(uint8_t));
    CHECK_PARSER_RESULT_RETURN_VALUE(ret, "read data");

    Barom->unpensatePre = (int32_t)(SENSOR_DATA_SHIFT_RIGHT(
        (SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_BAR_MSB], SENSOR_DATA_WIDTH_16_BIT) |
            SENSOR_DATA_SHIFT_LEFT(reg[BAROMETER_BAR_LSB], SENSOR_DATA_WIDTH_8_BIT) | reg[BAROMETER_BAR_XLSB]),
        (BMP180_CONSTANT_4 - OSSETTING)));
    }
    return ret;
}

static int32_t CalcBarometerData(struct  BarometerRawData *barometerData, int32_t tnp[BAROMETER_SUM])
{
    struct Coefficient coefficientData = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    // Calculated temperature

    coefficientData.x1 = ((barometerData->unpensateTemp - g_calibraData.ac6) * (g_calibraData.ac5))
        >> BMP180_CONSTANT_8;
    coefficientData.x2 = (g_calibraData.mc << BMP180_CONSTANT_5) / (coefficientData.x1 + g_calibraData.md);
    coefficientData.b5 = coefficientData.x1 + coefficientData.x2;
    tnp[BAROMETER_TEMPERATURE] = (coefficientData.b5 + BMP180_CONSTANT_4) >> BMP180_CONSTANT_3;

    // Calculated pressure

    coefficientData.b6 = coefficientData.b5 - BMP180_CONSTANT_12;
    coefficientData.x1 = (g_calibraData.b2 * ((coefficientData.b6 * coefficientData.b6) >> BMP180_CONSTANT_6))
        >> BMP180_CONSTANT_5;
    coefficientData.x2 = (g_calibraData.ac2 * coefficientData.b6) >> BMP180_CONSTANT_5;
    coefficientData.x3 = coefficientData.x1 + coefficientData.x2;
    coefficientData.b3 = (((((int32_t)g_calibraData.ac1) * BMP180_CONSTANT_3 + coefficientData.x3) << OSSETTING)
        + BMP180_CONSTANT_2) >> BMP180_CONSTANT_2;
    coefficientData.x1 = (g_calibraData.ac3 * coefficientData.b6) >> BMP180_CONSTANT_7;
    coefficientData.x2 = (g_calibraData.b1 * ((coefficientData.b6 * coefficientData.b6) >> BMP180_CONSTANT_6))
        >> BMP180_CONSTANT_9;
    coefficientData.x3 = ((coefficientData.x1 + coefficientData.x2) + BMP180_CONSTANT_2) >> BMP180_CONSTANT_2;
    coefficientData.b4 = (g_calibraData.ac4 * (uint32_t)(coefficientData.x3 + BMP180_CONSTANT_13))
        >> BMP180_CONSTANT_8;
    coefficientData.b7 = ((uint32_t)(barometerData->unpensatePre) - (uint32_t)coefficientData.b3)
        * (BMP180_CONSTANT_14 >> OSSETTING);
    if (coefficientData.b7 < BMP180_CONSTANT_15) {
        coefficientData.p = (coefficientData.b7 << BMP180_CONSTANT_1) / coefficientData.b4;
    } else {
        coefficientData.p = (coefficientData.b7 / coefficientData.b4) << BMP180_CONSTANT_1;
    }
    coefficientData.x1 = (coefficientData.p >> BMP180_CONSTANT_4) * (coefficientData.p >> BMP180_CONSTANT_4);
    coefficientData.x1 = (coefficientData.x1 * BMP180_CONSTANT_10) >> BMP180_CONSTANT_9;
    coefficientData.x2 = (BMP180_CONSTANT_0 * coefficientData.p) >> BMP180_CONSTANT_9;
    tnp[BAROMETER_BAROMETER] = coefficientData.p + ((coefficientData.x1 + coefficientData.x2
        + BMP180_CONSTANT_11) >> BMP180_CONSTANT_3);

    return HDF_SUCCESS;
}

int32_t ReadBmp180Data(struct SensorCfgData *data)
{
    int32_t ret;
    int32_t tmp[BAROMETER_SUM];
    struct  BarometerRawData barometerData = {0, 0};
    OsalTimespec time;
    struct SensorReportEvent event;

    (void)memset_s(&time, sizeof(time), 0, sizeof(time));
    (void)memset_s(&event, sizeof(event), 0, sizeof(event));

    if (OsalGetTime(&time) != HDF_SUCCESS) {
        HDF_LOGE("%s: Get time failed", __func__);
        return HDF_FAILURE;
    }
    event.timestamp = time.sec * SENSOR_SECOND_CONVERT_NANOSECOND + time.usec * SENSOR_CONVERT_UNIT;

    ret = ReadTempData(data, &barometerData);
    if (ret != HDF_SUCCESS) {
    return HDF_FAILURE;
    }

    ret = ReadBarometerData(data, &barometerData);
    if (ret != HDF_SUCCESS) {
    return HDF_FAILURE;
    }

    ret = CalcBarometerData(&barometerData, tmp);
    if (ret != HDF_SUCCESS) {
    return HDF_FAILURE;
    }

    event.sensorId = SENSOR_TAG_BAROMETER;
    event.option = 0;
    event.mode = SENSOR_WORK_MODE_REALTIME;
    event.dataLen = sizeof(tmp);
    event.data = (uint8_t *)&tmp;
    ret = ReportSensorEvent(&event);
    return ret;
}

static int32_t InitBmp180(struct SensorCfgData *data)
{
    int32_t ret;
    CHECK_NULL_PTR_RETURN_VALUE(data, HDF_ERR_INVALID_PARAM);
    struct SensorReportEvent event;

    (void)memset_s(&event, sizeof(event), 0, sizeof(event));

    ret = ReadEepromData(data, &g_calibraData);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    ret = SetSensorRegCfgArray(&data->busCfg, data->regCfgGroup[SENSOR_INIT_GROUP]);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: BMP180 sensor init config failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int32_t InitBarometerPreConfig(void)
{
    if (SetSensorPinMux(SENSOR_I2C6_DATA_REG_ADDR, SENSOR_ADDR_WIDTH_4_BYTE, SENSOR_I2C_REG_CFG) != HDF_SUCCESS) {
        HDF_LOGE("%s: Data write mux pin failed", __func__);
        return HDF_FAILURE;
    }
    if (SetSensorPinMux(SENSOR_I2C6_CLK_REG_ADDR, SENSOR_ADDR_WIDTH_4_BYTE, SENSOR_I2C_REG_CFG) != HDF_SUCCESS) {
        HDF_LOGE("%s: Clk write mux pin failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static int32_t DispatchBMP180(struct HdfDeviceIoClient *client,
    int cmd, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmd;
    (void)data;
    (void)reply;

    return HDF_SUCCESS;
}

int32_t Bmp180BindDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);

    struct Bmp180DrvData *drvData = (struct Bmp180DrvData *)OsalMemCalloc(sizeof(*drvData));
    if (drvData == NULL) {
        HDF_LOGE("%s: Malloc Bmi160 drv data fail", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    drvData->ioService.Dispatch = DispatchBMP180;
    drvData->device = device;
    device->service = &drvData->ioService;
    g_bmp180DrvData = drvData;

    return HDF_SUCCESS;
}

int32_t Bmp180InitDriver(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct BarometerOpsCall ops;

    CHECK_NULL_PTR_RETURN_VALUE(device, HDF_ERR_INVALID_PARAM);
    struct Bmp180DrvData *drvData = (struct Bmp180DrvData *)device->service;
    CHECK_NULL_PTR_RETURN_VALUE(drvData, HDF_ERR_INVALID_PARAM);

    ret = InitBarometerPreConfig();
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init  BMp180 bus mux config", __func__);
        return HDF_FAILURE;
    }

    drvData->sensorCfg = BarometerCreateCfgData(device->property);
    if (drvData->sensorCfg == NULL || drvData->sensorCfg->root == NULL) {
        HDF_LOGD("%s: Creating barometercfg failed because detection failed", __func__);
        return HDF_ERR_NOT_SUPPORT;
    }

    ops.Init = NULL;
    ops.ReadData = ReadBmp180Data;
    ret = BarometerRegisterChipOps(&ops);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Register BMp180 barometer failed", __func__);
        return HDF_FAILURE;
    }

    ret = InitBmp180(drvData->sensorCfg);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: Init BMP180 barometer failed", __func__);
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

void Bmp180ReleaseDriver(struct HdfDeviceObject *device)
{
    CHECK_NULL_PTR_RETURN(device);

    struct Bmp180DrvData *drvData = (struct Bmp180DrvData *)device->service;
    CHECK_NULL_PTR_RETURN(drvData);

    if (drvData->sensorCfg != NULL) {
        BarometerReleaseCfgData(drvData->sensorCfg);
        drvData->sensorCfg = NULL;
    }
    OsalMemFree(drvData);
}

struct HdfDriverEntry g_barometerBmp180DevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_SENSOR_BAROMETER_BMP180",
    .Bind = Bmp180BindDriver,
    .Init = Bmp180InitDriver,
    .Release = Bmp180ReleaseDriver,
};

HDF_INIT(g_barometerBmp180DevEntry);