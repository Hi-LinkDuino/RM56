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
import Subscriber from '@ohos.commonevent'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'

describe('ActsSubscriberTestUnorder', async function (done) {
    console.info("===========ActsSubscriberTestUnorder start====================>");
    let commonEventSubscriber001;
    let commonEventSubscriber002;
    let commonEventSubscriber003;
    let commonEventSubscriber004;
    let commonEventSubscriber0051;
    let commonEventSubscriber0052;
    let commonEventSubscriber006;
    let commonEventSubscriber008;
    let commonEventSubscriber0101;
    let commonEventSubscriber0102;
    let commonEventSubscriber011;
    let commonEventSubscriber012;

    function publishCallback(err) {
        console.info("==========================>publishCallback");
    }

    function publishCallback10001(err) {
        console.info("==========================>publishCallback");
    }

    function publishCallback10002(err) {
        console.info("==========================>publishCallback");
    }

    function unsubscriberCallBack(err) {
        console.info("==========================>unsubscriberCallBack");
    }

    /*
     * @tc.number    : ActsSubscriberTestUnorder_0100
     * @tc.name      : verify subscribe and publish : Check subscribe and publish common event data
     * @tc.desc      : Check the subscriber can receive event "publish_event0100" type of the interface (by Promise)
     */
    it('ActsSubscriberTestUnorder_0100', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorder_0100=============================>");
        function subscriberCallBack001(err, data) {
            console.info("==========================>subscriberCallBack001");
            expect(data.event).assertEqual("publish_event0100");
            expect(data.bundleName).assertEqual("");
            expect(data.code).assertEqual(0);
            expect(data.data).assertEqual("");
            done();
        }

        let commonEventSubscribeInfo = {
            events: ["publish_event0100"],
        };

        Subscriber.createSubscriber(
            commonEventSubscribeInfo
        ).then((data)=>{
            console.info("===============ActsSubscriberTestUnorder_0100=========createSubscriber promise");
            commonEventSubscriber001 = data;
            data.getSubscribeInfo().then(()=>{
                console.info("===============ActsSubscriberTestUnorder_0100=========getSubscribeInfo promise");
                Subscriber.subscribe(commonEventSubscriber001, subscriberCallBack001);
                setTimeout(function (){
                    console.info("==========ActsSubscriberTestUnorder_0100 publish start============");
                    Subscriber.publish("publish_event0100", publishCallback);
                }, 1000);
            });
        })
    })

    /*
     * @tc.number    : ActsSubscriberTestUnorder_0200
     * @tc.name      : verify subscribe and publish : Check subscribe and publish common event data
     *                 of containing special characters
     * @tc.desc      : Check the subscriber can receive event "@#￥#3243adsafdf_" type of the interface (by Promise)
     */
    it('ActsSubscriberTestUnorder_0200', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorder_0200==========================>");

        function subscriberCallBack002(err, data) {
            console.info("==========================>subscriberCallBack002");
            expect(data.event).assertEqual("@#￥#3243adsafdf_");
            expect(data.bundleName).assertEqual("");
            expect(data.code).assertEqual(0);
            expect(data.data).assertEqual("");
            done();
        }

        let commonEventSubscribeInfo = {
            events: ["@#￥#3243adsafdf_"],
        };

        Subscriber.createSubscriber(
            commonEventSubscribeInfo
        ).then((data)=>{
            console.info("===============ActsSubscriberTestUnorder_0200=========createSubscriber promise");
            commonEventSubscriber002 = data;
            data.getSubscribeInfo().then(()=>{
                console.info("===============ActsSubscriberTestUnorder_0200=========getSubscribeInfo promise");
                Subscriber.subscribe(commonEventSubscriber002, subscriberCallBack002);
                setTimeout(function (){
                    console.info("==========ActsSubscriberTestUnorder_0200 publish start============");
                    Subscriber.publish("@#￥#3243adsafdf_", publishCallback);
                }, 1000);
            });
        })
    })

    /*
     * @tc.number    : ActsSubscriberTestUnorder_0300
     * @tc.name      : verify subscribe and publish : Check subscribe and publish common event data
     *                 with publishInfo data
     * @tc.desc      : Check the subscriber can receive event "publish_event0300" type of the interface (by Promise)
     */
    it('ActsSubscriberTestUnorder_0300', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorder_0300==========================>");
        let commonEventSubscribeInfo = {
            events: ["publish_event0300"],
            publisherDeviceId: "PublishDeviceId0300",
            priority: 10,
        };

        let commonEventPublishData = {
            isOrdered: false,
            bundleName: "PublishBundleName0300",
            code: 55,
            data: "PublishData0300",
        }

        function isOrderedCommonEventCallback003(err, data) {
            console.info("==========================>isOrderedCommonEventCallback003");
            expect(data).assertEqual(false);
            done();
        }

        function subscriberCallBack003(err, data) {
            console.info("==========================>subscriberCallBack003");
            expect(data.event).assertEqual("publish_event0300");
            expect(data.bundleName).assertEqual("PublishBundleName0300");
            expect(data.code).assertEqual(55);
            expect(data.data).assertEqual("PublishData0300");
            commonEventSubscriber003.isOrderedCommonEvent(isOrderedCommonEventCallback003);
        }

        Subscriber.createSubscriber(
            commonEventSubscribeInfo
        ).then((data)=>{
            console.info("===============ActsSubscriberTestUnorder_0300=========createSubscriber promise");
            commonEventSubscriber003 = data;
            data.getSubscribeInfo().then(()=>{
                console.info("===============ActsSubscriberTestUnorder_0300=========getSubscribeInfo promise");
                Subscriber.subscribe(commonEventSubscriber003, subscriberCallBack003);
                setTimeout(function (){
                    console.info("==========ActsSubscriberTestUnorder_0300 publish start============");
                    Subscriber.publish("publish_event0300", commonEventPublishData, publishCallback);
                }, 1000);
            });
        })
    })

    /*
     * @tc.number    : ActsSubscriberTestUnorder_0400
     * @tc.name      : verify subscribe and publish : Check subscribe and publish sticky event
     * @tc.desc      : Check the subscriber can receive event "publish_event0400" type of the interface (by Promise)
     */
    it('ActsSubscriberTestUnorder_0400', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorder_0400==========================>");
        let commonEventSubscribeInfo = {
            events: ["publish_eventSticky0400"]
        };

        let commonEventPublishData = {
            isOrdered: false,
            isSticky: true,
        }

        function isStickyCommonEventCallback004(err, data) {
            console.info("==========================>isStickyCommonEventCallback004 isSticky:"+ data);
            expect(data).assertEqual(true);
            done();
        }

        function subscriberCallBack004(err, data) {
            console.info("==========================>subscriberCallBack0400");
            expect(data.event).assertEqual("publish_eventSticky0400");
            expect(data.bundleName).assertEqual("");
            expect(data.code).assertEqual(0);
            expect(data.data).assertEqual("");
            commonEventSubscriber004.isStickyCommonEvent(isStickyCommonEventCallback004);
        }

        Subscriber.createSubscriber(
            commonEventSubscribeInfo
        ).then((data)=>{
            console.info("===============ActsSubscriberTestUnorder_0400=========createSubscriber promise");
            commonEventSubscriber004 = data;
            data.getSubscribeInfo().then(()=>{
                console.info("===============ActsSubscriberTestUnorder_0400=========getSubscribeInfo promise");
                Subscriber.subscribe(commonEventSubscriber004, subscriberCallBack004);
                setTimeout(function (){
                    console.info("==========ActsSubscriberTestUnorder_0400 publish start============");
                    Subscriber.publish("publish_eventSticky0400", commonEventPublishData, publishCallback);
                }, 1000);
            });
        })
    })

    /*
     * @tc.number    : ActsSubscriberTestUnorder_0500
     * @tc.name      : verify subscribe and publish : Check the two different subscribe and one publish,
     *                 and check unsubscribe event
     * @tc.desc      : Check the subscriber can receive event "publish_event0500" type of the interface (by Promise)
     */
    it('ActsSubscriberTestUnorder_0500', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorder_0500==========================>");
        let commonEventSubscribeInfo = {
            events: ["publish_event0500"]
        };

        let commonEventPublishData = {
            isOrdered: false,
            isSticky: false,
        }

        function subscriberCallBack005(err, data) {
            console.info("==========================>subscriberCallBack0500");
            expect(data.event).assertEqual("publish_event0500");
            expect(data.bundleName).assertEqual("");
            expect(data.code).assertEqual(0);
            expect(data.data).assertEqual("");
            done();
        }

        Subscriber.createSubscriber(
            commonEventSubscribeInfo
        ).then((data)=>{
            console.info("===============ActsSubscriberTestUnorder_0500_1=========createSubscriber promise");
            commonEventSubscriber0051 = data;
            data.getSubscribeInfo().then(()=>{
                console.info("===============ActsSubscriberTestUnorder_0500_1=========getSubscribeInfo promise");
                Subscriber.subscribe(commonEventSubscriber0051, subscriberCallBack005);
            });
        })

        Subscriber.createSubscriber(
            commonEventSubscribeInfo
        ).then((data)=>{
            console.info("===============ActsSubscriberTestUnorder_0500_2=========createSubscriber promise");
            commonEventSubscriber0052 = data;
            data.getSubscribeInfo().then(()=>{
                console.info("===============ActsSubscriberTestUnorder_0500_2=========getSubscribeInfo promise");
                Subscriber.subscribe(commonEventSubscriber0052, subscriberCallBack005);
                Subscriber.unsubscribe(commonEventSubscriber0051, unsubscriberCallBack);
                setTimeout(function (){
                    console.info("==========ActsSubscriberTestUnorder_0500 publish start============");
                    Subscriber.publish("publish_event0500", commonEventPublishData, publishCallback);
                }, 1000);
            });
        })
    })

    /*
     * @tc.number    : ActsSubscriberTestUnorder_0600
     * @tc.name      : verify subscribe and publish : Check one subscribe and twice publish common events
     * @tc.desc      : Check the subscriber can receive event "publish_event0600" type of the interface (by Promise)
     */
    it('ActsSubscriberTestUnorder_0600', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorder_0600==========================>");
        let num = 0;
        let commonEventSubscribeInfo = {
            events: ["publish_event0600"]
        };

        let commonEventPublishData = {
            isOrdered: false,
            isSticky: false,
        }

        function subscriberCallBack006(err, data) {
            console.info("==========================>subscriberCallBack0600");
            expect(data.event).assertEqual("publish_event0600");
            expect(data.bundleName).assertEqual("");
            expect(data.code).assertEqual(0);
            expect(data.data).assertEqual("");
            console.info("==========================>subscriberCallBack0600 num = " + num);
            if (num == 0) {
                num++;
            } else if(num == 1) {
                num = 0;
                done();
            }
        }
        console.info("===============ActsSubscriberTestUnorder_0600 11111111111111==========================>");
        Subscriber.createSubscriber(
            commonEventSubscribeInfo
        ).then((data)=>{
            console.info("===============ActsSubscriberTestUnorder_0600=========createSubscriber promise");
            commonEventSubscriber006 = data;
            console.info("===============ActsSubscriberTestUnorder_0600 22222222222222==========================>");
            data.getSubscribeInfo().then(()=>{
                console.info("===============ActsSubscriberTestUnorder_0700=========getSubscribeInfo promise");
                Subscriber.subscribe(commonEventSubscriber006, subscriberCallBack006);
                setTimeout(function (){
                    console.info("==========ActsSubscriberTestUnorder_0600 publish start============");
                    Subscriber.publish("publish_event0600", commonEventPublishData, publishCallback);
                    Subscriber.publish("publish_event0600", commonEventPublishData, publishCallback);
                }, 1000);
            });
        })
    })

    /*
     * @tc.number    : ActsSubscriberTestUnorder_0800
     * @tc.name      : verify subscribe and publish : Check subscribe and publish common event
     *                 with permission and check IsOrderedCommonEvent
     * @tc.desc      : Check the subscriber can receive event "publish_event0800" type of the interface (by Promise)
     */
    it('ActsSubscriberTestUnorder_0800', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorder_0800==========================>");
        let commonEventSubscribeInfo = {
            events: ["publish_event0800"],
            publisherPermission:"publish_event0800_publisherPermission"
        };

        let commonEventPublishData = {
            subscriberPermissions:["publish_event0800_subscriberPermissions"],
            isOrdered: false,
            isSticky: false
        }

        function isOrderedCommonEventCallback009(err, data) {
            console.info("==========================>isOrderedCommonEventCallback008");
            expect(data).assertEqual(false);
            done();
        }

        function sleep(delay) {
            let start = (new Date()).getTime();
            while ((new Date()).getTime() - start < delay) {
            }
            done();
        }

        function subscriberCallBack008(err, data) {
            console.info("==========================>subscriberCallBack0800");
            expect(data.event).assertEqual("publish_event0800");
            expect(data.bundleName).assertEqual("");
            expect(data.code).assertEqual(0);
            expect(data.data).assertEqual("");
            expect().assertFail();
            commonEventSubscriber008.isOrderedCommonEvent(isOrderedCommonEventCallback009);
        }

        Subscriber.createSubscriber(
            commonEventSubscribeInfo
        ).then((data)=>{
            console.info("==========================>createSubscriberPromise0800=======================>");
            commonEventSubscriber008 = data;
            data.getSubscribeInfo().then(()=>{
                console.info("===============ActsSubscriberTestUnorder_0800=========getSubscribeInfo promise");
                Subscriber.subscribe(commonEventSubscriber008, subscriberCallBack008);
                setTimeout(function (){
                    console.debug("==========ActsSubscriberTestUnorder_0800 publish start============");
                    Subscriber.publish("publish_event0800", commonEventPublishData, publishCallback);
                }, 1000);
            });
        });

        sleep(1000);
    })

    /*
     * @tc.number    : ActsSubscriberTestUnorder_1000
     * @tc.name      : verify subscribe and publish : Check different subscribes and publish different common event
     * @tc.desc      : Check the subscriber can receive event "publish_event1000" type of the interface
     */
    it('ActsSubscriberTestUnorder_1000', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorder_1000==========================>");

        let num = 0;

        let commonEventSubscribeInfo1 = {
            events: ["publish_event1000_1"],
            priority: 5
        };

        let commonEventSubscribeInfo2 = {
            events: ["publish_event1000_2"],
            priority: 10
        };

        let commonEventPublishData1 = {
            bundleName: "publish_event1000_bundleName1",
            code: 3,
            data: "publish_event1000_data",
            isOrdered: false,
            isSticky: false,
        }

        let commonEventPublishData2 = {
            bundleName: "publish_event1000_bundleName2",
            code: 5,
            data: "publish_event1000_data",
            isOrdered: false,
            isSticky: false,
        }

        async function subscriberCallBack0101(err, data) {
            console.info("==========================>subscriberCallBack0101");
            console.info("==========================>subscriberCallBack0101 event:"+data.event);
            console.info("==========================>subscriberCallBack0101 bundleName:"+data.bundleName);
            expect(data.event).assertEqual("publish_event1000_1");
            expect(data.bundleName).assertEqual("publish_event1000_bundleName1");
            expect(data.code).assertEqual(3);
            expect(data.data).assertEqual("publish_event1000_data");
            if (num == 0) {
                num++;
            } else if (num == 1) {
                done();
            }
        }

        function subscriberCallBack0102(err, data) {
            console.info("==========================>subscriberCallBack0102");
            console.info("==========================>subscriberCallBack0102 event:"+data.event);
            console.info("==========================>subscriberCallBack0102 bundleName:"+data.bundleName);
            expect(data.event).assertEqual("publish_event1000_2");
            expect(data.bundleName).assertEqual("publish_event1000_bundleName2");
            expect(data.code).assertEqual(5);
            expect(data.data).assertEqual("publish_event1000_data");
            if (num == 0) {
                num++;
            } else if (num == 1) {
                done();
            }
        }

        Subscriber.createSubscriber(commonEventSubscribeInfo1).then((data)=>{
            console.info("=================ActsSubscriberTestUnorder_1000=========>createSubscriber Promise1000_1");
            commonEventSubscriber0101 = data;
            data.getSubscribeInfo().then(()=>{
                console.info("===============ActsSubscriberTestUnorder_1000=========getSubscribeInfo promise1000_1");
                Subscriber.subscribe(commonEventSubscriber0101, subscriberCallBack0101);
            });
        });

        Subscriber.createSubscriber(commonEventSubscribeInfo2).then((data)=>{
            console.info("=================ActsSubscriberTestUnorder_1000=========>createSubscriber Promise1000_2");
            commonEventSubscriber0102 = data;
            data.getSubscribeInfo().then(()=>{
                console.info("===============ActsSubscriberTestUnorder_1000=========getSubscribeInfo promise1000_2");
                Subscriber.subscribe(commonEventSubscriber0102, subscriberCallBack0102);
                setTimeout(function (){
                    console.info("==========ActsSubscriberTestUnorder_1000 publish start============");
                    Subscriber.publish("publish_event1000_1", commonEventPublishData1, publishCallback10001);
                    Subscriber.publish("publish_event1000_2", commonEventPublishData2, publishCallback10002);
                }, 1000);
            });
        });
    })

    /*
     * @tc.number    : ActsSubscriberTestUnorder_1100
     * @tc.name      : verify subscribe and publish : Check one subscribes and publish one common event
     * @tc.desc      : Check the subscriber can receive event "publish_event1100" type of the callback interface
     */
    it('ActsSubscriberTestUnorder_1100', 0, async function (done) {
        console.info("===============ActsSubscriberTestUnorder_1100==========================>");
        function subscriberCallBack011(err, data) {
            console.info("==========================>subscriberCallBack011");
            expect(data.event).assertEqual("publish_event1100");
            expect(data.bundleName).assertEqual("");
            expect(data.code).assertEqual(0);
            expect(data.data).assertEqual("");
            done();
        }

        let commonEventSubscribeInfo = {
            events: ["publish_event1100"],
        };

        function getSubscribeInfo1100CallBack(err, data) {
            console.info("==========================>getSubscribeInfo1100CallBack=======================>");
            expect(data.events[0]).assertEqual("publish_event1100");
            Subscriber.subscribe(commonEventSubscriber011, subscriberCallBack011);
            setTimeout(function (){
                console.debug("==========ActsSubscriberTestUnorder_1100 publish start============");
                Subscriber.publish("publish_event1100", publishCallback);
            }, 1000);
        }

        function createSubscriber1100CallBack(err, data) {
            console.info("==========================>createSubscriber1100CallBack=======================>");
            commonEventSubscriber011 = data;
            data.getSubscribeInfo(getSubscribeInfo1100CallBack);
        }

        Subscriber.createSubscriber(commonEventSubscribeInfo, createSubscriber1100CallBack);
    })

    /*
     * @tc.number    : ActsSubscriberTestUnorder_1200
     * @tc.name      : verify subscribe and publish : Check one subscribes and publish one common event with parameters
     * @tc.desc      : Check the subscriber can receive event "publish_event1200" type of the callback interface
     */
    it('ActsSubscriberTestUnorder_1200', 0, async function (done) {
       console.info("===============ActsSubscriberTestUnorder_1200==========================>");
       function subscriberCallBack012(err, data) {
           console.info("==========================>subscriberCallBack012");
           console.info("==========================>subscriberCallBack012 " + JSON.stringify(data));
           expect(data.event).assertEqual("publish_event1200");
           expect(data.bundleName).assertEqual("");
           expect(data.code).assertEqual(0);
           expect(data.data).assertEqual("");
           console.info("==========================>subscriberCallBack012:key1-value = "+ data.parameters["key1"]);
           console.info("==========================>subscriberCallBack012:key2-value = "+ data.parameters["key2"][0]);
           console.info("==========================>subscriberCallBack012:key2-value = "+ data.parameters["key2"][1]);
           console.info("==========================>subscriberCallBack012:key3-value = "+ data.parameters["key3"]);
           done();
       }

       let commonEventSubscribeInfo = {
           events: ["publish_event1200"],
       };

       let commonEventPublishData = {
           isOrdered: false,
           isSticky: false,
           parameters: {
               key1: "ActsSubscriberTestUnorder_1200_key",
               key2: [100,200],
               key3: 44
           }
       }

       function getSubscribeInfo1200CallBack(err, data) {
           console.info("==========================>getSubscribeInfo1200CallBack=======================>");
           expect(data.events[0]).assertEqual("publish_event1200");
           Subscriber.subscribe(commonEventSubscriber012, subscriberCallBack012);
           setTimeout(function (){
            console.info("==========ActsSubscriberTestUnorder_1200 publish start============");
            Subscriber.publish("publish_event1200", commonEventPublishData, publishCallback);
        }, 1000);
       }

       function createSubscriber1200CallBack(err, data) {
           console.info("==========================>createSubscriber1200CallBack=======================>");
           commonEventSubscriber012 = data;
           data.getSubscribeInfo(getSubscribeInfo1200CallBack);
       }

       Subscriber.createSubscriber(commonEventSubscribeInfo, createSubscriber1200CallBack);
   })
})

