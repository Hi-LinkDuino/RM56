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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import contact from '@ohos.contact';
import utils from '../Utils.ets'

export default function contacterJsunit() {
  describe('ContacterTest',function () {
    /**
     * @tc.number  Telephony_contact_Contact_0100
     * @tc.name    Test The  Contact enum
     * @tc.desc    Function test
     */

    it('Telephony_contact_Contact_0100', 0, async function (done) {
      console.info("Telephony_contact_Contact_0100 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(contact.Contact.INVALID_CONTACT_ID === -1).assertTrue();
      expect(contact.Contact.id === 0).assertTrue();
      expect(contact.Contact.key !== "test").assertTrue();
      console.info("Telephony_contact_Contact_0100 end")
      done();
    });

    /**
     * @tc.number  Telephony_contact_Contact_0200
     * @tc.name    Test The  Contact enum
     * @tc.desc    Function test
     */

    it('Telephony_contact_Contact_0200', 0, async function (done) {
      console.info("Telephony_contact_Contact_0200 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(contact.Contact.INVALID_CONTACT_ID === -1).assertTrue();
      expect(contact.Contact.id === 1).assertTrue();
      expect(contact.Contact.key !== "key").assertTrue();
      console.info("Telephony_contact_Contact_0200 end")
      done();
    });

    /**
     * @tc.number  Telephony_contact_Attribute_0100
     * @tc.name    Test The  Attribute enum
     * @tc.desc    Function test
     */
    it('Telephony_contact_Attribute_0100', 0, async function (done) {
      console.info("Telephony_contact_Attribute_0100 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(contact.Attribute.ATTR_CONTACT_EVENT === 0).assertTrue();
      expect(contact.Attribute.ATTR_EMAIL === 0).assertTrue();
      expect(contact.Attribute.ATTR_GROUP_MEMBERSHIP === 0).assertTrue();
      expect(contact.Attribute.ATTR_IM === 0).assertTrue();
      expect(contact.Attribute.ATTR_NAME === 0).assertTrue();
      expect(contact.Attribute.ATTR_NICKNAME === 0).assertTrue();
      expect(contact.Attribute.ATTR_NOTE === 0).assertTrue();
      expect(contact.Attribute.ATTR_ORGANIZATION === 0).assertTrue();
      expect(contact.Attribute.ATTR_PHONE === 0).assertTrue();
      expect(contact.Attribute.ATTR_PORTRAIT === 0).assertTrue();
      expect(contact.Attribute.ATTR_POSTAL_ADDRESS === 0).assertTrue();
      expect(contact.Attribute.ATTR_RELATION === 0).assertTrue();
      expect(contact.Attribute.ATTR_SIP_ADDRESS === 0).assertTrue();
      expect(contact.Attribute.ATTR_WEBSITE === 0).assertTrue();
      console.info("Telephony_contact_Attribute_0100 end")
      done();
    });

    /**
     * @tc.number  Telephony_contact_Attribute_0200
     * @tc.name    Test The  Attribute enum
     * @tc.desc    Function test
     */
    it('Telephony_contact_Attribute_0200', 0, async function (done) {
      console.info("Telephony_contact_Attribute_0100 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(contact.Attribute.ATTR_CONTACT_EVENT === 1).assertTrue();
      expect(contact.Attribute.ATTR_EMAIL === 1).assertTrue();
      expect(contact.Attribute.ATTR_GROUP_MEMBERSHIP === 1).assertTrue();
      expect(contact.Attribute.ATTR_IM === 1).assertTrue();
      expect(contact.Attribute.ATTR_NAME === 1).assertTrue();
      expect(contact.Attribute.ATTR_NICKNAME === 1).assertTrue();
      expect(contact.Attribute.ATTR_NOTE === 1).assertTrue();
      expect(contact.Attribute.ATTR_ORGANIZATION === 1).assertTrue();
      expect(contact.Attribute.ATTR_PHONE === 1).assertTrue();
      expect(contact.Attribute.ATTR_PORTRAIT === 1).assertTrue();
      expect(contact.Attribute.ATTR_POSTAL_ADDRESS === 1).assertTrue();
      expect(contact.Attribute.ATTR_RELATION === 1).assertTrue();
      expect(contact.Attribute.ATTR_SIP_ADDRESS === 1).assertTrue();
      expect(contact.Attribute.ATTR_WEBSITE === 1).assertTrue();
      console.info("Telephony_contact_Attribute_0200 end")
      done();
    });

    /**
     * @tc.number  Telephony_contact_Email_0100
     * @tc.name    Test The  Attribute Email
     * @tc.desc    Function test
     */
    it('Telephony_contact_Email_0100', 0, async function (done) {
      console.info("Telephony_contact_Email_0100 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(contact.Email.CUSTOM_LABEL === 0).assertTrue();
      expect(contact.Email.EMAIL_HOME === 1).assertTrue();
      expect(contact.Email.EMAIL_WORK === 2).assertTrue();
      expect(contact.Email.EMAIL_OTHER === 3).assertTrue();
      expect(contact.Email.INVALID_LABEL_ID === -1).assertTrue();
      expect(contact.Email.email !== "email").assertTrue();
      expect(contact.Email.labelName !== "email").assertTrue();
      expect(contact.Email.displayName !== "email").assertTrue();
      console.info("Telephony_contact_Email_0100 end")
      done();
    });

    /**
     * @tc.number  Telephony_contact_Email_0200
     * @tc.name    Test The  Attribute Email
     * @tc.desc    Function test
     */
    it('Telephony_contact_Email_0200', 0, async function (done) {
      console.info("Telephony_contact_Email_0200 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(contact.Email.CUSTOM_LABEL === 0).assertTrue();
      expect(contact.Email.EMAIL_HOME === 1).assertTrue();
      expect(contact.Email.EMAIL_WORK === 2).assertTrue();
      expect(contact.Email.EMAIL_OTHER === 3).assertTrue();
      expect(contact.Email.INVALID_LABEL_ID === -1).assertTrue();
      expect(contact.Email.email !== "email2").assertTrue();
      expect(contact.Email.labelName !== "email2").assertTrue();
      expect(contact.Email.displayName !== "email2").assertTrue();
      console.info("Telephony_contact_Email_0200 end")
      done();
    });

    /**
     * @tc.number  Telephony_contact_Event_0100
     * @tc.name    Test The  Attribute Event
     * @tc.desc    Function test
     */
    it('Telephony_contact_Event_0100', 0, async function (done) {
      console.info("Telephony_contact_Event_0100 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(contact.Event.CUSTOM_LABEL === 0).assertTrue();
      expect(contact.Event.EVENT_ANNIVERSARY === 1).assertTrue();
      expect(contact.Event.EVENT_OTHER === 2).assertTrue();
      expect(contact.Event.EVENT_BIRTHDAY === 3).assertTrue();
      expect(contact.Event.INVALID_LABEL_ID === -1).assertTrue();
      expect(contact.Event.eventDate !== "event").assertTrue();
      expect(contact.Event.labelName !== "event").assertTrue();
      console.info("Telephony_contact_Event_0100 end")
      done();
    });

    /**
     * @tc.number  Telephony_contact_Event_0200
     * @tc.name    Test The  Attribute Event
     * @tc.desc    Function test
     */
    it('Telephony_contact_Event_0200', 0, async function (done) {
      console.info("Telephony_contact_Event_0200 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(contact.Event.CUSTOM_LABEL === 0).assertTrue();
      expect(contact.Event.EVENT_ANNIVERSARY === 1).assertTrue();
      expect(contact.Event.EVENT_OTHER === 2).assertTrue();
      expect(contact.Event.EVENT_BIRTHDAY === 3).assertTrue();
      expect(contact.Event.INVALID_LABEL_ID === -1).assertTrue();
      expect(contact.Event.eventDate !== "event2").assertTrue();
      expect(contact.Event.labelName !== "event2").assertTrue();
      console.info("Telephony_contact_Event_0200 end")
      done();
    });

    /**
     * @tc.number  Telephony_contact_ImAddress_0100
     * @tc.name    Test The  Attribute ImAddress
     * @tc.desc    Function test
     */
    it('Telephony_contact_ImAddress_0100', 0, async function (done) {
      console.info("Telephony_contact_ImAddress_0100 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(contact.ImAddress.CUSTOM_LABEL === -1).assertTrue();
      expect(contact.ImAddress.IM_AIM === 0).assertTrue();
      expect(contact.ImAddress.IM_MSN === 1).assertTrue();
      expect(contact.ImAddress.IM_YAHOO === 2).assertTrue();
      expect(contact.ImAddress.IM_SKYPE === 3).assertTrue();
      expect(contact.ImAddress.IM_QQ === 4).assertTrue();
      expect(contact.ImAddress.IM_ICQ === 6).assertTrue();
      expect(contact.ImAddress.IM_JABBER === 7).assertTrue();
      expect(contact.ImAddress.INVALID_LABEL_ID === -2).assertTrue();
      expect(contact.ImAddress.imAddress !== "imAddress").assertTrue();
      expect(contact.ImAddress.labelName !== "imAddress").assertTrue();
      console.info("Telephony_contact_ImAddress_0100 end")
      done();
    });

    /**
     * @tc.number  Telephony_contact_ImAddress_0200
     * @tc.name    Test The  Attribute ImAddress
     * @tc.desc    Function test
     */
    it('Telephony_contact_ImAddress_0200', 0, async function (done) {
      console.info("Telephony_contact_ImAddress_0100 start")
      if(utils.notCheck){
        done();
        return;
      }
      expect(contact.ImAddress.CUSTOM_LABEL === -1).assertTrue();
      expect(contact.ImAddress.IM_AIM === 0).assertTrue();
      expect(contact.ImAddress.IM_MSN === 1).assertTrue();
      expect(contact.ImAddress.IM_YAHOO === 2).assertTrue();
      expect(contact.ImAddress.IM_SKYPE === 3).assertTrue();
      expect(contact.ImAddress.IM_QQ === 4).assertTrue();
      expect(contact.ImAddress.IM_ICQ === 6).assertTrue();
      expect(contact.ImAddress.IM_JABBER === 7).assertTrue();
      expect(contact.ImAddress.INVALID_LABEL_ID === -2).assertTrue();
      expect(contact.ImAddress.imAddress !== "imAddress2").assertTrue();
      expect(contact.ImAddress.labelName !== "imAddress2").assertTrue();
      console.info("Telephony_contact_ImAddress_0200 end")
      done();
    });

  })
}