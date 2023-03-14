/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License')
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http:www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import {describe, beforeAll, afterAll, it, expect, afterEach} from 'deccjsunit/index.ets';
import http from '@ohos.net.http';

export default function responseCodeJsunit() {
  describe("responseCodeJsunitTest", function () {

    /**
     * @tc.number Telephony_Http_ResponseCode_OK_0100
     * @tc.name ResponseCode::OK
     * @tc.desc Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_OK_0100", 0, async function (done) {
      console.log("-----------------------Telephony_Http_ResponseCode_OK_0100 start-----------------------");
      try {
        expect(200).assertEqual(http.ResponseCode.OK);
        console.log("-----------------------Telephony_Http_ResponseCode_OK_0100 OK assertEqual SUCCESS-----------------------");
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_OK_0100 : error = " + error);
      }
      done();
      console.log("-----------------------Telephony_Http_ResponseCode_OK_0100 end-----------------------");
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_ACCEPTED_0100
     * @tc.name ResponseCode::ACCEPTED
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_ACCEPTED_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode_ACCEPTED Test is starting-----------------------");
      try {
        expect(202).assertEqual(http.ResponseCode.ACCEPTED);
        console.log("-----------------------ResponseCode ACCEPTED Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_ACCEPTED_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_NOT_AUTHORITATIVE_0100
     * @tc.name ResponseCode::NOT_AUTHORITATIVE
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_NOT_AUTHORITATIVE_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode NOT_AUTHORITATIVE Test is starting-----------------------");
      try {
        expect(203).assertEqual(http.ResponseCode.NOT_AUTHORITATIVE);
        console.log("-----------------------ResponseCode NOT_AUTHORITATIVE Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_NOT_AUTHORITATIVE_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_NO_CONTENT_0100
     * @tc.name ResponseCode::NO_CONTENT
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_NO_CONTENT_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode NO_CONTENT Test is starting-----------------------");
      try {
        expect(204).assertEqual(http.ResponseCode.NO_CONTENT);
        console.log("-----------------------ResponseCode NO_CONTENT Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_NO_CONTENT_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_MULT_CHOICE_0100
     * @tc.name ResponseCode::MULT_CHOICE
     * @tc.desc Test Test ResponseCode property.
     */
    it("Telephony_Http_ResponseCode_MULT_CHOICE_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode MULT_CHOICE Test is starting-----------------------");
      try {
        expect(300).assertEqual(http.ResponseCode.MULT_CHOICE);
        console.log("-----------------------ResponseCode MULT_CHOICE Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_MULT_CHOICE_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_MOVED_PERM_0100
     * @tc.name ResponseCode::MOVED_PERM
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_MOVED_PERM_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode MOVED_PERM Test is starting-----------------------");
      try {
        expect(301).assertEqual(http.ResponseCode.MOVED_PERM);
        console.log("-----------------------ResponseCode MOVED_PERM Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_MOVED_PERM_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_SEE_OTHER_0100
     * @tc.name ResponseCode::SEE_OTHER
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_SEE_OTHER_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode SEE_OTHER Test is starting-----------------------");
      try {
        expect(303).assertEqual(http.ResponseCode.SEE_OTHER);
        console.log("-----------------------ResponseCode SEE_OTHER Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_SEE_OTHER_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_NOT_MODIFIED_0100
     * @tc.name ResponseCode::NOT_MODIFIED
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_NOT_MODIFIED_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode NOT_MODIFIED Test is starting-----------------------");
      try {
        expect(304).assertEqual(http.ResponseCode.NOT_MODIFIED);
        console.log("-----------------------ResponseCode NOT_MODIFIED Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_NOT_MODIFIED_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_USE_PROXY_0100
     * @tc.name ResponseCode::USE_PROXY
     * @tc.desc Test Test ResponseCode property.
     */
    it("Telephony_Http_ResponseCode_USE_PROXY_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode USE_PROXY Test is starting-----------------------");
      try {
        expect(305).assertEqual(http.ResponseCode.USE_PROXY);
        console.log("-----------------------ResponseCode USE_PROXY Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_USE_PROXY_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_BAD_REQUEST_0100
     * @tc.name ResponseCode::BAD_REQUEST
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_BAD_REQUEST_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode BAD_REQUEST Test is starting-----------------------");
      try {
        expect(400).assertEqual(http.ResponseCode.BAD_REQUEST);
        console.log("-----------------------ResponseCode BAD_REQUEST Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_BAD_REQUEST_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_UNAUTHORIZED_0100
     * @tc.name ResponseCode::ResponseCode_UNAUTHORIZED
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_UNAUTHORIZED_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode UNAUTHORIZED Test is starting-----------------------");
      try {
        expect(401).assertEqual(http.ResponseCode.UNAUTHORIZED);
        console.log("-----------------------ResponseCode UNAUTHORIZED Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_UNAUTHORIZED_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_PAYMENT_REQUIRED_0100
     * @tc.name ResponseCode::PAYMENT_REQUIRED
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_PAYMENT_REQUIRED_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode PAYMENT_REQUIRED Test is starting-----------------------");
      try {
        expect(402).assertEqual(http.ResponseCode.PAYMENT_REQUIRED);
        console.log("-----------------------ResponseCode PAYMENT_REQUIRED Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_PAYMENT_REQUIRED_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_BAD_METHOD_0100
     * @tc.name ResponseCode::BAD_METHOD
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_BAD_METHOD_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode BAD_METHOD Test is starting-----------------------");
      try {
        expect(405).assertEqual(http.ResponseCode.BAD_METHOD);
        console.log("-----------------------ResponseCode BAD_METHOD Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_BAD_METHOD_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_NOT_ACCEPTABLE_0100
     * @tc.name ResponseCode::NOT_ACCEPTABLE
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_NOT_ACCEPTABLE_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode NOT_ACCEPTABLE Test is starting-----------------------");
      try {
        expect(406).assertEqual(http.ResponseCode.NOT_ACCEPTABLE);
        console.log("-----------------------ResponseCode NOT_ACCEPTABLE Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_NOT_ACCEPTABLED_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_PROXY_AUTH_0100
     * @tc.name ResponseCode::PROXY_AUTH
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_PROXY_AUTH_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode PROXY_AUTH Test is starting-----------------------");
      try {
        expect(407).assertEqual(http.ResponseCode.PROXY_AUTH);
        console.log("-----------------------ResponseCode PROXY_AUTH Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_PROXY_AUTH_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_CLIENT_TIMEOUT_0100
     * @tc.name ResponseCode::CLIENT_TIMEOUT
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_CLIENT_TIMEOUT_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode CLIENT_TIMEOUT Test is starting-----------------------");
      try {
        expect(408).assertEqual(http.ResponseCode.CLIENT_TIMEOUT);
        console.log("-----------------------ResponseCode CLIENT_TIMEOUT Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_CLIENT_TIMEOUT_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_LENGTH_REQUIRED_0100
     * @tc.name ResponseCode::LENGTH_REQUIRED
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_LENGTH_REQUIRED_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode LENGTH_REQUIRED Test is starting-----------------------");
      try {
        expect(411).assertEqual(http.ResponseCode.LENGTH_REQUIRED);
        console.log("-----------------------ResponseCode LENGTH_REQUIRED Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_LENGTH_REQUIRED_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_PRECON_FAILED_0100
     * @tc.name ResponseCode::PRECON_FAILED
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_PRECON_FAILED_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode PRECON_FAILED Test is starting-----------------------");
      try {
        expect(412).assertEqual(http.ResponseCode.PRECON_FAILED);
        console.log("-----------------------ResponseCode PRECON_FAILED Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_PRECON_FAILED_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_ENTITY_TOO_LARGE_0100
     * @tc.name ResponseCode::ENTITY_TOO_LARGE
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_ENTITY_TOO_LARGE_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode ENTITY_TOO_LARGE Test is starting-----------------------");
      try {
        expect(413).assertEqual(http.ResponseCode.ENTITY_TOO_LARGE);
        console.log("-----------------------ResponseCode ENTITY_TOO_LARGE Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_ENTITY_TOO_LARGE_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_REQ_TOO_LONG_0100
     * @tc.name ResponseCode::REQ_TOO_LONG
     * @tc.desc Test Test ResponseCode property.
     */


    it("Telephony_Http_ResponseCode_REQ_TOO_LONG_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode REQ_TOO_LONG Test is starting-----------------------");
      try {
        expect(414).assertEqual(http.ResponseCode.REQ_TOO_LONG);
        console.log("-----------------------ResponseCode REQ_TOO_LONG Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_REQ_TOO_LONG_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_UNSUPPORTED_TYPE_0100
     * @tc.name ResponseCode::UNSUPPORTED_TYPE
     * @tc.desc Test Test ResponseCode property.
     */


    it("Telephony_Http_ResponseCode_UNSUPPORTED_TYPE_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode UNSUPPORTED_TYPE Test is starting-----------------------");
      try {
        expect(415).assertEqual(http.ResponseCode.UNSUPPORTED_TYPE);
        console.log("-----------------------ResponseCode UNSUPPORTED_TYPE Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_UNSUPPORTED_TYPE_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_INTERNAL_ERROR_0100
     * @tc.name ResponseCode::INTERNAL_ERROR
     * @tc.desc Test Test ResponseCode property.
     */
    it("Telephony_Http_ResponseCode_INTERNAL_ERROR_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode INTERNAL_ERROR Test is starting-----------------------");
      try {
        expect(500).assertEqual(http.ResponseCode.INTERNAL_ERROR);
        console.log("-----------------------ResponseCode INTERNAL_ERROR Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_INTERNAL_ERROR_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_NOT_IMPLEMENTED_0100
     * @tc.name ResponseCode::NOT_IMPLEMENTED
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_NOT_IMPLEMENTED_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode NOT_IMPLEMENTED Test is starting-----------------------");
      try {
        expect(501).assertEqual(http.ResponseCode.NOT_IMPLEMENTED);
        console.log("-----------------------ResponseCode NOT_IMPLEMENTED Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_NOT_IMPLEMENTED_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_BAD_GATEWAY_0100
     * @tc.name ResponseCode::BAD_GATEWAY
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_BAD_GATEWAY_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode BAD_GATEWAY Test is starting-----------------------");
      try {
        expect(502).assertEqual(http.ResponseCode.BAD_GATEWAY);
        console.log("-----------------------ResponseCode BAD_GATEWAY Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_BAD_GATEWAY_0100 : error = " + error);
        done();
      }
    });

    /**
     * @tc.number Telephony_Http_ResponseCode_GATEWAY_TIMEOUT_0100
     * @tc.name ResponseCode::GATEWAY_TIMEOUT
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_GATEWAY_TIMEOUT_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode GATEWAY_TIMEOUT Test is starting-----------------------");
      try {
        expect(504).assertEqual(http.ResponseCode.GATEWAY_TIMEOUT);
        console.log("-----------------------ResponseCode GATEWAY_TIMEOUT Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_GATEWAY_TIMEOUT_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_ResponseCode_CREATED_0100
     * @tc.name ResponseCode::CREATED
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_CREATED_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode CREATED Test is starting-----------------------");
      try {
        expect(201).assertEqual(http.ResponseCode.CREATED);
        console.log("-----------------------ResponseCode CREATED Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_CREATED_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_ResponseCode_PARTIAL_0100
     * @tc.name ResponseCode::PARTIAL
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_PARTIAL_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode PARTIAL Test is starting-----------------------");
      try {
        expect(206).assertEqual(http.ResponseCode.PARTIAL);
        console.log("-----------------------ResponseCode PARTIAL Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_PARTIAL_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_ResponseCode_MOVED_TEMP_0100
     * @tc.name ResponseCode::MOVED_TEMP
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_MOVED_TEMP_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode MOVED_TEMP Test is starting-----------------------");
      try {
        expect(302).assertEqual(http.ResponseCode.MOVED_TEMP);
        console.log("-----------------------ResponseCode MOVED_TEMP Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_MOVED_TEMP_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_ResponseCode_NOT_FOUND_0100
     * @tc.name ResponseCode::NOT_FOUND
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_NOT_FOUND_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode NOT_FOUND Test is starting-----------------------");
      try {
        expect(404).assertEqual(http.ResponseCode.NOT_FOUND);
        console.log("-----------------------ResponseCode NOT_FOUND Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_NOT_FOUND_0100 : error = " + error);
        done();
      }
    });



    /**
     * @tc.number Telephony_Http_ResponseCode_CONFLICT_0100
     * @tc.name ResponseCode::CONFLICT
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_CONFLICT_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode CONFLICT Test is starting-----------------------");
      try {
        expect(409).assertEqual(http.ResponseCode.CONFLICT);
        console.log("-----------------------ResponseCode CONFLICT Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_CONFLICT_0100 : error = " + error);
        done();
      }
    });




    /**
     * @tc.number Telephony_Http_ResponseCode_GONE_0100
     * @tc.name ResponseCode::GONE
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_GONE_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode GONE Test is starting-----------------------");
      try {
        expect(410).assertEqual(http.ResponseCode.GONE);
        console.log("-----------------------ResponseCode GONE Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_GONE_0100 : error = " + error);
        done();
      }
    });


    /**
     * @tc.number Telephony_Http_ResponseCode_UNAVAILABLE_0100
     * @tc.name ResponseCode::UNAVAILABLE
     * @tc.desc Test Test ResponseCode property.
     */

    it("Telephony_Http_ResponseCode_UNAVAILABLE_0100", 0, async function (done) {
      console.log("-----------------------ResponseCode UNAVAILABLE Test is starting-----------------------");
      try {
        expect(503).assertEqual(http.ResponseCode.UNAVAILABLE);
        console.log("-----------------------ResponseCode UNAVAILABLE Test end-----------------------");
        done();
      } catch (error) {
        console.log("Telephony_Http_ResponseCode_UNAVAILABLE_0100 : error = " + error);
        done();
      }
    });


  })

};

