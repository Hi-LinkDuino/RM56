/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

import usb from '@ohos.usb';
import CheckEmptyUtils from './CheckEmptyUtils.js';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

/* usb core functions test */
describe('UsbFunctionsJsFunctionsTest', function () {

  beforeAll(function () {
    console.log('*************Usb Unit UsbFunctionsJsFunctionsTest Begin*************');
    var Version = usb.getVersion()
    console.info('begin test getversion :' + Version)
    // version > 17  host currentMode = 2 device currentMode = 1
    var usbPortList = usb.getPorts()
    if (usbPortList.length > 0) {
      if (usbPortList[0].status.currentMode == 2) {
        usb.setPortRoles(usbPortList[0].id, usb.SINK, usb.DEVICE).then(data => {
          console.info('usb case setPortRoles return: ' + data);
        }).catch(error => {
          console.info('usb case setPortRoles error : ' + error);
        });
        console.log('*************Usb Unit switch to device Begin*************');
      }
    }
  })
  beforeEach(function () {
    console.info('beforeEach: *************Usb Unit  Test Case*************');
  })
  afterEach(function () {
    console.info('afterEach: *************Usb Unit  Test Case*************');
  })
  afterAll(function () {
    console.log('*************Usb Unit UsbFunctionsJsFunctionsTest End*************');
  })

  /**
   * @tc.number    : SUB_USB_get_current_functions_test_01
   * @tc.name      : getCurrentFunctions
   * @tc.desc      : 获取当前设备模式 掩码与描述字符转换
   */
  it('SUB_USB_get_current_functions_test_01', 0, function () {
    console.info('usb get_current_functions_test_01 begin');
    var maskCode = usb.getCurrentFunctions();
    console.info('usb case getCurrentFunctions return: ' + maskCode);

    var strMaskCode = usb.usbFunctionsToString(maskCode)
    console.info('usb case usbFunctionsToString return str: ' + strMaskCode);
    var nMaskCode = usb.usbFunctionsFromString(strMaskCode)
    console.info('usb case strMaskCode usbFunctionsFromString return int: ' + nMaskCode);
    expect(nMaskCode).assertEqual(maskCode);
    console.info('usb get_current_functions_test_01 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_get_current_functions_test_02
   * @tc.name      : usbFunctionString
   * @tc.desc      : 反向测试 获取当前设备模式 掩码与描述字符转换
   */
  it('SUB_USB_get_current_functions_test_02', 0, function () {
    console.info('usb get_current_functions_test_02 begin');
    var maskCode = usb.getCurrentFunctions();
    console.info('usb case getCurrentFunctions return: ' + maskCode);

    var strMaskCode = usb.usbFunctionsToString(maskCode)
    console.info('usb case usbFunctionsToString return str: ' + strMaskCode);
    var nMaskCode = usb.usbFunctionsFromString(strMaskCode)
    console.info('usb case strMaskCode usbFunctionsFromString return int: ' + nMaskCode);

    var errmaskCode = 0
    var strMaskCode = usb.usbFunctionsToString(errmaskCode)

    console.info('usb case ' + errmaskCode + ' usbFunctionsToString return str: ' + strMaskCode);

    var errStrMaskCode = 'none'
    var nMaskCode = usb.usbFunctionsFromString(errStrMaskCode)
    expect(nMaskCode).assertEqual(usb.NONE);
    console.info('usb case errStrMaskCode ' + errStrMaskCode +
      ' usbFunctionsFromString return int: ' + nMaskCode);

    console.info('usb get_current_functions_test_02 :  PASS');
  })

  function callSetCurFunction(caseName, iValue) {
    CheckEmptyUtils.sleep(3000)
    console.info('usb case param case name:' + caseName);
    console.info('usb case param iValue:' + iValue);
    usb.setCurrentFunctions(iValue).then(data => {
      console.info('usb case SetCurFunction ret:' + data);
      expect(data).assertTrue();
      console.info('usb case ' + caseName + ': PASS');
    }).catch(error => {
      console.info('usb case ' + caseName + ' error : ' + error);
      expect(false).assertTrue();
    });
  }

  /**
   * @tc.number    : SUB_USB_set_current_functions_test_02
   * @tc.name      : functions_test
   * @tc.desc      : 在设备模式下设置当前的USB功能列表 Set 1 设置为ACM功能
   */
  it('SUB_USB_set_current_functions_test_02', 0, function () {
    CheckEmptyUtils.sleep(3000)
    console.info('usb set_current_functions_test_02 set ACM begin');
    var maskCode = usb.getCurrentFunctions();
    console.info('usb case getCurrentFunctions return: ' + maskCode);
    var funcString = usb.usbFunctionsToString(maskCode);
    console.info('usb case funcString:' + funcString);
    callSetCurFunction('set_current_functions_test_02 ACM 1', usb.ACM)
  })

  /**
   * @tc.number    : SUB_USB_set_current_functions_test_03
   * @tc.name      : functions_test
   * @tc.desc      : 在设备模式下设置当前的USB功能列表 Set 2 设置为ECM功能
   */
  it('SUB_USB_set_current_functions_test_03', 0, function () {
    CheckEmptyUtils.sleep(3000)
    console.info('usb set_current_functions_test_03 set ECM begin');
    var maskCode = usb.getCurrentFunctions();
    console.info('usb case getCurrentFunctions return: ' + maskCode);
    var funcString = usb.usbFunctionsToString(maskCode);
    console.info('usb case funcString:' + funcString);
    callSetCurFunction('set_current_functions_test_03 ECM 2', usb.ECM)
  })

  /**
   * @tc.number    : SUB_USB_set_current_functions_test_04
   * @tc.name      : functions_test
   * @tc.desc      : 在设备模式下设置当前的USB功能列表 Set 3 设置为ACM、ECM功能
   */
  it('SUB_USB_set_current_functions_test_04', 0, function () {
    CheckEmptyUtils.sleep(3000)
    console.info('usb set_current_functions_test_04 set ACM ECM begin');
    var maskCode = usb.getCurrentFunctions();
    console.info('usb case getCurrentFunctions return: ' + maskCode);
    var funcString = usb.usbFunctionsToString(maskCode);
    console.info('usb case funcString:' + funcString);
    callSetCurFunction('set_current_functions_test_04 ACM ECM 3', (usb.ACM | usb.ECM))
  })

  /**
   * @tc.number    : SUB_USB_set_current_functions_test_05
   * @tc.name      : functions_test
   * @tc.desc      : 在设备模式下设置当前的USB功能列表 Set 4 设置为HDC功能
   */
  it('SUB_USB_set_current_functions_test_05', 0, function () {
    CheckEmptyUtils.sleep(3000)
    console.info('usb set_current_functions_test_05 set HDC begin');
    var maskCode = usb.getCurrentFunctions();
    console.info('usb case getCurrentFunctions return: ' + maskCode);
    var funcString = usb.usbFunctionsToString(maskCode);
    console.info('usb case funcString:' + funcString);
    callSetCurFunction('set_current_functions_test_05 HDC 4', usb.HDC)
  })

  /**
   * @tc.number    : SUB_USB_set_current_functions_test_06
   * @tc.name      : functions_test
   * @tc.desc      : 在设备模式下设置当前的USB功能列表 Set 5 设置为ACM、HDC功能
   */
  it('SUB_USB_set_current_functions_test_06', 0, function () {
    CheckEmptyUtils.sleep(3000)
    console.info('usb set_current_functions_test_06 set ACM HDC begin');
    var maskCode = usb.getCurrentFunctions();
    console.info('usb case getCurrentFunctions return: ' + maskCode);
    var funcString = usb.usbFunctionsToString(maskCode);
    console.info('usb case funcString:' + funcString);
    callSetCurFunction('set_current_functions_test_06 ACM HDC 5', (usb.HDC | usb.ACM))
  })

  /**
   * @tc.number    : SUB_USB_set_current_functions_test_07
   * @tc.name      : functions_test
   * @tc.desc      : 在设备模式下设置当前的USB功能列表 Set 6 设置为ECM、HDC功能
   */
  it('SUB_USB_set_current_functions_test_07', 0, function () {
    CheckEmptyUtils.sleep(3000)
    console.info('usb set_current_functions_test_07 set ECM HDC begin');
    var maskCode = usb.getCurrentFunctions();
    console.info('usb case getCurrentFunctions return: ' + maskCode);
    var funcString = usb.usbFunctionsToString(maskCode);
    console.info('usb case funcString:' + funcString);
    callSetCurFunction('set_current_functions_test_07 ECM HDC 6', (usb.HDC | usb.ECM))
  })

})
