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
#include "hctest.h"
#include "iot_errno.h"
#ifdef CONFIG_I2C_SUPPORT
#include "iot_i2c.h"
#endif

const unsigned int INIT_BAUD_RATE = 115200;
const unsigned int COMPILABILITY_TEST_I2C_DEVICE = 0xFFFFFFFF;
const unsigned short COMPILABILITY_TEST_I2C_DEVICE_ADDR = 0xFFFF;
#define DATA_LEN 3


/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is wifiiot
 * @param        : module name is wifiiotlite
 * @param        : test suit name is UtilsFileFuncTestSuite
 */
LITE_TEST_SUIT(wifiiot, wifiiotlite, IotI2cTestSuite);

#ifdef CONFIG_I2C_SUPPORT

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL IotI2cTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL IotI2cTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

void IOT_ResetFunc(void)
{
    printf("Abnormal callback function\n");
}

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_3100
 * @tc.name      : I2C operation with IoTI2cInit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotI2cTestSuite, testIotI2C001, Function | MediumTest | Level1)
{
    unsigned int ret;
    ret = IoTI2cInit(COMPILABILITY_TEST_I2C_DEVICE, INIT_BAUD_RATE);
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_3200
 * @tc.name      : I2C operation with IoTI2cWrite
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotI2cTestSuite, testIotI2C002, Function | MediumTest | Level1)
{
    unsigned int ret;
    unsigned char senddata[DATA_LEN] = {0};

    senddata[0] = 0x0;
    senddata[1] = 0x3;
    ret = IoTI2cWrite(COMPILABILITY_TEST_I2C_DEVICE,
                      COMPILABILITY_TEST_I2C_DEVICE_ADDR,
                      (unsigned char*)&senddata,
                      DATA_LEN);
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_3300
 * @tc.name      : I2C operation with IoTI2cRead
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotI2cTestSuite, testIotI2C003, Function | MediumTest | Level1)
{
    unsigned int ret;
    unsigned char recvdata[DATA_LEN] = {0};

    ret = IoTI2cRead(COMPILABILITY_TEST_I2C_DEVICE,
                     COMPILABILITY_TEST_I2C_DEVICE_ADDR,
                     (unsigned char*)&recvdata,
                     DATA_LEN);
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_3400
 * @tc.name      : I2C operation with IoTI2cSetBaudrate
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotI2cTestSuite, testIotI2C004, Function | MediumTest | Level1)
{
    unsigned int ret;
    ret = IoTI2cSetBaudrate(COMPILABILITY_TEST_I2C_DEVICE, INIT_BAUD_RATE);
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_3500
 * @tc.name      : I2C operation with IoTI2cDeinit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotI2cTestSuite, testIotI2C005, Function | MediumTest | Level1)
{
    unsigned int ret;
    ret = IoTI2cDeinit(COMPILABILITY_TEST_I2C_DEVICE);
};
#endif

RUN_TEST_SUITE(IotI2cTestSuite);
