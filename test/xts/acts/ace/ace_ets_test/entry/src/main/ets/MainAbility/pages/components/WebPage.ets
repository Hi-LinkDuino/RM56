// @ts-nocheck
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

@Entry
@Component
struct WebComponent {
  @State javaScriptAccess: boolean = true;
  @State fileAccess: boolean = true;
  @State geolocationAccess: boolean = true;
  @State imageAccess: boolean = true;
  @State domStorageAccess: boolean = false;
  @State onlineImageAccess: boolean = true;
  @State zoomAccess: boolean = true;
  @State accessBackward: boolean = true;
  @State accessForward: boolean = true;
  @State accessStep: boolean = true;
  @State password: boolean = true;
  @State tableData: boolean = true;
  @State wideViewModeAccess: boolean = true;
  @State overviewModeAccess: boolean = true;
  @State databaseAccess: boolean = true;
  @State textZoomAtio: number = 1;
  @State mixedMode: MixedMode = MixedMode.None
  @State cacheMode: CacheMode = CacheMode.None
  @State getHitTest: HitTestType = HitTestType.Unknown
  controller: WebController = new WebController();
  cookie: WebCookie = new WebCookie()
  testObj = {
    test: (data1, data2, data3) => {
      console.log('ets test web  : ' + data1)
      console.log('ets test web  : ' + data2)
      console.log('ets test web  : ' + data3)
      return 'AceString'
    },
    toString: () => {
      console.log('ets test web  : ++++++toString')
    }
  }

  build() {
    Column() {
      Row() {
        Button('混合内容')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.mixedMode = MixedMode.All
          })
        Button('混合内容兼容性模式')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.mixedMode = MixedMode.Compatible
          })
        Button('不允许混合内容')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.mixedMode = MixedMode.None
          })
        Button('确认弹框')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            AlertDialog.show(
              {
                title: 'title',
                message: 'text',
                primaryButton: {
                  value: 'cancel',
                  action: () => {
                    console.info('Callback when the first button is clicked')
                  }
                },
                secondaryButton: {
                  value: 'ok',
                  action: () => {
                    console.info('Callback when the second button is clicked')
                  }
                },
                cancel: () => {
                  console.info('Closed callbacks')
                }
              }
            )
          })
        Button('警告弹框')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            AlertDialog.show(
              {
                title: 'title',
                message: 'text',
                confirm: {
                  value: 'button',
                  action: () => {
                    console.info('Button-clicking callback')
                  }
                },
                cancel: () => {
                  console.info('Closed callbacks')
                }
              }
            )
          })

      }

      Row() {
        Button('先缓存后在线')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.cacheMode = CacheMode.Online
          })
        Button('加载缓存')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.cacheMode = CacheMode.Only
          })
        Button('加载在线')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.cacheMode = CacheMode.None
          })
        Button('设置cookie')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.cookie.setCookie()
          })
        Button('保存cookie')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.cookie.saveCookie()
          })
      }

      Row() {
        Button(this.accessBackward ? '页面可后退' : '页面不可后退')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.accessBackward = this.controller.accessBackward()
          })
        Button(this.accessForward ? '页面可前进' : '页面不可前进')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.accessForward = this.controller.accessForward()
          })
        Button(this.accessStep ? '页面可前进1步' : '页面不可前进1步')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.accessStep = this.controller.accessStep(1)
          })
        Button('清空JavaScript方法')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.deleteJavaScriptRegister('objName')
          })
        Button('Web组件获焦')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.requestFocus()
          })
        Button('停止页面加载')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.stop()
          })
      }

      Row() {
        Button('页面后退')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.backward()
          })
        Button('页面前进')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.forward()
          })
        Button('清理历史')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.clearHistory()
          })
        Button('获取点击对象类型')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            let getHitTest = this.controller.getHitTest()
            if (getHitTest == HitTestType.Edit) {
              console.log('this getHitTest : Edit');
            } else if (getHitTest == HitTestType.Email) {
              console.log('this getHitTest : Email');
            } else if (getHitTest == HitTestType.Http) {
              console.log('this getHitTest : Http');
            } else if (getHitTest == HitTestType.HttpImg) {
              console.log('this getHitTest : HttpImg');
            } else if (getHitTest == HitTestType.Img) {
              console.log('this getHitTest : Img');
            } else if (getHitTest == HitTestType.Map) {
              console.log('this getHitTest : Map');
            } else if (getHitTest == HitTestType.Phone) {
              console.log('this getHitTest : Phone');
            } else {
              getHitTest = HitTestType.Unknown
              console.log('this getHitTest : Unknown');
            }
          })
        Button('加载数据')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.loadData({
              data: "<html><body bgcolor=\"white\">Source : <pre> source:</pre></body></html>",
              mimetype: "text/html",
              encoding: "UTF-8",
              baseUrl: "https://www.baidu.com",
              historyUrl: "https://www.gitee.com"
            })
          })
        Button('加载URL')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.loadUrl({
              url: "https://www.baidu.com",
              additionalHttpHeaders: [{ key: 'div', value: 'div' }]
            })
          })
      }

      Row() {
        Button('页面重新激活')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.onActive()
          })
        Button('页面暂停渲染')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.onInactive()
          })
        Button('刷新页面')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.refresh()
          })
        Button('注册JavaScript')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.registerJavaScriptProxy({
              obj: this.testObj,
              name: 'objName',
              methodList: ['test', 'toString']
            })
          })
        Button('执行JavaScript脚本')
          .margin({ right: 10 })
          .fontSize(15)
          .width(150)
          .onClick(() => {
            this.controller.runJavaScript({
              script: "test()",
              callback: (result: string) => {
                console.log('run javascript result : ' + result);
              }
            })
          })
      }

      Web({ src: $rawfile('indextest.html'), controller: this.controller })
        .domStorageAccess(this.domStorageAccess)
        .fileAccess(this.fileAccess)
        .geolocationAccess(this.geolocationAccess)
        .imageAccess(this.imageAccess)
        .javaScriptAccess(this.javaScriptAccess)
        .mixedMode(this.mixedMode)
        .onlineImageAccess(this.onlineImageAccess)
        .zoomAccess(this.zoomAccess)
        .cacheMode(this.cacheMode)
        .tableData(this.tableData)
        .wideViewModeAccess(this.wideViewModeAccess)
        .overviewModeAccess(this.overviewModeAccess)
        .textZoomAtio(this.textZoomAtio)
        .databaseAccess(this.databaseAccess)
        .javaScriptProxy({
          obj: this.testObj,
          name: 'objName',
          methodList: ['test', 'toString']
        })
        .password(this.password)
        .onPageBegin((e) => {
          console.log("webTest : onPageBegin: " + e.url);
        })
        .onPageEnd(e => {
          console.log("webTest : onPageEnd: " + e.url);
          this.controller.runJavaScript('test()')
        })
        .onProgressChange((e) => {
          console.log("webTest : onProgressChange: " + e.newProgress);
        })
        .onTitleReceive((e) => {
          console.log("webTest : onProgressChange: " + e.title);
        })
        .onDownloadStart((e) => {
          console.log("webTest : onDownloadStart url : " + e.url);
          console.log("webTest : onDownloadStart userAgent : " + e.userAgent);
          console.log("webTest : onDownloadStart contentDisposition : " + e.contentDisposition);
          console.log("webTest : onDownloadStart mimetype : " + e.mimetype);
          console.log("webTest : onDownloadStart contentLength : " + e.contentLength);
        })
        .onGeolocationHide(() => {
          console.log("webTest : onGeolocationHide: ");
        })
        .onGeolocationShow((e) => {
          console.log("webTest : onDownloadStart origin : " + e.origin);
          e.geolocation.invoke(e.origin, true, true)
          console.log("webTest : onDownloadStart origin : " + e.geolocation.invoke);
        })
        .onRequestSelected(() => {
          console.log("webTest : onRequestSelected: ");
        })
        .onAlert((e) => {
          console.log("webTest : onAlert: " + e.url);
          console.log("webTest : onAlert: " + e.message);
          console.log("webTest : onAlert .result : " + e.result);
          console.log("webTest : onAlert .result : " + e.result.handleCancel());
          console.log("webTest : onAlert .result : " + e.result.handleConfirm());
        })
        .onBeforeUnload((e) => {
          console.log("webTest : onBeforeUnload: " + e.message);
          console.log("webTest : onBeforeUnload .result : " + e.result);
          console.log("webTest : onBeforeUnload .result : " + e.result.handleCancel());
          console.log("webTest : onBeforeUnload .result : " + e.result.handleConfirm());
        })
        .onConfirm((e) => {
          console.log("webTest : onConfirm: " + e.url);
          console.log("webTest : onConfirm: " + e.message);
          console.log("webTest : onConfirm .result : " + e.result);
          console.log("webTest : onConfirm .result : " + e.result.handleCancel());
          console.log("webTest : onConfirm .result : " + e.result.handleConfirm());
        })
        .onConsole((e) => {
          console.log("webTest : onConsole .message : " + e.message);
          console.log("webTest : onConsole .message : " + e.message.getMessage());
          console.log("webTest : onConsole .message : " + e.message.getSourceId());
          console.log("webTest : onConsole .message : " + e.message.getLineNumber());
          let level = e.message.getMessageLevel()
          if (level == MessageLevel.Debug) {
            console.log("webTest : onConsole: level Debug");
          } else if (level == MessageLevel.Error) {
            console.log("webTest : onConsole: level Error");
          } else if (level == MessageLevel.Info) {
            console.log("webTest : onConsole: level Info");
          } else if (level == MessageLevel.Log) {
            console.log("webTest : onConsole: level Log");
          } else {
            level = MessageLevel.Warn
            console.log("webTest : onConsole: level Warn");
          }
        })
        .onErrorReceive((e) => {
          console.log("webTest : onErrorReceive .request : " + e.request);
          console.log("webTest : onErrorReceive .request : " + e.request.getRequestHeader());
          console.log("webTest : onErrorReceive .request : " + e.request.getRequestUrl());
          console.log("webTest : onErrorReceive .request : " + e.request.isRequestGesture());
          console.log("webTest : onErrorReceive .request : " + e.request.isMainFrame());
          console.log("webTest : onErrorReceive .request : " + e.request.isRedirect());

          console.log("webTest : onErrorReceive .error : " + e.error);
          console.log("webTest : onErrorReceive .error : " + e.error.getErrorInfo());
          console.log("webTest : onErrorReceive .error : " + e.error.getErrorCode());
        })
        .onHttpErrorReceive((e) => {
          console.log("webTest : onHttpErrorReceive .request : " + e.request);
          console.log("webTest : onHttpErrorReceive .request : " + e.request.getRequestHeader());
          console.log("webTest : onHttpErrorReceive .request : " + e.request.getRequestUrl());
          console.log("webTest : onHttpErrorReceive .request : " + e.request.isRequestGesture());
          console.log("webTest : onHttpErrorReceive .request : " + e.request.isMainFrame());
          console.log("webTest : onHttpErrorReceive .request : " + e.request.isRedirect());

          console.log("webTest : onHttpErrorReceive .error : " + e.error);
          console.log("webTest : onHttpErrorReceive .error : " + e.error.getErrorInfo());
          console.log("webTest : onHttpErrorReceive .error : " + e.error.getErrorCode());
        })
        .onRefreshAccessedHistory((e) => {
          console.log("webTest : onRefreshAccessedHistory .url : " + e.url);
          console.log("webTest : onRefreshAccessedHistory .refreshed : " + e.refreshed);
        })
        .onUrlLoadIntercept((e) => {
          console.log("webTest : onUrlLoadIntercept .data : " + e.data);
        })
        .onSslErrorReceive((e) => {
          console.log("webTest : onSslErrorReceive .handle : " + e.handle);
          console.log("webTest : onSslErrorReceive .error : " + e.error);
        })
        .onRenderExited((e) => {
          console.log("webTest : onRenderExited .detail : " + JSON.stringify(e.detail));
        })
        .onFileSelectorShow((e) => {
          console.log("webTest : onFileSelectorShow .callback : " + e.callback);
          console.log("webTest : onFileSelectorShow .fileSelector : " + e.fileSelector);
        })

    }
  }
}