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
describe('UsbPortJsFunctionsTest', function () {

  beforeAll(function () {
    console.log('*************Usb Unit UsbPortJsFunctionsTest Begin*************');
    var Version = usb.getVersion()
    console.info('begin test getversion :' + Version)
  })
  beforeEach(function () {
    console.info('beforeEach: *************Usb Unit Test  Case*************');
  })
  afterEach(function () {
    console.info('afterEach: *************Usb Unit Test  Case*************');
  })
  afterAll(function () {
    console.log('*************Usb Unit UsbPortJsFunctionsTest End*************');
  })

  /**
   * @tc.number    : SUB_USB_get_ports_test_01
   * @tc.name      : getPorts
   * @tc.desc      : 获取USB端口描述信息列表
   */
  it('SUB_USB_get_ports_test_01', 0, function () {
    console.info('usb get_ports_test_01 begin');
    var usbPortList = usb.getPorts()
    if (usbPortList.length == 0) {
      console.info('usb get_ports_test_01 usbPortList is null');
      expect(false).assertTrue();
      return
    }
    expect(usbPortList.length > 0).assertTrue();
    console.info('usb get_ports_test_01 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_get_supported_modes_test_01
   * @tc.name      : getSupportedModes
   * @tc.desc      : 获取指定的端口支持的模式列表的组合掩码
   */
  it('SUB_USB_get_supported_modes_test_01', 0, function () {
    console.info('usb get_supported_modes_test_01 begin');
    var usbPortList = usb.getPorts()
    if (usbPortList.length == 0) {
      console.info('usb get_supported_modes_test_01 usbPortList is null');
      expect(false).assertTrue();
      return
    }

    expect(usbPortList.length > 0).assertTrue();
    for (var i = 0; i < usbPortList.length; i++) {
      var maskCode = usb.getSupportedModes(usbPortList[i].id)
      expect(maskCode).assertEqual(usbPortList[i].supportedModes);
    }

    console.info('usb get_supported_modes_test_01 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_set_port_roles_test_01
   * @tc.name      : setPortRoles
   * @tc.desc      : 切换为Device set powerRole 2 DataRole 2
   */
  it('SUB_USB_set_port_roles_test_01', 0, function () {
    console.info('usb set_port_roles_test_01 device 2 2 begin');
    var usbPortList = usb.getPorts()
    if (usbPortList.length == 0) {
      console.info('usb set_port_roles_test_01 device 2 2 usbPortList is null');
      expect(false).assertTrue();
      return
    }

    for (var i = 0; i < usbPortList.length; i++) {
      console.info('usb case set data role 2, data role 2');
      usb.setPortRoles(usbPortList[i].id, usb.SINK, usb.DEVICE).then(data => {
        console.info('usb case setPortRoles return: ' + data);
        expect(data).assertTrue();
      }).catch(error => {
        console.info('usb case setPortRoles error : ' + error);
        expect(false).assertTrue();
      });
    }

    console.info('usb set_port_roles_test_01 device 2 2:  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_set_port_roles_test_02
   * @tc.name      : setPortRoles
   * @tc.desc      : 切换为host set powerRole 1 DataRole 1
   */
  it('SUB_USB_set_port_roles_test_02', 0, function () {
    console.info('usb set_port_roles_test_02 host 1 1 begin');
    var usbPortList = usb.getPorts()
    if (usbPortList.length == 0) {
      console.info('usb set_port_roles_test_02 host 1 1 usbPortList is null');
      expect(false).assertTrue();
      return
    }

    for (var i = 0; i < usbPortList.length; i++) {
      console.info('usb case set data role 1, data role 1');
      CheckEmptyUtils.sleep(5000)
      usb.setPortRoles(usbPortList[i].id, usb.SOURCE, usb.HOST).then(data => {
        expect(data).assertTrue();
        console.info('usb case setPortRoles return: ' + data);
      }).catch(error => {
        console.info('usb case setPortRoles error : ' + error);
        expect(false).assertTrue();
      });
    }

    console.info('usb set_port_roles_test_02 host 1 1:  PASS');
    expect(true).assertTrue();
  })

})
