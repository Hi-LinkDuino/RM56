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
import socket from '@ohos.net.socket'
import utils from './Utils.ets'

export default function socketJsunit() {
  describe('socketTest', function () {
    console.log("************* socket Test start*************");

    /*
     * @tc.number    : Telephony_Socket_Socket_constructUDPSocketInstance_0100
     * @tc.name      : constructUDPSocketInstance
     * @tc.desc      : Creates a UDPSocket object.
     */
    it('Telephony_Socket_Socket_constructUDPSocketInstance_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_Socket_constructUDPSocketInstance_0100 Test start*************");
      var udpSocket = socket.constructUDPSocketInstance();
      expect(udpSocket != null).assertTrue();
      console.log("************* Telephony_Socket_Socket_constructUDPSocketInstance_0100 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_Socket_Socket_constructTCPSocketInstance_0100
     * @tc.name      : constructTCPSocketInstance
     * @tc.desc      : Creates a TCPSocket object.
     */
    it('Telephony_Socket_Socket_constructTCPSocketInstance_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_Socket_constructTCPSocketInstance_0100 Test start*************");
      var tcpSocket = socket.constructTCPSocketInstance();
      expect(tcpSocket != null).assertTrue();
      console.log("************* Telephony_Socket_Socket_constructTCPSocketInstance_0100 Test end*************");
      done();
    })

    /*
     * @tc.number    : Telephony_Socket_Socket_NetAddress_0100
     * @tc.name      : NetAddress
     * @tc.desc      : interface NetAddress
     */
    it('Telephony_Socket_Socket_NetAddress_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_Socket_NetAddress_0100 Test start*************");
      let netAddress: socket.NetAddress = {
        address: "test address",
        family: 1,
        port: 8080
      }
      expect(netAddress.address === "test address").assertTrue()
      expect(netAddress.family === 1).assertTrue()
      expect(netAddress.port === 8080).assertTrue()
      console.log("************* Telephony_Socket_Socket_NetAddress_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSendOptions_data_0100
     * @tc.name      : UDPSendOptions.data
     * @tc.desc      : UDPSendOptions.data
     */
    it('Telephony_Socket_UDPSendOptions_data_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSendOptions_data_0100 Test start*************");
      let udpSendOptions: socket.UDPSendOptions = {
        data: "test data",
        address: {
          address: "test address",
          family: 1,
          port: 8080
        }
      }
      expect(udpSendOptions.data === "test data").assertTrue()
      console.log("************* Telephony_Socket_UDPSendOptions_data_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_ExtraOptionsBase_receiveBufferSize_0100
     * @tc.name      : ExtraOptionsBase.receiveBufferSize
     * @tc.desc      : ExtraOptionsBase.receiveBufferSize
     */
    it('Telephony_Socket_ExtraOptionsBase_receiveBufferSize_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_ExtraOptionsBase_receiveBufferSize_0100 Test start*************");
      let extraOptionsBase: socket.ExtraOptionsBase = {
        receiveBufferSize: 1,
        sendBufferSize: 1,
        reuseAddress: true,
        socketTimeout: 5000,
      }
      expect(extraOptionsBase.receiveBufferSize === 1).assertTrue()
      console.log("************* Telephony_Socket_ExtraOptionsBase_receiveBufferSize_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_ExtraOptionsBase_sendBufferSize_0100
     * @tc.name      : ExtraOptionsBase.sendBufferSize
     * @tc.desc      : ExtraOptionsBase.sendBufferSize
     */
    it('Telephony_Socket_ExtraOptionsBase_sendBufferSize_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_ExtraOptionsBase_sendBufferSize_0100 Test start*************");
      let extraOptionsBase: socket.ExtraOptionsBase = {
        receiveBufferSize: 1,
        sendBufferSize: 1,
        reuseAddress: true,
        socketTimeout: 5000,
      }
      expect(extraOptionsBase.sendBufferSize === 1).assertTrue()
      console.log("************* Telephony_Socket_ExtraOptionsBase_sendBufferSize_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_ExtraOptionsBase_reuseAddress_0100
     * @tc.name      : ExtraOptionsBase.reuseAddress
     * @tc.desc      : ExtraOptionsBase.reuseAddress
     */
    it('Telephony_Socket_ExtraOptionsBase_reuseAddress_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_ExtraOptionsBase_reuseAddress_0100 Test start*************");
      let extraOptionsBase: socket.ExtraOptionsBase = {
        receiveBufferSize: 1,
        sendBufferSize: 1,
        reuseAddress: true,
        socketTimeout: 5000,
      }
      expect(extraOptionsBase.reuseAddress).assertTrue()
      console.log("************* Telephony_Socket_ExtraOptionsBase_reuseAddress_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_ExtraOptionsBase_socketTimeout_0100
     * @tc.name      : ExtraOptionsBase.socketTimeout
     * @tc.desc      : ExtraOptionsBase.socketTimeout
     */
    it('Telephony_Socket_ExtraOptionsBase_socketTimeout_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_ExtraOptionsBase_socketTimeout_0100 Test start*************");
      let extraOptionsBase: socket.ExtraOptionsBase = {
        receiveBufferSize: 1,
        sendBufferSize: 1,
        reuseAddress: true,
        socketTimeout: 5000,
      }
      expect(extraOptionsBase.socketTimeout === 5000).assertTrue()
      console.log("************* Telephony_Socket_ExtraOptionsBase_socketTimeout_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_SocketStateBase_isClose_0100
     * @tc.name      : SocketStateBase.isClose
     * @tc.desc      : SocketStateBase.isClose
     */
    it('Telephony_Socket_SocketStateBase_isClose_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_SocketStateBase_isClose_0100 Test start*************");
      let socketStateBase: socket.SocketStateBase = {
        isBound: true,
        isClose: true,
        isConnected: true
      }
      expect(socketStateBase.isClose).assertTrue()
      console.log("************* Telephony_Socket_SocketStateBase_isClose_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_SocketRemoteInfo_port_0100
     * @tc.name      : SocketRemoteInfo.port
     * @tc.desc      : SocketRemoteInfo.port
     */
    it('Telephony_Socket_SocketRemoteInfo_port_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_SocketRemoteInfo_port_0100 Test start*************");
      let socketRemoteInfo: socket.SocketRemoteInfo = {
        address: "test address",
        family: 'IPv4',
        port: 8080,
        size: 1
      }
      expect(socketRemoteInfo.port === 8080).assertTrue()
      console.log("************* Telephony_Socket_SocketRemoteInfo_port_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_SocketRemoteInfo_size_0100
     * @tc.name      : SocketRemoteInfo.size
     * @tc.desc      : SocketRemoteInfo.size
     */
    it('Telephony_Socket_SocketRemoteInfo_size_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_SocketRemoteInfo_size_0100 Test start*************");
      let socketRemoteInfo: socket.SocketRemoteInfo = {
        address: "test address",
        family: 'IPv4',
        port: 8080,
        size: 1
      }
      expect(socketRemoteInfo.size === 1).assertTrue()
      console.log("************* Telephony_Socket_SocketRemoteInfo_size_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPConnectOptions_timeout_0100
     * @tc.name      : TCPConnectOptions.timeout
     * @tc.desc      : TCPConnectOptions.timeout
     */
    it('Telephony_Socket_TCPConnectOptions_timeout_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPConnectOptions_timeout_0100 Test start*************");
      let tcpConnectOptions: socket.TCPConnectOptions = {
        address: {
          address: "test address",
          family: 1,
          port: 8080
        },
        timeout: 1000
      }
      expect(tcpConnectOptions.timeout === 1000).assertTrue()
      console.log("************* Telephony_Socket_TCPConnectOptions_timeout_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSendOptions_data_0100
     * @tc.name      : TCPSendOptions.data
     * @tc.desc      : TCPSendOptions.data
     */
    it('Telephony_Socket_TCPSendOptions_data_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSendOptions_data_0100 Test start*************");
      let tcpSendOptions: socket.TCPSendOptions = {
        data: "test data",
        encoding: "test encoding"
      }
      expect(tcpSendOptions.data === "test data").assertTrue()
      console.log("************* Telephony_Socket_TCPSendOptions_data_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSendOptions_encoding_0100
     * @tc.name      : TCPSendOptions.encoding
     * @tc.desc      : TCPSendOptions.encoding
     */
    it('Telephony_Socket_TCPSendOptions_encoding_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSendOptions_encoding_0100 Test start*************");
      let tcpSendOptions: socket.TCPSendOptions = {
        data: "test data",
        encoding: "test encoding"
      }
      expect(tcpSendOptions.encoding === "test encoding").assertTrue()
      console.log("************* Telephony_Socket_TCPSendOptions_encoding_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPExtraOptions_keepAlive_0100
     * @tc.name      : TCPExtraOptions.keepAlive
     * @tc.desc      : TCPExtraOptions.keepAlive
     */
    it('Telephony_Socket_TCPExtraOptions_keepAlive_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPExtraOptions_keepAlive_0100 Test start*************");
      let tcpExtraOptions: socket.TCPExtraOptions = {
        keepAlive: true,
        OOBInline: false,
        TCPNoDelay: true,
        socketLinger: {
          on: true,
          linger: false
        }
      }
      expect(tcpExtraOptions.keepAlive).assertTrue()
      console.log("************* Telephony_Socket_TCPExtraOptions_keepAlive_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPExtraOptions_OOBInline_0100
     * @tc.name      : TCPExtraOptions.OOBInline
     * @tc.desc      : TCPExtraOptions.OOBInline
     */
    it('Telephony_Socket_TCPExtraOptions_OOBInline_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPExtraOptions_OOBInline_0100 Test start*************");
      let tcpExtraOptions: socket.TCPExtraOptions = {
        keepAlive: true,
        OOBInline: false,
        TCPNoDelay: true,
        socketLinger: {
          on: true,
          linger: false
        }
      }
      expect(!tcpExtraOptions.OOBInline).assertTrue()
      console.log("************* Telephony_Socket_TCPExtraOptions_OOBInline_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPExtraOptions_TCPNoDelay_0100
     * @tc.name      : TCPExtraOptions.TCPNoDelay
     * @tc.desc      : TCPExtraOptions.TCPNoDelay
     */
    it('Telephony_Socket_TCPExtraOptions_TCPNoDelay_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPExtraOptions_TCPNoDelay_0100 Test start*************");
      let tcpExtraOptions: socket.TCPExtraOptions = {
        keepAlive: true,
        OOBInline: false,
        TCPNoDelay: true,
        socketLinger: {
          on: true,
          linger: false
        }
      }
      expect(tcpExtraOptions.TCPNoDelay).assertTrue()
      console.log("************* Telephony_Socket_TCPExtraOptions_TCPNoDelay_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPExtraOptions_socketLinger_0100
     * @tc.name      : TCPExtraOptions.socketLinger
     * @tc.desc      : TCPExtraOptions.socketLinger
     */
    it('Telephony_Socket_TCPExtraOptions_socketLinger_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPExtraOptions_socketLinger_0100 Test start*************");
      let tcpExtraOptions: socket.TCPExtraOptions = {
        keepAlive: true,
        OOBInline: false,
        TCPNoDelay: true,
        socketLinger: {
          on: true,
          linger: false
        }
      }
      expect(tcpExtraOptions.socketLinger.on).assertTrue()
      expect(!tcpExtraOptions.socketLinger.linger).assertTrue()
      console.log("************* Telephony_Socket_TCPExtraOptions_socketLinger_0100 Test end*************");
      done();
    });


    /*
     * @tc.number    : Telephony_Socket_UDPSocket_bind_0100
     * @tc.name      : bind
     * @tc.desc      : Binds the IP address and port number.
     */
    it('Telephony_Socket_UDPSocket_bind_0100', 0, async function (done) {
      let caseName = "Telephony_Socket_UDPSocket_bind_0100"
      console.log("************* Telephony_Socket_UDPSocket_bind_0100 Test start*************");
      let netAddress: socket.NetAddress = {
        address: "test address",
        family: 1,
        port: 8080
      }
      socket.UDPSocket.bind(netAddress, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_UDPSocket_bind_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_bind_0200
     * @tc.name      : bind
     * @tc.desc      : Binds the IP address and port number.
     */
    it('Telephony_Socket_UDPSocket_bind_0200', 0, async function (done) {
      let caseName = "Telephony_Socket_UDPSocket_bind_0200"
      console.log("************* Telephony_Socket_UDPSocket_bind_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let netAddress: socket.NetAddress = {
        address: "test address",
        family: 1,
        port: 8080
      }
      try {
        await socket.UDPSocket.bind(netAddress).then(data => {
          console.log(`${caseName} success,data:${toString(data)}`);
        }).catch((error) => {
          console.log(`${caseName} failed,error:${toString(error)}`);
        });
      } catch (err) {
        console.log(`${caseName} failed,catch error:${toString(error)}`);
      }
      console.log("************* Telephony_Socket_UDPSocket_bind_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_bind_0300
     * @tc.name      : bind
     * @tc.desc      : Binds the IP address and port number.
     */
    it('Telephony_Socket_UDPSocket_bind_0300', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_bind_0300 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_bind_0300"
      let netAddress: socket.NetAddress = {
        address: "second test address"
      }
      socket.UDPSocket.bind(netAddress, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_UDPSocket_bind_0300 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_bind_0400
     * @tc.name      : bind
     * @tc.desc      : Binds the IP address and port number.
     */
    it('Telephony_Socket_UDPSocket_bind_0400', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_bind_0400 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_bind_0400"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let netAddress: socket.NetAddress = {
        address: "third test address",
        family: 3,
        port: 8080
      }
      socket.UDPSocket.bind(netAddress).then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_UDPSocket_bind_0400 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_send_0100
     * @tc.name      : send
     * @tc.desc      : Sends data over a UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_send_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_send_0100 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_send_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let udpSendOptions: socket.UDPSendOptions = {
        data: "test data",
        address: {
          address: "test address",
          family: 1,
          port: 8080
        }
      }
      socket.UDPSocket.send(udpSendOptions, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_UDPSocket_send_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_send_0200
     * @tc.name      : send
     * @tc.desc      : Sends data over a UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_send_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_send_0200 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_send_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let udpSendOptions: socket.UDPSendOptions = {
        data: "test data",
        address: {
          address: "test address",
          family: 1,
          port: 8080
        }
      }
      socket.UDPSocket.send(udpSendOptions).then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_UDPSocket_send_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_send_0300
     * @tc.name      : send
     * @tc.desc      : Sends data over a UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_send_0300', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_send_0300 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_send_0300"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let udpSendOptions: socket.UDPSendOptions = {
        data: "test data",
        address: {
          address: "second test address",
        }
      }
      socket.UDPSocket.send(udpSendOptions, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_UDPSocket_send_0300 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_send_0400
     * @tc.name      : send
     * @tc.desc      : Sends data over a UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_send_0400', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_send_0400 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_send_0400"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let udpSendOptions: socket.UDPSendOptions = {
        data: "test data",
        address: {
          address: "third test address"
        }
      }
      socket.UDPSocket.send(udpSendOptions).then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_UDPSocket_send_0400 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_close_0100
     * @tc.name      : close
     * @tc.desc      : Closes a UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_close_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_close_0100 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_close_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.close((error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        done();
      })
      console.log("************* Telephony_Socket_UDPSocket_close_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_close_0200
     * @tc.name      : close
     * @tc.desc      : Closes a UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_close_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_close_0200 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_close_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.close().then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_UDPSocket_close_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_getState_0100
     * @tc.name      : getState
     * @tc.desc      : Obtains the status of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_getState_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_getState_0100 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_getState_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.getState((error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        let socketStateBase: socket.SocketStateBase = data
        console.log(`${caseName} data:${toString(socketStateBase.isClose)}`);
        done();
      })
      console.log("************* Telephony_Socket_UDPSocket_getState_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_getState_0200
     * @tc.name      : getState
     * @tc.desc      : Obtains the status of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_getState_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_getState_0200 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_getState_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.getState().then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });

      console.log("************* Telephony_Socket_UDPSocket_getState_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_setExtraOptions_0100
     * @tc.name      : setExtraOptions
     * @tc.desc      : Sets other attributes of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_setExtraOptions_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_setExtraOptions_0100 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_setExtraOptions_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let udpExtraOptions: socket.UDPExtraOptions = {
        broadcast: true
      }
      socket.UDPSocket.setExtraOptions(udpExtraOptions, (error) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        expect(true).assertTrue();
        console.log(`${caseName} error, case failed`);
        done();
      })
      console.log("************* Telephony_Socket_UDPSocket_setExtraOptions_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_setExtraOptions_0200
     * @tc.name      : setExtraOptions
     * @tc.desc      : Sets other attributes of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_setExtraOptions_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_setExtraOptions_0200 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_setExtraOptions_0200"
      let udpExtraOptions: socket.UDPExtraOptions = {
        broadcast: true
      }
      try {
        await socket.UDPSocket.setExtraOptions(udpExtraOptions);
        console.log(`${caseName} success`);
        done();
      } catch (error) {
        console.log(`${caseName} failed,error:${error}`);
        done();
      }
      console.log("************* Telephony_Socket_UDPSocket_setExtraOptions_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_setExtraOptions_0300
     * @tc.name      : setExtraOptions
     * @tc.desc      : Sets other attributes of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_setExtraOptions_0300', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_setExtraOptions_0300 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_setExtraOptions_0300"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let udpExtraOptions: socket.UDPExtraOptions = {}
      socket.UDPSocket.setExtraOptions(udpExtraOptions, (error) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed`);
        done();
      })
      console.log("************* Telephony_Socket_UDPSocket_setExtraOptions_0300 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_setExtraOptions_0400
     * @tc.name      : setExtraOptions
     * @tc.desc      : Sets other attributes of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_setExtraOptions_0400', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_setExtraOptions_0400 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_setExtraOptions_0400"
      let udpExtraOptions: socket.UDPExtraOptions = {}
      try {
        await socket.UDPSocket.setExtraOptions(udpExtraOptions);
        console.log(`${caseName} success`);
        done();
      } catch (error) {
        console.log(`${caseName} failed,error:${error}`);
        expect(true).assertTrue();
        done();
      }
      console.log("************* Telephony_Socket_UDPSocket_setExtraOptions_0400 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_on_message_0100
     * @tc.name      : on
     * @tc.desc      : Listens for message receiving events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_on_message_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_on_message_0100 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_on_message_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.on('message', (data) => {
        let socketRemoteInfo: socket.SocketRemoteInfo = data.remoteInfo;
        console.log(`${caseName} port:${socketRemoteInfo.port}`);
        console.log(`${caseName} size:${socketRemoteInfo.size}`);
      });
      console.log("************* Telephony_Socket_UDPSocket_on_message_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_off_message_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for message receiving events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_off_message_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_off_message_0100 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_off_message_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.off("message", data => {
        let socketRemoteInfo: socket.SocketRemoteInfo = data.remoteInfo;
        console.log(`${caseName} port:${socketRemoteInfo.port}`);
        console.log(`${caseName} size:${socketRemoteInfo.size}`);
      });
      console.log("************* Telephony_Socket_UDPSocket_off_message_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_off_message_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for message receiving events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_off_message_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_off_message_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.off("message");
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_UDPSocket_off_message_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_on_listening_0100
     * @tc.name      : on
     * @tc.desc      : Listens for data packet message events or close events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_on_listening_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_on_listening_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.on("listening", data => {
        expect(true).assertTrue();
      });
      console.log("************* Telephony_Socket_UDPSocket_on_listening_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_off_listening_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for data packet message events or close events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_off_listening_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_off_listening_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.off("listening", data => {
        expect(true).assertTrue();
      });
      console.log("************* Telephony_Socket_UDPSocket_off_listening_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_off_listening_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for data packet message events or close events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_off_listening_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_off_listening_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.off("listening");
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_UDPSocket_off_listening_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_on_close_0100
     * @tc.name      : on
     * @tc.desc      : Listens for data packet message events or close events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_on_close_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_on_close_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.on("close", data => {
        expect(true).assertTrue();
      });
      console.log("************* Telephony_Socket_UDPSocket_on_close_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_off_close_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for data packet message events or close events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_off_close_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_off_close_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.off("close", data => {
        expect(true).assertTrue();
      });
      console.log("************* Telephony_Socket_UDPSocket_off_close_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_off_close_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for data packet message events or close events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_off_close_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_off_close_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.off("close");
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_UDPSocket_off_close_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_on_error_0100
     * @tc.name      : on
     * @tc.desc      : Listens for error events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_on_error_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_on_error_0100 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_on_error_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.on("error", error => {
        console.log(`${caseName} fail, error:${toString(error)}`);
        expect().assertFail();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_UDPSocket_on_error_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_off_error_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for error events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_off_error_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_off_error_0100 Test start*************");
      let caseName = "Telephony_Socket_UDPSocket_off_error_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.off("error", error => {
        console.log(`${caseName} fail, error:${toString(error)}`);
        expect().assertFail();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_UDPSocket_off_error_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_UDPSocket_off_error_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for error events of the UDPSocket connection.
     */
    it('Telephony_Socket_UDPSocket_off_error_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_UDPSocket_off_error_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.UDPSocket.off("error");
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_UDPSocket_off_error_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_bind_0100
     * @tc.name      : bind
     * @tc.desc      : Binds the IP address and port number
     */
    it('Telephony_Socket_TCPSocket_bind_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_bind_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_bind_0100"
      let netAddress: socket.NetAddress = {
        address: "test address",
        family: 1,
        port: 8080
      }
      socket.TCPSocket.bind(netAddress, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_bind_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_bind_0200
     * @tc.name      : bind
     * @tc.desc      : Binds the IP address and port number
     */
    it('Telephony_Socket_TCPSocket_bind_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_bind_0200 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_bind_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let netAddress: socket.NetAddress = {
        address: "test address",
        family: 1,
        port: 8080
      }
      socket.TCPSocket.bind(netAddress).then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_TCPSocket_bind_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_bind_0300
     * @tc.name      : bind
     * @tc.desc      : Binds the IP address and port number
     */
    it('Telephony_Socket_TCPSocket_bind_0300', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_bind_0300 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_bind_0300"
      let netAddress: socket.NetAddress = {
        address: "second test address",
      }
      socket.TCPSocket.bind(netAddress, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_bind_0300 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_bind_0400
     * @tc.name      : bind
     * @tc.desc      : Binds the IP address and port number
     */
    it('Telephony_Socket_TCPSocket_bind_0400', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_bind_0400 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_bind_0400"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let netAddress: socket.NetAddress = {
        address: "third test address"
      }
      socket.TCPSocket.bind(netAddress).then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_TCPSocket_bind_0400 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_send_0100
     * @tc.name      : send
     * @tc.desc      : Sends data over a TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_send_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_send_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_send_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpSendOptions: socket.TCPSendOptions = {
        data: "test data",
        encoding: "test encoding"
      }
      socket.TCPSocket.send(tcpSendOptions, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_send_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_send_0200
     * @tc.name      : send
     * @tc.desc      : Sends data over a TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_send_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_send_0200 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_send_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpSendOptions: socket.TCPSendOptions = {
        data: "test data",
        encoding: "test encoding"
      }
      socket.TCPSocket.send(tcpSendOptions).then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_TCPSocket_send_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_send_0300
     * @tc.name      : send
     * @tc.desc      : Sends data over a TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_send_0300', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_send_0300 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_send_0300"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpSendOptions: socket.TCPSendOptions = {
        data: "second test data",
      }
      socket.TCPSocket.send(tcpSendOptions, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_send_0300 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_send_0400
     * @tc.name      : send
     * @tc.desc      : Sends data over a TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_send_0400', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_send_0400 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_send_0400"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpSendOptions: socket.TCPSendOptions = {
        data: "second test data",
      }
      socket.TCPSocket.send(tcpSendOptions).then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_TCPSocket_send_0400 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_connect_0100
     * @tc.name      : connect
     * @tc.desc      : Sets up a connection to the specified IP address and port number.
     */
    it('Telephony_Socket_TCPSocket_connect_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_connect_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_connect_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpConnectOptions: socket.TCPConnectOptions = {
        address: {
          address: "test address",
          family: 1,
          port: 8080
        },
        timeout: 1000
      }
      socket.TCPSocket.connect(tcpConnectOptions, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_connect_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_connect_0200
     * @tc.name      : connect
     * @tc.desc      : Sets up a connection to the specified IP address and port number.
     */
    it('Telephony_Socket_TCPSocket_connect_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_connect_0200 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_connect_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpConnectOptions: socket.TCPConnectOptions = {
        address: {
          address: "test address",
          family: 1,
          port: 8080
        },
        timeout: 1000
      }
      socket.TCPSocket.connect(tcpConnectOptions).then(function (data) {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_TCPSocket_connect_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_connect_0300
     * @tc.name      : connect
     * @tc.desc      : Sets up a connection to the specified IP address and port number.
     */
    it('Telephony_Socket_TCPSocket_connect_0300', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_connect_0300 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_connect_0300"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpConnectOptions: socket.TCPConnectOptions = {
        address: {
          address: "second test address",
          family: 1,
          port: 8080
        }
      }
      socket.TCPSocket.connect(tcpConnectOptions, (error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_connect_0300 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_connect_0400
     * @tc.name      : connect
     * @tc.desc      : Sets up a connection to the specified IP address and port number.
     */
    it('Telephony_Socket_TCPSocket_connect_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_connect_0400 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_connect_0400"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpConnectOptions: socket.TCPConnectOptions = {
        address: {
          address: "second test address",
          family: 1,
          port: 8080
        }
      }
      socket.TCPSocket.connect(tcpConnectOptions).then(function (data) {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_TCPSocket_connect_0400 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_close_0100
     * @tc.name      : close
     * @tc.desc      : Closes a UDPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_close_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_close_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_close_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.close((error, data) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_close_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_close_0200
     * @tc.name      : close
     * @tc.desc      : Closes a UDPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_close_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_close_0200 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_close_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.close().then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_TCPSocket_close_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_getState_0100
     * @tc.name      : getState
     * @tc.desc      : Obtains the status of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_getState_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_getState_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_getState_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.getState((error, data) => {
        if (error) {
          console.log(`${caseName} case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} case failed,data:${toString(data)}`);
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_getState_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_getState_0200
     * @tc.name      : getState
     * @tc.desc      : Obtains the status of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_getState_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_getState_0200 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_getState_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.getState().then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });

      console.log("************* Telephony_Socket_TCPSocket_getState_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_setExtraOptions_0100
     * @tc.name      : setExtraOptions
     * @tc.desc      : Sets other attributes of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_setExtraOptions_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_setExtraOptions_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_setExtraOptions_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpExtraOptions: socket.TCPExtraOptions = {
        keepAlive: true,
        OOBInline: true,
        TCPNoDelay: true,
        socketLinger: {
          on: true,
          linger: true
        }
      }
      socket.TCPSocket.setExtraOptions(tcpExtraOptions, (error) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed`);
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_setExtraOptions_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_setExtraOptions_0200
     * @tc.name      : setExtraOptions
     * @tc.desc      : Sets other attributes of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_setExtraOptions_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_setExtraOptions_0200 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_setExtraOptions_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpExtraOptions: socket.TCPExtraOptions = {
        keepAlive: true,
        OOBInline: true,
        TCPNoDelay: true,
        socketLinger: {
          on: true,
          linger: true
        }
      }
      try {
        await socket.TCPSocket.setExtraOptions(tcpExtraOptions);
        console.log(`${caseName} success`);
        expect(true).assertTrue();
        done();
      } catch (error) {
        console.log(`${caseName} failed,error:${error}`);
        done();
      }
      console.log("************* Telephony_Socket_TCPSocket_setExtraOptions_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_setExtraOptions_0300
     * @tc.name      : setExtraOptions
     * @tc.desc      : Sets other attributes of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_setExtraOptions_0300', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_setExtraOptions_0300 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_setExtraOptions_0300"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpExtraOptions: socket.TCPExtraOptions = {
        socketLinger: {
          on: true,
          linger: true
        }
      }
      socket.TCPSocket.setExtraOptions(tcpExtraOptions, (error) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        console.log(`${caseName} error, case failed`);
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_setExtraOptions_0300 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_setExtraOptions_0400
     * @tc.name      : setExtraOptions
     * @tc.desc      : Sets other attributes of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_setExtraOptions_0400', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_setExtraOptions_0400 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_setExtraOptions_0400"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let tcpExtraOptions: socket.TCPExtraOptions = {
        socketLinger: {
          on: true,
          linger: true
        }
      }
      try {
        await socket.TCPSocket.setExtraOptions(tcpExtraOptions);
        console.log(`${caseName} success`);
        expect(true).assertTrue();
        done();
      } catch (error) {
        console.log(`${caseName} failed,error:${error}`);
        done();
      }
      console.log("************* Telephony_Socket_TCPSocket_setExtraOptions_0400 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_getRemoteAddress_0100
     * @tc.name      : getRemoteAddress
     * @tc.desc      : Obtains the peer address of a TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_getRemoteAddress_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_getRemoteAddress_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_getRemoteAddress_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.getRemoteAddress((error) => {
        if (error) {
          console.log(`${caseName} error, case success,error:${toString(error)}`);
          done();
          return;
        }
        expect().assertFail();
        done();
      })
      console.log("************* Telephony_Socket_TCPSocket_getRemoteAddress_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_getRemoteAddress_0200
     * @tc.name      : getRemoteAddress
     * @tc.desc      : Obtains the peer address of a TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_getRemoteAddress_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_getRemoteAddress_0200 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_getRemoteAddress_0200"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.getRemoteAddress().then(data => {
        console.log(`${caseName} success,data:${toString(data)}`);
      }).catch((error) => {
        console.log(`${caseName} failed,error:${toString(error)}`);
      });
      console.log("************* Telephony_Socket_TCPSocket_getRemoteAddress_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_on_message_0100
     * @tc.name      : on
     * @tc.desc      : Listens for message receiving events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_on_message_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_on_message_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_on_message_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.on("message", data => {
        let socketRemoteInfo: socket.SocketRemoteInfo = data.remoteInfo;
        console.log(`${caseName} port:${socketRemoteInfo.port}`);
        console.log(`${caseName} size:${socketRemoteInfo.size}`);
      });
      console.log("************* Telephony_Socket_TCPSocket_on_message_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_off_message_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for message receiving events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_off_message_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_off_message_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_off_message_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.off("message", data => {
        let socketRemoteInfo: socket.SocketRemoteInfo = data.remoteInfo;
        console.log(`${caseName} port:${socketRemoteInfo.port}`);
        console.log(`${caseName} size:${socketRemoteInfo.size}`);
      });
      console.log("************* Telephony_Socket_TCPSocket_off_message_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_off_message_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for message receiving events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_off_message_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_off_message_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.off("message");
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_TCPSocket_off_message_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_on_connect_0100
     * @tc.name      : on
     * @tc.desc      : Listens for connection or close events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_on_connect_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_on_connect_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.on("connect", data => {
        expect(true).assertTrue();
      });
      console.log("************* Telephony_Socket_TCPSocket_on_connect_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_off_connect_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for connection or close events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_off_connect_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_off_connect_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.off("connect", data => {
        expect(true).assertTrue();
      });
      console.log("************* Telephony_Socket_TCPSocket_off_connect_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_off_connect_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for connection or close events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_off_connect_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_off_connect_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.off("connect");
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_TCPSocket_off_connect_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_on_close_0100
     * @tc.name      : on
     * @tc.desc      : Listens for connection or close events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_on_close_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_on_close_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.on("close", data => {
        expect(true).assertTrue();
      });
      console.log("************* Telephony_Socket_TCPSocket_on_close_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_off_close_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for connection or close events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_off_close_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_off_close_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.off("close", data => {
        expect(true).assertTrue();
      });
      console.log("************* Telephony_Socket_TCPSocket_off_close_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_off_close_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for connection or close events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_off_close_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_off_close_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.off("close");
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_TCPSocket_off_close_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_on_error_0100
     * @tc.name      : on
     * @tc.desc      : Listens for error events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_on_error_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_on_error_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_on_error_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.on("error", error => {
        console.log(`${caseName} fail, error:${toString(error)}`);
        expect().assertFail();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_TCPSocket_on_error_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_off_error_0100
     * @tc.name      : off
     * @tc.desc      : Cancels listening for error events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_off_error_0100', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_off_error_0100 Test start*************");
      let caseName = "Telephony_Socket_TCPSocket_off_error_0100"
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.off("error", error => {
        console.log(`${caseName} fail, error:${toString(error)}`);
        expect().assertFail();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_TCPSocket_off_error_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Socket_TCPSocket_off_error_0200
     * @tc.name      : off
     * @tc.desc      : Cancels listening for error events of the TCPSocket connection.
     */
    it('Telephony_Socket_TCPSocket_off_error_0200', 0, async function (done) {
      console.log("************* Telephony_Socket_TCPSocket_off_error_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      socket.TCPSocket.off("error");
      expect(true).assertTrue();
      console.log("************* Telephony_Socket_TCPSocket_off_error_0200 Test end*************");
      done();
    });

    console.log("************* socket Test end*************");

  })
}