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
import hiTraceChain from '@ohos.hiTraceChain'
import hiTraceMeter from '@ohos.hiTraceMeter'

export default function hiTraceChainJsunit() {
  describe('hiTraceChainTest', function () {
    console.log("************* byTraceHiDebug Test start*************");

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceFlag_INCLUDE_ASYNC_0100
     * @tc.name      : INCLUDE_ASYNC
     * @tc.desc      : enum test
     */
    it('DFX_HiTraceChain_HiTraceFlag_INCLUDE_ASYNC_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceFlag_INCLUDE_ASYNC_0100 Test start*************");
      try {
        expect(1).assertEqual(hiTraceChain.HiTraceFlag.INCLUDE_ASYNC);
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceFlag_INCLUDE_ASYNC_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceFlag_DONOT_CREATE_SPAN_0100
     * @tc.name      : DONOT_CREATE_SPAN
     * @tc.desc      : enum test
     */
    it('DFX_HiTraceChain_HiTraceFlag_DONOT_CREATE_SPAN_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceFlag_DONOT_CREATE_SPAN_0100 Test start*************");
      try {
        expect(1 << 1).assertEqual(hiTraceChain.HiTraceFlag.DONOT_CREATE_SPAN);
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceFlag_DONOT_CREATE_SPAN_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceFlag_TP_INFO_0100
     * @tc.name      : TP_INFO
     * @tc.desc      : enum test
     */
    it('DFX_HiTraceChain_HiTraceFlag_TP_INFO_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceFlag_TP_INFO_0100 Test start*************");
      try {
        expect(1 << 2).assertEqual(hiTraceChain.HiTraceFlag.TP_INFO);
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceFlag_TP_INFO_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceFlag_NO_BE_INFO_0100
     * @tc.name      : NO_BE_INFO
     * @tc.desc      : enum test
     */
    it('DFX_HiTraceChain_HiTraceFlag_NO_BE_INFO_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceFlag_NO_BE_INFO_0100 Test start*************");
      try {
        expect(1 << 3).assertEqual(hiTraceChain.HiTraceFlag.NO_BE_INFO);
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceFlag_NO_BE_INFO_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceFlag_DISABLE_LOG_0100
     * @tc.name      : DISABLE_LOG
     * @tc.desc      : enum test
     */
    it('DFX_HiTraceChain_HiTraceFlag_DISABLE_LOG_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceFlag_DISABLE_LOG_0100 Test start*************");
      try {
        expect(1 << 4).assertEqual(hiTraceChain.HiTraceFlag.DISABLE_LOG);
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceFlag_DISABLE_LOG_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceFlag_FAILURE_TRIGGER_0100
     * @tc.name      : FAILURE_TRIGGER
     * @tc.desc      : enum test
     */
    it('DFX_HiTraceChain_HiTraceFlag_FAILURE_TRIGGER_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceFlag_FAILURE_TRIGGER_0100 Test start*************");
      try {
        expect(1 << 5).assertEqual(hiTraceChain.HiTraceFlag.FAILURE_TRIGGER);
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceFlag_FAILURE_TRIGGER_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceFlag_D2D_TP_INFO_0100
     * @tc.name      : D2D_TP_INFO
     * @tc.desc      : enum test
     */
    it('DFX_HiTraceChain_HiTraceFlag_D2D_TP_INFO_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceFlag_D2D_TP_INFO_0100 Test start*************");
      try {
        expect(1 << 6).assertEqual(hiTraceChain.HiTraceFlag.D2D_TP_INFO);
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceFlag_D2D_TP_INFO_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceTracepointType_CS_0100
     * @tc.name      : CS
     * @tc.desc      : enum test
     */
    it('DFX_HiTraceChain_HiTraceTracepointType_CS_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceTracepointType_CS_0100 Test start*************");
      try {
        expect(0).assertEqual(hiTraceChain.HiTraceTracepointType.CS);
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceTracepointType_CS_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceTracepointType_CR_0100
     * @tc.name      : CR
     * @tc.desc      : enum test
     */
    it('DFX_HiTraceChain_HiTraceTracepointType_CR_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceTracepointType_CR_0100 Test start*************");
      try {
        expect(1).assertEqual(hiTraceChain.HiTraceTracepointType.CR);
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceTracepointType_CR_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceTracepointType_SS_0100
     * @tc.name      : SS
     * @tc.desc      : enum test
     */
    it('DFX_HiTraceChain_HiTraceTracepointType_SS_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceTracepointType_SS_0100 Test start*************");
      try {
        expect(2).assertEqual(hiTraceChain.HiTraceTracepointType.SS);
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceTracepointType_SS_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceChain_chainId_0100
     * @tc.name      : chainId
     * @tc.desc      : chainId test
     */
    it('DFX_HiTraceChain_HiTraceChain_chainId_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceChain_chainId_0100 Test start*************");
      try {
        let hiTraceId = {
          chainId: 1,
          spanId: 2,
          parentSpanId: 3
        }
        hiTraceChain.setId(hiTraceId);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceChain_chainId_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceChain_spanId_0100
     * @tc.name      : spanId
     * @tc.desc      : spanId test
     */
    it('DFX_HiTraceChain_HiTraceChain_spanId_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceChain_spanId_0100 Test start*************");
      try {
        let hiTraceId = {
          chainId: 2,
          spanId: 3,
          parentSpanId: 4
        }
        hiTraceChain.setId(hiTraceId);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceChain_spanId_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceChain_createSpan_0100
     * @tc.name      : createSpan
     * @tc.desc      : createSpan method test
     */
    it('DFX_HiTraceChain_HiTraceChain_createSpan_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceChain_createSpan_0100 Test start*************");
      try {
        let hiTraceId = hiTraceChain.createSpan();
        console.log("DFX_HiTraceChain_HiTraceChain_createSpan_0100 chainId " + hiTraceId.chainId);
        console.log("DFX_HiTraceChain_HiTraceChain_createSpan_0100 spanId " + hiTraceId.spanId);
        console.log("DFX_HiTraceChain_HiTraceChain_createSpan_0100 parentSpanId " + hiTraceId.parentSpanId);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceChain_createSpan_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceChain_setId_0100
     * @tc.name      : setId
     * @tc.desc      : setId method test
     */
    it('DFX_HiTraceChain_HiTraceChain_setId_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceChain_setId_0100 Test start*************");
      try {
        let hiTraceId = {
          chainId: 3,
          spanId: 4,
          parentSpanId: 5
        }
        hiTraceChain.setId(hiTraceId);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceChain_setId_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceChain_end_0100
     * @tc.name      : end
     * @tc.desc      : end test
     */
    it('DFX_HiTraceChain_HiTraceChain_end_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceChain_end_0100 Test start*************");
      try {
        let hiTraceId = {
          chainId: 5,
          spanId: 6,
          parentSpanId: 7
        }
        hiTraceChain.end(hiTraceId);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceChain_end_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceChain_clearId_0100
     * @tc.name      : clearId
     * @tc.desc      : clearId test
     */
    it('DFX_HiTraceChain_HiTraceChain_clearId_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceChain_clearId_0100 Test start*************");
      try {
        hiTraceChain.clearId();
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceChain_clearId_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceChain_tracepoint_0100
     * @tc.name      : tracepoint
     * @tc.desc      : tracepoint test
     */
    it('DFX_HiTraceChain_HiTraceChain_tracepoint_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceChain_tracepoint_0100 Test start*************");
      try {
        let hiTraceId = {
          chainId: 5,
          spanId: 6,
          parentSpanId: 7
        }
        hiTraceChain.tracepoint(hiTraceChain.HiTraceCommunicationMode.DEFAULT, hiTraceChain.HiTraceTracepointType.CS, hiTraceId);
        expect(true).assertTrue();
      } catch (error) {
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceChain_tracepoint_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceChain_isValid_0100
     * @tc.name      : isValid
     * @tc.desc      : isValid method test
     */
    it('DFX_HiTraceChain_HiTraceChain_isValid_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceChain_isValid_0100 Test start*************");
      try {
        let hiTraceId = hiTraceChain.createSpan();
        let isValid = hiTraceChain.isValid(hiTraceId);
        console.log("DFX_HiTraceChain_HiTraceChain_isValid_0100 isValid " + isValid);
        expect(true).assertTrue();
      } catch (error) {
        console.log("DFX_HiTraceChain_HiTraceChain_isValid_0100 error " + error);
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceChain_isValid_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceChain_enableFlag_0100
     * @tc.name      : enableFlag
     * @tc.desc      : enableFlag method test
     */
    it('DFX_HiTraceChain_HiTraceChain_enableFlag_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceChain_enableFlag_0100 Test start*************");
      try {
        let hiTraceId = hiTraceChain.createSpan();
        hiTraceChain.enableFlag(hiTraceId, hiTraceChain.HiTraceFlag.DEFAULT);
        expect(true).assertTrue();
      } catch (error) {
        console.log("DFX_HiTraceChain_HiTraceChain_enableFlag_0100 error " + error);
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceChain_enableFlag_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceChain_HiTraceChain_isFlagEnabled_0100
     * @tc.name      : isFlagEnabled
     * @tc.desc      : isFlagEnabled method test
     */
    it('DFX_HiTraceChain_HiTraceChain_isFlagEnabled_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceChain_HiTraceChain_isFlagEnabled_0100 Test start*************");
      try {
        let hiTraceId = hiTraceChain.createSpan();
        let isFlagEnabled = hiTraceChain.isFlagEnabled(hiTraceId, hiTraceChain.HiTraceFlag.DEFAULT);
        console.log("DFX_HiTraceChain_HiTraceChain_isFlagEnabled_0100 isFlagEnabled " + isFlagEnabled);
        expect(true).assertTrue();
      } catch (error) {
        console.log("DFX_HiTraceChain_HiTraceChain_isFlagEnabled_0100 error " + error);
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceChain_HiTraceChain_isFlagEnabled_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceMeter_HiTraceMeter_startTrace_0100
     * @tc.name      : startTrace
     * @tc.desc      : Records a trace marking it as the start of a task, can with the expected completion time between
                       startTrace and finishTrace.
     */
    it('DFX_HiTraceMeter_HiTraceMeter_startTrace_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceMeter_HiTraceMeter_startTrace_0100 Test start*************");
      try {
        hiTraceMeter.startTrace('testName', 1);
      } catch (error) {
        console.log("DFX_HiTraceMeter_HiTraceMeter_startTrace_0100 error " + error);
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceMeter_HiTraceMeter_startTrace_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceMeter_HiTraceMeter_finishTrace_0100
     * @tc.name      : finishTrace
     * @tc.desc      : Records a trace and marks it as the end of a task.
     */
    it('DFX_HiTraceMeter_HiTraceMeter_finishTrace_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceMeter_HiTraceMeter_finishTrace_0100 Test start*************");
      try {
        hiTraceMeter.finishTrace('testName', 1);
      } catch (error) {
        console.log("DFX_HiTraceMeter_HiTraceMeter_finishTrace_0100 error " + error);
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceMeter_HiTraceMeter_finishTrace_0100 Test end*************");
      done();
    });

    /*
     * @tc.number    : DFX_HiTraceMeter_HiTraceMeter_traceByValue_0100
     * @tc.name      : traceByValue
     * @tc.desc      : Records a trace and marks it as the end of a task.
     */
    it('DFX_HiTraceMeter_HiTraceMeter_traceByValue_0100', 0, async function (done) {
      console.log("************* DFX_HiTraceMeter_HiTraceMeter_traceByValue_0100 Test start*************");
      try {
        hiTraceMeter.traceByValue('testName', 2);
      } catch (error) {
        console.log("DFX_HiTraceMeter_HiTraceMeter_traceByValue_0100 error " + error);
        expect().assertFail();
      }
      console.log("************* DFX_HiTraceMeter_HiTraceMeter_traceByValue_0100 Test end*************");
      done();
    });
    console.log("************* byTraceHiDebug Test end*************");
  })
}
