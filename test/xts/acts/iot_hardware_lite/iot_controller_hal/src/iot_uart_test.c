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
#include "iot_uart.h"

#define TEST_UART_SIZE 8
const unsigned int INIT_UART_BAUD_RATE = 115200;
const unsigned int COMPILABILITY_TEST_UART_PORT = 0xFFFFFFFF;

/**
 * @tc.desc      : this suite is used to test basic flow and interface dependency(Compile overlay)
 * @param        : subsystem name is wifiiot
 * @param        : module name is wifiiotlite
 * @param        : test suit name is UtilsFileFuncTestSuite
 */
LITE_TEST_SUIT(wifiiot, wifiiotlite, IotUartTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL IotUartTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL IotUartTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_5000
 * @tc.name      : UART operation with IoTUartInit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotUartTestSuite, testIotUart001, Function | MediumTest | Level1)
{
    IotUartAttribute uartAttr;
    uartAttr.baudRate = INIT_UART_BAUD_RATE,
    uartAttr.dataBits = IOT_UART_DATA_BIT_8;
    uartAttr.stopBits = IOT_UART_STOP_BIT_1;
    uartAttr.parity = IOT_UART_PARITY_NONE;
    uartAttr.rxBlock = IOT_UART_BLOCK_STATE_NONE_BLOCK;
    uartAttr.txBlock = IOT_UART_BLOCK_STATE_NONE_BLOCK;
    uartAttr.pad ='M'; // Reserved field

    IoTUartInit(COMPILABILITY_TEST_UART_PORT, &uartAttr);
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_5100
 * @tc.name      : UART operation with IoTUartWrite
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotUartTestSuite, testIotUart002, Function | MediumTest | Level1)
{
    unsigned char uartWriteBuff[] = "hello uart!";
    IoTUartWrite(COMPILABILITY_TEST_UART_PORT, uartWriteBuff, sizeof(uartWriteBuff));
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_5200
 * @tc.name      : UART operation with IoTUartRead
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotUartTestSuite, testIotUart003, Function | MediumTest | Level1)
{
    unsigned char uartReadBuff[TEST_UART_SIZE] = {0};
    IoTUartRead(COMPILABILITY_TEST_UART_PORT, uartReadBuff, sizeof(uartReadBuff));
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_5300
 * @tc.name      : UART operation with IoTUartSetFlowCtrl
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotUartTestSuite, testIotUart004, Function | MediumTest | Level1)
{
    IoTUartSetFlowCtrl(COMPILABILITY_TEST_UART_PORT, IOT_FLOW_CTRL_NONE);
};

/**
 * @tc.number    : SUB_UTILS_WIFIIOT_API_5400
 * @tc.name      : UART operation with IoTUartDeinit
 * @tc.desc      : [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(IotUartTestSuite, testIotUart005, Function | MediumTest | Level1)
{
    IoTUartDeinit(COMPILABILITY_TEST_UART_PORT);
};

RUN_TEST_SUITE(IotUartTestSuite);
