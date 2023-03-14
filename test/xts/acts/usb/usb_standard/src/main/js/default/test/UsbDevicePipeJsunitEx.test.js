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
import EventConstants from './EventConstants.js';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

/* usb device pipe test */
describe('UsbDevicePipeJsFunctionsTestEx', function () {
  var gDeviceList
  var gPipe

  beforeAll(function () {
    console.log('*************Usb Unit UsbDevicePipeJsFunctionsTestEx Begin*************');
    var Version = usb.getVersion()
    console.info('usb unit begin test getversion :' + Version)

    // version > 17  host currentMode = 2 device currentMode = 1
    var usbPortList = usb.getPorts()
    if (usbPortList.length > 0) {
      if (usbPortList[0].status.currentMode == 1) {
        usb.setPortRoles(usbPortList[0].id, usb.SOURCE, usb.HOST).then(data => {
          console.info('usb case setPortRoles  return: ' + data);
        }).catch(error => {
          console.info('usb case setPortRoles  error : ' + error);
        });

        console.log('*************Usb Unit switch to host Ex Begin*************');
        CheckEmptyUtils.sleep(3000)
      }
    }

    gDeviceList = usb.getDevices();
    gPipe = usb.connectDevice(gDeviceList[0])
    console.info('usb unit connectDevice  gPipe ret : ' + JSON.stringify(gPipe));
  })
  beforeEach(function () {
    console.info('beforeEach: *************Usb Unit Test Ex Case*************');
  })
  afterEach(function () {
    console.info('afterEach: *************Usb Unit Test Ex Case*************');
  })
  afterAll(function () {
    var isPipClose = usb.closePipe(gPipe)
    console.info('usb unit close gPipe ret : ' + isPipClose);
    console.log('*************Usb Unit UsbDevicePipeJsFunctionsTestEx End*************');
  })

  function findInitPoint(testParam, j) {
    var bfind = false
    for (var k = 0; k < testParam.config.interfaces[j].endpoints.length; k++) {
      var endpoint = testParam.config.interfaces[j].endpoints[k];
      if (endpoint.type == EventConstants.USB_ENDPOINT_XFER_BULK) {

        bfind = true
        if (endpoint.direction == usb.USB_REQUEST_DIR_TO_DEVICE) {
          testParam.outEndpoint = endpoint;
          testParam.maxOutSize = endpoint.maxPacketSize;
        } else if (endpoint.direction == usb.USB_REQUEST_DIR_FROM_DEVICE) {
          testParam.inEndpoint = endpoint
          testParam.maxInSize = endpoint.maxPacketSize;
        }
      }
    }
    if (bfind) {
      testParam.interface = testParam.config.interfaces[j]
      return true
    }
    return false
  }

  function getFlag(testParam, j) {
    if (testParam.config.interfaces[j].clazz != 10 ||
      testParam.config.interfaces[j].subclass != 0 ||
      testParam.config.interfaces[j].protocol != 2) {
      return false
    }

    if (testParam.config.interfaces[j].endpoints.length == 0) {
      return false
    }

    return true
  }

  function initPoint(testParam) {
    for (var j = 0; j < testParam.config.interfaces.length; j++) {
      if (getFlag(testParam, j)) {
        if (findInitPoint(testParam, j)) {
          break
        }
      }
    }
  }

  // 预制传输相关参数
  function getTransferTestParam() {
    var testParam = {
      device: null,
      config: null,
      pip: null,
      inEndpoint: null,
      outEndpoint: null,
      interface: null,
      usbRequest: null,
      sendData: '',
      isClaimed: 0,
      maxInSize: 1024,
      maxOutSize: 1024
    }

    console.info('usb case gDeviceList.length: ' + gDeviceList.length);
    for (var i = 0; i < gDeviceList.length; i++) {
      testParam.device = gDeviceList[i]
      testParam.config = testParam.device.configs[0]
      testParam.pip = gPipe
      initPoint(testParam)
    }
    return testParam
  }

  /**
   * @tc.number    : SUB_USB_bulk_transfer_test_03
   * @tc.name      : bulkTransfer
   * @tc.desc      : 反向测试 批量传输 发数据 修改 outEndpoint.address = 123
   */
  it('SUB_USB_bulk_transfer_test_03', 0, function () {
    console.info('usb bulk_transfer_test_03 begin');
    var testParam = getTransferTestParam()
    if (testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    testParam.isClaimed = usb.claimInterface(testParam.pip, testParam.interface, true);
    expect(testParam.isClaimed).assertEqual(0);

    testParam.sendData = 'send time 13213213 wzy 03';
    var tmpUint8Array = CheckEmptyUtils.str2ab(testParam.sendData);
    var TmpTestParam = testParam
    TmpTestParam.outEndpoint.address = 123
    usb.bulkTransfer(TmpTestParam.pip, TmpTestParam.outEndpoint, tmpUint8Array, 5000).then(data => {
      console.info('usb case bulk_transfer_test_03 ret: ' + data);
      console.info('usb case bulk_transfer_test_03 :  PASS');
      expect(data).assertEqual(-1);
    }).catch(error => {
      console.info('usb 03 write error : ' + JSON.stringify(error));
      expect(false).assertFalse();
      console.info('usb case bulk_transfer_test_03 :  PASS');
    });
  })

  /**
   * @tc.number    : SUB_USB_bulk_transfer_test_04
   * @tc.name      : bulkTransfer
   * @tc.desc      : 反向测试 批量传输 发数据 修改 outEndpoint.number = 123
   */
  it('SUB_USB_bulk_transfer_test_04', 0, function () {
    console.info('usb bulk_transfer_test_04 begin');
    var testParam = getTransferTestParam()
    if (testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    testParam.isClaimed = usb.claimInterface(testParam.pip, testParam.interface, true);
    expect(testParam.isClaimed).assertEqual(0);

    testParam.sendData = 'send time 13213213 wzy  04';
    var tmpUint8Array = CheckEmptyUtils.str2ab(testParam.sendData);
    var TmpTestParam = testParam
    TmpTestParam.outEndpoint.number = 123
    usb.bulkTransfer(TmpTestParam.pip, TmpTestParam.outEndpoint, tmpUint8Array, 5000).then(data => {
      console.info('usb case bulk_transfer_test_04 ret: ' + data);
      console.info('usb case bulk_transfer_test_04 :  PASS');
      expect(data).assertEqual(-1);
    }).catch(error => {
      console.info('usb 04 write error : ' + JSON.stringify(error));
      expect(false).assertFalse();
      console.info('usb case bulk_transfer_test_04 :  PASS');
    });
  })

  /**
   * @tc.number    : SUB_USB_bulk_transfer_test_05
   * @tc.name      : bulkTransfer
   * @tc.desc      : 反向测试 批量传输 发数据 修改 outEndpoint.type = 123
   */
  it('SUB_USB_bulk_transfer_test_05', 0, function () {
    console.info('usb bulk_transfer_test_05 begin');
    var testParam = getTransferTestParam()
    if (testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    testParam.isClaimed = usb.claimInterface(testParam.pip, testParam.interface, true);
    expect(testParam.isClaimed).assertEqual(0);

    testParam.sendData = 'send time 13213213 wzy 05';
    var tmpUint8Array = CheckEmptyUtils.str2ab(testParam.sendData);
    var TmpTestParam = testParam
    TmpTestParam.outEndpoint.type = 123
    usb.bulkTransfer(TmpTestParam.pip, TmpTestParam.outEndpoint, tmpUint8Array, 5000).then(data => {
      console.info('usb case bulk_transfer_test_05 ret: ' + data);
      console.info('usb case bulk_transfer_test_05 :  PASS');
      expect(data).assertEqual(-1);
    }).catch(error => {
      console.info('usb 05 write error : ' + JSON.stringify(error));
      expect(false).assertFalse();
      console.info('usb case bulk_transfer_test_05 :  PASS');
    });
  })

  /**
   * @tc.number    : SUB_USB_claim_interface_test_02
   * @tc.name      : claimInterface
   * @tc.desc      : 反向测试 USBInterface传入异常id 获取接口 失败
   */
  it('SUB_USB_claim_interface_test_02', 0, function () {
    console.info('usb claim_interface_test_02 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb case 02 current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb case 02 current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
        var TmpInterface = gDeviceList[0].configs[j].interfaces[k]
        TmpInterface.id = 123
        var isClaim = usb.claimInterface(gPipe, TmpInterface, true)
        console.info('usb case claimInterface function return: ' + isClaim);
        expect(isClaim).assertLess(0);
      }
    }

    console.info('usb claim_interface_test_02 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_claim_interface_test_03
   * @tc.name      : claimInterface
   * @tc.desc      : 反向测试 USBInterface传入异常protocol 获取接口 失败
   */
  it('SUB_USB_claim_interface_test_03', 0, function () {
    console.info('usb claim_interface_test_03 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb case 03 current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb case 03 current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
        var TmpInterface = gDeviceList[0].configs[j].interfaces[k]
        TmpInterface.protocol = 120
        var isClaim = usb.claimInterface(gPipe, TmpInterface, true)
        console.info('usb case claimInterface function return: ' + isClaim);
        expect(isClaim).assertLess(0);
      }
    }

    console.info('usb claim_interface_test_03 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_claim_interface_test_04
   * @tc.name      : claimInterface
   * @tc.desc      : 反向测试 USBInterface传入异常clazz 获取接口 失败
   */
  it('SUB_USB_claim_interface_test_04', 0, function () {
    console.info('usb claim_interface_test_04 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb case 04 current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb case 04 current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
        var TmpInterface = gDeviceList[0].configs[j].interfaces[k]
        TmpInterface.clazz = 784
        var isClaim = usb.claimInterface(gPipe, TmpInterface, true)
        console.info('usb case claimInterface function return: ' + isClaim);
        expect(isClaim).assertLess(0);
      }
    }

    console.info('usb claim_interface_test_04 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_claim_interface_test_05
   * @tc.name      : claimInterface
   * @tc.desc      : 反向测试 USBInterface传入异常name 获取接口 失败
   */
  it('SUB_USB_claim_interface_test_05', 0, function () {
    console.info('usb claim_interface_test_05 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb case 05 current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb case 05 current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
        var TmpInterface = gDeviceList[0].configs[j].interfaces[k]
        TmpInterface.name = '123sdf'
        var isClaim = usb.claimInterface(gPipe, TmpInterface, true)
        console.info('usb case claimInterface function return: ' + isClaim);
        expect(isClaim).assertLess(0);
      }
    }

    console.info('usb claim_interface_test_05 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_claim_interface_test_06
   * @tc.name      : claimInterface
   * @tc.desc      : 反向测试 USBInterface传入异常subClass 获取接口 失败
   */
  it('SUB_USB_claim_interface_test_06', 0, function () {
    console.info('usb claim_interface_test_06 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb case 06 current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb case 06 current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
        var TmpInterface = gDeviceList[0].configs[j].interfaces[k]
        TmpInterface.subClass = 1210
        var isClaim = usb.claimInterface(gPipe, TmpInterface, true)
        console.info('usb case claimInterface function return: ' + isClaim);
        expect(isClaim).assertLess(0);
      }
    }

    console.info('usb claim_interface_test_06 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_release_interface_test_01
   * @tc.name      : releaseInterface
   * @tc.desc      : 反向测试 获取接口 release传入Interface的错误ID 并释放
   */
  it('SUB_USB_release_interface_test_01', 0, function () {
    console.info('usb release_interface_test_01 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb 01 case current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }
    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb 01 case current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
        var tmpInterface = gDeviceList[0].configs[j].interfaces[k]
        tmpInterface.id = 134
        var isClaim = usb.releaseInterface(gPipe, tmpInterface)
        console.info('usb case releaseInterface function return: ' + isClaim);
        expect(isClaim).assertLess(0);
      }
    }

    console.info('usb release_interface_test_01 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_release_interface_test_02
   * @tc.name      : releaseInterface
   * @tc.desc      : 反向测试 获取接口 release传入Interface的错误name 并释放
   */
  it('SUB_USB_release_interface_test_02', 0, function () {
    console.info('usb release_interface_test_02 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb 02 case current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }
    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb 02 case current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
        var tmpInterface = gDeviceList[0].configs[j].interfaces[k]
        tmpInterface.name = '134wer'
        var isClaim = usb.releaseInterface(gPipe, tmpInterface)
        console.info('usb case releaseInterface function return: ' + isClaim);
        expect(isClaim).assertLess(0);
      }
    }

    console.info('usb release_interface_test_02 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_release_interface_test_03
   * @tc.name      : releaseInterface
   * @tc.desc      : 反向测试 获取接口 release传入Interface的错误clazz 并释放
   */
  it('SUB_USB_release_interface_test_03', 0, function () {
    console.info('usb release_interface_test_03 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb 03 case current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }
    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb 03 case current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
        var tmpInterface = gDeviceList[0].configs[j].interfaces[k]
        tmpInterface.clazz = 78
        var isClaim = usb.releaseInterface(gPipe, tmpInterface)
        console.info('usb case releaseInterface function return: ' + isClaim);
        expect(isClaim).assertLess(0);
      }
    }

    console.info('usb release_interface_test_03 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_release_interface_test_04
   * @tc.name      : releaseInterface
   * @tc.desc      : 反向测试 获取接口 release传入Interface的错误protocol 并释放
   */
  it('SUB_USB_release_interface_test_04', 0, function () {
    console.info('usb release_interface_test_04 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb 04 case current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }
    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb 04 case current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
        var tmpInterface = gDeviceList[0].configs[j].interfaces[k]
        tmpInterface.protocol = 124
        var isClaim = usb.releaseInterface(gPipe, tmpInterface)
        console.info('usb case releaseInterface function return: ' + isClaim);
        expect(isClaim).assertLess(0);
      }
    }

    console.info('usb release_interface_test_04 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_release_interface_test_05
   * @tc.name      : releaseInterface
   * @tc.desc      : 反向测试 获取接口 release传入Interface的错误subClass 并释放
   */
  it('SUB_USB_release_interface_test_05', 0, function () {
    console.info('usb release_interface_test_05 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb 05 case current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }
    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb 05 case current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
        var tmpInterface = gDeviceList[0].configs[j].interfaces[k]
        tmpInterface.subClass = 784
        var isClaim = usb.releaseInterface(gPipe, tmpInterface)
        console.info('usb case releaseInterface function return: ' + isClaim);
        expect(isClaim).assertLess(0);
      }
    }

    console.info('usb release_interface_test_05 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_set_interface_test_03
   * @tc.name      : setInterface
   * @tc.desc      : 反向测试 Interface的protocol 设置设备接口
   */
  it('SUB_USB_set_interface_test_03', 0, function () {
    console.info('usb set_interface_test_03 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue()
      return
    }
    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var isClaimed = usb.claimInterface(gPipe, gDeviceList[0].configs[j].interfaces[0], true);

      var tmpInterface = gDeviceList[0].configs[j].interfaces[0]
      tmpInterface.protocol = 482
      var ret = usb.setInterface(gPipe, tmpInterface)
      expect(ret).assertLess(0)
      console.info('usb case setInterface return : ' + ret)
    }

    console.info('usb set_interface_test_03 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_set_interface_test_04
   * @tc.name      : setInterface
   * @tc.desc      : 反向测试 Interface的clazz 设置设备接口
   */
  it('SUB_USB_set_interface_test_04', 0, function () {
    console.info('usb set_interface_test_04 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue()
      return
    }
    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var isClaim = usb.claimInterface(gPipe, gDeviceList[0].configs[j].interfaces[0], true)

      var tmpInterface = gDeviceList[0].configs[j].interfaces[0]
      tmpInterface.clazz = 482
      var ret = usb.setInterface(gPipe, tmpInterface)
      expect(ret).assertLess(0)
      console.info('usb case setInterface return : ' + ret)

    }

    console.info('usb set_interface_test_04 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_set_interface_test_05
   * @tc.name      : setInterface
   * @tc.desc      : 反向测试 Interface的subClass 设置设备接口
   */
  it('SUB_USB_set_interface_test_05', 0, function () {
    console.info('usb set_interface_test_05 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue()
      return
    }
    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var isClaim = usb.claimInterface(gPipe, gDeviceList[0].configs[j].interfaces[0], true)

      var tmpInterface = gDeviceList[0].configs[j].interfaces[0]
      tmpInterface.subClass = 482
      var ret = usb.setInterface(gPipe, tmpInterface)
      expect(ret).assertLess(0)
      console.info('usb case setInterface return : ' + ret)

    }

    console.info('usb set_interface_test_05 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_set_interface_test_06
   * @tc.name      : setInterface
   * @tc.desc      : 反向测试 Interface的name 设置设备接口
   */
  it('SUB_USB_set_interface_test_06', 0, function () {
    console.info('usb set_interface_test_05 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue()
      return
    }
    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var isClaim = usb.claimInterface(gPipe, gDeviceList[0].configs[j].interfaces[0], true)

      var tmpInterface = gDeviceList[0].configs[j].interfaces[0]
      tmpInterface.name = 'wer32'
      var ret = usb.setInterface(gPipe, tmpInterface)
      expect(ret).assertLess(0)
      console.info('usb case setInterface return : ' + ret)
    }

    console.info('usb set_interface_test_06 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_set_configuration_test_03
   * @tc.name      : setConfiguration
   * @tc.desc      : 反向测试 USBConfig的name 赋值错误 设置设备接口
   */
  it('SUB_USB_set_configuration_test_03', 0, function () {
    console.info('usb set_configuration_test_03 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }
    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var config = gDeviceList[0].configs[j]
      config.name = 'asdfsd'
      var ret = usb.setConfiguration(gPipe, config)
      console.info('usb case setConfiguration return : ' + ret);
      expect(ret).assertEqual(0);
    }

    console.info('usb set_configuration_test_03 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_set_configuration_test_04
   * @tc.name      : setConfiguration
   * @tc.desc      : 反向测试 USBConfig的name 赋值错误 设置设备接口
   */
  it('SUB_USB_set_configuration_test_04', 0, function () {
    console.info('usb set_configuration_test_04 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var config = gDeviceList[0].configs[j]
      config.name = 'asdfsd'
      config.id = 154
      var ret = usb.setConfiguration(gPipe, config)
      console.info('usb case setConfiguration return : ' + ret);
      expect(ret).assertLess(0);
    }

    console.info('usb set_configuration_test_04 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_set_configuration_test_05
   * @tc.name      : setConfiguration
   * @tc.desc      : 反向测试 USBConfig的attributes 赋值错误 设置设备接口
   */
  it('SUB_USB_set_configuration_test_05', 0, function () {
    console.info('usb set_configuration_test_05 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var config = gDeviceList[0].configs[j]
      config.attributes = 154
      var ret = usb.setConfiguration(gPipe, config)
      console.info('usb case setConfiguration return : ' + ret);
      expect(ret).assertLess(0);
    }

    console.info('usb set_configuration_test_05 :  PASS');
  })


})
