/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import usb from '@ohos.usb';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

/* usb core functions test */
describe('UsbFunctionsJsFunctionsTestEx', function () {

  beforeAll(function () {
    console.log('*************Usb Unit UsbFunctionsJsFunctionsTestEx Begin*************');
    var Version = usb.getVersion()
    console.info('begin test getversion :' + Version)
    //  17 version host currentMode = 2 device currentMode = 1
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
    console.info('beforeEach: *************Usb Unit Test Case*************');
  })

  afterEach(function () {
    console.info('afterEach: *************Usb Unit Test Case*************');
  })

  afterAll(function () {
    console.log('*************Usb Unit UsbFunctionsJsFunctionsTest End*************');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_to_string_test_01
   * @tc.name      : usbFunctionsToString
   * @tc.desc      : 正向测试 0 返回 'none'
   */
  it('SUB_USB_usb_function_to_string_test_01', 0, function () {
    console.info('usb usb_function_to_string_test_01 begin');
    var maskCode = usb.NONE
    console.info('usb case maskCode : ' + usb.NONE);
    var strMaskCode = usb.usbFunctionsToString(maskCode)
    expect(strMaskCode).assertEqual('none');
    console.info('usb case maskCode ' + maskCode + ' usbFunctionsToString return int: ' + strMaskCode);
    console.info('usb usb_function_to_string_test_01 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_to_string_test_02
   * @tc.name      : usbFunctionsToString
   * @tc.desc      : 正向测试 1 返回 'acm'
   */
  it('SUB_USB_usb_function_to_string_test_02', 0, function () {
    console.info('usb usb_function_to_string_test_02 begin');
    var maskCode = usb.ACM
    console.info('usb case maskCode : ' + usb.ACM);
    var strMaskCode = usb.usbFunctionsToString(maskCode)
    expect(strMaskCode).assertEqual('acm');
    console.info('usb case maskCode ' + maskCode + ' usbFunctionsToString return int: ' + strMaskCode);
    console.info('usb usb_function_to_string_test_02 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_to_string_test_03
   * @tc.name      : usbFunctionsToString
   * @tc.desc      : 正向测试 2 返回 'ecm'
   */
  it('SUB_USB_usb_function_to_string_test_03', 0, function () {
    console.info('usb usb_function_to_string_test_02 begin');
    var maskCode = usb.ECM
    console.info('usb case maskCode : ' + maskCode);
    var strMaskCode = usb.usbFunctionsToString(maskCode)
    expect(strMaskCode).assertEqual('ecm');
    console.info('usb case maskCode ' + maskCode + ' usbFunctionsToString return int: ' + strMaskCode);
    console.info('usb usb_function_to_string_test_02 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_to_string_test_04
   * @tc.name      : usbFunctionsToString
   * @tc.desc      : 正向测试 3 返回 'acm,ecm'
   */
  it('SUB_USB_usb_function_to_string_test_04', 0, function () {
    console.info('usb usb_function_to_string_test_02 begin');
    var maskCode = usb.ACM | usb.ECM
    console.info('usb case maskCode : ' + maskCode);
    var strMaskCode = usb.usbFunctionsToString(maskCode)
    expect(strMaskCode).assertEqual('acm,ecm');
    console.info('usb case maskCode ' + maskCode + ' usbFunctionsToString return int: ' + strMaskCode);
    console.info('usb usb_function_to_string_test_02 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_to_string_test_05
   * @tc.name      : usbFunctionsToString
   * @tc.desc      : 正向测试 4 返回 'hdc'
   */
  it('SUB_USB_usb_function_to_string_test_05', 0, function () {
    console.info('usb usb_function_to_string_test_05 begin');
    var maskCode = usb.HDC
    console.info('usb case maskCode : ' + maskCode);
    var strMaskCode = usb.usbFunctionsToString(maskCode)
    expect(strMaskCode).assertEqual('hdc');
    console.info('usb case maskCode ' + maskCode + ' usbFunctionsToString return int: ' + strMaskCode);
    console.info('usb usb_function_to_string_test_05 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_to_string_test_06
   * @tc.name      : usbFunctionsToString
   * @tc.desc      : 正向测试 5 返回 'acm,hdc'
   */
  it('SUB_USB_usb_function_to_string_test_06', 0, function () {
    console.info('usb usb_function_to_string_test_06 begin');
    var maskCode = usb.ACM | usb.HDC
    console.info('usb case maskCode : ' + maskCode);
    var strMaskCode = usb.usbFunctionsToString(maskCode)
    expect(strMaskCode).assertEqual('hdc,acm');
    console.info('usb case maskCode ' + maskCode + ' usbFunctionsToString return int: ' + strMaskCode);
    console.info('usb usb_function_to_string_test_06 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_to_string_test_07
   * @tc.name      : usbFunctionsToString
   * @tc.desc      : 正向测试 6 返回 'ecm,hdc'
   */
  it('SUB_USB_usb_function_to_string_test_07', 0, function () {
    console.info('usb usb_function_to_string_test_07 begin');
    var maskCode = usb.ECM | usb.HDC
    console.info('usb case maskCode : ' + maskCode);
    var strMaskCode = usb.usbFunctionsToString(maskCode)
    expect(strMaskCode).assertEqual('hdc,ecm');
    console.info('usb case maskCode ' + maskCode + ' usbFunctionsToString return int: ' + strMaskCode);
    console.info('usb usb_function_to_string_test_07 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_from_string_test_01
   * @tc.name      : usbFunctionsFromString
   * @tc.desc      : 正向测试 'none' 返回 0
   */
  it('SUB_USB_usb_function_from_string_test_01', 0, function () {
    console.info('usb usb_function_from_string_test_01 begin');
    var strMaskCode = 'none'
    var nMaskCode = usb.usbFunctionsFromString(strMaskCode)
    expect(nMaskCode).assertEqual(usb.NONE);
    console.info('usb case strMaskCode ' + strMaskCode + ' usbFunctionsFromString return int: ' + nMaskCode);
    console.info('usb usb_function_to_string_test_01 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_from_string_test_02
   * @tc.name      : usbFunctionsFromString
   * @tc.desc      : 正向测试 'acm' 返回 1
   */
  it('SUB_USB_usb_function_from_string_test_02', 0, function () {
    console.info('usb usb_function_from_string_test_02 begin');
    var strMaskCode = 'acm'
    var nMaskCode = usb.usbFunctionsFromString(strMaskCode)
    expect(nMaskCode).assertEqual(usb.ACM);
    console.info('usb case strMaskCode ' + strMaskCode + ' usbFunctionsFromString return int: ' + nMaskCode);
    console.info('usb usb_function_from_string_test_02 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_from_string_test_03
   * @tc.name      : usbFunctionsFromString
   * @tc.desc      : 正向测试 'ecm' 返回 2
   */
  it('SUB_USB_usb_function_from_string_test_03', 0, function () {
    console.info('usb usb_function_from_string_test_03 begin');
    var strMaskCode = 'ecm'
    var nMaskCode = usb.usbFunctionsFromString(strMaskCode)
    expect(nMaskCode).assertEqual(usb.ECM);
    console.info('usb case strMaskCode ' + strMaskCode + ' usbFunctionsFromString return int: ' + nMaskCode);
    console.info('usb usb_function_from_string_test_03 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_from_string_test_04
   * @tc.name      : usbFunctionsFromString
   * @tc.desc      : 正向测试 'acm,ecm' 返回 3
   */
  it('SUB_USB_usb_function_from_string_test_04', 0, function () {
    console.info('usb usb_function_from_string_test_04 begin');
    var strMaskCode = 'acm,ecm'
    var nMaskCode = usb.usbFunctionsFromString(strMaskCode)
    expect(nMaskCode).assertEqual(usb.ACM | usb.ECM);
    console.info('usb case strMaskCode ' + strMaskCode + ' usbFunctionsFromString return int: ' + nMaskCode);
    console.info('usb usb_function_from_string_test_04 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_from_string_test_05
   * @tc.name      : usbFunctionsFromString
   * @tc.desc      : 正向测试 'hdc' 返回 4
   */
  it('SUB_USB_usb_function_from_string_test_05', 0, function () {
    console.info('usb usb_function_from_string_test_05 begin');
    var strMaskCode = 'hdc'
    var nMaskCode = usb.usbFunctionsFromString(strMaskCode)
    expect(nMaskCode).assertEqual(usb.HDC);
    console.info('usb case strMaskCode ' + strMaskCode + ' usbFunctionsFromString return int: ' + nMaskCode);
    console.info('usb usb_function_from_string_test_04 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_from_string_test_06
   * @tc.name      : usbFunctionsFromString
   * @tc.desc      : 正向测试 'acm,hdc' 返回 5
   */
  it('SUB_USB_usb_function_from_string_test_06', 0, function () {
    console.info('usb usb_function_from_string_test_06 begin');
    var strMaskCode = 'acm,hdc'
    var nMaskCode = usb.usbFunctionsFromString(strMaskCode)
    expect(nMaskCode).assertEqual(usb.HDC | usb.ACM);
    console.info('usb case strMaskCode ' + strMaskCode + ' usbFunctionsFromString return int: ' + nMaskCode);
    console.info('usb usb_function_from_string_test_06 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_usb_function_from_string_test_07
   * @tc.name      : usbFunctionsFromString
   * @tc.desc      : 正向测试 'ecm,hdc' 返回 6
   */
  it('SUB_USB_usb_function_from_string_test_07', 0, function () {
    console.info('usb usb_function_from_string_test_07 begin');
    var strMaskCode = 'ecm,hdc'
    var nMaskCode = usb.usbFunctionsFromString(strMaskCode)
    expect(nMaskCode).assertEqual(usb.HDC | usb.ECM);
    console.info('usb case strMaskCode ' + strMaskCode + ' usbFunctionsFromString return int: ' + nMaskCode);
    console.info('usb usb_function_from_string_test_07 :  PASS');
  })

})
