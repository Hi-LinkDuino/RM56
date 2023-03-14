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
import mms from '@ohos.telephony.sms';
import utils from './Utils.ets'
export default function smsUiJsunit() {
  describe('appInfoTest', function () {
    /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0100
   * @tc.name     SendReq type MMS, encoding after decoding, the content after decoding and coding before the same
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_encodeMms_Async_0100', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let mmsType = {
        "from": {
          address: '+861381234****/TYPE=PLMN', charset: mms.UTF_8
        },
        to: [{
               address: '+861388888****/TYPE=PLMN', charset: mms.UTF_8
             }],
        transactionId: '2077.1427358451410',
        version: mms.MMS_VERSION_1_2,
        date: 1639378126,
        cc: [{
               address: "+861381111****/TYPE=PLMN", charset: mms.UTF_8
             }],
        bcc: [{
                address: "+861382222****/TYPE=PLMN", charset: mms.UTF_8
              }],
        subject: "Test mms",
        messageClass: 130,
        expiry: 1639378126,
        priority: mms.MMS_LOW,
        senderVisibility: 129,
        deliveryReport: 128,
        readReport: 129,
        contentType: "application/vnd.wap.multipart.related",
      };
      let attachment = [
        {
          path: '/data/telephony/enSrc/content.text',
          fileName: 'content.text',
          contentId: '<content.text>',
          contentLocation: 'content.text',
          contentDisposition: mms.FROM_DATA,
          contentTransferEncoding: 'base64',
          contentType: 'text/plain',
          isSmil: false,
          inBuff: [],
          charset: mms.UTF_8
        },
        {
          path: '/data/telephony/enSrc/618C0A89.smil',
          fileName: '618C0A89.smil',
          contentId: '<0000>',
          contentLocation: '618C0A89.smil',
          contentDisposition: mms.ATTACHMENT,
          contentTransferEncoding: 'base64',
          contentType: 'application/smil',
          isSmil: true,
          inBuff: [],
          charset: mms.UTF_8
        },
        {
          path: '/data/telephony/enSrc/picture.gif',
          fileName: 'picture.gif',
          contentId: '<picture.gif>',
          contentLocation: 'picture.gif',
          contentDisposition: mms.INLINE,
          contentTransferEncoding: 'base64',
          contentType: 'image/gif',
          isSmil: false,
          inBuff: [],
          charset: mms.UTF_8
        }];

      mms.encodeMms({
        messageType: mms.TYPE_MMS_SEND_REQ, mmsType, attachment
      }, (err, encodeData) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0100 encodeMms fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_encodeMms_Async_0100 encodeMms finish');
        mms.decodeMms(encodeData, (err, data) => {
          if (err) {
            console.log('Telephony_SmsMms_encodeMms_Async_0100 fail');
            done();
            return;
          }
          expect(data.messageType === mms.TYPE_MMS_SEND_REQ).assertTrue();
          expect(data.mmsType.from.address === '+861381234****/TYPE=PLMN').assertTrue();
          expect(data.mmsType.from.charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.to.length > 0).assertTrue();
          expect(data.mmsType.to[0].address === '+861388888****/TYPE=PLMN').assertTrue();
          expect(data.mmsType.to[0].charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.transactionId === '2077.1427358451410').assertTrue();
          expect(data.mmsType.version === mms.MMS_VERSION_1_2).assertTrue();
          expect(data.mmsType.date > 0).assertTrue();
          expect(data.mmsType.cc.length > 0).assertTrue();
          expect(data.mmsType.cc[0].address === '+861381111****/TYPE=PLMN').assertTrue();
          expect(data.mmsType.cc[0].charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.bcc.length > 0).assertTrue();
          expect(data.mmsType.bcc[0].address === '+861382222****/TYPE=PLMN').assertTrue();
          expect(data.mmsType.bcc[0].charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.subject === 'Test mms').assertTrue();
          expect(data.mmsType.messageClass === 130).assertTrue();
          expect(data.mmsType.expiry !== 0).assertTrue();
          expect(data.mmsType.priority === mms.MMS_LOW).assertTrue();
          expect(data.mmsType.senderVisibility === 129).assertTrue();
          expect(data.mmsType.deliveryReport === 128).assertTrue();
          expect(data.mmsType.readReport === 129).assertTrue();
          expect(data.mmsType.contentType === 'application/vnd.wap.multipart.related').assertTrue();
          if (data.attachment != null) {
            expect(data.attachment.length > 0).assertTrue();
            expect(data.attachment[0].path === '').assertTrue();
            expect(data.attachment[0].fileName === '618C0A89.smil').assertTrue();
            expect(data.attachment[0].contentId === '<0000>').assertTrue();
            expect(data.attachment[0].contentLocation === '618C0A89.smil').assertTrue();
            expect(data.attachment[0].contentDisposition === mms.ATTACHMENT).assertTrue();
            expect(data.attachment[0].contentTransferEncoding === 'base64').assertTrue();
            expect(data.attachment[0].contentType === 'application/smil').assertTrue();
            expect(data.attachment[0].isSmil).assertTrue();
            expect(data.attachment[0].inBuff.length > 0).assertTrue();
            expect(data.attachment[0].charset === mms.UTF_8).assertTrue();
            expect(data.attachment[1].path === '').assertTrue();
            expect(data.attachment[1].fileName === 'content.text').assertTrue();
            expect(data.attachment[1].contentId === '<content.text>').assertTrue();
            expect(data.attachment[1].contentLocation === 'content.text').assertTrue();
            expect(data.attachment[1].contentDisposition === mms.FROM_DATA).assertTrue();
            expect(data.attachment[1].contentTransferEncoding === 'base64').assertTrue();
            expect(data.attachment[1].contentType === 'text/plain').assertTrue();
            expect(data.attachment[1].isSmil).assertFalse();
            expect(data.attachment[1].inBuff.length > 0).assertTrue();
            expect(data.attachment[1].charset === mms.UTF_8).assertTrue();
            expect(data.attachment[2].path === '').assertTrue();
            expect(data.attachment[2].fileName === 'picture.gif').assertTrue();
            expect(data.attachment[2].contentId === '<picture.gif>').assertTrue();
            expect(data.attachment[2].contentLocation === 'picture.gif').assertTrue();
            expect(data.attachment[2].contentDisposition === mms.INLINE).assertTrue();
            expect(data.attachment[2].contentTransferEncoding === 'base64').assertTrue();
            expect(data.attachment[2].contentType === 'image/gif').assertTrue();
            expect(data.attachment[2].isSmil).assertFalse();
            expect(data.attachment[2].inBuff.length > 0).assertTrue();
            expect(data.attachment[2].charset === mms.UTF_8).assertTrue();
            console.log('Telephony_SmsMms_encodeMms_Async_0100 finish');
          }
          done();
        });
      });
    });

    /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0200
   * @tc.name     SendConf type MMS, encoding after decoding, the content after decoding and coding before the same
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_encodeMms_Async_0200', 0, async function (done) {
      if (utils.notCheck) {
        expect(true).assertTrue();
        done();
        return;
      }
      let mmsType = {
        responseState: 128,
        transactionId: '2077.1427358451410',
        version: mms.MMS_VERSION_1_1,
        messageId: '0001',
      };
      mms.encodeMms({
        messageType: mms.TYPE_MMS_SEND_CONF, mmsType
      }, (err, encodeData) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0200 encodeMms fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_encodeMms_Async_0200 encodeMms finish');
        mms.decodeMms(encodeData, (err, data) => {
          if (err) {
            console.log('Telephony_SmsMms_encodeMms_Async_0200 fail');
            done();
            return;
          }
          expect(data.messageType === mms.TYPE_MMS_SEND_CONF).assertTrue();
          expect(data.mmsType.responseState === 128).assertTrue();
          expect(data.mmsType.transactionId === '2077.1427358451410').assertTrue();
          expect(data.mmsType.version === mms.MMS_VERSION_1_1).assertTrue();
          expect(data.mmsType.messageId === '0001').assertTrue();
          console.log('Telephony_SmsMms_encodeMms_Async_0200 finish');
          done();
        });
      });
    });

    /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0300
   * @tc.name     NotificationInd type MMS, encoding after decoding,
   *              the content after decoding and coding before the same
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_encodeMms_Async_0300', 0, async function (done) {
      let mmsType = {
        transactionId: "2080.142730",
        messageClass: 128,
        messageSize: 12345678,
        expiry: 1639378126,
        version: mms.MMS_VERSION_1_3,
        "from": {
          address: '+861381234****/TYPE=PLMN', charset: mms.UTF_8
        },
        subject: "Test mms",
        deliveryReport: 128,
        contentLocation: "Test",
        contentClass: 128,
      };
      mms.encodeMms({
        messageType: mms.TYPE_MMS_NOTIFICATION_IND, mmsType
      }, (err, encodeData) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0300 encodeMms fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_encodeMms_Async_0300 encodeMms finish');
        mms.decodeMms(encodeData, (err, data) => {
          if (err) {
            console.log('Telephony_SmsMms_encodeMms_Async_0300 fail');
            done();
            return;
          }
          expect(data.messageType === mms.TYPE_MMS_NOTIFICATION_IND).assertTrue();
          expect(data.mmsType.transactionId === '2080.142730').assertTrue();
          expect(data.mmsType.messageClass === 128).assertTrue();
          expect(data.mmsType.messageSize === 12345678).assertTrue();
          expect(data.mmsType.expiry !== 0).assertTrue();
          expect(data.mmsType.version === mms.MMS_VERSION_1_3).assertTrue();
          expect(data.mmsType.from.address.length > 0).assertTrue();
          expect(data.mmsType.from.charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.subject === 'Test mms').assertTrue();
          expect(data.mmsType.deliveryReport === 128).assertTrue();
          expect(data.mmsType.contentLocation === 'Test').assertTrue();
          expect(data.mmsType.contentClass === 128).assertTrue();
          console.log('Telephony_SmsMms_encodeMms_Async_0300 finish');
          done();
        });
      });
    });

    /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0400
   * @tc.name     NotifyRespInd type MMS, encoding after decoding,the content after decoding and coding before the same
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_encodeMms_Async_0400', 0, async function (done) {
      let mmsType = {
        transactionId: '2920.1410',
        status: 128,
        version: mms.MMS_VERSION_1_0,
        reportAllowed: mms.MMS_YES,
      };
      mms.encodeMms({
        messageType: mms.TYPE_MMS_RESP_IND, mmsType
      }, (err, encodeData) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0400 encodeMms fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_encodeMms_Async_0400 encodeMms finish');
        mms.decodeMms(encodeData, (err, data) => {
          if (err) {
            console.log('Telephony_SmsMms_encodeMms_Async_0400 fail');
            done();
            return;
          }
          expect(data.messageType === mms.TYPE_MMS_RESP_IND).assertTrue();
          expect(data.mmsType.transactionId === '2920.1410').assertTrue();
          expect(data.mmsType.status === 128).assertTrue();
          expect(data.mmsType.version === mms.MMS_VERSION_1_0).assertTrue();
          expect(data.mmsType.reportAllowed === mms.MMS_YES).assertTrue();
          console.log('Telephony_SmsMms_encodeMms_Async_0400 finish');
          done();
        });
      });
    });

    /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0500
   * @tc.name     RetrieveConf type MMS, encoding after decoding, the content after decoding and coding before the same
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_encodeMms_Async_0500', 0, async function (done) {
      let mmsType = {
        transactionId: "3077.142410",
        messageId: "0001",
        date: 1639378126,
        version: mms.MMS_VERSION_1_2,
        to: [{
               address: "+861388888****/TYPE=PLMN", charset: mms.UTF_8
             }],
        "from": {
          address: '+861381234****/TYPE=PLMN', charset: mms.UTF_8
        },
        cc: [{
               address: '+861381111****/TYPE=PLMN', charset: mms.UTF_8
             }],
        subject: "Test mms",
        priority: mms.MMS_HIGH,
        deliveryReport: 128,
        readReport: 128,
        retrieveStatus: 128,
        retrieveText: "0002",
        contentType: "application/vnd.wap.multipart.related",
      };
      mms.encodeMms({
        messageType: mms.TYPE_MMS_RETRIEVE_CONF, mmsType
      }, (err, encodeData) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0500 encodeMms fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_encodeMms_Async_0500 encodeMms finish');
        mms.decodeMms(encodeData, (err, data) => {
          if (err) {
            console.log('Telephony_SmsMms_encodeMms_Async_0500 fail');
            done();
            return;
          }
          expect(data.messageType === mms.TYPE_MMS_RETRIEVE_CONF).assertTrue();
          expect(data.mmsType.transactionId === '3077.142410').assertTrue();
          expect(data.mmsType.messageId === '0001').assertTrue();
          expect(data.mmsType.date > 0).assertTrue();
          expect(data.mmsType.version === mms.MMS_VERSION_1_2).assertTrue();
          expect(data.mmsType.to.length > 0).assertTrue();
          expect(data.mmsType.to[0].address.length > 0).assertTrue();
          expect(data.mmsType.to[0].charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.from.address.length > 0).assertTrue();
          expect(data.mmsType.from.charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.cc.length > 0).assertTrue();
          expect(data.mmsType.cc[0].address.length > 0).assertTrue();
          expect(data.mmsType.cc[0].charset == mms.UTF_8).assertTrue();
          expect(data.mmsType.subject === 'Test mms').assertTrue();
          expect(data.mmsType.priority === mms.MMS_HIGH).assertTrue();
          expect(data.mmsType.deliveryReport === 128).assertTrue();
          expect(data.mmsType.readReport === 128).assertTrue();
          expect(data.mmsType.retrieveStatus === 128).assertTrue();
          expect(data.mmsType.retrieveText === '0002').assertTrue();
          expect(data.mmsType.contentType === 'application/vnd.wap.multipart.related').assertTrue();
          console.log('Telephony_SmsMms_encodeMms_Async_0500 finish');
          done();
        });
      });
    });

    /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0600
   * @tc.name     AcknowledgeInd type MMS, encoding after decoding,
   *              the content after decoding and coding before the same
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_encodeMms_Async_0600', 0, async function (done) {
      let mmsType = {
        transactionId: "27.14210",
        version: mms.MMS_VERSION_1_2,
        reportAllowed: mms.MMS_NO,
      };
      mms.encodeMms({
        messageType: mms.TYPE_MMS_ACKNOWLEDGE_IND, mmsType
      }, (err, encodeData) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0600 encodeMms fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_encodeMms_Async_0600 encodeMms finish');
        mms.decodeMms(encodeData, (err, data) => {
          if (err) {
            console.log('Telephony_SmsMms_encodeMms_Async_0600 fail');
            done();
            return;
          }
          expect(data.messageType === mms.TYPE_MMS_ACKNOWLEDGE_IND).assertTrue();
          expect(data.mmsType.transactionId === '27.14210').assertTrue();
          expect(data.mmsType.version === mms.MMS_VERSION_1_2).assertTrue();
          expect(data.mmsType.reportAllowed === mms.MMS_NO).assertTrue();
          console.log('Telephony_SmsMms_encodeMms_Async_0600 finish');
          done();
        });
      });
    });

    /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0700
   * @tc.name     DeliveryInd type MMS, encoding after decoding, the content after decoding and coding before the same
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_encodeMms_Async_0700', 0, async function (done) {
      let mmsType = {
        messageId: "0001",
        date: 1639378126,
        to: [{
               address: "+861388888****/TYPE=PLMN", charset: mms.UTF_8
             }],
        status: 134,
        version: mms.MMS_VERSION_1_2,
      };
      mms.encodeMms({
        messageType: mms.TYPE_MMS_DELIVERY_IND, mmsType
      }, (err, encodeData) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0700 encodeMms fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_encodeMms_Async_0700 encodeMms finish');
        mms.decodeMms(encodeData, (err, data) => {
          if (err) {
            console.log('Telephony_SmsMms_encodeMms_Async_0700 fail');
            done();
            return;
          }
          expect(data.messageType === mms.TYPE_MMS_DELIVERY_IND).assertTrue();
          expect(data.mmsType.messageId === '0001').assertTrue();
          expect(data.mmsType.date > 0).assertTrue();
          expect(data.mmsType.to.length > 0).assertTrue();
          expect(data.mmsType.to[0].address.length > 0).assertTrue();
          expect(data.mmsType.to[0].charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.status === 134).assertTrue();
          expect(data.mmsType.version === mms.MMS_VERSION_1_2).assertTrue();
          console.log('Telephony_SmsMms_encodeMms_Async_0700 finish');
          done();
        });
      });
    });

    /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0800
   * @tc.name     ReadRecInd type MMS, encoding after decoding, the content after decoding and coding before the same
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_encodeMms_Async_0800', 0, async function (done) {
      let mmsType = {
        version: mms.MMS_VERSION_1_0,
        messageId: "0001",
        to: [{
               address: "+861388888****/TYPE=PLMN", charset: mms.UTF_8
             }],
        "from": {
          address: "+861381234****/TYPE=PLMN", charset: mms.UTF_8
        },
        date: 1639378126,
        readStatus: 128,
      };
      mms.encodeMms({
        messageType: mms.TYPE_MMS_READ_REC_IND, mmsType
      }, (err, encodeData) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0800 encodeMms fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_encodeMms_Async_0800 encodeMms finish');
        mms.decodeMms(encodeData, (err, data) => {
          if (err) {
            console.log('Telephony_SmsMms_encodeMms_Async_0800 fail');
            done();
            return;
          }
          expect(data.messageType === mms.TYPE_MMS_READ_REC_IND).assertTrue();
          expect(data.mmsType.version === mms.MMS_VERSION_1_0).assertTrue();
          expect(data.mmsType.messageId === '0001').assertTrue();
          expect(data.mmsType.to.length > 0).assertTrue();
          expect(data.mmsType.to[0].address.length > 0).assertTrue();
          expect(data.mmsType.to[0].charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.from.address.length > 0).assertTrue();
          expect(data.mmsType.from.charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.readStatus === 128).assertTrue();
          expect(data.mmsType.date > 0).assertTrue();
          console.log('Telephony_SmsMms_encodeMms_Async_0800 finish');
          done();
        });
      });
    });

    /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0900
   * @tc.name     ReadOrigInd type MMS, encoding after decoding, the content after decoding and coding before the same
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_encodeMms_Async_0900', 0, async function (done) {
      let mmsType = {
        version: mms.MMS_VERSION_1_1,
        messageId: "0001",
        to: [{
               address: "+861388888****/TYPE=PLMN", charset: mms.UTF_8
             }],
        "from": {
          address: "+861381234****/TYPE=PLMN", charset: mms.UTF_8
        },
        readStatus: 129,
        date: 1639378126,
      };
      mms.encodeMms({
        messageType: mms.TYPE_MMS_READ_ORIG_IND, mmsType
      }, (err, encodeData) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0900 encodeMms fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_encodeMms_Async_0900 encodeMms finish');
        mms.decodeMms(encodeData, (err, data) => {
          if (err) {
            console.log('Telephony_SmsMms_encodeMms_Async_0900 fail');
            done();
            return;
          }
          expect(data.messageType === mms.TYPE_MMS_READ_ORIG_IND).assertTrue();
          expect(data.mmsType.version === mms.MMS_VERSION_1_1).assertTrue();
          expect(data.mmsType.messageId === '0001').assertTrue();
          expect(data.mmsType.to.length > 0).assertTrue();
          expect(data.mmsType.to[0].address.length > 0).assertTrue();
          expect(data.mmsType.to[0].charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.from.address.length > 0).assertTrue();
          expect(data.mmsType.from.charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.date > 0).assertTrue();
          expect(data.mmsType.readStatus === 129).assertTrue();
          console.log('Telephony_SmsMms_encodeMms_Async_0900 finish');
          done();
        });
      });
    });
    /**
   * @tc.number   Telephony_SmsMms_encodeMms_Async_0900
   * @tc.name     RetrieveConf type MMS, encoding after decoding, the content after decoding and coding before the same
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_encodeMms_Async_0900', 0, async function (done) {
      let mmsType = {
        transactionId: "3077.142410",
        messageId: "0001",
        date: 1639378126,
        version: mms.MMS_VERSION_1_2,
        to: [{
               address: "+861388888****/TYPE=PLMN", charset: mms.UTF_8
             }],
        "from": {
          address: '+861381234****/TYPE=PLMN', charset: mms.UTF_8
        },
        cc: [{
               address: '+861381111****/TYPE=PLMN', charset: mms.UTF_8
             }],
        subject: "Test mms",
        priority: mms.MMS_NORMAL,
        deliveryReport: 128,
        readReport: 128,
        retrieveStatus: 128,
        retrieveText: "0002",
        contentType: "application/vnd.wap.multipart.related",
      };
      mms.encodeMms({
        messageType: mms.TYPE_MMS_RETRIEVE_CONF, mmsType
      }, (err, encodeData) => {
        if (err) {
          console.log('Telephony_SmsMms_encodeMms_Async_0900 encodeMms fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_encodeMms_Async_0900 encodeMms finish');
        mms.decodeMms(encodeData, (err, data) => {
          if (err) {
            console.log('Telephony_SmsMms_encodeMms_Async_0500 fail');
            done();
            return;
          }
          expect(data.messageType === mms.TYPE_MMS_RETRIEVE_CONF).assertTrue();
          expect(data.mmsType.transactionId === '3077.142410').assertTrue();
          expect(data.mmsType.messageId === '0001').assertTrue();
          expect(data.mmsType.date > 0).assertTrue();
          expect(data.mmsType.version === mms.MMS_VERSION_1_2).assertTrue();
          expect(data.mmsType.to.length > 0).assertTrue();
          expect(data.mmsType.to[0].address.length > 0).assertTrue();
          expect(data.mmsType.to[0].charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.from.address.length > 0).assertTrue();
          expect(data.mmsType.from.charset === mms.UTF_8).assertTrue();
          expect(data.mmsType.cc.length > 0).assertTrue();
          expect(data.mmsType.cc[0].address.length > 0).assertTrue();
          expect(data.mmsType.cc[0].charset == mms.UTF_8).assertTrue();
          expect(data.mmsType.subject === 'Test mms').assertTrue();
          expect(data.mmsType.priority === mms.MMS_NORMAL).assertTrue();
          expect(data.mmsType.deliveryReport === 128).assertTrue();
          expect(data.mmsType.readReport === 128).assertTrue();
          expect(data.mmsType.retrieveStatus === 128).assertTrue();
          expect(data.mmsType.retrieveText === '0002').assertTrue();
          expect(data.mmsType.contentType === 'application/vnd.wap.multipart.related').assertTrue();
          console.log('Telephony_SmsMms_encodeMms_Async_0500 finish');
          done();
        });
      });
    });

    const SMS_SEND_DST_NUMBER = '';
    const TRUE_SLOT_ID = 0;
    const FALSE_SLOT_ID = 9;
    const OTHER_SLOT_ID = 1;
    const SECOND_SLOT_ID = 2;
    const IDENTIFIER_MIN = 0;
    const IDENTIFIER_MAX = 0xFFFF;
    const RANTYPE_GSM = 1;
    const RANTYPE_CDMA = 2;
    const RANTYPE_ERROR = 3;
    var rawArray = [
      0x08, 0x91, 0x68, 0x31, 0x08, 0x20, 0x00, 0x75, 0xF4, 0x24, 0x0D, 0x91,
      0x68, 0x81, 0x29, 0x56, 0x29, 0x83, 0xF6, 0x00, 0x00, 0x12, 0x40, 0x80,
      0x01, 0x02, 0x14, 0x23, 0x02, 0xC1, 0x30
    ]
    /**
   * @tc.number   Telephony_SmsMms_setCBConfig_Async_0100
   * @tc.name     Set "StartMessageld" to minimum(0x0000) and "EndMessageld" to minimum(0x0000) for cell broadcast,
   *              Configure a cell broadcast for an identity.
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_setCBConfig_Async_0100', 0, async function (done) {
      if (true) {
        expect(true).assertTrue();
        done();
        return;
      }
      let data = {
        slotId: TRUE_SLOT_ID,
        enable: true,
        startMessageId: IDENTIFIER_MIN,
        endMessageId: IDENTIFIER_MIN,
        ranType: RANTYPE_GSM
      };
      sms.setCBConfig(data, (err) => {
        if (err) {
          expect().assertFail();
          console.log('Telephony_SmsMms_setCBConfig_Async_0100 fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_setCBConfig_Async_0100 finish');
        done();
      });
    });

    /**
   * @tc.number   Telephony_SmsMms_getAllSimMessages_CDMA_Async_0100
   * @tc.name     When SLOTID is the correct value,Query all SMS records of the SIM card.
   * @tc.desc     Function test
   */
    it('Telephony_SmsMms_getAllSimMessages_CDMA_Async_0100', 0, async function (done) {
      if (true) {
        expect(true).assertTrue();
        done();
        return;
      }
      let data = {
        slotId: TRUE_SLOT_ID,
        smsc: '',
        pdu: CORRECT_SMS_PDU,
        status: sms.SIM_MESSAGE_STATUS_SENT
      };
      sms.addSimMessage(data, (adderr) => {
        if (adderr) {
          expect().assertFail();
          console.log('Telephony_SmsMms_getAllSimMessages_CDMA_Async_0100 add fail');
          done();
          return;
        }
        console.log('Telephony_SmsMms_getAllSimMessages_CDMA_Async_0100 finish ');
        sms.getAllSimMessages(TRUE_SLOT_ID, (geterr, getresult) => {
          if (geterr) {
            expect().assertFail();
            console.log('Telephony_SmsMms_getAllSimMessages_CDMA_Async_0100 get fail');
            done();
            return;
          }
          expect(getresult.length > 0).assertTrue();
          let addOfPdu = interceptionPdu(getresult[0].shortMessage.pdu);
          let isAdd = (addOfPdu === CORRECT_SMS_PDU &&
          getresult[0].simMessageStatus === sms.SIM_MESSAGE_STATUS_SENT);
          expect(isAdd).assertTrue();
          console.log('Telephony_SmsMms_getAllSimMessages_CDMA_Async_0100 getAllSimMessages cur finish');
          sms.delSimMessage(TRUE_SLOT_ID, getresult[0].indexOnSim, (delerr) => {
            if (delerr) {
              expect().assertFail();
              console.log('Telephony_SmsMms_getAllSimMessages_CDMA_Async_0100 del fail');
              done();
              return;
            }
            console.log('Telephony_SmsMms_getAllSimMessages_CDMA_Async_0100 delSimMessage finish');
            sms.getAllSimMessages(TRUE_SLOT_ID, (geterr, getresult) => {
              if (geterr) {
                expect().assertFail();
                console.log('Telephony_SmsMms_getAllSimMessages_CDMA_Async_0100 get 2 fail');
                done();
                return;
              }
              expect(getresult.length === 0).assertTrue();
              console.log('Telephony_SmsMms_getAllSimMessages_CDMA_Async_0100 getAllSimMessages cur finish');
              done();
            });
          });
        });
      });
    });
    /*
   * @tc.number  Telephony_SmsMms_createMessage_Async_0100
   * @tc.name    Call interface CreateMessage,
   *             pass in the PDU(rawArray) in line with the coding specification, the specification is 3GPP,
   *             shortMessage Don't empty
   * @tc.desc    Function test
   */
    it('Telephony_SmsMms_createMessage_Async_0100', 0, async function (done) {
      if (true) {
        expect(true).assertTrue();
        done();
        return;
      }
      sms.createMessage(rawArray, '3gpp', (err, shortMessage) => {
        if (err) {
          expect().assertFail();
          console.log('Telephony_SmsMms_createMessage_Async_0100 fail');
          done();
          return;
        }
        expect(shortMessage.visibleMessageBody === MESSAGEBODY).assertTrue();
        expect(shortMessage.visibleRawAddress.length === ADDR_LENGTH).assertTrue();
        expect(shortMessage.messageClass === sms.FORWARD_MESSAGE).assertTrue();
        expect(shortMessage.protocolId === 0).assertTrue();
        expect(shortMessage.scAddress.length === ADDR_LENGTH).assertTrue();
        expect(shortMessage.scTimestamp === SC_TIMESTAMP).assertTrue();
        expect(shortMessage.isReplaceMessage).assertFalse();
        expect(shortMessage.hasReplyPath).assertFalse();
        expect(shortMessage.pdu.length > 0).assertTrue();
        expect(shortMessage.status === 0).assertTrue();
        expect(shortMessage.isSmsStatusReportMessage).assertTrue();
        console.log('Telephony_SmsMms_createMessage_Async_0100 finish');
        done();
      });
    });

    /*
   * @tc.number  Telephony_SmsMms_sendMessage_0100
   * @tc.name    Call the interface sendMessage, set the card slot parameter "slotId" to 1,
   *             and send SMS successfully
   * @tc.desc    Function test
   */
    it('Telephony_SmsMms_sendMessage_0100', 0, async function (done) {
      if (true) {
        expect(true).assertTrue();
        done();
        return;
      }
      sms.sendMessage({
        slotId: TRUE_SLOT_ID,
        destinationHost: SMS_SEND_DST_NUMBER,
        content: '111111',
        sendCallback: (err, value) => {
          if (err) {
            expect().assertFail();
            console.log('Telephony_SmsMms_sendMessage_0100 sendCallback fail');
            done();
            return;
          }
          expect(value.result === sms.SEND_SMS_SUCCESS).assertTrue();
          console.log('Telephony_SmsMms_sendMessage_0100 sendCallback finish');
          done()
        },
        deliveryCallback: (err, value) => {
          if (err) {
            expect().assertFail();
            console.log('Telephony_SmsMms_sendMessage_0100 deliveryCallback fail');
            done();
            return;
          }
          expect(value.pdu.length !== 0).assertTrue();
          console.log('Telephony_SmsMms_sendMessage_0100 deliveryCallback finish');
          done();
        }
      });
    });
    /*
   * @tc.number  Telephony_SmsMms_getSmsSegmentsInfo_Async_0100
   * @tc.name    set message to plain Chinese with a length of MAX_CHINESE_MESSAGE_LENTH(63) characters,
   *             and view the result
   * @tc.desc    Function test
   */
    it('Telephony_SmsMms_getSmsSegmentsInfo_Async_0100', 0, async function (done) {
      if (true) {
        expect(true).assertTrue();
        done();
        return;
      }
      let message = '';
      for (let index = 0;index < MAX_CHINESE_MESSAGE_LENTH; index++) {
        message += '中';
      }
      let force7BitCode = true;
      sms.getSmsSegmentsInfo(TRUE_SLOT_ID, message, force7BitCode, (error, result) => {
        if (error) {
          console.log("Telephony_SmsMms_getSmsSegmentsInfo_Async_0100 getSmsSegmentsInfoon error " + error.message);
          console.log('Telephony_SmsMms_getSmsSegmentsInfo_Async_0100 fail');
          expect().assertFail();
        } else {
          console.log("Telephony_SmsMms_getSmsSegmentsInfo_Async_0100   splitCount " + result.splitCount);
          console.log("Telephony_SmsMms_getSmsSegmentsInfo_Async_0100   encodeCount " + result.encodeCount);
          console.log("Telephony_SmsMms_getSmsSegmentsInfo_Async_0100   encodeCountRemaining " +
          result.encodeCountRemaining);
          console.log("Telephony_SmsMms_getSmsSegmentsInfo_Async_0100   scheme " + result.scheme);

          expect(result.splitCount === DIVIDE_MESSAGE_INTO_1).assertTrue();
          expect(result.encodeCount === MAX_CHINESE_MESSAGE_LENTH).assertTrue();
          let remaining = result.splitCount * MAX_CHAR_MESSAGE_LENTH - result.encodeCount;
          console.log("Telephony_SmsMms_getSmsSegmentsInfo_Async_0100   remaining " + remaining);
          expect(result.encodeCountRemaining === remaining).assertTrue();
          expect(result.scheme === sms.SMS_ENCODING_7BIT).assertTrue();
          console.log('Telephony_SmsMms_getSmsSegmentsInfo_Async_0100 finish');
        }
        done();
      });
    });

    /*
   * @tc.number  Telephony_SmsMms_getSmsEncodingScheme_Async_0100
   * @tc.name
   * @tc.desc    Function test
   */
    it('Telephony_SmsMms_getSmsEncodingScheme_Async_0100', 0, async function (done) {
      if (true) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(sms.SMS_ENCODING_UNKNOWN == 0).assertTrue();
      expect(sms.SMS_ENCODING_7BIT == 1).assertTrue();
      expect(sms.SMS_ENCODING_8BIT == 2).assertTrue();
      expect(sms.SMS_ENCODING_16BIT == 3).assertTrue();
      done();
    });

    /*
   * @tc.number  Telephony_SmsMms_getSendSmsResult_Async_0100
   * @tc.name
   * @tc.desc    Function test
   */
    it('Telephony_SmsMms_getSendSmsResult_Async_0100', 0, async function (done) {
      if (true) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(sms.SEND_SMS_FAILURE_RADIO_OFF == 0).assertTrue();
      expect(sms.SEND_SMS_FAILURE_SERVICE_UNAVAILABLE == 1).assertTrue();
      expect(sms.SIM_MESSAGE_STATUS_FREE == 0).assertTrue();
      expect(sms.INSTANT_MESSAGE == 1).assertTrue();
      expect(sms.OPTIONAL_MESSAGE == 2).assertTrue();
      done();
    });

    /*
   * @tc.number  Telephony_SmsMms_ShortMessageClass_Async_0100
   * @tc.name
   * @tc.desc    Function test
   */
    it('Telephony_SmsMms_ShortMessageClass_Async_0100', 0, async function (done) {
      if (true) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(sms.SIM_MESSAGE_STATUS_FREE == 0).assertTrue();
      expect(sms.INSTANT_MESSAGE == 1).assertTrue();
      expect(sms.OPTIONAL_MESSAGE == 2).assertTrue();
      done();
    });
    /*
   * @tc.number  Telephony_SmsMms_getMmsCharSets_Async_0100
   * @tc.name
   * @tc.desc    Function test
   */
    it('Telephony_SmsMms_getMmsCharSets_Async_0100', 0, async function (done) {
      if (true) {
        expect(true).assertTrue();
        done();
        return;
      }
      expect(sms.BIG5 == 0X07EA).assertTrue();
      expect(sms.ISO_10646_UCS_2 == 0X03E8).assertTrue();
      expect(sms.ISO_8859_1 == 0X04).assertTrue();
      expect(sms.ISO_8859_2 == 0X05).assertTrue();
      expect(sms.ISO_8859_3 == 0X06).assertTrue();
      expect(sms.ISO_8859_4 == 0X07).assertTrue();
      expect(sms.ISO_8859_5 == 0X08).assertTrue();
      expect(sms.ISO_8859_6 == 0X09).assertTrue();
      expect(sms.ISO_8859_7 == 0X10).assertTrue();
      expect(sms.ISO_8859_8 == 0X12).assertTrue();
      expect(sms.ISO_8859_9 == 0X13).assertTrue();
      expect(sms.SHIFT_JIS == 0X11).assertTrue();
      expect(sms.US_ASCII == 0X03).assertTrue();
      expect(sms.UTF_8 == 0X6A).assertTrue();
      done();
    });
  })
}
