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
import {describe, it, expect} from "deccjsunit/index.ets";
import connection from '@ohos.net.connection'
import utils from './Utils.ets'

export default function connectionJsunit() {
  describe('connectionTest', function () {
    const TIMEOUT = 100;
    const DELAY = 1000;
    const NETID_IVVALID = 99;
    const NETID_IVVALID2 = 0;
    console.log("************* connection Test start*************");

    /*
     * @tc.number    : Telephony_Connection_Connection_createNetConnection_0100
     * @tc.name      : createNetConnection
     * @tc.desc      : Create a network connection with optional netSpefifier and timeout.
     */
    it('Telephony_Connection_Connection_createNetConnection_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_createNetConnection_0100 Test start*************");
      try {
        connection.createNetConnection(5000);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* Telephony_Connection_Connection_createNetConnection_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_createNetConnection_0200
     * @tc.name      : createNetConnection
     * @tc.desc      : Create a network connection with optional netSpefifier and timeout.
     */
    it('Telephony_Connection_Connection_createNetConnection_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_createNetConnection_0200 Test start*************");
      try {
        connection.createNetConnection(3000);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* Telephony_Connection_Connection_createNetConnection_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_createNetConnection_0300
     * @tc.name      : createNetConnection
     * @tc.desc      : Create a network connection with optional netSpefifier and timeout.
     */
    it('Telephony_Connection_Connection_createNetConnection_0300', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_createNetConnection_0300 Test start*************");
      try {
        connection.createNetConnection(8000);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* Telephony_Connection_Connection_createNetConnection_0300 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_createNetConnection_0400
     * @tc.name      : createNetConnection
     * @tc.desc      : Create a network connection with optional netSpefifier and timeout.
     */
    it('Telephony_Connection_Connection_createNetConnection_0400', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_createNetConnection_0400 Test start*************");
      try {
        let netSpecifier = {
          netCapabilities: {
            linkUpBandwidthKbps: 1000,
            linkDownBandwidthKbps: 2000,
            bearerTypes: [1],
            networkCap: [0],
          }, bearerPrivateIdentifier: '123'
        }
        connection.createNetConnection(netSpecifier, 5000);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* Telephony_Connection_Connection_createNetConnection_0400 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_createNetConnection_0500
     * @tc.name      : createNetConnection
     * @tc.desc      : Create a network connection with optional netSpefifier and timeout.
     */
    it('Telephony_Connection_Connection_createNetConnection_0500', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_createNetConnection_0500 Test start*************");
      try {
        let netSpecifier = {
          netCapabilities: {
            linkUpBandwidthKbps: 0,
            linkDownBandwidthKbps: 0,
            bearerTypes: [0],
            networkCap: [0],
          }, bearerPrivateIdentifier: '123'
        }
        connection.createNetConnection(netSpecifier, 5000);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* Telephony_Connection_Connection_createNetConnection_0500 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getDefaultNet_0100
     * @tc.name      : getDefaultNet
     * @tc.desc      : Obtains the data network that is activated by default.
     */
    it('Telephony_Connection_Connection_getDefaultNet_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getDefaultNet_0100 Test start*************");
      connection.getDefaultNet((error, data) => {
        expect().assertFail();
        done();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_getDefaultNet_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getDefaultNet_0200
     * @tc.name      : getDefaultNet
     * @tc.desc      : Obtains the data network that is activated by default.
     */
    it('Telephony_Connection_Connection_getDefaultNet_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getDefaultNet_0200 Test start*************");
      connection.getDefaultNet().then(data => {
        console.log("Telephony_Connection_Connection_getDefaultNet_0200 getDefaultNet success, case fail");
        expect().assertFail();
      }).catch(err => {
        console.log("Telephony_Connection_Connection_getDefaultNet_0200 getDefaultNet error, case success");
        expect(true).assertTrue();
      });
      console.log("************* Telephony_Connection_Connection_getDefaultNet_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getAllNets_0100
     * @tc.name      : getAllNets
     * @tc.desc      : Obtains the list of data networks that are activated.
     */
    it('Telephony_Connection_Connection_getAllNets_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getAllNets_0100 Test start*************");
      connection.getAllNets((error, data) => {
        if (error) {
          expect(true).assertTrue();
          return;
        }
        expect().assertFail();
      });
      done();
      console.log("************* Telephony_Connection_Connection_getAllNets_0100 Test end*************");
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getAllNets_0200
     * @tc.name      : getAllNets
     * @tc.desc      : Obtains the list of data networks that are activated.
     */
    it('Telephony_Connection_Connection_getAllNets_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getAllNets_0200 Test start*************");
      connection.getAllNets().then(data => {
        expect(true).assertTrue();
      }).catch(err => {
        console.log("Telephony_Connection_Connection_getAllNets_0200 getAllNets error ");
        expect().assertFail();
      });
      done();
      console.log("************* Telephony_Connection_Connection_getAllNets_0200 Test end*************");
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getConnectionProperties_0100
     * @tc.name      : getConnectionProperties
     * @tc.desc      : Queries the connection properties of a network.
     */
    it('Telephony_Connection_Connection_getConnectionProperties_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getConnectionProperties_0100 Test start*************");
      connection.getConnectionProperties((error, data) => {
        expect().assertFail();
        done();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_getConnectionProperties_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getConnectionProperties_0200
     * @tc.name      : getConnectionProperties
     * @tc.desc      : Queries the connection properties of a network.
     */
    it('Telephony_Connection_Connection_getConnectionProperties_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getConnectionProperties_0200 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      connection.getConnectionProperties().then(data => {
        expect().assertFail();
      }).catch(err => {
        expect(true).assertTrue();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_getConnectionProperties_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getConnectionProperties_0300
     * @tc.name      : getConnectionProperties
     * @tc.desc      : Queries the connection properties of a network.
     */
    it('Telephony_Connection_Connection_getConnectionProperties_0300', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getConnectionProperties_0300 Test start*************");
      var handle = new connection.NetHandle(NETID_IVVALID);
      connection.getConnectionProperties(handle, (error, data) => {
        if (error) {
          expect(true).assertTrue();
        }
      });
      console.log("************* Telephony_Connection_Connection_getConnectionProperties_0300 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getConnectionProperties_0400
     * @tc.name      : getConnectionProperties
     * @tc.desc      : Queries the connection properties of a network.
     */
    it('Telephony_Connection_Connection_getConnectionProperties_0400', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getConnectionProperties_0400 Test start*************");
      connection.getAllNets((error, value) => {
        if (error) {
          expect().assertFail();
          done();
          return;
        } else {
          connection.getConnectionProperties(value[0], (error, value_2) => {
            if (!error) {
              console.log(`Telephony_Connection_Connection_getConnectionProperties_0400  get value success: ${JSON.stringify(value_2)}`);
            }
          });
        }
      });
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_getConnectionProperties_0400 Test end*************");
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getNetCapabilities_0100
     * @tc.name      : getNetCapabilities
     * @tc.desc      : Obtains {@link NetCapabilities} of a {@link NetHandle} object.
     */
    it('Telephony_Connection_Connection_getNetCapabilities_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getNetCapabilities_0100 Test start*************");
      connection.getNetCapabilities((error, data) => {
        expect().assertFail();
        done();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_getNetCapabilities_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getNetCapabilities_0200
     * @tc.name      : getNetCapabilities
     * @tc.desc      : Obtains {@link NetCapabilities} of a {@link NetHandle} object.
     */
    it('Telephony_Connection_Connection_getNetCapabilities_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getNetCapabilities_0200 Test start*************");
      var handle = new connection.NetHandle(NETID_IVVALID);
      connection.getNetCapabilities(handle, (error, data) => {
        if (!error) {
          expect().assertFail();
          done();
          return;
        }
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_getNetCapabilities_0200 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getNetCapabilities_0300
     * @tc.name      : getNetCapabilities
     * @tc.desc      : Obtains {@link NetCapabilities} of a {@link NetHandle} object.
     */
    it('Telephony_Connection_Connection_getNetCapabilities_0300', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getNetCapabilities_0300 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      connection.getNetCapabilities().then(data => {
        expect().assertFail();
      }).catch(err => {
        expect(true).assertTrue();
      });
      console.log("************* Telephony_Connection_Connection_getNetCapabilities_0300 Test end*************");
      done();
    });

    /*
     * @tc.number    : Telephony_Connection_Connection_getNetCapabilities_0400
     * @tc.name      : getNetCapabilities
     * @tc.desc      : Obtains {@link NetCapabilities} of a {@link NetHandle} object.
     */
    it('Telephony_Connection_Connection_getNetCapabilities_0400', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getNetCapabilities_0400 Test start*************");
      let caseName = 'Telephony_Connection_Connection_getNetCapabilities_0400';
      connection.getAllNets((error, value) => {
        if (error) {
          console.log(`${caseName}  get data fail: ${error}`);
          expect().assertFail();
          done();
          return;
        } else {
          console.log(`getAllNets netID value : ${JSON.stringify(value[0].netId)}`);
          connection.getNetCapabilities(value[0], (error, data) => {
            if (error) {
              console.log(`${caseName}  get ID fail: ${error}`);
              expect().assertFail();
              done();
            } else {
              console.log(`${caseName}  get value success: ${JSON.stringify(data)}`);
              console.log(`${caseName}  get value success123456: ${JSON.stringify(data.netCaps_)}`);
              console.log(`${caseName}  get value success122222: ${JSON.stringify(data.netCaps_[0].netCap)}`);
              expect(data.netCaps_[0].netCap != "").assertTrue();
              done();
            }
          });
        }
      });
      console.log("************* Telephony_Connection_Connection_getNetCapabilities_0400 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_hasDefaultNet_0100
         * @tc.name      : hasDefaultNet
         * @tc.desc      : Checks whether the default data network is activated.
         */
    it('Telephony_Connection_Connection_hasDefaultNet_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_hasDefaultNet_0100 Test start*************");
      connection.hasDefaultNet((error, data) => {
        if (error) {
          expect(true).assertTrue();
          done();
          return;
        }
        expect().assertFail();
        done();
      });
      console.log("************* Telephony_Connection_Connection_hasDefaultNet_0100 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_hasDefaultNet_0200
         * @tc.name      : hasDefaultNet
         * @tc.desc      : Checks whether the default data network is activated.
         */
    it('Telephony_Connection_Connection_hasDefaultNet_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_hasDefaultNet_0200 Test start*************");
      connection.hasDefaultNet().then(activated => {
        expect().assertFail();
      }).catch(error => {
        console.log(`Telephony_Connection_Connection_hasDefaultNet_0200 hasDefaultNet error,case success,error:${
        toString(error)}`);
        expect(true).assertTrue();
      })
      console.log("************* Telephony_Connection_Connection_hasDefaultNet_0200 Test end*************");
      done();
    });


    /*
         * @tc.number    : Telephony_Connection_Connection_enableAirplaneMode_0100
         * @tc.name      : enableAirplaneMode
         * @tc.desc      : Enables the airplane mode for a device.
         */
    it('Telephony_Connection_Connection_enableAirplaneMode_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_enableAirplaneMode_0100 Test start*************");
      connection.enableAirplaneMode((error, data) => {
        if (error) {
          expect(true).assertTrue();
          done();
          return;
        }
        expect().assertFail();
        done();
      });
      console.log("************* Telephony_Connection_Connection_enableAirplaneMode_0100 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_enableAirplaneMode_0200
         * @tc.name      : enableAirplaneMode
         * @tc.desc      : Enables the airplane mode for a device.
         */
    it('Telephony_Connection_Connection_enableAirplaneMode_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_enableAirplaneMode_0200 Test start*************");
      connection.enableAirplaneMode().then(data => {
        expect(true).assertTrue();
      }).catch(err => {
        expect().assertFail();
      });
      console.log("************* Telephony_Connection_Connection_enableAirplaneMode_0200 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_disableAirplaneMode_0100
         * @tc.name      : disableAirplaneMode
         * @tc.desc      : Disables the airplane mode for a device.
         */
    it('Telephony_Connection_Connection_disableAirplaneMode_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_disableAirplaneMode_0100 Test start*************");
      connection.disableAirplaneMode((error, data) => {
        if (error) {
          expect(true).assertTrue();
          done();
          return;
        }
        expect().assertFail();
        done();
      });
      console.log("************* Telephony_Connection_Connection_disableAirplaneMode_0100 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_disableAirplaneMode_0200
         * @tc.name      : disableAirplaneMode
         * @tc.desc      : Disables the airplane mode for a device.
         */
    it('Telephony_Connection_Connection_disableAirplaneMode_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_reportNetConnected_0200 Test start*************");
      connection.disableAirplaneMode().then(data => {
        expect(true).assertTrue();
      }).catch(err => {
        expect().assertFail();
      });
      console.log("************* Telephony_Connection_Connection_disableAirplaneMode_0200 Test end*************");
      done();
    });


    /*
         * @tc.number    : Telephony_Connection_Connection_reportNetConnected_0100
         * @tc.name      : reportNetConnected
         * @tc.desc      : Reports the network state is connected.
         */
    it('Telephony_Connection_Connection_reportNetConnected_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_reportNetConnected_0100 Test start*************");
      connection.reportNetConnected((error, data) => {
        if (error) {
          expect(true).assertTrue();
          done();
          return;
        }
        expect().assertFail();
        done();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_reportNetConnected_0100 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_reportNetConnected_0200
         * @tc.name      : reportNetConnected
         * @tc.desc      : Reports the network state is connected.
         */
    it('Telephony_Connection_Connection_reportNetConnected_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_reportNetConnected_0200 Test start*************");
      var handle = new connection.NetHandle(NETID_IVVALID);
      connection.reportNetConnected(handle, (error, data) => {
        if (error) {
          expect(true).assertTrue();
          done();
          return;
        }
        expect().assertFail();
        done();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_reportNetConnected_0200 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_reportNetConnected_0300
         * @tc.name      : reportNetConnected
         * @tc.desc      : Reports the network state is connected.
         */
    it('Telephony_Connection_Connection_reportNetConnected_0300', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_reportNetConnected_0300 Test start*************");
      var handle = new connection.NetHandle(NETID_IVVALID2);
      connection.reportNetConnected(handle).then(() => {
        expect().assertFail();
      }).catch(err => {
        expect(true).assertTrue();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_reportNetConnected_0300 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_reportNetConnected_0400
         * @tc.name      : reportNetConnected
         * @tc.desc      : Reports the network state is connected.
         */
    it('Telephony_Connection_Connection_reportNetConnected_0400', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_reportNetConnected_0400 Test start*************");
      var handle = new connection.NetHandle(NETID_IVVALID);
      connection.reportNetConnected(handle).then(() => {
        expect().assertFail();
      }).catch(err => {
        expect(true).assertTrue();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_reportNetConnected_0400 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_reportNetDisconnected_0100
         * @tc.name      : reportNetDisconnected
         * @tc.desc      : Reports the network state is disconnected.
         */
    it('Telephony_Connection_Connection_reportNetDisconnected_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_reportNetDisconnected_0100 Test start*************");
      connection.reportNetDisconnected((error, data) => {
        if (error) {
          expect(true).assertTrue();
          done();
          return;
        }
        expect().assertFail();
        done();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_reportNetDisconnected_0100 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_reportNetDisconnected_0200
         * @tc.name      : reportNetDisconnected
         * @tc.desc      : Reports the network state is disconnected.
         */
    it('Telephony_Connection_Connection_reportNetDisconnected_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_reportNetDisconnected_0200 Test start*************");
      var handle = new connection.NetHandle(NETID_IVVALID);
      connection.reportNetDisconnected(handle, (error, data) => {
        if (error) {
          expect(true).assertTrue();
          done();
          return;
        }
        expect().assertFail();
        done();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_reportNetDisconnected_0200 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_reportNetDisconnected_0300
         * @tc.name      : reportNetDisconnected
         * @tc.desc      : Reports the network state is disconnected.
         */
    it('Telephony_Connection_Connection_reportNetDisconnected_0300', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_reportNetDisconnected_0300 Test start*************");
      var handle = new connection.NetHandle(NETID_IVVALID2);
      connection.reportNetDisconnected(handle).then(() => {
        expect().assertFail();
      }).catch(err => {
        expect(true).assertTrue();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_reportNetDisconnected_0300 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_reportNetDisconnected_0400
         * @tc.name      : reportNetDisconnected
         * @tc.desc      : Reports the network state is disconnected.
         */
    it('Telephony_Connection_Connection_reportNetDisconnected_0400', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_reportNetDisconnected_0400 Test start*************");
      var handle = new connection.NetHandle(NETID_IVVALID);
      connection.reportNetDisconnected(handle).then(() => {
        expect().assertFail();
      }).catch(err => {
        expect(true).assertTrue();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_reportNetDisconnected_0400 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_getAddressesByName_0100
         * @tc.name      : getAddressesByName
         * @tc.desc      : Resolves the host name to obtain all IP addresses based on the default data network.
         */
    it('Telephony_Connection_Connection_getAddressesByName_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getAddressesByName_0100 Test start*************");
      connection.getAddressesByName('www.test.com', (error, data) => {
        if (error) {
          expect(true).assertTrue();
          done();
          return;
        }
        expect().assertFail();
        done();
      });
      expect(true).assertTrue();
      console.log("************* Telephony_Connection_Connection_getAddressesByName_0100 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_getAddressesByName_0200
         * @tc.name      : getAddressesByName
         * @tc.desc      : Resolves the host name to obtain all IP addresses based on the default data network.
         */
    it('Telephony_Connection_Connection_getAddressesByName_0200', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getAddressesByName_0200 Test start*************");
      connection.getAddressesByName('www.test.com').then(data => {
        expect().assertFail();
        done();
        return;
      }).catch(error => {
        console.log(`Telephony_Connection_Connection_getAddressesByName_0200 getAddressesByName error,case success,error:${
        toString(error)}`);
        expect(true).assertTrue();
      })
      console.log("************* Telephony_Connection_Connection_getAddressesByName_0200 Test end*************");
      done();
    });

    /**
     * @tc.number  Telephony_Connection_Connection_netAvailable_0100
     * @tc.name    netAvailable
     * @tc.desc    Function test
     */
    it('Telephony_Connection_Connection_netAvailable_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_netAvailable_0100 Test start*************");
      try {
        let netConn = connection.createNetConnection();
        netConn.on('netAvailable', (error, value) => {
          if (error) {
            console.log(`netAvailable error: ${error}`);
          }
        });
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_netAvailable_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_netBlockStatusChange_0100
     * @tc.name    netBlockStatusChange
     * @tc.desc    Function test
     */
    it('Telephony_Connection_Connection_netBlockStatusChange_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_netBlockStatusChange_0100 Test start*************");
      try {
        let netConn = connection.createNetConnection();
        netConn.on('netBlockStatusChange', (error, value) => {
          if (error) {
            console.log(`netBlockStatusChange error: ${error}`);
          }
        });
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_netBlockStatusChange_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_netCapabilitiesChange_0100
     * @tc.name    netCapabilitiesChange
     * @tc.desc    Function test
     */
    it('Telephony_Connection_Connection_netCapabilitiesChange_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_netCapabilitiesChange_0100 Test start*************");
      try {
        let netConn = connection.createNetConnection();
        netConn.on('netCapabilitiesChange', (error, value) => {
          if (error) {
            console.log(`netCapabilitiesChange error: ${error}`);
          }
        });
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_netCapabilitiesChange_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_netConnectionPropertiesChange_0100
     * @tc.name    netConnectionPropertiesChange
     * @tc.desc    Function test
     */
    it('Telephony_Connection_Connection_netConnectionPropertiesChange_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_netConnectionPropertiesChange_0100 Test start*************");
      try {
        let netConn = connection.createNetConnection();
        netConn.on('netConnectionPropertiesChange', (error, value) => {
          if (error) {
            console.log(`netConnectionPropertiesChange error: ${error}`);
          }
        });
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_netConnectionPropertiesChange_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_netLost_0100
     * @tc.name    netLost
     * @tc.desc    Function test
     */
    it('Telephony_Connection_Connection_netLost_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_netLost_0100 Test start*************");
      try {
        let netConn = connection.createNetConnection();
        netConn.on('netLost', (error, value) => {
          if (error) {
            console.log(`netLost error: ${error}`);
          }
        });
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_netLost_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_netUnavailable_0100
     * @tc.name    netUnavailable
     * @tc.desc    Function test
     */
    it('Telephony_Connection_Connection_netUnavailable_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_netUnavailable_0100 Test start*************");
      try {
        let netConn = connection.createNetConnection();
        netConn.on('netUnavailable', (error, value) => {
          if (error) {
            console.log(`netUnavailable error: ${error}`);
          }
        });
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_netUnavailable_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_NetSpecifier_0100
     * @tc.name    NetSpecifier
     * @tc.desc    netCapabilities bearerPrivateIdentifier test
     */
    it('Telephony_Connection_Connection_NetSpecifier_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_NetSpecifier_0100 Test start*************");
      try {
        let netSpecifier = {
          netCapabilities: {
            linkUpBandwidthKbps: 1,
            linkDownBandwidthKbps: 2,
            bearerTypes: [0],
            networkCap: [11],
          }, bearerPrivateIdentifier: '123'
        }
        connection.createNetConnection(netSpecifier, TIMEOUT);
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_NetSpecifier_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_bindSocket_0100
     * @tc.name    bindSocket
     * @tc.desc    Function test
     */
    it('Telephony_Connection_Connection_bindSocket_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_bindSocket_0100 Test start*************");
      connection.getDefaultNet().then((handle) => {
        handle.bindSocket(1, (error, value) => {
          if (error) {
            expect().assertFail();
            done();
          }
        });
      }).catch((error) => {
        expect(true).assertTrue();
      });
      done();
      console.log("************* Telephony_Connection_Connection_bindSocket_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_NetHandle_getAddressesByName_0100
     * @tc.name    getAddressesByName
     * @tc.desc    Function test
     */
    it('Telephony_Connection_Connection_NetHandle_getAddressesByName_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_NetHandle_getAddressesByName_0100 Test start*************");
      connection.getDefaultNet().then((handle) => {
        handle.getAddressesByName(addressBaidu, (error, value) => {
          if (error) {
            expect().assertFail();
            done();
          }
        });
      }).catch((error) => {
        expect(true).assertTrue();
      });
      done();
      console.log("************* Telephony_Connection_Connection_NetHandle_getAddressesByName_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_getAddressByName_0100
     * @tc.name    getAddressByName
     * @tc.desc    Function test
     */
    it('Telephony_Connection_Connection_getAddressByName_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_getAddressByName_0100 Test start*************");
      connection.getDefaultNet().then((handle) => {
        handle.getAddressByName(addressBaidu, (error, value) => {
          if (error) {
            expect().assertFail();
          }
          done();
        });
      }).catch((error) => {
        expect(true).assertTrue();
      });
      done();
      console.log("************* Telephony_Connection_Connection_getAddressByName_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_linkUpBandwidthKbps_0100
     * @tc.name    linkUpBandwidthKbps
     * @tc.desc    linkUpBandwidthKbps test
     */
    it('Telephony_Connection_Connection_linkUpBandwidthKbps_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_linkUpBandwidthKbps_0100 Test start*************");
      try {
        let netSpecifier = {
          netCapabilities: {
            linkUpBandwidthKbps: 10,
            linkDownBandwidthKbps: 22,
            bearerTypes: [0],
            networkCap: [11],
          }, bearerPrivateIdentifier: '456'
        }
        connection.createNetConnection(netSpecifier, TIMEOUT);
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_linkUpBandwidthKbps_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_linkDownBandwidthKbps_0100
     * @tc.name    linkDownBandwidthKbps
     * @tc.desc    nlinkDownBandwidthKbps test
     */
    it('Telephony_Connection_Connection_linkDownBandwidthKbps_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_linkDownBandwidthKbps_0100 Test start*************");
      try {
        let netSpecifier = {
          netCapabilities: {
            linkUpBandwidthKbps: 44,
            linkDownBandwidthKbps: 55,
            bearerTypes: [1],
            networkCap: [12],
          }, bearerPrivateIdentifier: '789'
        }
        connection.createNetConnection(netSpecifier, TIMEOUT);
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_linkDownBandwidthKbps_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_networkCap_0100
     * @tc.name    networkCap
     * @tc.desc    networkCap test
     */
    it('Telephony_Connection_Connection_networkCap_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_networkCap_0100 Test start*************");
      try {
        let netSpecifier = {
          netCapabilities: {
            linkUpBandwidthKbps: 18,
            linkDownBandwidthKbps: 29,
            bearerTypes: [3],
            networkCap: [5],
          }, bearerPrivateIdentifier: '321'
        }
        connection.createNetConnection(netSpecifier, TIMEOUT);
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_networkCap_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_bearerTypes_0100
     * @tc.name    bearerTypes
     * @tc.desc    bearerTypes test
     */
    it('Telephony_Connection_Connection_bearerTypes_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_bearerTypes_0100 Test start*************");
      try {
        let netSpecifier = {
          netCapabilities: {
            linkUpBandwidthKbps: 41,
            linkDownBandwidthKbps: 62,
            bearerTypes: [1],
            networkCap: [9],
          }, bearerPrivateIdentifier: '876'
        }
        connection.createNetConnection(netSpecifier, TIMEOUT);
      } catch (error) {
        expect().assertFail();
      }
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_bearerTypes_0100 Test end*************");
    })

    /**
     * @tc.number  Telephony_Connection_Connection_register_0100
     * @tc.name    register
     * @tc.desc    Receives status change notifications of a specified network.
     */
    it('Telephony_Connection_Connection_register_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_register_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_Connection_Connection_register_0100';
      let netSpecifier = {
        netCapabilities: {
          linkUpBandwidthKbps: 0,
          linkDownBandwidthKbps: 0,
          bearerTypes: [0],
          networkCap: [0],
        }, bearerPrivateIdentifier: '123'
      }
      let netConn = connection.createNetConnection(netSpecifier, TIMEOUT);
      netConn.register((error) => {
        if (error) {
          console.log(`${caseName} register fail: ${error}`);
        }
      });
      await sleep(DELAY);
      netConn.unregister((error) => {
        if (error) {
          console.log(`${caseName} unregister fail: ${error}`);
        }
      });
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_register_0100 Test end*************");
    });

    /**
     * @tc.number  Telephony_Connection_Connection_unregister_0100
     * @tc.name    unregister
     * @tc.desc    Cancels listening for network status changes. test
     */
    it('Telephony_Connection_Connection_unregister_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_unregister_0100 Test start*************");
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_Connection_Connection_unregister_0100';
      let netSpecifier = {
        netCapabilities: {
          linkUpBandwidthKbps: 10,
          linkDownBandwidthKbps: 11,
          bearerTypes: [0],
          networkCap: [0],
        }, bearerPrivateIdentifier: '123'
      }
      let netConn = connection.createNetConnection(netSpecifier, TIMEOUT);
      netConn.register((error) => {
        if (error) {
          console.log(`${caseName} register fail: ${error}`);
        }
      });
      await sleep(DELAY);
      netConn.unregister((error) => {
        if (error) {
          console.log(`${caseName} unregister fail: ${error}`);
        }
      });
      expect(true).assertTrue();
      done();
      console.log("************* Telephony_Connection_Connection_unregister_0100 Test end*************");
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_netId_0100
         * @tc.name      : netId
         * @tc.desc      : netId test.
         */
    it('Telephony_Connection_Connection_netId_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_netId_0100 Test start*************");
      connection.getDefaultNet((error, data) => {
        if (error) {
          expect(true).assertTrue();
        } else {
          expect(data.netId).assertEqual(100);
        }
      });
      console.log("************* Telephony_Connection_Connection_netId_0100 Test end*************");
      done();
    });

    /*
         * @tc.number    : Telephony_Connection_Connection_domains_0100
         * @tc.name      : domains
         * @tc.desc      : domains test.
         */
    it('Telephony_Connection_Connection_domains_0100', 0, async function (done) {
      console.log("************* Telephony_Connection_Connection_domains_0100 Test start*************");
      var handle = new connection.NetHandle(NETID_IVVALID);
      connection.getConnectionProperties(handle, (error, data) => {
        if (error) {
          expect(true).assertTrue();
        } else {
          expect(data.domains).assertEqual("");
        }
      });
      console.log("************* Telephony_Connection_Connection_domains_0100 Test end*************");
      done();
    });

    console.log("************* connection Test end*************");
  })
}
