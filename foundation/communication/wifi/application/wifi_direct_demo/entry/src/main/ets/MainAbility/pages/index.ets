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

import InputComponent from './component/dialog/inputComponent.ets'
import CustomPromptDialog from './component/dialog/customPromptDialog.ets'
import wifiModel from '../model/wifiModeImpl/WifiModel.ets'
import logUtil from '../common/LogUtil.ets'
import storageUtil from '../common/StorageUtil.ets'
import {WifiEntity} from '../model/wifiModeImpl/WifiEntity.ets'
import prompt from '@system.prompt';

@Entry
@Component
struct Index {
  private dialogEvent: (value: string) => void //输入窗确认按钮回调方法
  private promptEvent: () => void //提示窗确认按钮回调方法
  private isFinding: boolean = true //是否正在扫描
  private connDeviceName: string = '' //提示框标题
  private connContent: string= '' //提示框内容
  private currentGroupInfo: any= {} //当前群组信息
  @State p2pDeviceList: any[] = [] //扫描到的P2P信息列表
  @State deviceName: string = 'OHOS' //本设备名称
  @State isDeviceStatus: boolean = true //设备状态
  @State isCreateDelGroup: boolean = false //创建/删除 群组 状态
  @State btnText: string= '停止' //

  private aboutToAppear() {
    logUtil.log('start -----------------   aboutToAppear')
    wifiModel.setDeviceName('OHOS') //设置设备名称
    storageUtil.putObjData('deviceName', 'OHOS') //缓存设备名称到本地
    wifiModel.getListener().on("p2pDevicesChange", this.onDevicesChange.bind(this))
    wifiModel.getListener().on("p2pStateChange", this.onP2pStateChange.bind(this))
    wifiModel.getListener().on("p2pConnStateChange", this.onP2pConnStateChange.bind(this))
    wifiModel.getListener().on("p2pPeerDiscoveryStateChange", this.onP2pPeerDiscoveryStateChange.bind(this))
    wifiModel.getListener().on("p2pCurrentDeviceChange", this.onP2pCurrentDeviceChange.bind(this))
    wifiModel.getListener().on("p2pGroupStateChange", this.onP2pGroupStateChange.bind(this))
    wifiModel.discoverDevices()
    logUtil.log('end -----------------   aboutToAppear')
  }

/*输入弹出窗*/
  inputController: CustomDialogController = new CustomDialogController({
    builder: InputComponent({
      title: '设备名称',
      inputHint: '请输入设备名称',
      inputValue: this.deviceName,
      inputType: InputType.Normal,
      eventConnect: (value: string) => {
        this.dialogEvent(value);
      }
    }),
    autoCancel: false
  });
//提示弹窗
  promptDialogController: CustomDialogController = new CustomDialogController({
    builder: CustomPromptDialog({
      title: this.connDeviceName,
      contentVal: this.connContent,
      eventAction: () => {
        this.promptEvent()
      }
    }),
    autoCancel: true
  });

  build() {
    Column() {
      Text('WLAN直连')
        .fontSize(24)
        .fontWeight(FontWeight.Bold)
        .margin({
          top: 15,
          left: 20,
          bottom: 15
        })
      Row() {
        Text('设备状态').fontSize(20)
        Text('').layoutWeight(1)
        Toggle({ type: ToggleType.Switch, isOn: this.isDeviceStatus })
          .width(40).height(30)
          .selectedColor(Color.Blue)
          .onChange(() => {
            this.isDeviceStatus = !this.isDeviceStatus;
            if (this.isDeviceStatus) {
              wifiModel.enableP2p()
            } else {
              wifiModel.disableP2p()
              this.p2pDeviceList = [];
            }
          });
      }
      .backgroundColor(Color.White)
      .borderRadius(6)
      .padding({
        left: 10,
        right: 10,
      })
      .margin({
        top: 10,
        left: 10,
        right: 10
      })

      Row() {
        Text('创建/删除 群组').fontSize(20)
        Text('').layoutWeight(1)
        Toggle({ type: ToggleType.Switch, isOn: this.isCreateDelGroup })
          .width(40).height(30)
          .selectedColor(Color.Blue)
          .onChange(() => {
            this.isCreateDelGroup = !this.isCreateDelGroup;
            if (this.isCreateDelGroup) {
              wifiModel.createGroup();
            } else {
              wifiModel.removeGroup();
            }
          });
      }
      .backgroundColor(Color.White)
      .borderRadius(6)
      .padding({
        left: 10,
        right: 10,
      })
      .margin({
        top: 10,
        left: 10,
        right: 10
      })

      Row() {
        Text('Current Group Info').fontSize(20)
        Text('').layoutWeight(1)
        Image('res/image/ic_right.svg').width(28).height(28).objectFit(ImageFit.Contain)
      }
      .backgroundColor(Color.White)
      .borderRadius(6)
      .padding({
        left: 10,
        right: 10,
        top: 10,
        bottom: 10
      })
      .margin({
        top: 10,
        bottom: 10,
        left: 10,
        right: 10
      })
      .onClick(() => {
        this.getCurrentGroupInfo()
      })

      Row() {
        Text('Delete Persist Group').fontSize(20)
        Text('').layoutWeight(1)
        Image('res/image/ic_right.svg').width(28).height(28).objectFit(ImageFit.Contain)
      }
      .backgroundColor(Color.White)
      .borderRadius(6)
      .padding({
        left: 10,
        right: 10,
        top: 10,
        bottom: 10
      })
      .margin({
        left: 10,
        right: 10
      })
      .onClick(() => {
        this.deletePersistGroup()
      })

      Text('我的设备')
        .fontSize(18)
        .fontColor(Color.Gray)
        .margin({
          left: 20,
          top: 10,
          bottom: 10
        })
      Row() {
        Text('设备名称').fontSize(20)
        Text(this.deviceName)
          .fontColor('#999')
          .fontSize(18)
          .layoutWeight(1)
          .maxLines(1)
          .textAlign(TextAlign.End)
          .margin({
            left: 30,
            right: 5
          })
        Image('res/image/ic_right.svg').width(28).height(28).objectFit(ImageFit.Contain)
      }
      .backgroundColor(Color.White)
      .borderRadius(6)
      .padding({
        left: 10,
        right: 10,
        top: 10,
        bottom: 10
      })
      .margin({
        left: 10,
        right: 10
      })
      .onClick(() => {
        this.dialogEvent = (value: string) => {
          if (wifiModel.setDeviceName(value)) {
            this.deviceName = value
            storageUtil.putObjData('deviceName', this.deviceName)
          }
        }
        this.inputController.open()
      })

      Row() {
        Text('可用设备')
          .backgroundColor('#F7FCFF')
          .fontSize(18)
          .fontColor(Color.Gray)
          .padding({
            top: 5,
            bottom: 5
          })
        Text('').layoutWeight(1)
        Image($r("app.media.ic_loading"))
          .width(24)
          .height(24)
          .objectFit(ImageFit.Contain)
          .visibility(this.isFinding ? Visibility.Visible : Visibility.None)
      }
      .margin({
        top: 10,
        right: 20,
        left: 20,
        bottom: 10
      })

      List({ space: 15 }) {
        ForEach(this.p2pDeviceList, (item) => {
          ListItem() {
            CustomItem({
              deviceName: item.deviceName,
              macAddress: item.macAddress,
              status: item.status
            })
          }.onClick(() => {
            this.connectP2pDevice(item)
          })
        })
      }.layoutWeight(1)

      Button(this.btnText)
        .fontSize(20)
        .width(200)
        .alignSelf(ItemAlign.Center)
        .margin({
          top: 20,
          bottom: 20
        })
        .onClick(() => {
          if (this.isFinding) {
            this.btnText = '扫描'
            this.isFinding = false;
            wifiModel.stopDiscoverDevices()
          } else {
            this.btnText = '停止'
            this.isFinding = true;
            wifiModel.discoverDevices()
          }
        })
    }
    .backgroundColor('#F7FCFF')
    .width('100%')
    .height('100%')
    .alignItems(HorizontalAlign.Start)
  }

  private connectP2pDevice(item) {
    logUtil.log("wifi direct: connectP2pDevice  " + item.deviceName + "   " + item.status);
    if (item.status == 0) {
      this.connDeviceName = item.deviceName
      wifiModel.getLinkInfo((result) => {
        this.connContent = result
      })
      this.promptEvent = () => {
      }
      if (this.connContent != null) {
        this.promptDialogController.open();
      }
    } else if (item.status == 1) {
      this.connDeviceName = '提示'
      this.connContent = '确定取消当前连接?'
      this.promptEvent = () => {
        prompt.showToast({ message: wifiModel.cancelConnect() ? '取消成功' : '取消失败' })
      }
      this.promptDialogController.open();
    } else if (item.status == 4) {
      prompt.showToast({ message: '当前不可用' })
    } else if (item.status == 3) {
      let config = {
        'macAddress': item.macAddress,
        'groupOwnerIntent': 7
      };
      wifiModel.connectP2pDevices(config);
    }
  }
//获取p2p设备列表
  private onDevicesChange() {
    logUtil.log("onDevicesChange");
    this.p2pDeviceList = []
    wifiModel.getP2pDevicesCallBack().then((list) => {
      logUtil.log('-----------------------------  ' + JSON.stringify(list))
      for (let i = 0;i < JSON.parse(JSON.stringify(list))
      .length; i++) {
        if (this.checkDouble(list[i].macAddress)) {
          continue
        }
        this.p2pDeviceList.push(new WifiEntity(i, list[i].ssid, list[i].macAddress, list[i].status))
      }
      logUtil.log('this.p2pDeviceList   ...   ' + JSON.stringify(this.p2pDeviceList))
    });
  }

  private checkDouble(macAddress: string): boolean{
    for (var index = 0; index < this.p2pDeviceList.length; index++) {
      const element = this.p2pDeviceList[index];
      if (element.macAddress == macAddress) {
        return true
      }
    }
    return false
  }

  private onP2pStateChange(code) {
    logUtil.log("onP2pStateChange " + code);
    if (code === 3) {
      logUtil.log("code is 3, call discoverDevices");
      this.isDeviceStatus = true
      wifiModel.discoverDevices()
    } else {
      this.isDeviceStatus = false
      wifiModel.stopDiscoverDevices();
    }
  }

  private onP2pConnStateChange(code) {
    logUtil.log("onP2pConnectedStateChange " + code);
  }

  private onP2pPeerDiscoveryStateChange(code) {
    logUtil.log("onP2pPeerDiscoveryStateChange " + JSON.stringify(code));
  }

  private onP2pCurrentDeviceChange(code) {
    logUtil.log("onP2pCurrentDeviceChange " + JSON.stringify(code));
  }

  private onP2pGroupStateChange(code) {
    logUtil.log("onP2pGroupStateChange " + JSON.stringify(code));
  }

  private getCurrentGroupInfo() {
    wifiModel.getCurrentGroupInfo((info) => {
      logUtil.log("current group info" + info);
      this.currentGroupInfo = {
        "isP2pGroupOwner": info.isP2pGroupOwner,
        "passphrase": info.passphrase,
        "interface": info.interface,
        "groupName": info.groupName,
        "networkId": info.networkId,
        "frequency": info.frequency,
        "isP2pPersistent": info.isP2pPersistent,
        "goIpAddress": info.goIpAddress
      }
      this.connDeviceName = '群组信息'
      this.connContent = JSON.stringify(this.currentGroupInfo)
      this.promptEvent = () => {
      }
      this.promptDialogController.open();
    });
  }

  deletePersistGroup() {
    this.connDeviceName = '群组信息'
    this.connContent = '确定删除群组?'
    this.promptEvent = () => {
      logUtil.log("deletePersistGroup" + JSON.stringify(this.currentGroupInfo));
      if (this.currentGroupInfo && this.currentGroupInfo.isP2pPersistent) {
        logUtil.log("networkid is " + this.currentGroupInfo.networkId);
        prompt.showToast({
          message: wifiModel.deletePersistentGroup(this.currentGroupInfo.networkId) ? '成功' : '失败'
        })
      }
    }
    this.promptDialogController.open();
  }

  private aboutToDisappear() {
    wifiModel.stopDiscoverDevices()
  }
}

@Component
struct CustomItem {
  private statusStr: string[] = ["已连接", "已邀请", "失败", "可用", "不可用"]
  @Prop deviceName: string
  @Prop macAddress: string
  @Prop status: number

  build() {
    Column() {
      Row() {
        Image('res/image/ic_phone.svg')
          .width(48).height(48)
          .objectFit(ImageFit.Contain)
          .margin({
            top: 10,
            bottom: 10,
            right: 10
          })
        Column() {
          Text(this.deviceName).fontSize(20).maxLines(1).fontWeight(FontWeight.Bold)
          Text(this.macAddress).fontSize(18).maxLines(1)
          Text(this.statusStr[this.status]).fontSize(16)
        }
        //        .borderWidth(1)
        //        .borderColor(Color.Red)
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)

        Image('res/image/ic_right.svg').width(28).height(28).objectFit(ImageFit.Contain)
      }
      .width('100%')
      .height(100)
      .borderRadius(10)
      .alignItems(VerticalAlign.Center)
      .backgroundColor('#D2E9FF')
      .padding({
        left: 10,
        right: 10
      })
    }
    .margin({
      right: 20,
      left: 20,
    })
  }
}