// @ts-nocheck
/**
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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from "deccjsunit/index.ets";
import webSocket from '@ohos.net.webSocket'
import utils from './Utils.ets'

export default function webSocketJsunit() {
  describe('webSocketTest', function () {
    console.log("************* webSocket Test start*************");

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_createWebSocket_0100
     * @tc.name      : createWebSocket
     * @tc.desc      : Creates a web socket connection.
     */
    it('Telephony_WebSocket_WebSocket_createWebSocket_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_createWebSocket_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      var mWebSocket = webSocket.createWebSocket();
      expect(mWebSocket != null).assertTrue();
      console.log("************* Telephony_WebSocket_WebSocket_createWebSocket_0100 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocketRequestOptions_header_0100
     * @tc.name      : webSocketRequestOptions.header
     * @tc.desc      : webSocketRequestOptions.header
     */
    it('Telephony_WebSocket_WebSocketRequestOptions_header_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocketRequestOptions_header_0100 Test start*************");
      let webSocketRequestOptions: webSocket.WebSocketRequestOptions = {
        header: "test header"
      }
      expect(webSocketRequestOptions.header === "test header").assertTrue();
      console.log("************* Telephony_WebSocket_WebSocketRequestOptions_header_0100 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocketCloseOptions_code_0100
     * @tc.name      : webSocketCloseOptions.code
     * @tc.desc      : webSocketCloseOptions.code
     */
    it('Telephony_WebSocket_WebSocketCloseOptions_code_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocketCloseOptions_code_0100 Test start*************");
      let webSocketCloseOptions: webSocket.WebSocketCloseOptions = {
        code: 1,
        reason: "test reason"
      }
      expect(webSocketCloseOptions.code === 1).assertTrue();
      console.log("************* Telephony_WebSocket_WebSocketCloseOptions_code_0100 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocketCloseOptions_reason_0100
     * @tc.name      : webSocketCloseOptions.reason
     * @tc.desc      : webSocketCloseOptions.reason
     */
    it('Telephony_WebSocket_WebSocketCloseOptions_reason_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocketCloseOptions_reason_0100 Test start*************");
      let webSocketCloseOptions: webSocket.WebSocketCloseOptions = {
        code: 1,
        reason: "test reason"
      }
      expect(webSocketCloseOptions.reason).assertEqual("test reason");
      console.log("************* Telephony_WebSocket_WebSocketCloseOptions_reason_0100 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_connect_0100
     * @tc.name      : connect
     * @tc.desc      : Initiates a WebSocket request to establish a WebSocket connection to a given URL.
     */
    it('Telephony_WebSocket_WebSocket_connect_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_connect_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_connect_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.connect({
        url: "www.baidu.com"
      }, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_WebSocket_WebSocket_connect_0100 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_connect_0200
     * @tc.name      : connect
     * @tc.desc      : Initiates a WebSocket request to establish a WebSocket connection to a given URL.
     */
    it('Telephony_WebSocket_WebSocket_connect_0200', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_connect_0200 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_connect_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let webSocketRequestOptions: webSocket.WebSocketRequestOptions = {
        header: "test header"
      }
      webSocket.connect("www.baidu.com", webSocketRequestOptions, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_WebSocket_WebSocket_connect_0200 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_connect_0300
     * @tc.name      : connect
     * @tc.desc      : Initiates a WebSocket request to establish a WebSocket connection to a given URL.
     */
    it('Telephony_WebSocket_WebSocket_connect_0300', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_connect_0300 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_connect_0300"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let webSocketRequestOptions: webSocket.WebSocketRequestOptions = {
        header: "test header"
      }
      webSocket.connect("www.baidu.com", webSocketRequestOptions).then(data => {
        expect(true).assertTrue();
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      })
      console.log("************* Telephony_WebSocket_WebSocket_connect_0300 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_connect_0400
     * @tc.name      : connect
     * @tc.desc      : Initiates a WebSocket request to establish a WebSocket connection to a given URL.
     */
    it('Telephony_WebSocket_WebSocket_connect_0400', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_connect_0400 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_connect_0400"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.connect("www.baidu.com").then(data => {
        expect(true).assertTrue();
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      })
      console.log("************* Telephony_WebSocket_WebSocket_connect_0400 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_send_0100
     * @tc.name      : send
     * @tc.desc      : Sends data through a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_send_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_send_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_send_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.send("test data", (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_WebSocket_WebSocket_send_0100 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_send_0200
     * @tc.name      : send
     * @tc.desc      : Sends data through a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_send_0200', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_send_0200 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_send_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.send("test data").then(data => {
        expect(true).assertTrue();
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      })
      console.log("************* Telephony_WebSocket_WebSocket_send_0200 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_close_0100
     * @tc.name      : close
     * @tc.desc      : Closes a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_close_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_close_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_close_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.close((error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_WebSocket_WebSocket_close_0100 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_close_0200
     * @tc.name      : close
     * @tc.desc      : Closes a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_close_0200', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_close_0200 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_close_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let webSocketCloseOptions: webSocket.WebSocketCloseOptions = {
        code: 1,
        reason: "test reason"
      }
      webSocket.close(webSocketCloseOptions, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_WebSocket_WebSocket_close_0200 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_close_0300
     * @tc.name      : close
     * @tc.desc      : Closes a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_close_0300', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_close_0300 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_close_0300"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let webSocketCloseOptions: webSocket.WebSocketCloseOptions = {
        code: 1,
        reason: "test reason"
      }
      webSocket.close(webSocketCloseOptions).then(data => {
        expect(true).assertTrue();
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      })
      console.log("************* Telephony_WebSocket_WebSocket_close_0300 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_on_open_0100
     * @tc.name      : on
     * @tc.desc      : Enables listening for the open events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_on_open_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_on_open_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_on_open_0100"
      try {
        webSocket.on("open", (error, data) => {
          if (error) {
            console.log(`${caseName} error, case success,error:${toString(error)}`);
            done();
            return;
          }
          expect().assertFail();
          console.log(`${caseName} error, case failed,data:${toString(data)}`);
          done();
        });
        expect(true).assertTrue();
        done();
      } catch (err) {
        expect(true).assertTrue();
        console.log(`${caseName} error, catch error:${toString(err)}`);
        done();
      }
      console.log("************* Telephony_WebSocket_WebSocket_on_open_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_off_open_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for the open events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_off_open_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_off_open_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_off_open_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.off("open", (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      });
      console.log("************* Telephony_WebSocket_WebSocket_off_open_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_off_open_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for the open events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_off_open_0200', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_off_open_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.off("open");
      expect(true).assertTrue();
      console.log("************* Telephony_WebSocket_WebSocket_off_open_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_on_message_0100
     * @tc.name      : on
     * @tc.desc      : Enables listening for the message events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_on_message_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_on_message_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_on_message_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.on("message", (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      });
      console.log("************* Telephony_WebSocket_WebSocket_on_message_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_off_message_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for the message events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_off_message_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_off_message_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_off_message_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.off("message", (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      });
      console.log("************* Telephony_WebSocket_WebSocket_off_message_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_off_message_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for the message events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_off_message_0200', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_off_message_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.off("message");
      expect(true).assertTrue();
      console.log("************* Telephony_WebSocket_WebSocket_off_message_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_on_close_0100
     * @tc.name      : on
     * @tc.desc      : Enables listening for the close events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_on_close_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_on_close_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_on_close_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.on("close", (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      });
      console.log("************* Telephony_WebSocket_WebSocket_on_close_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_off_close_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for the close events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_off_close_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_off_close_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_off_close_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.off("close", (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      });
      console.log("************* Telephony_WebSocket_WebSocket_off_close_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_off_close_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for the close events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_off_close_0200', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_off_close_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.off("close");
      expect(true).assertTrue();
      console.log("************* Telephony_WebSocket_WebSocket_off_close_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_on_error_0100
     * @tc.name      : on
     * @tc.desc      : Enables listening for the error events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_on_error_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_on_error_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_on_error_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.on("error", error => {
        console.log(`${caseName} fail, error:${toString(error)}`);
        expect().assertFail();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_WebSocket_WebSocket_on_error_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_off_error_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for the error events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_off_error_0100', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_off_error_0100 Test start*************");
      let caseName = "Telephony_WebSocket_WebSocket_off_error_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.off("error", error => {
        console.log(`${caseName} fail, error:${toString(error)}`);
        expect().assertFail();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_WebSocket_WebSocket_off_error_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_WebSocket_WebSocket_off_error_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for the error events of a WebSocket connection.
     */
    it('Telephony_WebSocket_WebSocket_off_error_0200', 0, async function (done) {
      console.log("************* Telephony_WebSocket_WebSocket_off_error_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      webSocket.off("error");
      expect(true).assertTrue();
      console.log("************* Telephony_WebSocket_WebSocket_off_error_0200 Test end*************");
      done();
    });

    console.log("************* webSocket Test end*************");

  })
}