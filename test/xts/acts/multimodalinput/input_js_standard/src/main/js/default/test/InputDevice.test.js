/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import inputDevice from '@ohos.multimodalInput.inputDevice';
import {
  describe,
  beforeAll,
  beforeEach,
  afterEach,
  afterAll,
  it,
  expect
} from 'deccjsunit/index'

describe('MultimodalInput_test', function () {

  // 参数正确,返回一个数组
  it('inputDevice::getDeviceIds_test-01', 0, function () {
    console.log(`inputDevice::getDeviceIds_test-01 enter`);
    inputDevice.getDeviceIds((data, err) => {
      if (err) {
        expect(false).assertTrue();
      } else {
        expect(data).assertInstanceOf('Array');
      }
      console.log(`inputDevice::getDeviceIds_test-01 exit`);
    })
  })

  // 参数正确,判断一种或多种设备
  it("inputDevice::getDeviceIds_test-02", 0, function () {
    console.log(`inputDevice::getDeviceIds_test-02 enter`);
    inputDevice.getDeviceIds((data, err) => {
      if (err) {
        expect(false).assertTrue();
      } else {
        expect(data.length > 0).assertTure();
      }
      console.log(`inputDevice::getDeviceIds_test-02 exit`);
    })
  })

  it("inputDevice::getDevice_test-01", 0, function () {
    console.log(`inputDevice::getDevice_test-03 enter`);
    inputDevice.getDevice(-1, (data, err) => {
      if (err) {
        expect(false).assertTrue();
        console.log(`inputDevice::getDevice_test-03 ${JSON.stringify(err)}`);
      } else {
        expect(JSON.stringify(data) !== "{}").assertTrue();
      }
      console.log(`inputDevice::getDevice_test-03 exit`);
    })
  })

  // 参数正常,返回值正常
  it("inputDevice::getDevice_test-02", 0, function () {
    console.log(`inputDevice::getDevice_test-04 enter`);
    inputDevice.getDeviceIds((data, err) => {
      if (err) {
        expect(false).assertTrue();
      } else {
        let arr = [];
        for (let i = 0; i < data.length; i++) {
          inputDevice.getDevice(data[i], (res, err) => {
            console.log(`getDevice:data_i ${JSON.stringify(data[i])}`)
            console.log(`getDevice:data ${JSON.stringify(data)}`)
            console.log(`getDevice:err ${JSON.stringify(err)}`)
            arr = Object.keys(res);
          })
          expect(arr.length > 0).assertTrue();
        }
      }
      console.log(`inputDevice::getDevice_test-04 exit`);
    });
  })

  it("inputDevice::getDevice_test03", 0, function () {
    console.log(`inputDevice::getDevice_test-04 enter`);
    inputDevice.getDeviceIds((data, err) => {
      if (err) {
        expect(false).assertTrue();
      } else {
        let arr = [];
        for (let i = 0; i < data.length; i++) {
          inputDevice.getDevice(data[i], (res, err) => {
            console.log(`getDevice:data_i ${JSON.stringify(data[i])}`)
            console.log(`getDevice:data ${JSON.stringify(data)}`)
            console.log(`getDevice:err ${JSON.stringify(err)}`)
            arr = Object.keys(res);
            expect(data[i].id).assertInstanceOf('number');
            expect(data[i].sources).assertInstanceOf('string');
            expect(data[i].name).assertInstanceOf('Array');
            expect(data[i].axisRanges).assertInstanceOf('Array');
          })
          expect(arr.length > 0).assertTrue();
        }
      }
      console.log(`inputDevice::getDevice_test-04 exit`);
    });
  })
})
