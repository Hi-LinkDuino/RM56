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
import statistics from '@ohos.net.statistics';
import utils from './Utils.ets'
export default function statisticsUiJsunit() {
  describe('appInfoTest', function () {
    const DATA_0 = 0;

    /**
     * @tc.number  Telephony_statistics_getCellularRxBytes_Async_0100
     * @tc.name    The APP calls getCellularRxBytes() and gets traffic successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getCellularRxBytes_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getCellularRxBytes_Async_0100';
      console.log(`${caseName} done`);
      statistics.getCellularRxBytes((err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getCellularRxBytes_Promise_0100
     * @tc.name    The APP calls getCellularRxBytes() and gets traffic successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getCellularRxBytes_Promise_0100', 0, async function (done) {
      let caseName = 'Telephony_statistics_getCellularRxBytes_Promise_0100';
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      statistics.getCellularRxBytes().then((data) => {
        console.log(`${caseName} get data = ${data.message}:${ data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        expect(data.rxBytes >= DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getCellularTxBytes_Async_0100
     * @tc.name    The APP calls getCellularTxBytes() and gets traffic successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getCellularTxBytes_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getCellularTxBytes_Async_0100';
      statistics.getCellularTxBytes((err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        expect(data.txBytes >= DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getCellularTxBytes_Promise_0100
     * @tc.name    The APP calls getCellularTxBytes() and gets traffic successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getCellularTxBytes_Promise_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getCellularTxBytes_Promise_0100';
      statistics.getCellularTxBytes().then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getUidRxBytes_Async_0100
     * @tc.name    The APP calls getUidRxBytes() and gets traffic successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidRxBytes_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidRxBytess_Async_0100';
      statistics.getUidRxBytes(1,(err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.message == 'successful').assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getUidRxBytes_Promise_0100
     * @tc.name    The APP calls getUidRxBytes() and gets traffic successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidRxBytes_Promise_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidRxBytes_Promise_0100';
      statistics.getUidRxBytes(1).then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.message == 'successful').assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getUidTxBytes_Async_0100
     * @tc.name    The APP calls getUidTxBytes and gets traffic successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidTxBytes_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidTxBytes_Async_0100';
      statistics.getUidTxBytes(1,(err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.message == 'successful').assertTrue();
        done();
      });
    });
    /**
     * @tc.number  Telephony_statistics_getUidTxBytess_Promise_0100
     * @tc.name    The APP calls getUidTxBytes() and gets traffic successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidTxBytes_Promise_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidTxBytes_Promise_0100';
      statistics.getUidTxBytes(1).then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.message == 'successful').assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });


    /**
     * @tc.number  Telephony_statistics_getIfaceRxBytes_Async_0100
     * @tc.name    Set the network interface to cellular,
     *             call getIfaceRxBytes() and get the traffic value successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceRxBytes_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceRxBytes_Async_0100';
      let ifaceName = "usb0";
      statistics.getIfaceRxBytes(ifaceName, (err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        expect(data.rxPackets >= DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceRxBytes_Async_0200
     * @tc.name    Set network interface to WIFI network interface,
     *             call getIfaceRxBytes(), successfully obtain traffic value (>=DATA_0)
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceRxBytes_Async_0200', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceRxBytes_Async_0200';
      let ifaceName = "wlan0";
      statistics.getIfaceRxBytes(ifaceName, (err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        expect(data.txPackets >= DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceRxBytes_Async_0300
     * @tc.name    Set the network interface to an Ethernet interface,
     *             call getIfaceRxBytes(), and get the traffic value successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceRxBytes_Async_0300', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceRxBytes_Async_0300';
      let ifaceName = "eth0";
      statistics.getIfaceRxBytes(ifaceName, (err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceRxBytes_Async_0400
     * @tc.name    Set the network interface to a nonexistent one, call getIfaceRxBytes(),
     *             and get a traffic value of DATA_0
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceRxBytes_Async_0400', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceRxBytes_Async_0400';
      let ifaceName = "xlan0";
      statistics.getIfaceRxBytes(ifaceName, (err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code == DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceRxBytes_Promise_0100
     * @tc.name    Set the network interface to cellular,
     *             call getIfaceRxBytes() and get the traffic value successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceRxBytes_Promise_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceRxBytes_Promise_0100';
      let ifaceName = "usb0";
      statistics.getIfaceRxBytes(ifaceName).then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceRxBytes_Promise_0200
     * @tc.name    Set network interface to WIFI network interface,
     *             call getIfaceRxBytes(), successfully obtain traffic value (>=DATA_0)
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceRxBytes_Promise_0200', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceRxBytes_Promise_0200';
      let ifaceName = "wlan0";
      statistics.getIfaceRxBytes(ifaceName).then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceRxBytes_Promise_0300
     * @tc.name    Set the network interface to an Ethernet interface,
     *             call getIfaceRxBytes(), and get the traffic value successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceRxBytes_Promise_0300', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceRxBytes_Promise_0300';
      let ifaceName = "eth0";
      statistics.getIfaceRxBytes(ifaceName).then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceRxBytes_Promise_0400
     * @tc.name    Set the network interface to a nonexistent one, call getIfaceRxBytes(),
     *             and get a traffic value of DATA_0
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceRxBytes_Promise_0400', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceRxBytes_Promise_0400';
      let ifaceName = "xlan0";
      statistics.getIfaceRxBytes(ifaceName).then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code == DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceTxBytes_Async_0100
     * @tc.name    Set the network interface to cellular,
     *             call getIfaceTxBytes() and get the traffic value successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceTxBytes_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceTxBytes_Async_0100';
      let ifaceName = "usb0";
      statistics.getIfaceTxBytes(ifaceName, (err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceTxBytes_Async_0200
     * @tc.name    Set network interface to WIFI network interface,
     *             call getIfaceTxBytes(), successfully obtain traffic value (>=DATA_0)
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceTxBytes_Async_0200', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceTxBytes_Async_0200';
      let ifaceName = "wlan0";
      statistics.getIfaceTxBytes(ifaceName, (err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceTxBytes_Async_0300
     * @tc.name    Set the network interface to an Ethernet interface,
     *             call getIfaceTxBytes(), and get the traffic value successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceTxBytes_Async_0300', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceTxBytes_Async_0300';
      let ifaceName = "eth0";
      statistics.getIfaceTxBytes(ifaceName, (err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceTxBytes_Async_0400
     * @tc.name    Set the network interface to a nonexistent one, call getIfaceTxBytes(),
     *             and get a traffic value of DATA_0
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceTxBytes_Async_0400', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceTxBytes_Async_0400';
      let ifaceName = "xlan0";
      statistics.getIfaceTxBytes(ifaceName, (err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code == DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceTxBytes_Promise_0100
     * @tc.name    Set the network interface to cellular,
     *             call getIfaceTxBytes() and get the traffic value successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceTxBytes_Promise_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceTxBytes_Promise_0100';
      let ifaceName = "usb0";
      statistics.getIfaceTxBytes(ifaceName).then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceTxBytes_Promise_0200
     * @tc.name    Set network interface to WIFI network interface,
     *             call getIfaceTxBytes(), successfully obtain traffic value (>=DATA_0)
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceTxBytes_Promise_0200', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceTxBytes_Promise_0200';
      let ifaceName = "wlan0";
      statistics.getIfaceTxBytes(ifaceName).then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceTxBytes_Promise_0300
     * @tc.name    Set the network interface to an Ethernet interface,
     *             call getIfaceTxBytes(), and get the traffic value successfully (>=DATA_0).
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceTxBytes_Promise_0300', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceTxBytes_Promise_0300';
      let ifaceName = "eth0";
      statistics.getIfaceTxBytes(ifaceName).then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceTxBytes_Promise_0400
     * @tc.name    Set the network interface to a nonexistent one, call getIfaceTxBytes(),
     *             and get a traffic value of DATA_0
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceTxBytes_Promise_0400', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceTxBytes_Promise_0400';
      let ifaceName = "xlan0";
      statistics.getIfaceTxBytes(ifaceName).then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code == DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getAllRxBytes_Async_0100
     * @tc.name    Call getAllRxBytes() and get traffic successfully (>=DATA_0)
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getAllRxBytes_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getAllRxBytes_Async_0100';
      statistics.getAllRxBytes((err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();
        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getAllRxBytes_Promise_0100
     * @tc.name    Call getAllRxBytes() and get traffic successfully (>=DATA_0)
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getAllRxBytes_Promise_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getAllRxBytes_Promise_0100';
      statistics.getAllRxBytes().then((data) => {
        console.log(`${caseName} get data = ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get erro = ` + err);
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getAllRxBytes_Async_0100
     * @tc.name    Call getAllTxBytes() and get traffic successfully (>=DATA_0)
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getAllTxBytes_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getAllTxBytes_Async_0100';
      statistics.getAllTxBytes((err, data) => {
        if (err) {
          console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
          done();

        }
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getAllTxBytes_Promise_0100
     * @tc.name    Call getAllTxBytes() and get traffic successfully (>=DATA_0)
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getAllTxBytes_Promise_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getAllTxBytes_Promise_0100';
      statistics.getAllTxBytes().then((data) => {
        console.log(`${caseName}  get data: ${data.message} + ${data.code}`);
        expect(data.code >= DATA_0).assertTrue();
        done();
      }).catch((err) => {
        console.log(`${caseName} get data fail: ${err.message} ${err.code}`);
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_statistics_on_0100
     * @tc.name    Test on() to check the callback result
     * @tc.desc    Function test
     */
    it('Telephony_statistics_statistics_on_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_statistics_on_0100';
      statistics.on('netStatsChange', (data) => {
        console.log(`${caseName} on data: ${JSON.stringify(data)}`);
        expect(data).assertUndefined();
        done();
      });
      done();
    });

    /**
     * @tc.number  Telephony_statistics_statistics_off_0100
     * @tc.name    Test off() to check the callback result
     * @tc.desc    Function test
     */
    it('Telephony_statistics_statistics_off_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_statistics_off_0100';
      statistics.off('netStatsChange', (data) => {
        console.log(`${caseName} off data: ${JSON.stringify(data)}`);
        expect(data).assertUndefined();
        done();
      });
      done();
    });
    /**
     * @tc.number  Telephony_statistics_getIfaceStatsDetail_Async_0100
     * @tc.name    Test getIfaceStatsDetail() to check the callback result when iface is test
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceStatsDetail_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceStatsDetail_Async_0100';
      let ifaceInfo = {
        "iface": "test",
        "startTime": Date.now() - 60 * 60,
        "endTime": Date.now()
      };
      statistics.getIfaceStatsDetail(ifaceInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getIfaceStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getIfaceStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceStatsDetail_Async_0200
     * @tc.name    Test getIfaceStatsDetail() to check the callback result when endTime more than the statTime
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceStatsDetail_Async_0200', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceStatsDetail_Async_0200';
      let ifaceInfo = {
        "iface": "eth0",
        "startTime": Date.now(),
        "endTime": Date.now() - 60 * 60
      };
      statistics.getIfaceStatsDetail(ifaceInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getIfaceStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getIfaceStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceStatsDetail_Async_0500
     * @tc.name    Test getIfaceStatsDetail() to check the callback result when endTime equal statTime
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceStatsDetail_Async_0500', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceStatsDetail_Async_0500';
      let time = Date.now();
      let ifaceInfo = {
        "iface": "eth0",
        "startTime": time,
        "endTime": time
      };
      statistics.getIfaceStatsDetail(ifaceInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getIfaceStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getIfaceStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceStatsDetail_Async_0600
     * @tc.name    Test getIfaceStatsDetail() to check the callback result when endTime and statTime before system time
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceStatsDetail_Async_0600', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceStatsDetail_Async_0600';
      let ifaceInfo = {
        "iface": "eth0",
        "startTime": 1,
        "endTime": 2
      };
      statistics.getIfaceStatsDetail(ifaceInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getIfaceStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getIfaceStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceStatsDetail_Async_0700
     * @tc.name    Test getIfaceStatsDetail() to check the callback result when endTime and statTime after system time
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceStatsDetail_Async_0700', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceStatsDetail_Async_0700';
      let ifaceInfo = {
        "iface": "eth0",
        "startTime": 1981080000,
        "endTime": 1981083600
      };
      statistics.getIfaceStatsDetail(ifaceInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getIfaceStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getIfaceStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceStatsDetail_Promise_0100
     * @tc.name    Test getIfaceStatsDetail() to check the callback result when iface is test
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceStatsDetail_Promise_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceStatsDetail_Promise_0100';
      let ifaceInfo = {
        "iface": "test",
        "startTime": Date.now() - 60 * 60,
        "endTime": Date.now()
      };
      statistics.getIfaceStatsDetail(ifaceInfo).then(data => {
        console.log(`${caseName} getIfaceStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getIfaceStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceStatsDetail_Promise_0200
     * @tc.name    Test getIfaceStatsDetail() to check the callback result when endTime more than the statTime
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceStatsDetail_Promise_0200', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceStatsDetail_Promise_0200';
      let ifaceInfo = {
        "iface": "eth0",
        "startTime": Date.now(),
        "endTime": Date.now() - 60 * 60
      };
      statistics.getIfaceStatsDetail(ifaceInfo).then(data => {
        console.log(`${caseName} getIfaceStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getIfaceStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceStatsDetail_Promise_0500
     * @tc.name    Test getIfaceStatsDetail() to check the callback result when endTime equal statTime
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceStatsDetail_Promise_0500', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceStatsDetail_Promise_0500';
      let time = Date.now();
      let ifaceInfo = {
        "iface": "eth0",
        "startTime": time,
        "endTime": time
      };
      statistics.getIfaceStatsDetail(ifaceInfo).then(data => {
        console.log(`${caseName} getIfaceStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getIfaceStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceStatsDetail_Promise_0600
     * @tc.name    Test getIfaceStatsDetail() to check the callback result when endTime and statTime before system time
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceStatsDetail_Promise_0600', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceStatsDetail_Promise_0600';
      let ifaceInfo = {
        "iface": "eth0",
        "startTime": 1,
        "endTime": 2
      };
      statistics.getIfaceStatsDetail(ifaceInfo).then(data => {
        console.log(`${caseName} getIfaceStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getIfaceStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });

    /**
     * @tc.number  Telephony_statistics_getIfaceStatsDetail_Promise_0700
     * @tc.name    Test getIfaceStatsDetail() to check the callback result when endTime and statTime after system time
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getIfaceStatsDetail_Promise_0700', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getIfaceStatsDetail_Promise_0700';
      let ifaceInfo = {
        "iface": "eth0",
        "startTime": 1981080000,
        "endTime": 1981083600
      };
      statistics.getIfaceStatsDetail(ifaceInfo).then(data => {
        console.log(`${caseName} getIfaceStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getIfaceStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Async_0100
     * @tc.name    Test getUidStatsDetail() to check the callback result when iface is test
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Async_0100';
      let uidInfo = {
        "ifaceInfo": {
          "iface": "test",
          "startTime": Date.now() - 60 * 60,
          "endTime": Date.now()
        },
        "uid": 1001
      };
      statistics.getUidStatsDetail(uidInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Async_0200
     * @tc.name    Test getUidStatsDetail() to check the callback result when endTime more than the statTime
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Async_0200', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Async_0200';
      let uidInfo = {
        "ifaceInfo": {
          "iface": "eth0",
          "startTime": Date.now(),
          "endTime": Date.now() - 60 * 60
        },
        "uid": 1001
      };
      statistics.getUidStatsDetail(uidInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Async_0500
     * @tc.name    Test getUidStatsDetail() to check the callback result when endTime equal statTime
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Async_0500', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Async_0500';
      let time = Date.now();
      let uidInfo = {
        "ifaceInfo": {
          "iface": "eth0",
          "startTime": time,
          "endTime": time
        },
        "uid": 1001
      };
      statistics.getUidStatsDetail(uidInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Async_0600
     * @tc.name    Test getUidStatsDetail() to check the callback result when uid is 0
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Async_0600', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Async_0600';
      let uidInfo = {
        "ifaceInfo": {
          "iface": "eth0",
          "startTime": Date.now() - 60 * 60,
          "endTime": Date.now()
        },
        "uid": 0
      };
      statistics.getUidStatsDetail(uidInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Async_0700
     * @tc.name    Test getUidStatsDetail() to check the callback result when endTime and statTime before system time
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Async_0700', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Async_0700';
      let uidInfo = {
        "ifaceInfo": {
          "iface": "eth0",
          "startTime": 1,
          "endTime": 2
        },
        "uid": 1001
      };
      statistics.getUidStatsDetail(uidInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Async_0800
     * @tc.name    Test getUidStatsDetail() to check the callback result when endTime and statTime after system time
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Async_0800', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Async_0800';
      let uidInfo = {
        "ifaceInfo": {
          "iface": "eth0",
          "startTime": 1981080000,
          "endTime": 1981083600
        },
        "uid": 1001
      };
      statistics.getUidStatsDetail(uidInfo, (err, data) => {
        if (err) {
          console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
          done();
          return;
        }
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      });
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Promise_0100
     * @tc.name    Test getUidStatsDetail() to check the callback result when iface is test
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Promise_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Promise_0100';
      let uidInfo = {
        "ifaceInfo": {
          "iface": "test",
          "startTime": Date.now() - 60 * 60,
          "endTime": Date.now()
        },
        "uid": 1001
      };
      statistics.getUidStatsDetail(uidInfo).then(data => {
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Promise_0200
     * @tc.name    Test getUidStatsDetail() to check the callback result when endTime more than the statTime
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Promise_0200', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Promise_0200';
      let uidInfo = {
        "ifaceInfo": {
          "iface": "eth0",
          "startTime": Date.now(),
          "endTime": Date.now() - 60 * 60
        },
        "uid": 1001
      };
      statistics.getUidStatsDetail(uidInfo).then(data => {
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Promise_0500
     * @tc.name    Test getUidStatsDetail() to check the callback result when endTime equal statTime
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Promise_0500', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Promise_0500';
      let time = Date.now();
      let uidInfo = {
        "ifaceInfo": {
          "iface": "eth0",
          "startTime": time,
          "endTime": time
        },
        "uid": 1001
      };
      statistics.getUidStatsDetail(uidInfo).then(data => {
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Promise_0600
     * @tc.name    Test getUidStatsDetail() to check the callback result when uid is 0
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Promise_0600', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Promise_0600';
      let uidInfo = {
        "ifaceInfo": {
          "iface": "eth0",
          "startTime": Date.now() - 60 * 60,
          "endTime": Date.now()
        },
        "uid": 0
      };
      statistics.getUidStatsDetail(uidInfo).then(data => {
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Promise_0700
     * @tc.name    Test getUidStatsDetail() to check the callback result when endTime and statTime before system time
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Promise_0700', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Promise_0700';
      let uidInfo = {
        "ifaceInfo": {
          "iface": "eth0",
          "startTime": 1,
          "endTime": 2
        },
        "uid": 1001
      };
      statistics.getUidStatsDetail(uidInfo).then(data => {
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });

    /**
     * @tc.number  Telephony_statistics_getUidStatsDetail_Promise_0800
     * @tc.name    Test getUidStatsDetail() to check the callback result when endTime and statTime after system time
     * @tc.desc    Function test
     */
    it('Telephony_statistics_getUidStatsDetail_Promise_0800', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let caseName = 'Telephony_statistics_getUidStatsDetail_Promise_0800';
      let uidInfo = {
        "ifaceInfo": {
          "iface": "eth0",
          "startTime": 1981080000,
          "endTime": 1981083600
        },
        "uid": 1001
      };
      statistics.getUidStatsDetail(uidInfo).then(data => {
        console.log(`${caseName} getUidStatsDetail data: ${JSON.stringify(data)}`);
        expect().assertFail();
        done();
      }).catch(err => {
        console.log(`${caseName} getUidStatsDetail fail: ${JSON.stringify(err)}`);
        done();
      })
    });
  })
}
