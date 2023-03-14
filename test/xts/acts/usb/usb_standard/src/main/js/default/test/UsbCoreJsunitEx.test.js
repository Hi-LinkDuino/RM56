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
describe('UsbCoreJsFunctionsTestEx', function () {
  var gDeviceList
  beforeAll(function () {
    console.log('*************Usb Unit UsbCoreJsFunctionsTestEx Begin*************');
    var Version = usb.getVersion()
    console.info('begin test getversion :' + Version)
    // version > 17  host currentMode = 2 device currentMode = 1
    var usbPortList = usb.getPorts()
    if (usbPortList.length > 0) {
      if (usbPortList[0].status.currentMode == 1) {
        usb.setPortRoles(usbPortList[0].id, usb.SOURCE, usb.HOST).then(data => {
          console.info('usb case setPortRolesEx return: ' + data);
        }).catch(error => {
          console.info('usb case setPortRolesEx error : ' + error);
        });
        console.log('*************Usb Unit Begin switch to host*************');
      }
    }
    gDeviceList = usb.getDevices();
  })
  beforeEach(function () {
    console.info('beforeEach: *************Usb Unit Test CaseEx*************');
  })
  afterEach(function () {
    console.info('afterEach: *************Usb Unit Test CaseEx*************');
  })
  afterAll(function () {
    console.log('*************Usb Unit UsbCoreJsFunctionsTestEx End*************');
  })


  /**
   * @tc.number    : SUB_USB_connect_device_02
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 错误参数 devAddress 打开设备
   */
  it('SUB_USB_connect_device_02', 0, function () {
    console.info('usb connect_device_02 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 02 requestRight error:' + error);
      });
      console.info('usb 02 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.devAddress = 2 + 10000
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) +
      ' devAddress ' + device.devAddress);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertTrue();

    console.info('usb connect_device_02 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_03
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 busNum错误参数 打开设备
   */
  it('SUB_USB_connect_device_03', 0, function () {
    console.info('usb connect_device_03 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 03 requestRight error:' + error);
      });
      console.info('usb 03 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.busNum = 2 + 1000
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) +
      ' busNum ' + device.busNum);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertTrue();
    console.info('usb connect_device_03 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_04
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 serial错误参数 打开设备
   */
  it('SUB_USB_connect_device_04', 0, function () {
    console.info('usb connect_device_04 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 04 requestRight error:' + error);
      });
      console.info('usb 04 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.serial = 'asdfsd'
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) +
      ' serial ' + device.serial);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertFalse();
    var isPipClose = usb.closePipe(usbDevicePipe);
    console.info('usb case closePipe ret: ' + isPipClose);
    expect(isPipClose).assertEqual(0);
    console.info('usb connect_device_04 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_05
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 name错误参数 打开设备
   */
  it('SUB_USB_connect_device_05', 0, function () {
    console.info('usb connect_device_05 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 05 requestRight error:' + error);
      });
      console.info('usb 05 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.name = 2 + 10000
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) + ' name ' + device.name);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertFalse();
    var isPipClose = usb.closePipe(usbDevicePipe);
    console.info('usb case closePipe ret: ' + isPipClose);
    expect(isPipClose).assertEqual(0);
    console.info('usb connect_device_05 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_06
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 manufacturerName错误参数 打开设备
   */
  it('SUB_USB_connect_device_06', 0, function () {
    console.info('usb connect_device_06 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 06 requestRight error:' + error);
      });
      console.info('usb 06 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.manufacturerName = 2 + 10000
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) +
      ' manufacturerName ' + device.manufacturerName);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertFalse();
    var isPipClose = usb.closePipe(usbDevicePipe);
    console.info('usb case closePipe ret: ' + isPipClose);
    expect(isPipClose).assertEqual(0);
    console.info('usb connect_device_06 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_07
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 productName错误参数 打开设备
   */
  it('SUB_USB_connect_device_07', 0, function () {
    console.info('usb connect_device_07 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 07 requestRight error:' + error);
      });
      console.info('usb 07 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.productName = 'sdfsdfe'
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) +
      ' productName ' + device.productName);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertFalse();
    var isPipClose = usb.closePipe(usbDevicePipe);
    console.info('usb case closePipe ret: ' + isPipClose);
    expect(isPipClose).assertEqual(0);
    console.info('usb connect_device_07 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_08
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 version错误参数 打开设备
   */
  it('SUB_USB_connect_device_08', 0, function () {
    console.info('usb connect_device_08 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 08 requestRight error:' + error);
      });
      console.info('usb 08 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.version = 'gwefsdf'
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) +
      ' version ' + device.version);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertFalse();
    var isPipClose = usb.closePipe(usbDevicePipe);
    console.info('usb case closePipe ret: ' + isPipClose);
    expect(isPipClose).assertEqual(0);
    console.info('usb connect_device_08 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_09
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 vendorId错误参数 打开设备
   */
  it('SUB_USB_connect_device_09', 0, function () {
    console.info('usb connect_device_09 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 09 requestRight error:' + error);
      });
      console.info('usb 09 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.vendorId = 2 + 10000
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) +
      ' vendorId ' + device.vendorId);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertFalse();
    var isPipClose = usb.closePipe(usbDevicePipe);
    console.info('usb case closePipe ret: ' + isPipClose);
    expect(isPipClose).assertEqual(0);
    console.info('usb connect_device_09 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_10
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 productId错误参数 打开设备
   */
  it('SUB_USB_connect_device_10', 0, function () {
    console.info('usb connect_device_10 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 10 requestRight error:' + error);
      });
      console.info('usb 10 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.productId = 2 + 10000
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) +
      ' productId ' + device.productId);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertFalse();
    var isPipClose = usb.closePipe(usbDevicePipe);
    console.info('usb case closePipe ret: ' + isPipClose);
    expect(isPipClose).assertEqual(0);
    console.info('usb connect_device_10 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_11
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 clazz错误参数 打开设备
   */
  it('SUB_USB_connect_device_11', 0, function () {
    console.info('usb connect_device_11 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 11 requestRight error:' + error);
      });
      console.info('usb 11 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.clazz = 2 + 10000
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) + ' clazz ' + device.clazz);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertFalse();
    var isPipClose = usb.closePipe(usbDevicePipe);
    console.info('usb case closePipe ret: ' + isPipClose);
    expect(isPipClose).assertEqual(0);
    console.info('usb connect_device_11 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_12
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 subclass错误参数 打开设备
   */
  it('SUB_USB_connect_device_12', 0, function () {
    console.info('usb connect_device_12 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 12 requestRight error:' + error);
      });
      console.info('usb 12 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.subclass = 2 + 10000
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) + ' subclass ' + device.subclass);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertFalse();
    var isPipClose = usb.closePipe(usbDevicePipe);
    console.info('usb case closePipe ret: ' + isPipClose);
    expect(isPipClose).assertEqual(0);
    console.info('usb connect_device_12 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_connect_device_13
   * @tc.name      : connectDevice
   * @tc.desc      : 反向测试 protocol错误参数 打开设备
   */
  it('SUB_USB_connect_device_13', 0, function () {
    console.info('usb connect_device_13 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    var isRight = usb.hasRight(gDeviceList[0].name);
    if (!isRight) {
      usb.requestRight(gDeviceList[0].name).then(hasRight => {
      }).catch(error => {
        console.info('usb 13 requestRight error:' + error);
      });
      console.info('usb 13 requestRight end:');
      CheckEmptyUtils.sleep(3000)
    }

    var device = JSON.parse(JSON.stringify(gDeviceList[0]));
    device.protocol = 2 + 10000
    try {
      var usbDevicePipe = usb.connectDevice(device)
    } catch (err) {
      expect(true).assertTrue();
      console.info('usb connectDevice fail：' + err);
    }
    console.info('usb case connectDevice ret: ' + JSON.stringify(usbDevicePipe) + ' protocol ' + device.protocol);
    expect(CheckEmptyUtils.isEmpty(usbDevicePipe)).assertFalse();
    var isPipClose = usb.closePipe(usbDevicePipe);
    console.info('usb case closePipe ret: ' + isPipClose);
    expect(isPipClose).assertEqual(0);
    console.info('usb connect_device_13 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_has_right_02
   * @tc.name      : hasRight
   * @tc.desc      : 反向测试 给错误设备名字
   */
  it('SUB_USB_has_right_02', 0, function () {
    console.info('usb has_right_02 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    for (var i = 0; i < gDeviceList.length; i++) {
      var deviceName = gDeviceList[i].name
      deviceName = deviceName + '$#'
      var hasRight = usb.hasRight(deviceName)
      console.info('usb has_right ret :' + hasRight);
      expect(hasRight == false).assertTrue();
    }

    console.info('usb has_right_02 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_has_right_03
   * @tc.name      : hasRight
   * @tc.desc      : 反向测试 给设备名字为字母
   */
  it('SUB_USB_has_right_03', 0, function () {
    console.info('usb has_right_03 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    for (var i = 0; i < gDeviceList.length; i++) {
      var deviceName = gDeviceList[i].name
      deviceName = deviceName + 'abcdg'
      var hasRight = usb.hasRight(deviceName)
      console.info('usb hasRight ret :' + hasRight);
      expect(hasRight == false).assertTrue();
    }

    console.info('usb has_right_03 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_request_right_02
   * @tc.name      : requestRight
   * @tc.desc      : 反向测试 错误设备名字 请求权限
   */
  it('SUB_USB_request_right_02', 0, function () {
    console.info('usb request_right_02 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    for (var i = 0; i < gDeviceList.length; i++) {
      var diviceName = gDeviceList[i].name
      diviceName = diviceName + '@#'
      usb.requestRight(diviceName).then(hasRight => {
        console.info('usb request_right ret :' + hasRight);
        expect(hasRight).assertFalse();
        console.info('usb request_right_02 :  PASS');
      }).catch(error => {
        expect(error).assertFalse();
        console.info('usb request_right_02 error: ' + error);
      });
    }
  })

  /**
   * @tc.number    : SUB_USB_request_right_03
   * @tc.name      : requestRight
   * @tc.desc      : 反向测试 设备名字为数字 请求权限
   */
  it('SUB_USB_request_right_03', 0, function () {
    console.info('usb request_right_03 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    for (var i = 0; i < gDeviceList.length; i++) {
      var diviceName = gDeviceList[i].name
      diviceName = diviceName + '123'
      usb.requestRight(diviceName).then(hasRight => {
        console.info('usb request_right ret :' + hasRight);
        expect(hasRight).assertFalse();
        console.info('usb request_right_03 :  PASS');
      }).catch(error => {
        expect(error).assertFalse();
        console.info('usb request_right_03 error: ' + error);
      });
    }
  })
})
