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
import parameter from '@ohos.systemparameter';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

/* usb device pipe test */
describe('UsbDevicePipeJsFunctionsTest', function () {
  var gDeviceList
  var gPipe

  beforeAll(function () {
    console.log('*************Usb Unit UsbDevicePipeJsFunctionsTest Begin*************');
    var Version = usb.getVersion()
    console.info('usb unit begin test getversion :' + Version)
    // version > 17  host currentMode = 2 device currentMode = 1
    var usbPortList = usb.getPorts()
    if (usbPortList.length > 0) {
      if (usbPortList[0].status.currentMode == 1) {
        usb.setPortRoles(usbPortList[0].id, usb.SOURCE, usb.HOST).then(data => {
          console.info('usb case setPortRoles return: ' + data);
        }).catch(error => {
          console.info('usb case setPortRoles error : ' + error);
        });
        console.log('*************Usb Unit switch to host Begin*************');
      }
    }

    gDeviceList = usb.getDevices();
    gDeviceList[0].name;
    gDeviceList[0].version;
    gDeviceList[0].configs;
    gPipe = usb.connectDevice(gDeviceList[0])
    console.info('usb unit connectDevice gPipe ret : ' + JSON.stringify(gPipe));
  })

  beforeEach(function () {
    console.info('beforeEach: *************Usb Unit Test Case*************');
  })
  afterEach(function () {
    console.info('afterEach: *************Usb Unit Test Case*************');
  })

  afterAll(function () {
    var isPipClose = usb.closePipe(gPipe)
    console.info('usb unit close gPipe ret : ' + isPipClose);
    console.log('*************Usb Unit UsbDevicePipeJsFunctionsTest End*************');
  })

  function findInitPoint(testParam, j) {
    var bfind = false
    for (var k = 0; k < testParam.config.interfaces[j].endpoints.length; k++) {
      var endpoint = testParam.config.interfaces[j].endpoints[k];
      endpoint.number = 1;
      endpoint.type = 1;
      endpoint.interfaceid = 1;
      if (endpoint.type == EventConstants.USB_ENDPOINT_XFER_BULK) {
        bfind = true
        if (endpoint.direction == usb.USB_REQUEST_DIR_TO_DEVICE) {
          testParam.maxOutSize = endpoint.maxPacketSize;
          testParam.outEndpoint = endpoint;
        } else if (endpoint.direction == usb.USB_REQUEST_DIR_FROM_DEVICE) {
          testParam.maxInSize = endpoint.maxPacketSize;
          testParam.inEndpoint = endpoint
        }
      }
    }
    if (bfind) {
      testParam.interface = testParam.config.interfaces[j]
      return bfind
    }
    return false
  }

  function getFlag(testParam, j) {
    if (testParam.config.interfaces[j].endpoints.length == 0) {
      return false
    }

    if (testParam.config.interfaces[j].clazz != 10 ||
      testParam.config.interfaces[j].subclass != 0 ||
      testParam.config.interfaces[j].protocol != 2) {
      return false
    }
    return true
  }

  function initPoint(testParam) {
    for (var j = 0; j < testParam.config.interfaces.length; j++) {
      if (getFlag(testParam, j) == true) {
        if (findInitPoint(testParam, j) == true) {
          break
        }
      }
    }
  }

  function getTransferTestParam() {
    var testParam = {
      config: null,
      device: null,
      pip: null,
      inEndpoint: null,
      interface: null,
      outEndpoint: null,
      usbRequest: null,
      sendData: '',
      isClaimed: 0,
      maxInSize: 1024,
      maxOutSize: 1024
    }

    console.info('usb case  gDeviceList.length: ' + gDeviceList.length);
    for (var i = 0; i < gDeviceList.length; i++) {
      testParam.device = gDeviceList[i]
      testParam.config = testParam.device.configs[0]
      testParam.pip = gPipe
      initPoint(testParam)
    }
    return testParam
  }

  /**
   * @tc.number    : SUB_USB_bulk_transfer_test_01
   * @tc.name      : bulkTransfer
   * @tc.desc      : 批量传输 收数据
   */
  it('SUB_USB_bulk_transfer_test_01', 0, function () {
    console.info('usb bulk_transfer_test_01 begin');
    var testParam = getTransferTestParam()
    if (testParam.interface == null || testParam.inEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    testParam.isClaimed = usb.claimInterface(testParam.pip, testParam.interface, true);
    expect(testParam.isClaimed).assertEqual(0);

    console.info('usb case readData begin');
    var tmpUint8Array = new Uint8Array(testParam.maxInSize);
    usb.bulkTransfer(testParam.pip, testParam.inEndpoint, tmpUint8Array, 5000).then(data => {
      console.info('usb case readData tmpUint8Array buffer : ' + CheckEmptyUtils.ab2str(tmpUint8Array));
      console.info('usb case readData ret: ' + data);
      console.info('usb case bulk_transfer_test_01 :  PASS');
      expect(data >= 0).assertTrue();
    }).catch(error => {
      console.info('usb case readData error : ' + JSON.stringify(error));
      expect(false).assertTrue();
    });
  })

  /**
   * @tc.number    : SUB_USB_bulk_transfer_test_02
   * @tc.name      : bulkTransfer
   * @tc.desc      : 批量传输 发数据
   */
  it('SUB_USB_bulk_transfer_test_02', 0, function () {
    console.info('usb bulk_transfer_test_02 begin');
    var testParam = getTransferTestParam()
    if (testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    testParam.isClaimed = usb.claimInterface(testParam.pip, testParam.interface, true);
    expect(testParam.isClaimed).assertEqual(0);

    testParam.sendData = 'send default';
    try {
      testParam.sendData = parameter.getSync('test_usb', 'default');
      console.log('usb parameter ' + JSON.stringify(testParam.sendData));
    } catch (e) {
      console.log('usb parameter getSync unexpected error: ' + e);
    }

    var tmpUint8Array = CheckEmptyUtils.str2ab(testParam.sendData);
    usb.bulkTransfer(testParam.pip, testParam.outEndpoint, tmpUint8Array, 5000).then(data => {
      console.info('usb case bulk_transfer_test_02 ret: ' + data);
      console.info('usb case bulk_transfer_test_02 send data: ' + testParam.sendData);
      console.info('usb case bulk_transfer_test_02 :  PASS');
      expect(true).assertTrue();
    }).catch(error => {
      console.info('usb write error : ' + JSON.stringify(error));
      expect(false).assertTrue();
    });

  })

  /**
   * @tc.number    : SUB_USB_claim_interface_test_01
   * @tc.name      : claimInterface
   * @tc.desc      : 获取接口 并释放
   */
  it('SUB_USB_claim_interface_test_01', 0, function () {
    console.info('usb claim_interface_test_01 begin');
    if (gDeviceList.length == 0) {
      console.info('usb 01 case get_device_list is null')
      expect(gDeviceList.length).assertEqual(-1);
      return
    }

    if (gDeviceList[0].configs.length == 0) {
      console.info('usb 01 case current device.configs.length = 0');
      expect(false).assertTrue();
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      if (gDeviceList[0].configs[j].interfaces.length == 0) {
        console.info('usb case current device.configs.interfaces.length = 0');
      }
      for (var k = 0; k < gDeviceList[0].configs[j].interfaces.length; k++) {
		//属性预制
		var interfaces = gDeviceList[0].configs[j].interfaces[k];
		interfaces.id = 1;
		interfaces.name = '';
		interfaces.alternateSetting = 1;
        var isClaim = usb.claimInterface(gPipe, gDeviceList[0].configs[j].interfaces[k], true)
        console.info('usb case claimInterface function return: ' + isClaim);
        expect(isClaim).assertEqual(0);
        if (isClaim == 0) {
          isClaim = usb.releaseInterface(gPipe, gDeviceList[0].configs[j].interfaces[k])
          console.info('usb case releaseInterface function return: ' + isClaim);
          expect(isClaim).assertEqual(0);
          gPipe = usb.connectDevice(gDeviceList[0])
        }
      }
    }

    console.info('usb claim_interface_test_01 :  PASS');
    expect(true).assertTrue();
  })

  function getTransferParam(iCmd, iReqType, iValue, iIndex) {
    var tmpUint8Array = new Uint8Array(512);
    var requestCmd = iCmd
    var requestType = iReqType
    var value = iValue;
    var index = iIndex;
    var controlParam = {
      request: requestCmd,
      reqType: requestType,
      value: value,
      index: index,
      data: tmpUint8Array
    }
    return controlParam
  }

  /**
   * @tc.number    : SUB_USB_set_configuration_test_01
   * @tc.name      : setConfiguration
   * @tc.desc      : 设置设备接口
   */
  it('SUB_USB_set_configuration_test_01', 0, function () {
    console.info('usb set_configuration_test_01 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    gPipe = usb.connectDevice(gDeviceList[0])
    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var configs = gDeviceList[0].configs[j];
      configs.id = 1;
      configs.name = '';
      configs.maxPower = 1;
      configs.isRemoteWakeUp = true;
      configs.isSelfPowered = true;
      var ret = usb.setConfiguration(gPipe, gDeviceList[0].configs[j])
      console.info('usb case setConfiguration return : ' + ret);
      expect(ret).assertEqual(0);
    }

    console.info('usb set_configuration_test_01 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_set_configuration_test_02
   * @tc.name      : setConfiguration
   * @tc.desc      : 反向测试 设置设备接口
   */
  it('SUB_USB_set_configuration_test_02', 0, function () {
    console.info('usb set_configuration_test_02 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var config = JSON.parse(JSON.stringify(gDeviceList[0].configs[j]));
      config.id = 255
      var ret = usb.setConfiguration(gPipe, config)
      console.info('usb case setConfiguration return : ' + ret);
      expect(ret).assertLess(0);
    }

    console.info('usb set_configuration_test_02 :  PASS');
  })

  /**
   * @tc.number    : SUB_USB_set_interface_test_01
   * @tc.name      : setInterface
   * @tc.desc      : 设置设备接口
   */
  it('SUB_USB_set_interface_test_01', 0, function () {
    console.info('usb set_interface_test_01 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue();
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var isClaim = usb.claimInterface(gPipe, gDeviceList[0].configs[j].interfaces[0], true)
      expect(isClaim).assertEqual(0)
      if (isClaim == 0) {
        var ret = usb.setInterface(gPipe, gDeviceList[0].configs[j].interfaces[0])
        expect(ret).assertEqual(0);
        console.info('usb case setInterface return : ' + ret);
      }
    }

    console.info('usb set_interface_test_01 :  PASS');
    expect(true).assertTrue();
  })

  /**
   * @tc.number    : SUB_USB_set_interface_test_02
   * @tc.name      : setInterface
   * @tc.desc      : 反向测试 设置设备接口
   */
  it('SUB_USB_set_interface_test_02', 0, function () {
    console.info('usb set_interface_test_02 begin');
    if (gDeviceList.length == 0) {
      console.info('usb case get_device_list is null')
      expect(false).assertTrue()
      return
    }

    for (var j = 0; j < gDeviceList[0].configs.length; j++) {
      var isClaim = usb.claimInterface(gPipe, gDeviceList[0].configs[j].interfaces[0], true)
      expect(isClaim).assertEqual(0)
      if (isClaim == 0) {
        var tmpInterface = JSON.parse(JSON.stringify(gDeviceList[0].configs[j].interfaces[0]));
        tmpInterface.id = 234
        var ret = usb.setInterface(gPipe, tmpInterface)
        expect(ret).assertLess(0)
        console.info('usb case setInterface return : ' + ret)
      }
    }

    console.info('usb set_interface_test_02 :  PASS');
    expect(true).assertTrue();
  })

  function callControlTransfer(pip, controlParam, timeout, caseName) {
    usb.controlTransfer(pip, controlParam, timeout).then(data => {
      console.info('usb controlTransfer ret data : ' + data + ' ' + caseName);
      console.info('usb controlTransfer controlParam.data buffer : ' + controlParam.data + ' ' + caseName);
      console.info('usb' + caseName + ':  PASS');
      expect(true).assertTrue();
    }).catch(error => {
      console.info('usb controlTransfer error : ' + JSON.stringify(error));
      console.info('usb' + caseName + ':  PASS');
      expect(false).assertTrue();
    });
  }

  /**
   * @tc.number    : SUB_USB_control_transfer_test_01
   * @tc.name      : controlTransfer
   * @tc.desc      : 控制传输 GetDescriptor: cmd 6 reqType 128 value 512 index 0
   */
  it('SUB_USB_control_transfer_test_01', 0, function () {
    console.info('usb control_transfer_test_01 begin');
    var testParam = getTransferTestParam()
    if (testParam.inEndpoint == null || testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    var timeout = 5000;
    var controlParam = getTransferParam(6, (usb.USB_REQUEST_DIR_FROM_DEVICE << 7)
        | (usb.USB_REQUEST_TYPE_STANDARD << 5) | (usb.USB_REQUEST_TARGET_DEVICE & 0x1f), (2 << 8), 0)
    callControlTransfer(testParam.pip, controlParam, timeout, 'control_transfer_test_01 GetDescriptor')
  })

  /**
   * @tc.number    : SUB_USB_control_transfer_test_02
   * @tc.name      : controlTransfer
   * @tc.desc      : 控制传输 GetStatus: cmd 0 reqType 128 value 0 index 0
   */
  it('SUB_USB_control_transfer_test_02', 0, function () {
    console.info('usb control_transfer_test_02 begin');
    var testParam = getTransferTestParam()
    if (testParam.inEndpoint == null || testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    var timeout = 5000;
    var controlParam = getTransferParam(0, (usb.USB_REQUEST_DIR_FROM_DEVICE << 7)
        | (usb.USB_REQUEST_TYPE_STANDARD << 5) | (usb.USB_REQUEST_TARGET_DEVICE & 0x1f), 0, 0)
    callControlTransfer(testParam.pip, controlParam, timeout, 'control_transfer_test_02 GetStatus')
  })

  /**
   * @tc.number    : SUB_USB_control_transfer_test_03
   * @tc.name      : controlTransfer
   * @tc.desc      : 控制传输 GetConfiguration: cmd 8 reqType 128 value 0 index 0
   */
  it('SUB_USB_control_transfer_test_03', 0, function () {
    console.info('usb control_transfer_test_03 begin');
    var testParam = getTransferTestParam()
    if (testParam.inEndpoint == null || testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    var timeout = 5000;
    var controlParam = getTransferParam(8, (usb.USB_REQUEST_DIR_FROM_DEVICE << 7)
        | (usb.USB_REQUEST_TYPE_STANDARD << 5) | (usb.USB_REQUEST_TARGET_DEVICE & 0x1f), 0, 0)
    callControlTransfer(testParam.pip, controlParam, timeout, 'control_transfer_test_03 GetConfiguration')
  })

  /**
   * @tc.number    : SUB_USB_control_transfer_test_04
   * @tc.name      : controlTransfer
   * @tc.desc      : 控制传输 GetInterface: cmd 10 reqType 129 value 0 index 1
   */
  it('SUB_USB_control_transfer_test_04', 0, function () {
    console.info('usb control_transfer_test_04 begin');
    var testParam = getTransferTestParam()
    if (testParam.inEndpoint == null || testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    var timeout = 5000;
    var controlParam = getTransferParam(10, (usb.USB_REQUEST_DIR_FROM_DEVICE << 7)
        | (usb.USB_REQUEST_TYPE_STANDARD << 5) | (usb.USB_REQUEST_TARGET_INTERFACE & 0x1f), 0, 1)
    callControlTransfer(testParam.pip, controlParam, timeout, 'control_transfer_test_04 GetInterface')
  })

  /**
   * @tc.number    : SUB_USB_control_transfer_test_05
   * @tc.name      : controlTransfer
   * @tc.desc      : 控制传输 ClearFeature: cmd 1 reqType 0 value 0 index 0
   */
  it('SUB_USB_control_transfer_test_05', 0, function () {
    console.info('usb control_transfer_test_05 begin');
    var testParam = getTransferTestParam()
    if (testParam.inEndpoint == null || testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    var timeout = 5000;
    var controlParam = getTransferParam(1, (usb.USB_REQUEST_DIR_TO_DEVICE << 7)
        | (usb.USB_REQUEST_TYPE_STANDARD << 5) | (usb.USB_REQUEST_TARGET_DEVICE & 0x1f), 0, 0)
    callControlTransfer(testParam.pip, controlParam, timeout, 'control_transfer_test_05 ClearFeature')
  })

  /**
   * @tc.number    : SUB_USB_control_transfer_test_06
   * @tc.name      : controlTransfer
   * @tc.desc      : 控制传输 ClearFeature: cmd 1 reqType 0 value 0 index 0
   */
   it('SUB_USB_control_transfer_test_06', 0, function () {
    console.info('usb control_transfer_test_06 begin');
    var testParam = getTransferTestParam()
    if (testParam.inEndpoint == null || testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    var timeout = 5000;
    var controlParam = getTransferParam(255, (usb.USB_REQUEST_DIR_FROM_DEVICE << 7)
        | (usb.USB_REQUEST_TYPE_STANDARD << 5) | (usb.USB_REQUEST_TARGET_INTERFACE & 0x1f), (2 << 8), 0)
    callControlTransfer(testParam.pip, controlParam, timeout, 'control_transfer_test_06 ClearFeature')
  })

  /**
   * @tc.number    : SUB_USB_control_transfer_test_07
   * @tc.name      : controlTransfer
   * @tc.desc      : 控制传输 ClearFeature: cmd 1 reqType 0 value 0 index 0
   */
   it('SUB_USB_control_transfer_test_07', 0, function () {
    console.info('usb control_transfer_test_07 begin');
    var testParam = getTransferTestParam()
    if (testParam.inEndpoint == null || testParam.interface == null || testParam.outEndpoint == null) {
      expect(false).assertTrue();
      return
    }

    var timeout = 5000;
    var controlParam = getTransferParam(255, (usb.USB_REQUEST_DIR_TO_DEVICE << 7)
        | (usb.USB_REQUEST_TYPE_CLASS << 5) | (usb.USB_REQUEST_TARGET_ENDPOINT & 0x1f), (2 << 8), 0)
    callControlTransfer(testParam.pip, controlParam, timeout, 'control_transfer_test_07 ClearFeature')
  })

  /**
   * @tc.number    : SUB_USB_control_transfer_test_08
   * @tc.name      : controlTransfer
   * @tc.desc      : 控制传输 ClearFeature: cmd 1 reqType 0 value 0 index 0
   */
    it('SUB_USB_control_transfer_test_08', 0, function () {
      console.info('usb control_transfer_test_08 begin');
      var testParam = getTransferTestParam()
      if (testParam.inEndpoint == null || testParam.interface == null || testParam.outEndpoint == null) {
        expect(false).assertTrue();
        return
      }

      var timeout = 5000;
      var controlParam = getTransferParam(255, (usb.USB_REQUEST_DIR_TO_DEVICE << 7)
          | (usb.USB_REQUEST_TYPE_VENDOR << 5) | (usb.USB_REQUEST_TARGET_OTHER & 0x1f), (2 << 8), 0)
      callControlTransfer(testParam.pip, controlParam, timeout, 'control_transfer_test_08 ClearFeature')
    })

  /**
   * @tc.number    : SUB_USB_control_transfer_test_09
   * @tc.name      : controlTransfer
   * @tc.desc      : 控制传输 ClearFeature: cmd 1 reqType 0 value 0 index 0
   */
      it('SUB_USB_control_transfer_test_09', 0, function () {
        console.info('usb control_transfer_test_09 begin');
        var testParam = getTransferTestParam()
        if (testParam.inEndpoint == null || testParam.interface == null || testParam.outEndpoint == null) {
          expect(false).assertTrue();
          return
        }

        var timeout = 5000;
        var controlParam = getTransferParam(255, (usb.USB_REQUEST_DIR_TO_DEVICE << 7)
            | (usb.USB_REQUEST_TYPE_CLASS << 5) | (usb.USB_REQUEST_TARGET_OTHER & 0x1f), 0, 0)
        callControlTransfer(testParam.pip, controlParam, timeout, 'control_transfer_test_09 ClearFeature')
      })

})
