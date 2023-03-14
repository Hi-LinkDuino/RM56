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
import HiAppEvent from '@ohos.hiAppEvent'
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import Constant from 'deccjsunit/src/Constant'

describe('HiAppEventApiTest', function () {

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0100
     * @tc.name testHiAppEventApi01
     * @tc.desc HiAppEvent write interface test.
     */
    it('testHiAppEventApi01', 1, async function (done) {
        console.info('testHiAppEventApi01 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_int": 100, "key_string": "demo",
        "key_bool":true, "key_float":1.1,"key_array_int": [1, 2, 3], "key_array_float": [1.1, 2.2, 3.3],
        "key_array_str": ["a", "b", "c"], "key_array_bool": [true, false],"key_array_int2": [1, 2, 3],
        "key_arr_float2": [1.1, 2.2, 3.3], "key_arr_str2": ["a", "b", "c"], "key_array_bool2": [true, false]},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0300
     * @tc.name testHiAppEventApi03
     * @tc.desc HiAppEvent write EventType of STATISTIC.
     */
    it('testHiAppEventApi03', 2, async function (done) {
        console.info('testHiAppEventApi03 start')
        HiAppEvent.write("write", HiAppEvent.EventType.STATISTIC, {"key_int": 100, "key_string": "demo",
        "key_bool":true, "key_float":1.1,"key_array_int": [1, 2, 3], "key_array_float": [1.1, 2.2, 3.3],
        "key_array_str": ["a", "b", "c"], "key_array_bool": [true, false],"key_array_int2": [1, 2, 3],
        "key_arr_float2": [1.1, 2.2, 3.3], "key_arr_str2": ["a", "b", "c"], "key_array_bool2": [true, false]},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi03 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0400
     * @tc.name testHiAppEventApi04
     * @tc.desc HiAppEvent write EventType of SECURITY.
     */
    it('testHiAppEventApi04', 2, async function (done) {
        console.info('testHiAppEventApi04 start')
        HiAppEvent.write("write", HiAppEvent.EventType.SECURITY, {"key_int": 100, "key_string": "demo",
        "key_bool":true, "key_float":1.1,"key_array_int": [1, 2, 3], "key_array_float": [1.1, 2.2, 3.3],
        "key_array_str": ["a", "b", "c"], "key_array_bool": [true, false],"key_array_int2": [1, 2, 3],
        "key_arr_float2": [1.1, 2.2, 3.3], "key_arr_str2": ["a", "b", "c"], "key_array_bool2": [true, false]},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi04 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0500
     * @tc.name testHiAppEventApi05
     * @tc.desc HiAppEvent write EventType of BEHAVIOR.
     */
    it('testHiAppEventApi05', 2, async function (done) {
        console.info('testHiAppEventApi05 start')
        HiAppEvent.write("write", HiAppEvent.EventType.BEHAVIOR, {"key_int": 100, "key_string": "demo",
        "key_bool":true, "key_float":1.1,"key_array_int": [1, 2, 3], "key_array_float": [1.1, 2.2, 3.3],
        "key_array_str": ["a", "b", "c"], "key_array_bool": [true, false],"key_array_int2": [1, 2, 3],
        "key_arr_float2": [1.1, 2.2, 3.3], "key_arr_str2": ["a", "b", "c"], "key_array_bool2": [true, false]},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi05 end')
    })


    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0600
     * @tc.name testHiAppEventApi06
     * @tc.desc HiAppEvent write datatype of int.
     */
    it('testHiAppEventApi06', 2, async function (done) {
        console.info('testHiAppEventApi06 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_int":100},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi06 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0700
     * @tc.name testHiAppEventApi07
     * @tc.desc HiAppEvent write datatype of int array.
     */
    it('testHiAppEventApi07', 2, async function (done) {
        console.info('testHiAppEventApi07 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_int_array":[100, 200]},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi07 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0800
     * @tc.name testHiAppEventApi08
     * @tc.desc HiAppEvent write datatype of float.
     */
    it('testHiAppEventApi08', 2, async function (done) {
        console.info('testHiAppEventApi08 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_float":1.1},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi08 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_0900
     * @tc.name testHiAppEventApi09
     * @tc.desc HiAppEvent write datatype of float array.
     */
    it('testHiAppEventApi09', 3, async function (done) {
        console.info('testHiAppEventApi09 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_float_array":[1.1, 1.2]},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi09 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1000
     * @tc.name testHiAppEventApi10
     * @tc.desc HiAppEvent write datatype of string.
     */
    it('testHiAppEventApi10', 3, async function (done) {
        console.info('testHiAppEventApi10 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_string":"hello world"},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi10 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1100
     * @tc.name testHiAppEventApi11
     * @tc.desc HiAppEvent write datatype of string array.
     */
    it('testHiAppEventApi11', 3, async function (done) {
        console.info('testHiAppEventApi11 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_string_array":["hello world", "hello world2"]},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi11 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1200
     * @tc.name testHiAppEventApi12
     * @tc.desc HiAppEvent write datatype of bool with true.
     */
    it('testHiAppEventApi12', 3, async function (done) {
        console.info('testHiAppEventApi12 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_bool":true},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi12 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1300
     * @tc.name testHiAppEventApi13
     * @tc.desc HiAppEvent write datatype of bool with false.
     */
    it('testHiAppEventApi13', 3, async function (done) {
        console.info('testHiAppEventApi13 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_bool":false},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi13 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1400
     * @tc.name testHiAppEventApi14
     * @tc.desc HiAppEvent write datatype of bool array.
     */
    it('testHiAppEventApi14', 3, async function (done) {
        console.info('testHiAppEventApi14 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_bool_array":[false, true]},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi14 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1500
     * @tc.name testHiAppEventApi15
     * @tc.desc HiAppEvent write datatype of char.
     */
    it('testHiAppEventApi15', 3, async function (done) {
        console.info('testHiAppEventApi15 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_char":'c'},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi15 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1600
     * @tc.name testHiAppEventApi16
     * @tc.desc HiAppEvent write datatype of long.
     */
    it('testHiAppEventApi16', 3, async function (done) {
        console.info('testHiAppEventApi16 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_long":2147483647},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi16 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1700
     * @tc.name testHiAppEventApi17
     * @tc.desc HiAppEvent write datatype of double.
     */
    it('testHiAppEventApi17', 3, async function (done) {
        console.info('testHiAppEventApi17 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_double":100.123},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi17 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1800
     * @tc.name testHiAppEventApi18
     * @tc.desc HiAppEvent write datatype of long.
     */
    it('testHiAppEventApi18', 3, async function (done) {
        console.info('testHiAppEventApi18 start')
        HiAppEvent.write("write", HiAppEvent.EventType.FAULT, {"key_longlong":2147483647},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi18 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_1900
     * @tc.name testHiAppEventApi19
     * @tc.desc HiAppEvent write eventtype of none exists.
     */
    it('testHiAppEventApi19', 3, async function (done) {
        console.info('testHiAppEventApi19 start')
        HiAppEvent.write("write", HiAppEvent.EventType.BEHAVIOR + 1, {"key_longlong":2147483647},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent json-callback-error code=${err.code}`);
                    expect().assertFail();
                    done();
                } else {
                    console.log(`HiAppEvent json-callback-success value=${value}`);
                    expect(value == 0).assertTrue();
                    done();
                }
            });
        console.info('testHiAppEventApi19 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_2000
     * @tc.name testHiAppEventApi20
     * @tc.desc HiAppEvent write empty eventname .
     */
    it('testHiAppEventApi20', 3, async function (done) {
        console.info('testHiAppEventApi20 start')
        HiAppEvent.write("", HiAppEvent.EventType.FAULT, {"key_longlong":2147483647},
            (err, value) => {
                console.log('HiAppEvent20 into json-callback');
                if (err) {
                    console.error(`HiAppEvent20 json-callback-error code=${err.code}`);
                    expect(err.code == -1).assertTrue();
                    done();
                } else {
                    console.log(`HiAppEvent20 json-callback-success value=${value}`);
                    expect().assertFail();
                    done();
                }
            });
        console.info('testHiAppEventApi20 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_2100
     * @tc.name testHiAppEventApi21
     * @tc.desc HiAppEvent write long eventname .
     */
    it('testHiAppEventApi21', 3, async function (done) {
        console.info('testHiAppEventApi21 start')
        var i = 0;
        var strlong = "";
        while (i < 1000) {
            strlong = strlong + "eventnamelong";
            i = i + 1;
        }
        HiAppEvent.write(strlong, HiAppEvent.EventType.FAULT, {"key_longlong":2147483647},
            (err, value) => {
                console.log('HiAppEvent into json-callback');
                if (err) {
                    console.error(`HiAppEvent21 json-callback-error code=${err.code}`);
                    expect(err.code == -1).assertTrue();
                    done();
                } else {
                    console.log(`HiAppEvent21 json-callback-success value=${value}`);
                    expect().assertFail();
                    done();
                }
            });
        console.info('testHiAppEventApi21 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_2200
     * @tc.name testHiAppEventApi22
     * @tc.desc HiAppEvent configure interface with disable option set true.
     */
     it('testHiAppEventApi22', 3, function () {
        console.info('testHiAppEventApi22 start')
        let configret =false;
        configret = HiAppEvent.configure({disable:true});
        expect(configret == true).assertTrue();
        HiAppEvent.configure({disable: false});
        console.info('testHiAppEventApi22 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_2300
     * @tc.name testHiAppEventApi23
     * @tc.desc HiAppEvent configure interface with disable option set false.
     */
    it('testHiAppEventApi23', 3, function () {
        console.info('testHiAppEventApi23 start')
        let configret =false;
        configret = HiAppEvent.configure({disable: false});
        expect(configret == true).assertTrue();
        console.info('testHiAppEventApi23 end')
    })

    /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_2400
     * @tc.name testHiAppEventApi24
     * @tc.desc HiAppEvent configure interface with maxStorage option set 100M.
     */
    it('testHiAppEventApi24', 3, function () {
        console.info('testHiAppEventApi24 start')
        let configret =false;
        configret = HiAppEvent.configure({maxStorage:'100M'});
        console.info('configret = %d', configret);
        expect(configret == true).assertTrue();
        console.info('testHiAppEventApi24 end')
    })

  /**
     * @tc.number DFX_DFT_HiviewKit_HiAppEvent_JSNAPI_2500
     * @tc.name testHiAppEventApi25
     * @tc.desc HiAppEvent with predefined event and param.
     */
   it('testHiAppEventApi25', 3, async function (done) {
    console.info('testHiAppEventApi25 start')
    HiAppEvent.write(HiAppEvent.Event.USER_LOGIN, HiAppEvent.EventType.BEHAVIOR,
        {[HiAppEvent.Param.USER_ID]: 'userlogin', [HiAppEvent.Param.DISTRIBUTED_SERVICE_NAME]: 'HiAppEvent',
        [HiAppEvent.Param.DISTRIBUTED_SERVICE_INSTANCE_ID]: 100},
        (err, value) => {
            console.log('HiAppEvent into json-callback');
            if (err) {
                console.error(`HiAppEvent json-callback-error code=${err.code}`);
                expect().assertFail();
                done();
            } else {
                console.log(`HiAppEvent json-callback-success value=${value}`);
                expect(value == 0).assertTrue();
                done();
            }
        });

    HiAppEvent.write(HiAppEvent.Event.USER_LOGOUT, HiAppEvent.EventType.BEHAVIOR,
        {[HiAppEvent.Param.USER_ID]: 'userlogout', [HiAppEvent.Param.DISTRIBUTED_SERVICE_NAME]: 'HiAppEvent',
        [HiAppEvent.Param.DISTRIBUTED_SERVICE_INSTANCE_ID]: 100},
        (err, value) => {
            console.log('HiAppEvent into json-callback');
            if (err) {
                console.error(`HiAppEvent json-callback-error code=${err.code}`);
                expect().assertFail();
                done();
            } else {
                console.log(`HiAppEvent json-callback-success value=${value}`);
                expect(value == 0).assertTrue();
                done();
            }
        });
    console.info('testHiAppEventApi25 end')
    })
})
