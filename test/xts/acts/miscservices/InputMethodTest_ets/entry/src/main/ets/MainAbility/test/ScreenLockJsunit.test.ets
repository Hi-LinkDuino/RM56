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
import screenLock from '@ohos.screenLock';

export default function screenLockJsunit() {
  describe('screenLockTest', function () {
    console.log("************* screenLock Test start*************");

    /*
     * @tc.number    : MiscServices_ScreenLock_isScreenLocked_0100
     * @tc.name      : isScreenLocked
     * @tc.desc      : Checks whether the screen is currently locked.
     */
    it('MiscServices_ScreenLock_isScreenLocked_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_isScreenLocked_0100";
      try {
        screenLock.isScreenLocked((error, data) => {
          if (error) {
            console.error(caseName + 'Operation failed. Cause:' + JSON.stringify(error));
            return;
          }
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_isScreenLocked_0200
     * @tc.name      : isScreenLocked
     * @tc.desc      : Checks whether the screen is currently locked.
     */
    it('MiscServices_ScreenLock_isScreenLocked_0200', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_isScreenLocked_0200";
      try {
        screenLock.isScreenLocked().then((data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        }).catch((error) => {
          console.error(caseName + 'Operation failed. Cause: ' + JSON.stringify(error));
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_isSecureMode_0100
     * @tc.name      : isSecureMode
     * @tc.desc      : Checks whether the screen lock of the current device is secure.
     */
    it('MiscServices_ScreenLock_isSecureMode_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_isSecureMode_0100";
      try {
        screenLock.isSecureMode((error, data) => {
          if (error) {
            console.error(caseName + 'Operation failed. Cause:' + JSON.stringify(error));
            return;
          }
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_isSecureMode_0200
     * @tc.name      : isSecureMode
     * @tc.desc      : Checks whether the screen lock of the current device is secure.
     */
    it('MiscServices_ScreenLock_isSecureMode_0200', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_isSecureMode_0200";
      try {
        screenLock.isSecureMode().then((data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        }).catch((error) => {
          console.error(caseName + 'Operation failed. Cause: ' + JSON.stringify(error));
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_unlockScreen_0100
     * @tc.name      : unlockScreen
     * @tc.desc      : Unlocks the screen.
     */
    it('MiscServices_ScreenLock_unlockScreen_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_unlockScreen_0100"
      try {
        screenLock.unlockScreen((error, data) => {
          if (error) {
            console.error(caseName + 'Operation failed. Cause:' + JSON.stringify(error));
            return;
          }
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_unlockScreen_0200
     * @tc.name      : unlockScreen
     * @tc.desc      : Unlocks the screen.
     */
    it('MiscServices_ScreenLock_unlockScreen_0200', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_unlockScreen_0200";
      try {
        screenLock.unlockScreen().then((data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        }).catch((error) => {
          console.error(caseName + 'Operation failed. Cause: ' + JSON.stringify(error));
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_beginWakeUp_0100
     * @tc.name      : on
     * @tc.desc      : Receives beginWakeUp called.
     */
    it('MiscServices_ScreenLock_on_beginWakeUp_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_beginWakeUp_0100";
      try {
        screenLock.on('beginWakeUp', (data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_endWakeUp_0100
     * @tc.name      : on
     * @tc.desc      : Receives endWakeUp called.
     */
    it('MiscServices_ScreenLock_on_endWakeUp_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_endWakeUp_0100";
      try {
        screenLock.on('endWakeUp', (data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_beginScreenOn_0100
     * @tc.name      : on
     * @tc.desc      : Receives beginScreenOn called.
     */
    it('MiscServices_ScreenLock_on_beginScreenOn_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_beginScreenOn_0100";
      try {
        screenLock.on('beginScreenOn', (data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_endScreenOn_0100
     * @tc.name      : on
     * @tc.desc      : Receives endScreenOn called.
     */
    it('MiscServices_ScreenLock_on_endScreenOn_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_endScreenOn_0100";
      try {
        screenLock.on('endScreenOn', (data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_beginScreenOff_0100
     * @tc.name      : on
     * @tc.desc      : Receives beginScreenOff called.
     */
    it('MiscServices_ScreenLock_on_beginScreenOff_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_beginScreenOff_0100";
      try {
        screenLock.on('beginScreenOff', (data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_endScreenOff_0100
     * @tc.name      : on
     * @tc.desc      : Receives endScreenOff called.
     */
    it('MiscServices_ScreenLock_on_endScreenOff_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_endScreenOff_0100";
      try {
        screenLock.on('endScreenOff', (data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_unlockScreen_0100
     * @tc.name      : on
     * @tc.desc      : Receives unlockScreen called.
     */
    it('MiscServices_ScreenLock_on_unlockScreen_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_unlockScreen_0100";
      try {
        screenLock.on('unlockScreen', (data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done()
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_beginExitAnimation_0100
     * @tc.name      : on
     * @tc.desc      : Receives beginExitAnimation called.
     */
    it('MiscServices_ScreenLock_on_beginExitAnimation_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_beginExitAnimation_0100";
      try {
        screenLock.on('beginExitAnimation', (data) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(data));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_beginSleep_0100
     * @tc.name      : on
     * @tc.desc      : Receives beginSleep called.
     */
    it('MiscServices_ScreenLock_on_beginSleep_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_beginSleep_0100";
      try {
        screenLock.on('beginSleep', (number) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(number));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_endSleep_0100
     * @tc.name      : on
     * @tc.desc      : Receives endSleep called.
     */
    it('MiscServices_ScreenLock_on_endSleep_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_endSleep_0100";
      try {
        screenLock.on('endSleep', (number) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(number));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done()
    });

    /*
     * @tc.number    : MiscServices_ScreenLock_on_changeUser_0100
     * @tc.name      : on
     * @tc.desc      : Receives changeUser called.
     */
    it('MiscServices_ScreenLock_on_changeUser_0100', 0, async function (done) {
      let caseName = "MiscServices_ScreenLock_on_changeUser_0100";
      try {
        screenLock.on('changeUser', (number) => {
          console.info(caseName + 'Operation successful. Data: ' + JSON.stringify(number));
          expect(true).assertTrue();
          done();
        })
      } catch (err) {
        console.info(caseName + 'catch error: ' + err);
        expect(true).assertTrue();
        done();
      }
      done();
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_screenlockEnabled_0100
     * @tc.name    on_screenlockEnabled
     * @tc.desc    Receives screenlockEnabled change. This callback is invoked when screenlockEnabled is called by runtime
     */
    it('Miscservices_ScreenLock_ScreenLock_on_screenlockEnabled_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_on_screenlockEnabled_0100 start*************");
      try {
        screenLock.on('screenlockEnabled', (error, enable) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_on_screenlockEnabled_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_on_screenlockEnabled_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_beginWakeUp_0100
     * @tc.name    off_beginWakeUp
     * @tc.desc    Remove the receives of beginWakeUp
     */
    it('Miscservices_ScreenLock_ScreenLock_beginWakeUp_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_beginWakeUp_0100 start*************");
      try {
        screenLock.off('beginWakeUp', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_beginWakeUp_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_on_screenlockEnabled_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_endWakeUp_0100
     * @tc.name    off_endWakeUp
     * @tc.desc    Remove the receives of endWakeUp
     */
    it('Miscservices_ScreenLock_ScreenLock_endWakeUp_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_endWakeUp_0100 start*************");
      try {
        screenLock.off('endWakeUp', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_endWakeUp_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_on_screenlockEnabled_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_beginScreenOn_0100
     * @tc.name    off_beginScreenOn
     * @tc.desc    Remove the receives of beginScreenOn
     */
    it('Miscservices_ScreenLock_ScreenLock_beginScreenOn_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_beginScreenOn_0100 start*************");
      try {
        screenLock.off('beginScreenOn', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_beginScreenOn_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_beginScreenOn_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_endScreenOn_0100
     * @tc.name    off_endScreenOn
     * @tc.desc    Remove the receives of endScreenOn
     */
    it('Miscservices_ScreenLock_ScreenLock_endScreenOn_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_endScreenOn_0100 start*************");
      try {
        screenLock.off('endScreenOn', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_endScreenOn_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_endScreenOn_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_beginScreenOff_0100
     * @tc.name    off_beginScreenOff
     * @tc.desc    Remove the receives of beginScreenOff
     */
    it('Miscservices_ScreenLock_ScreenLock_beginScreenOff_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_beginScreenOff_0100 start*************");
      try {
        screenLock.off('beginScreenOff', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_beginScreenOff_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_beginScreenOff_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_unlockScreen_0100
     * @tc.name    off_unlockScreen
     * @tc.desc    Remove the receives of unlockScreen
     */
    it('Miscservices_ScreenLock_ScreenLock_unlockScreen_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_unlockScreen_0100 start*************");
      try {
        screenLock.off('unlockScreen', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_unlockScreen_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_unlockScreen_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_beginExitAnimation_0100
     * @tc.name    off_beginExitAnimation
     * @tc.desc    Remove the receives of beginExitAnimation
     */
    it('Miscservices_ScreenLock_ScreenLock_beginExitAnimation_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_beginExitAnimation_0100 start*************");
      try {
        screenLock.off('beginExitAnimation', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_beginExitAnimation_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_beginExitAnimation_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_screenlockEnabled_0100
     * @tc.name    off_screenlockEnabled
     * @tc.desc    Remove the receives of screenlockEnabled
     */
    it('Miscservices_ScreenLock_ScreenLock_screenlockEnabled_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_screenlockEnabled_0100 start*************");
      try {
        screenLock.off('screenlockEnabled', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_screenlockEnabled_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_screenlockEnabled_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_beginSleep_0100
     * @tc.name    off_beginSleep
     * @tc.desc    Remove the receives of beginSleep
     */
    it('Miscservices_ScreenLock_ScreenLock_beginSleep_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_beginSleep_0100 start*************");
      try {
        screenLock.off('beginSleep', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_beginSleep_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_beginSleep_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_endSleep_0100
     * @tc.name    off_endSleep
     * @tc.desc    Remove the receives of endSleep
     */
    it('Miscservices_ScreenLock_ScreenLock_endSleep_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_endSleep_0100 start*************");
      try {
        screenLock.off('endSleep', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_endSleep_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_endSleep_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_changeUser_0100
     * @tc.name    off_changeUser
     * @tc.desc    Remove the receives of changeUser
     */
    it('Miscservices_ScreenLock_ScreenLock_changeUser_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_changeUser_0100 start*************");
      try {
        screenLock.off('changeUser', (error) => {
          if (error) {
            expect().assertFail();
          } else {
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_changeUser_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_changeUser_0100 end*************");
    });

    /**
     * @tc.number  Miscservices_ScreenLock_ScreenLock_sendScreenLockEvent_0100
     * @tc.name    sendScreenLockEvent
     * @tc.desc    screenlockAPP send event to sendScreenLockEvent
     */
    it('Miscservices_ScreenLock_ScreenLock_sendScreenLockEvent_0100', 0, async function (done) {
      console.log("************* Miscservices_ScreenLock_ScreenLock_sendScreenLockEvent_0100 start*************");
      try {
        screenLock.sendScreenLockEvent('clock', 1000, (error, result) => {
          if (error) {
            expect().assertFail();
          } else {
            console.log("Miscservices_ScreenLock_ScreenLock_sendScreenLockEvent_0100 success " + JSON.stringify(result));
            expect(true).assertTrue();
          }
        })
      } catch (error) {
        console.log("Miscservices_ScreenLock_ScreenLock_sendScreenLockEvent_0100 error " + JSON.stringify(error));
        expect(true).assertTrue();
      }
      done();
      console.log("************* Miscservices_ScreenLock_ScreenLock_sendScreenLockEvent_0100 end*************");
    });

    console.log("************* screenLock Test end*************");
  })
}