/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import accessibility from '@ohos.accessibility'

const bundleName = 'com.sample.testfora11y';
const triggerAction = 'accessibilityFocus';
const eventType = 'accessibilityFocus';

describe('AccessibleSendEvent', function () {

    beforeEach(async function (done) {
        console.info(`AccessibleSendEvent: beforeEach starts`);
        done();
    })

    afterEach(async function (done) {
        console.info(`AccessibleSendEvent: afterEach starts`);
        setTimeout(done, 1000);
    })
    
    /******************************************************************************** */
    /* Cases SendEvent_0010-0020 & SendEvent_Null_0010-0020  */
    /*    are for accessibility.sendEvent() API test                                  */
    /******************************************************************************** */
    
    /*
    * @tc.number  SendEvent_0010
    * @tc.name    SendEvent_0010
    * @tc.desc    The parameter input is EventInfo, test the sendEvent() function
    *             The result of sendEvent() should be equal to an error code with no error
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('SendEvent_0010', 0, async function (done) {
        console.info('SendEvent_0010');
        let event = new accessibility.EventInfo();
        event.type = eventType;
        event.bundleName = bundleName;
        event.triggerAction = triggerAction;

        accessibility.sendEvent(event, (err, data) => {
            console.info(`AccessibleSendEvent: SendEvent_0010 has error: ${err.code}`);
            expect(err.code).assertEqual(0);
            console.info(`AccessibleSendEvent: SendEvent_0010 has data: ${data}`);
            expect(data).assertEqual(undefined);
            done();      
        })
    })

    /*
    * @tc.number  SendEvent_0020
    * @tc.name    SendEvent_0020
    * @tc.desc    The parameter input is EventInfo, test the sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('SendEvent_0020', 0, async function (done) {
        console.info('SendEvent_0020');
        let event = new accessibility.EventInfo();
        event.type = eventType;
        event.bundleName = bundleName;
        event.triggerAction = triggerAction;

        accessibility.sendEvent(event).then((result) => {
            console.info(`AccessibleSendEvent: SendEvent_0020 result ${result}`);
            expect(result).assertEqual(undefined);
            done();
        }).catch((err) => {
            console.error(`AccessibleSendEvent: SendEvent_0020 has error: ${err}`);
            expect(null).assertFail();
            done();
        });
    })

    /*
    * @tc.number  SendEvent_null_0010
    * @tc.name    SendEvent_null_0010
    * @tc.desc    The parameter input is null, test the sendEvent() function
    *             The result of sendEvent() should be equal to an error code with error
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('SendEvent_null_0010', 0, async function (done) {
        console.info('SendEvent_null_0010');
        let event = null;

        accessibility.sendEvent(event, (err, data) => {
            console.info(`AccessibleSendEvent: SendEvent_null_0010 has error: ${err.code}`);
            expect(err.code).assertEqual(-1);
            console.info(`AccessibleSendEvent: SendEvent_null_0010 has data: ${data}`);
            expect(data).assertEqual(undefined);
            done();
        })
    })

    /*
    * @tc.number  SendEvent_null_0020
    * @tc.name    SendEvent_null_0020
    * @tc.desc    The parameter input is null, test the sendEvent() function
    *             The result of sendEvent() should be equal to a rejected promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('SendEvent_null_0020', 0, async function (done) {
        console.info('SendEvent_null_0020');
        let event = null;

        accessibility.sendEvent(event).then((result) => {           
            console.error(`AccessibleSendEvent: SendEvent_null_0020 result ${result}`);
            expect(null).assertFail();            
            done();
        }).catch((err) => {
            console.info(`AccessibleSendEvent: SendEvent_null_0020 has error: ${err}`);
            expect(err).assertEqual(undefined);
            done();
        });
    })

    /*********************************************************************************************************** */
    /* Cases SendEvent_type_0010-SendEvent_gestureType_0180 are for interface accessibility.EventInfo API test   */                               
    /*********************************************************************************************************** */

  /*
    * @tc.number  SendEvent_type_0010
    * @tc.name    SendEvent_type_0010
    * @tc.desc    The type of EventInfo is 'accessibilityFocus', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0010 starts`);

    let eventType = 'accessibilityFocus';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0020
    * @tc.name    SendEvent_type_0020
    * @tc.desc    The type of EventInfo is 'accessibilityFocusClear', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0020 starts`);

    let eventType = 'accessibilityFocusClear';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0030
    * @tc.name    SendEvent_type_0030
    * @tc.desc    The type of EventInfo is 'click', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0030 starts`);

    let eventType = 'click';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0040
    * @tc.name    SendEvent_type_0040
    * @tc.desc    The type of EventInfo is 'longClick', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0040', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0040 starts`);

    let eventType = 'longClick';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0040 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0050
    * @tc.name    SendEvent_type_0050
    * @tc.desc    The type of EventInfo is 'focus', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0050', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0050 starts`);

    let eventType = 'focus';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0050 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0060
    * @tc.name    SendEvent_type_0060
    * @tc.desc    The type of EventInfo is 'select', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0060', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0060 starts`);

    let eventType = 'select';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0060 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0070
    * @tc.name    SendEvent_type_0070
    * @tc.desc    The type of EventInfo is 'hoverEnter', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0070', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0070 starts`);

    let eventType = 'hoverEnter';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0070 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0080
    * @tc.name    SendEvent_type_0080
    * @tc.desc    The type of EventInfo is 'hoverExit', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0080', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0080 starts`);

    let eventType = 'hoverExit';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0080 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0090
    * @tc.name    SendEvent_type_0090
    * @tc.desc    The type of EventInfo is 'textUpdate', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0090', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0090 starts`);

    let eventType = 'textUpdate';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0090 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0100
    * @tc.name    SendEvent_type_0100
    * @tc.desc    The type of EventInfo is 'textSelectionUpdate', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0100', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0100 starts`);

    let eventType = 'textSelectionUpdate';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0100 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0110
    * @tc.name    SendEvent_type_0110
    * @tc.desc    The type of EventInfo is 'scroll', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0110', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0110 starts`);

    let eventType = 'scroll';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0110 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0120
    * @tc.name    SendEvent_type_0120
    * @tc.desc    The type of EventInfo is 'textMoveUnit', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0120', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0120 starts`);

    let eventType = 'textMoveUnit';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0120 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0130
    * @tc.name    SendEvent_type_0130
    * @tc.desc    The type of EventInfo is 'touchBegin', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0130', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0130 starts`);

    let eventType = 'touchBegin';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0130 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0140
    * @tc.name    SendEvent_type_0140
    * @tc.desc    The type of EventInfo is 'touchEnd', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0140', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0140 starts`);

    let eventType = 'touchEnd';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0140 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0150
    * @tc.name    SendEvent_type_0150
    * @tc.desc    The type of EventInfo is 'touchGuideBegin', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0150', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0150 starts`);

    let eventType = 'touchGuideBegin';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0150 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0160
    * @tc.name    SendEvent_type_0160
    * @tc.desc    The type of EventInfo is 'touchGuideEnd', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0160', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0160 starts`);

    let eventType = 'touchGuideEnd';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0160 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0170
    * @tc.name    SendEvent_type_0170
    * @tc.desc    The type of EventInfo is 'touchGuideGestureBegin', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0170', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0170 starts`);

    let eventType = 'touchGuideGestureBegin';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0170 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0180
    * @tc.name    SendEvent_type_0180
    * @tc.desc    The type of EventInfo is 'touchGuideGestureEnd', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0180', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0180 starts`);

    let eventType = 'touchGuideGestureEnd';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0180 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0190
    * @tc.name    SendEvent_type_0190
    * @tc.desc    The type of EventInfo is 'windowUpdate', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0190', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0190 starts`);

    let eventType = 'windowUpdate';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0190 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0200
    * @tc.name    SendEvent_type_0200
    * @tc.desc    The type of EventInfo is 'pageContentUpdate', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0200', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0200 starts`);

    let eventType = 'pageContentUpdate';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0200 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0210
    * @tc.name    SendEvent_type_0210
    * @tc.desc    The type of EventInfo is 'pageStateUpdate', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0210', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0210 starts`);

    let eventType = 'pageStateUpdate';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0210 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0220
    * @tc.name    SendEvent_type_0220
    * @tc.desc    The type of EventInfo is 'publicNotice', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0220', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0220 starts`);

    let eventType = 'publicNotice';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0220 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0230
    * @tc.name    SendEvent_type_0230
    * @tc.desc    The type of EventInfo is 'notificationUpdate', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0230', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0230 starts`);

    let eventType = 'notificationUpdate';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0230 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0240
    * @tc.name    SendEvent_type_0240
    * @tc.desc    The type of EventInfo is 'interrupt', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0240', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0240 starts`);

    let eventType = 'interrupt';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0240 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0250
    * @tc.name    SendEvent_type_0250
    * @tc.desc    The type of EventInfo is 'gesture', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0250', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0250 starts`);

    let eventType = 'gesture';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_type_0250 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_type_0260
    * @tc.name    SendEvent_type_0260
    * @tc.desc    The type of EventInfo is '', test sendEvent() function
    *             The result of sendEvent() should be equal to a rejected promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0260', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0260 starts`);

    let eventType = '';
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) =>{           
        console.error(`AccessibleSendEvent: SendEvent_type_0260 result ${result}`);
        expect(null).assertFail();  
        done();          
    }).catch((err) => {
        console.info(`AccessibleSendEvent: SendEvent_type_0260 has error: ${err}`);
        expect(err).assertEqual(undefined);
        done();    
    });
  })

  /*
    * @tc.number  SendEvent_type_0270
    * @tc.name    SendEvent_type_0270
    * @tc.desc    The type of EventInfo is null, test sendEvent() function
    *             The result of sendEvent() should be equal to a rejected promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_type_0270', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_type_0270 starts`);

    let eventType = null;
    let event = new accessibility.EventInfo();
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) =>{           
        console.error(`AccessibleSendEvent: SendEvent_type_0270 result ${result}`);
        expect(null).assertFail();  
        done();          
    }).catch((err) => {
        console.info(`AccessibleSendEvent: SendEvent_type_0270 has error: ${err}`);
        expect(err).assertEqual(undefined);
        done();    
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0010
    * @tc.name    SendEvent_windowUpdateType_0010
    * @tc.desc    The windowUpdateType of EventInfo is 'add', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0010 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'add';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0020
    * @tc.name    SendEvent_windowUpdateType_0020
    * @tc.desc    The windowUpdateType of EventInfo is 'remove', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0020 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'remove';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0030
    * @tc.name    SendEvent_windowUpdateType_0030
    * @tc.desc    The windowUpdateType of EventInfo is 'title', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0030 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'title';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0040
    * @tc.name    SendEvent_windowUpdateType_0040
    * @tc.desc    The windowUpdateType of EventInfo is 'bounds', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0040', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0040 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'bounds';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0040 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0050
    * @tc.name    SendEvent_windowUpdateType_0050
    * @tc.desc    The windowUpdateType of EventInfo is 'layer', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0050', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0050 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'layer';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0050 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0060
    * @tc.name    SendEvent_windowUpdateType_0060
    * @tc.desc    The windowUpdateType of EventInfo is 'active', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0060', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0060 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'active';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0060 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0070
    * @tc.name    SendEvent_windowUpdateType_0070
    * @tc.desc    The windowUpdateType of EventInfo is 'focus', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0070', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0070 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'focus';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0070 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0080
    * @tc.name    SendEvent_windowUpdateType_0080
    * @tc.desc    The windowUpdateType of EventInfo is 'accessibilityFocus', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0080', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0080 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'accessibilityFocus';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0080 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0090
    * @tc.name    SendEvent_windowUpdateType_0090
    * @tc.desc    The windowUpdateType of EventInfo is 'parent', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0090', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0090 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'parent';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0090 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0010
    * @tc.name    SendEvent_windowUpdateType_0010
    * @tc.desc    The windowUpdateType of EventInfo is 'children', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0010 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'children';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0011
    * @tc.name    SendEvent_windowUpdateType_0011
    * @tc.desc    The windowUpdateType of EventInfo is 'pip', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0011', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0011 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = 'pip';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0011 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0012
    * @tc.name    SendEvent_windowUpdateType_0012
    * @tc.desc    The windowUpdateType of EventInfo is '', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0012', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0012 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = '';
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0012 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowUpdateType_0013
    * @tc.name    SendEvent_windowUpdateType_0013
    * @tc.desc    The windowUpdateType of EventInfo is null, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowUpdateType_0013', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowUpdateType_0013 starts`);

    let event = new accessibility.EventInfo();
    let windowUpdateType = null;
    event.type = eventType;
    event.windowUpdateType = windowUpdateType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowUpdateType_0013 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_bundleName_0010
    * @tc.name    SendEvent_bundleName_0010
    * @tc.desc    The bundleName of EventInfo is 'com.ixaa.testfora11y', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_bundleName_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_bundleName_0010 starts`);

    let event = new accessibility.EventInfo();
    let localBundleName = 'com.ixaa.testfora11y';
    event.type = eventType;
    event.bundleName = localBundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_bundleName_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_bundleName_0020
    * @tc.name    SendEvent_bundleName_0020
    * @tc.desc    The bundleName of EventInfo is '', test sendEvent() function
    *             The result of sendEvent() should be equal to a rejected promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_bundleName_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_bundleName_0020 starts`);

    let event = new accessibility.EventInfo();
    let localBundleName = '';
    event.type = eventType;
    event.bundleName = localBundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) =>{           
        console.error(`AccessibleSendEvent: SendEvent_bundleName_0020 result ${result}`);
        expect(null).assertFail();  
        done();          
    }).catch((err) => {
        console.info(`AccessibleSendEvent: SendEvent_bundleName_0020 has error: ${err}`);
        expect(err).assertEqual(undefined);
        done();    
    });
  })

  /*
    * @tc.number  SendEvent_bundleName_0030
    * @tc.name    SendEvent_bundleName_0030
    * @tc.desc    The bundleName of EventInfo is null, test sendEvent() function
    *             The result of sendEvent() should be equal to a rejected promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_bundleName_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_bundleName_0030 starts`);

    let event = new accessibility.EventInfo();
    let localBundleName = null;
    event.type = eventType;
    event.bundleName = localBundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) =>{           
        console.error(`AccessibleSendEvent: SendEvent_bundleName_0030 result ${result}`);
        expect(null).assertFail();  
        done();          
    }).catch((err) => {
        console.info(`AccessibleSendEvent: SendEvent_bundleName_0030 has error: ${err}`);
        expect(err).assertEqual(undefined);
        done();    
    });
  })

  /*
    * @tc.number  SendEvent_componentType_0010
    * @tc.name    SendEvent_componentType_0010
    * @tc.desc    The componentType of EventInfo is 'button', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_componentType_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_componentType_0010 starts`);

    let event = new accessibility.EventInfo();
    let componentType = 'button';
    event.type = eventType;
    event.bundleName = bundleName;
    event.componentType = componentType;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_componentType_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_componentType_0020
    * @tc.name    SendEvent_componentType_0020
    * @tc.desc    The componentType of EventInfo is '', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_componentType_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_componentType_0020 starts`);

    let event = new accessibility.EventInfo();
    let componentType = '';
    event.type = eventType;
    event.bundleName = bundleName;
    event.componentType = componentType;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_componentType_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_componentType_0030
    * @tc.name    SendEvent_componentType_0030
    * @tc.desc    The componentType of EventInfo is null, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_componentType_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_componentType_0030 starts`);

    let event = new accessibility.EventInfo();
    let componentType = null;
    event.type = eventType;
    event.bundleName = bundleName;
    event.componentType = componentType;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_componentType_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_timeStamp_0010
    * @tc.name    SendEvent_timeStamp_0010
    * @tc.desc    The timeStamp of EventInfo is 1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_timeStamp_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_timeStamp_0010 starts`);

    let event = new accessibility.EventInfo();
    let timeStamp = 1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.timeStamp = timeStamp;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_timeStamp_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_timeStamp_0020
    * @tc.name    SendEvent_timeStamp_0020
    * @tc.desc    The timeStamp of EventInfo is 0, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_timeStamp_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_timeStamp_0020 starts`);

    let event = new accessibility.EventInfo();
    let timeStamp = 0;
    event.type = eventType;
    event.bundleName = bundleName;
    event.timeStamp = timeStamp;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_timeStamp_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_timeStamp_0030
    * @tc.name    SendEvent_timeStamp_0030
    * @tc.desc    The timeStamp of EventInfo is -1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_timeStamp_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_timeStamp_0030 starts`);

    let event = new accessibility.EventInfo();
    let timeStamp = -1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.timeStamp = timeStamp;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_timeStamp_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowId_0010
    * @tc.name    SendEvent_windowId_0010
    * @tc.desc    The windowId of EventInfo is 1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowId_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowId_0010 starts`);

    let event = new accessibility.EventInfo();
    let windowId = 1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.windowId = windowId;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowId_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowId_0020
    * @tc.name    SendEvent_windowId_0020
    * @tc.desc    The windowId of EventInfo is 0, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowId_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowId_0020 starts`);

    let event = new accessibility.EventInfo();
    let windowId = 0;
    event.type = eventType;
    event.bundleName = bundleName;
    event.windowId = windowId;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowId_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_windowId_0030
    * @tc.name    SendEvent_windowId_0030
    * @tc.desc    The windowId of EventInfo is -1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_windowId_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_windowId_0030 starts`);

    let event = new accessibility.EventInfo();
    let windowId = -1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.windowId = windowId;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_windowId_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_pageId_0010
    * @tc.name    SendEvent_pageId_0010
    * @tc.desc    The pageId of EventInfo is 1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_pageId_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_pageId_0010 starts`);

    let event = new accessibility.EventInfo();
    let pageId = 1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.pageId = pageId;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_pageId_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_pageId_0020
    * @tc.name    SendEvent_pageId_0020
    * @tc.desc    The pageId of EventInfo is 0, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_pageId_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_pageId_0020 starts`);

    let event = new accessibility.EventInfo();
    let pageId = 0;
    event.type = eventType;
    event.bundleName = bundleName;
    event.pageId = pageId;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_pageId_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_pageId_0030
    * @tc.name    SendEvent_pageId_0030
    * @tc.desc    The pageId of EventInfo is -1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_pageId_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_pageId_0030 starts`);

    let event = new accessibility.EventInfo();
    let pageId = -1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.pageId = pageId;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_pageId_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_componentId_0010
    * @tc.name    SendEvent_componentId_0010
    * @tc.desc    The componentId of EventInfo is 1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_componentId_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_componentId_0010 starts`);

    let event = new accessibility.EventInfo();
    let componentId = 1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.componentId = componentId;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_componentId_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_componentId_0020
    * @tc.name    SendEvent_componentId_0020
    * @tc.desc    The componentId of EventInfo is 0, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_componentId_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_componentId_0020 starts`);

    let event = new accessibility.EventInfo();
    let componentId = 0;
    event.type = eventType;
    event.bundleName = bundleName;
    event.componentId = componentId;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_componentId_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_componentId_0030
    * @tc.name    SendEvent_componentId_0030
    * @tc.desc    The componentId of EventInfo is -1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_componentId_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_componentId_0030 starts`);

    let event = new accessibility.EventInfo();
    let componentId = -1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.componentId = componentId;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_componentId_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_description_0010
    * @tc.name    SendEvent_description_0010
    * @tc.desc    The description of EventInfo is '1', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_description_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_description_0010 starts`);

    let event = new accessibility.EventInfo();
    let description = '1';
    event.type = eventType;
    event.bundleName = bundleName;
    event.description = description;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_description_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_description_0020
    * @tc.name    SendEvent_description_0020
    * @tc.desc    The description of EventInfo is '', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_description_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_description_0020 starts`);

    let event = new accessibility.EventInfo();
    let description = '';
    event.type = eventType;
    event.bundleName = bundleName;
    event.description = description;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_description_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_description_0030
    * @tc.name    SendEvent_description_0030
    * @tc.desc    The description of EventInfo is null, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_description_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_description_0030 starts`);

    let event = new accessibility.EventInfo();
    let description = null;
    event.type = eventType;
    event.bundleName = bundleName;
    event.description = description;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_description_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0010
    * @tc.name    SendEvent_triggerAction_0010
    * @tc.desc    The triggerAction of EventInfo is 'accessibilityFocus', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0010 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'accessibilityFocus';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0020
    * @tc.name    SendEvent_triggerAction_0020
    * @tc.desc    The triggerAction of EventInfo is 'clearAccessibilityFocus', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0020 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'clearAccessibilityFocus';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0030
    * @tc.name    SendEvent_triggerAction_0030
    * @tc.desc    The triggerAction of EventInfo is 'focus', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0030 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'focus';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0040
    * @tc.name    SendEvent_triggerAction_0040
    * @tc.desc    The triggerAction of EventInfo is 'clearFocus', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0040', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0040 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'clearFocus';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0040 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0050
    * @tc.name    SendEvent_triggerAction_0050
    * @tc.desc    The triggerAction of EventInfo is 'clearSelection', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0050', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0050 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'clearSelection';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0050 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0060
    * @tc.name    SendEvent_triggerAction_0060
    * @tc.desc    The triggerAction of EventInfo is 'click', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0060', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0060 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'click';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0060 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0070
    * @tc.name    SendEvent_triggerAction_0070
    * @tc.desc    The triggerAction of EventInfo is 'longClick', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0070', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0070 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'longClick';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0070 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0080
    * @tc.name    SendEvent_triggerAction_0080
    * @tc.desc    The triggerAction of EventInfo is 'cut', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0080', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0080 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'cut';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0080 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0090
    * @tc.name    SendEvent_triggerAction_0090
    * @tc.desc    The triggerAction of EventInfo is 'copy', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0090', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0090 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'copy';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0090 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0100
    * @tc.name    SendEvent_triggerAction_0100
    * @tc.desc    The triggerAction of EventInfo is 'paste', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0100', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0100 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'paste';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0100 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0110
    * @tc.name    SendEvent_triggerAction_0110
    * @tc.desc    The triggerAction of EventInfo is 'select', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0110', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0110 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'select';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0110 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0120
    * @tc.name    SendEvent_triggerAction_0120
    * @tc.desc    The triggerAction of EventInfo is 'setText', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0120', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0120 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'setText';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0120 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0130
    * @tc.name    SendEvent_triggerAction_0130
    * @tc.desc    The triggerAction of EventInfo is 'delete', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0130', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0130 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'delete';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0130 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0140
    * @tc.name    SendEvent_triggerAction_0140
    * @tc.desc    The triggerAction of EventInfo is 'scrollForward', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0140', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0140 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'scrollForward';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0140 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0150
    * @tc.name    SendEvent_triggerAction_0150
    * @tc.desc    The triggerAction of EventInfo is 'scrollBackward', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0150', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0150 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'scrollBackward';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0150 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0160
    * @tc.name    SendEvent_triggerAction_0160
    * @tc.desc    The triggerAction of EventInfo is 'setSelection', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0160', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0160 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'setSelection';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0160 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0170
    * @tc.name    SendEvent_triggerAction_0170
    * @tc.desc    The triggerAction of EventInfo is 'unfold', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0170', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0170 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'unfold';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0170 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0180
    * @tc.name    SendEvent_triggerAction_0180
    * @tc.desc    The triggerAction of EventInfo is 'fold', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0180', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0180 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'fold';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0180 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0190
    * @tc.name    SendEvent_triggerAction_0190
    * @tc.desc    The triggerAction of EventInfo is 'nextText', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0190', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0190 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'nextText';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0190 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0200
    * @tc.name    SendEvent_triggerAction_0200
    * @tc.desc    The triggerAction of EventInfo is 'previousText', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0200', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0200 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'previousText';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0200 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0210
    * @tc.name    SendEvent_triggerAction_0210
    * @tc.desc    The triggerAction of EventInfo is 'nextHtmlItem', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0210', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0210 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'nextHtmlItem';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0210 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0220
    * @tc.name    SendEvent_triggerAction_0220
    * @tc.desc    The triggerAction of EventInfo is 'previousHtmlItem', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0220', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0220 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = 'previousHtmlItem';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_triggerAction_0220 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0230
    * @tc.name    SendEvent_triggerAction_0230
    * @tc.desc    The triggerAction of EventInfo is '', test sendEvent() function
    *             The result of sendEvent() should be equal to a rejected promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0230', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0230 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = '';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) =>{           
        console.error(`AccessibleSendEvent: SendEvent_triggerAction_0230 result ${result}`);
        expect(null).assertFail();  
        done();          
    }).catch((err) => {
        console.info(`AccessibleSendEvent: SendEvent_triggerAction_0230 has error: ${err}`);
        expect(err).assertEqual(undefined);
        done();    
    });
  })

  /*
    * @tc.number  SendEvent_triggerAction_0240
    * @tc.name    SendEvent_triggerAction_0240
    * @tc.desc    The triggerAction of EventInfo is null, test sendEvent() function
    *             The result of sendEvent() should be equal to a rejected promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_triggerAction_0240', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_triggerAction_0240 starts`);

    let event = new accessibility.EventInfo();
    let triggerAction = null;
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) =>{           
        console.error(`AccessibleSendEvent: SendEvent_triggerAction_0240 result ${result}`);
        expect(null).assertFail();  
        done();          
    }).catch((err) => {
        console.info(`AccessibleSendEvent: SendEvent_triggerAction_0240 has error: ${err}`);
        expect(err).assertEqual(undefined);
        done();    
    });
  })

  /*
    * @tc.number  SendEvent_textMoveUnit_0010
    * @tc.name    SendEvent_textMoveUnit_0010
    * @tc.desc    The textMoveUnit of EventInfo is 'char', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_textMoveUnit_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_textMoveUnit_0010 starts`);

    let event = new accessibility.EventInfo();
    let textMoveUnit = 'char';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.textMoveUnit = textMoveUnit;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_textMoveUnit_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_textMoveUnit_0020
    * @tc.name    SendEvent_textMoveUnit_0020
    * @tc.desc    The textMoveUnit of EventInfo is 'word', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_textMoveUnit_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_textMoveUnit_0020 starts`);

    let event = new accessibility.EventInfo();
    let textMoveUnit = 'word';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.textMoveUnit = textMoveUnit;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_textMoveUnit_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_textMoveUnit_0030
    * @tc.name    SendEvent_textMoveUnit_0030
    * @tc.desc    The textMoveUnit of EventInfo is 'line', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_textMoveUnit_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_textMoveUnit_0030 starts`);

    let event = new accessibility.EventInfo();
    let textMoveUnit = 'line';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.textMoveUnit = textMoveUnit;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_textMoveUnit_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_textMoveUnit_0040
    * @tc.name    SendEvent_textMoveUnit_0040
    * @tc.desc    The textMoveUnit of EventInfo is 'page', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_textMoveUnit_0040', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_textMoveUnit_0040 starts`);

    let event = new accessibility.EventInfo();
    let textMoveUnit = 'page';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.textMoveUnit = textMoveUnit;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_textMoveUnit_0040 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_textMoveUnit_0050
    * @tc.name    SendEvent_textMoveUnit_0050
    * @tc.desc    The textMoveUnit of EventInfo is 'paragraph', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_textMoveUnit_0050', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_textMoveUnit_0050 starts`);

    let event = new accessibility.EventInfo();
    let textMoveUnit = 'paragraph';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.textMoveUnit = textMoveUnit;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_textMoveUnit_0050 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_textMoveUnit_0060
    * @tc.name    SendEvent_textMoveUnit_0060
    * @tc.desc    The textMoveUnit of EventInfo is '', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_textMoveUnit_0060', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_textMoveUnit_0060 starts`);

    let event = new accessibility.EventInfo();
    let textMoveUnit = '';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.textMoveUnit = textMoveUnit;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_textMoveUnit_0060 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_textMoveUnit_0070
    * @tc.name    SendEvent_textMoveUnit_0070
    * @tc.desc    The textMoveUnit of EventInfo is null, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_textMoveUnit_0070', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_textMoveUnit_0070 starts`);

    let event = new accessibility.EventInfo();
    let textMoveUnit = null;
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.textMoveUnit = textMoveUnit;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_textMoveUnit_0070 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_contents_0010
    * @tc.name    SendEvent_contents_0010
    * @tc.desc    The contents of EventInfo is ['1'], test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_contents_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_contents_0010 starts`);

    let event = new accessibility.EventInfo();
    let contents = ['1'];
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.contents = contents;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_contents_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_contents_0020
    * @tc.name    SendEvent_contents_0020
    * @tc.desc    The contents of EventInfo is [], test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_contents_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_contents_0020 starts`);

    let event = new accessibility.EventInfo();
    let contents = [];
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.contents = contents;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_contents_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_lastContent_0010
    * @tc.name    SendEvent_lastContent_0010
    * @tc.desc    The lastContent of EventInfo is '1', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_lastContent_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_lastContent_0010 starts`);

    let event = new accessibility.EventInfo();
    let lastContent = '1';
    event.type = eventType;
    event.bundleName = bundleName;
    event.lastContent = lastContent;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_lastContent_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_lastContent_0020
    * @tc.name    SendEvent_lastContent_0020
    * @tc.desc    The lastContent of EventInfo is '', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_lastContent_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_lastContent_0020 starts`);

    let event = new accessibility.EventInfo();
    let lastContent = '';
    event.type = eventType;
    event.bundleName = bundleName;
    event.lastContent = lastContent;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_lastContent_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_lastContent_0030
    * @tc.name    SendEvent_lastContent_0030
    * @tc.desc    The lastContent of EventInfo is null, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_lastContent_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_lastContent_0030 starts`);

    let event = new accessibility.EventInfo();
    let lastContent = null;
    event.type = eventType;
    event.bundleName = bundleName;
    event.lastContent = lastContent;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_lastContent_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })


  /*
    * @tc.number  SendEvent_beginIndex_0010
    * @tc.name    SendEvent_beginIndex_0010
    * @tc.desc    The beginIndex of EventInfo is 1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_beginIndex_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_beginIndex_0010 starts`);

    let event = new accessibility.EventInfo();
    let beginIndex = 1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.beginIndex = beginIndex;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_beginIndex_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_beginIndex_0020
    * @tc.name    SendEvent_beginIndex_0020
    * @tc.desc    The beginIndex of EventInfo is 0, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_beginIndex_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_beginIndex_0020 starts`);

    let event = new accessibility.EventInfo();
    let beginIndex = 0;
    event.type = eventType;
    event.bundleName = bundleName;
    event.beginIndex = beginIndex;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_beginIndex_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_beginIndex_0030
    * @tc.name    SendEvent_beginIndex_0030
    * @tc.desc    The beginIndex of EventInfo is -1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_beginIndex_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_beginIndex_0030 starts`);

    let event = new accessibility.EventInfo();
    let beginIndex = -1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.beginIndex = beginIndex;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_beginIndex_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
      * @tc.number  SendEvent_currentIndex_0010
      * @tc.name    SendEvent_currentIndex_0010
      * @tc.desc    The currentIndex of EventInfo is 1, test sendEvent() function
      *             The result of sendEvent() should be equal to a promise of undefined
      * @tc.size    SmallTest
      * @tc.type    User
      */
  it('SendEvent_currentIndex_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_currentIndex_0010 starts`);

    let event = new accessibility.EventInfo();
    let currentIndex = 1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.currentIndex = currentIndex;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_currentIndex_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_currentIndex_0020
    * @tc.name    SendEvent_currentIndex_0020
    * @tc.desc    The currentIndex of EventInfo is 0, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_currentIndex_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_currentIndex_0020 starts`);

    let event = new accessibility.EventInfo();
    let currentIndex = 0;
    event.type = eventType;
    event.bundleName = bundleName;
    event.currentIndex = currentIndex;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_currentIndex_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_currentIndex_0030
    * @tc.name    SendEvent_currentIndex_0030
    * @tc.desc    The currentIndex of EventInfo is -1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_currentIndex_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_currentIndex_0030 starts`);

    let event = new accessibility.EventInfo();
    let currentIndex = -1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.currentIndex = currentIndex;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_currentIndex_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
      * @tc.number  SendEvent_endIndex_0010
      * @tc.name    SendEvent_endIndex_0010
      * @tc.desc    The endIndex of EventInfo is 1, test sendEvent() function
      *             The result of sendEvent() should be equal to a promise of undefined
      * @tc.size    SmallTest
      * @tc.type    User
      */
  it('SendEvent_endIndex_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_endIndex_0010 starts`);

    let event = new accessibility.EventInfo();
    let endIndex = 1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.endIndex = endIndex;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_endIndex_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_endIndex_0020
    * @tc.name    SendEvent_endIndex_0020
    * @tc.desc    The endIndex of EventInfo is 0, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_endIndex_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_endIndex_0020 starts`);

    let event = new accessibility.EventInfo();
    let endIndex = 0;
    event.type = eventType;
    event.bundleName = bundleName;
    event.endIndex = endIndex;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_endIndex_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_endIndex_0030
    * @tc.name    SendEvent_endIndex_0030
    * @tc.desc    The endIndex of EventInfo is -1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_endIndex_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_endIndex_0030 starts`);

    let event = new accessibility.EventInfo();
    let endIndex = -1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.endIndex = endIndex;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_endIndex_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
      * @tc.number  SendEvent_itemCount_0010
      * @tc.name    SendEvent_itemCount_0010
      * @tc.desc    The itemCount of EventInfo is 1, test sendEvent() function
      *             The result of sendEvent() should be equal to a promise of undefined
      * @tc.size    SmallTest
      * @tc.type    User
      */
  it('SendEvent_itemCount_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_itemCount_0010 starts`);

    let event = new accessibility.EventInfo();
    let itemCount = 1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.itemCount = itemCount;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_itemCount_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_itemCount_0020
    * @tc.name    SendEvent_itemCount_0020
    * @tc.desc    The itemCount of EventInfo is 0, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_itemCount_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_itemCount_0020 starts`);

    let event = new accessibility.EventInfo();
    let itemCount = 0;
    event.type = eventType;
    event.bundleName = bundleName;
    event.itemCount = itemCount;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_itemCount_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_itemCount_0030
    * @tc.name    SendEvent_itemCount_0030
    * @tc.desc    The itemCount of EventInfo is -1, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_itemCount_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_itemCount_0030 starts`);

    let event = new accessibility.EventInfo();
    let itemCount = -1;
    event.type = eventType;
    event.bundleName = bundleName;
    event.itemCount = itemCount;
    event.triggerAction = triggerAction;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_itemCount_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0010
    * @tc.name    SendEvent_categoryNotification_0010
    * @tc.desc    The categoryNotification of EventInfo is 'call', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0010 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'call';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0020
    * @tc.name    SendEvent_categoryNotification_0020
    * @tc.desc    The categoryNotification of EventInfo is 'msg', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0020 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'msg';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0030
    * @tc.name    SendEvent_categoryNotification_0030
    * @tc.desc    The categoryNotification of EventInfo is 'email', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0030 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'email';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0040
    * @tc.name    SendEvent_categoryNotification_0040
    * @tc.desc    The categoryNotification of EventInfo is 'event', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0040', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0040 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'event';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0040 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0050
    * @tc.name    SendEvent_categoryNotification_0050
    * @tc.desc    The categoryNotification of EventInfo is 'promo', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0050', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0050 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'promo';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0050 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0060
    * @tc.name    SendEvent_categoryNotification_0060
    * @tc.desc    The categoryNotification of EventInfo is 'alarm', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0060', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0060 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'alarm';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0060 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0070
    * @tc.name    SendEvent_categoryNotification_0070
    * @tc.desc    The categoryNotification of EventInfo is 'progress', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0070', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0070 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'progress';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0070 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0080
    * @tc.name    SendEvent_categoryNotification_0080
    * @tc.desc    The categoryNotification of EventInfo is 'social', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0080', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0080 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'social';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0080 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0090
    * @tc.name    SendEvent_categoryNotification_0090
    * @tc.desc    The categoryNotification of EventInfo is 'err', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0090', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0090 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'err';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0090 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0100
    * @tc.name    SendEvent_categoryNotification_0100
    * @tc.desc    The categoryNotification of EventInfo is 'transport', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0100', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0100 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'transport';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0100 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0110
    * @tc.name    SendEvent_categoryNotification_0110
    * @tc.desc    The categoryNotification of EventInfo is 'sys', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0110', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0110 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'sys';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0110 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0120
    * @tc.name    SendEvent_categoryNotification_0120
    * @tc.desc    The categoryNotification of EventInfo is 'service', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0120', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0120 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = 'service';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0120 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0130
    * @tc.name    SendEvent_categoryNotification_0130
    * @tc.desc    The categoryNotification of EventInfo is '', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0130', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0130 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = '';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0130 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_categoryNotification_0140
    * @tc.name    SendEvent_categoryNotification_0140
    * @tc.desc    The categoryNotification of EventInfo is null, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_categoryNotification_0140', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_categoryNotification_0140 starts`);

    let event = new accessibility.EventInfo();
    let categoryNotification = null;
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.categoryNotification = categoryNotification;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_categoryNotification_0140 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0010
    * @tc.name    SendEvent_gestureType_0010
    * @tc.desc    The gestureType of EventInfo is 'left', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0010', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0010 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'left';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0010 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0020
    * @tc.name    SendEvent_gestureType_0020
    * @tc.desc    The gestureType of EventInfo is 'leftThenRight', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0020', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0020 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'leftThenRight';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0020 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0030
    * @tc.name    SendEvent_gestureType_0030
    * @tc.desc    The gestureType of EventInfo is 'leftThenUp', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0030', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0030 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'leftThenUp';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0030 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0040
    * @tc.name    SendEvent_gestureType_0040
    * @tc.desc    The gestureType of EventInfo is 'leftThenDown', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0040', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0040 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'leftThenDown';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0040 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0050
    * @tc.name    SendEvent_gestureType_0050
    * @tc.desc    The gestureType of EventInfo is 'right', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0050', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0050 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'right';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0050 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0060
    * @tc.name    SendEvent_gestureType_0060
    * @tc.desc    The gestureType of EventInfo is 'rightThenLeft', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0060', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0060 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'rightThenLeft';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0060 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0070
    * @tc.name    SendEvent_gestureType_0070
    * @tc.desc    The gestureType of EventInfo is 'rightThenUp', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0070', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0070 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'rightThenUp';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0070 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0080
    * @tc.name    SendEvent_gestureType_0080
    * @tc.desc    The gestureType of EventInfo is 'rightThenDown', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0080', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0080 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'rightThenDown';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0080 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0090
    * @tc.name    SendEvent_gestureType_0090
    * @tc.desc    The gestureType of EventInfo is 'up', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0090', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0090 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'up';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0090 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0100
    * @tc.name    SendEvent_gestureType_0100
    * @tc.desc    The gestureType of EventInfo is 'upThenLeft', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0100', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0100 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'upThenLeft';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0100 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0110
    * @tc.name    SendEvent_gestureType_0110
    * @tc.desc    The gestureType of EventInfo is 'upThenRight', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0110', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0110 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'upThenRight';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0110 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0120
    * @tc.name    SendEvent_gestureType_0120
    * @tc.desc    The gestureType of EventInfo is 'upThenDown', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0120', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0120 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'upThenDown';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0120 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0130
    * @tc.name    SendEvent_gestureType_0130
    * @tc.desc    The gestureType of EventInfo is 'down', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0130', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0130 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'down';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0130 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0140
    * @tc.name    SendEvent_gestureType_0140
    * @tc.desc    The gestureType of EventInfo is 'downThenLeft', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0140', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0140 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'downThenLeft';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0140 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0150
    * @tc.name    SendEvent_gestureType_0150
    * @tc.desc    The gestureType of EventInfo is 'downThenRight', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0150', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0150 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'downThenRight';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0150 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0160
    * @tc.name    SendEvent_gestureType_0160
    * @tc.desc    The gestureType of EventInfo is 'downThenUp', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0160', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0160 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = 'downThenUp';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0160 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0170
    * @tc.name    SendEvent_gestureType_0170
    * @tc.desc    The gestureType of EventInfo is '', test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0170', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0170 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = '';
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0170 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

  /*
    * @tc.number  SendEvent_gestureType_0180
    * @tc.name    SendEvent_gestureType_0180
    * @tc.desc    The gestureType of EventInfo is null, test sendEvent() function
    *             The result of sendEvent() should be equal to a promise of undefined
    * @tc.size    SmallTest
    * @tc.type    User
    */
  it('SendEvent_gestureType_0180', 0, async function (done) {
    console.info(`AccessibleSendEvent: SendEvent_gestureType_0180 starts`);

    let event = new accessibility.EventInfo();
    let gestureType = null;
    event.type = eventType;
    event.bundleName = bundleName;
    event.triggerAction = triggerAction;
    event.gestureType = gestureType;
    accessibility.sendEvent(event).then((result) => {
      expect(result).assertEqual(undefined);
      done();
    }).catch(err => {
      console.error(`AccessibleSendEvent: SendEvent_gestureType_0180 has error: ${err}`);
      expect(null).assertFail();
      done();
    });
  })

    
    
})

