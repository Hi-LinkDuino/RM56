/**
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import BaseModel from '../BaseModel.ets';
import mLogUtil from '../../common/LogUtil.ets';
import WifiNativeJs from '@ohos.wifi';

export class WifiModel extends BaseModel {
  getListener() {
    return new WifiNativeJs.EventListener();
  }

  enableP2p() {
    mLogUtil.log("enableP2p");
    return WifiNativeJs.enableP2p();
  }

  disableP2p() {
    mLogUtil.log("disableP2p")
    return WifiNativeJs.disableP2p();
  }

  on(typeValue, callBack) {
    mLogUtil.log("on  " + typeValue);
    WifiNativeJs.on(typeValue, callBack);
  }

  discoverDevices() {
    mLogUtil.log("startDiscoverDevices");
    return WifiNativeJs.startDiscoverDevices();
  }

  stopDiscoverDevices() {
    mLogUtil.log("stopDiscoverDevices");
    return WifiNativeJs.stopDiscoverDevices();
  }

  discoverServices() {
    mLogUtil.log("DiscoverServices");
    return WifiNativeJs.discoverServices();
  }

  setDeviceName(name) {
    mLogUtil.log("setDeviceName");
    return WifiNativeJs.setP2pDeviceName(name);
  }

  stopDiscoverServices() {
    mLogUtil.log("stopDiscoverServices");
    return WifiNativeJs.stopDiscoverServices();
  }

  connectP2pDevices(config) {
    mLogUtil.log("connectP2pDevices");
    return WifiNativeJs.p2pConnect(config);
  }

  disconnectP2pDevices() {
    mLogUtil.log("disconnectP2pDevices");
    return WifiNativeJs.removeGroup();
  }

  cancelConnect() {
    mLogUtil.log("wifi direct - p2pCancelConnect()");
    return WifiNativeJs.p2pCancelConnect()();
  }

  getLinkInfo(cb) {
    mLogUtil.log("getLinkInfo");
    WifiNativeJs.getP2pLinkedInfo((result) => {
      mLogUtil.log("getLinkInfo  " + JSON.stringify(result));
      let resultStr = null
      if (result != null) {
        let info = JSON.parse(JSON.stringify(result));
        resultStr = 'connectState£º' + (info.connectState == 1 ? 'CONNECTED' : 'DISCONNECTED') + '\nisP2pGroupOwner£º' + info.isP2pGroupOwner + '\ngroupOwnerAddress£º' + info.groupOwnerAddress
      }
      cb(resultStr)
    });
  }

  getCurrentGroupInfo(cb) {
    mLogUtil.log("getCurrentGroupInfo");
    WifiNativeJs.getCurrentGroup((result) => {
      let info = result;
      mLogUtil.log("current group result = " + result);
      if (result != null) {
        mLogUtil.log("current group info = " + JSON.stringify(result));
        info = JSON.parse(JSON.stringify(result));
      }
      cb(info);
    });
  }

  getP2pDevicesCallBack() {
    mLogUtil.log("wifi model getP2pDevicesCallBack");
    let mDeviceList = [];
    return new Promise((resolve) => {
      WifiNativeJs.getP2pDevices((result) => {
        if (result == null) {
          mLogUtil.log("WifiNativeJs queryP2pDevices");
          return;
        }
        let datas = JSON.parse(JSON.stringify(result));
        mLogUtil.log('result ===   ' + JSON.stringify(result))
        for (let j = 0; j < datas.length; j++) {
          mDeviceList.push({
            ssid: datas[j].deviceName,
            macAddress: datas[j].macAddress,
            status: datas[j].status
          });
        }
        mLogUtil.log("mWifiList = " + JSON.stringify(mDeviceList));
        resolve(mDeviceList);
      });
    })
  }

  createGroup() {
    /*
        const int TEMPORARY_NET_ID = -1;
        const int PERSISTENT_NET_ID = -2;
        const int INVALID_NET_ID = -999;
         */
    WifiNativeJs.createGroup({
      'netId': -2
    });
  }

  deletePersistentGroup(id) {
    mLogUtil.log("deletePersistentGroup id = " + id)
    return WifiNativeJs.deletePersistentGroup(id)
  }

  removeGroup() {
    WifiNativeJs.removeGroup();
  }

  getMacAddress() {
    mLogUtil.log("getMacAddress");
    return WifiNativeJs.getDeviceMacAddress();
  }

  setWfdInfo() {
    return WifiNativeJs.setP2pWfdInfo(true);
  }
}

let wifiModel = new WifiModel();

export default wifiModel as WifiModel;