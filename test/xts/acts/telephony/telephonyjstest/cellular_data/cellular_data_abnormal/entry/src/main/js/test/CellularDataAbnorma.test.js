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

import cellular from "@ohos.telephony.data";
import { describe, it, expect } from "deccjsunit/index";

describe("ActsCellularDataAbnormalTest", function () {
    const ERR_SLOT_ID = 3;

    /*
     * @tc.number   Telephony_CellularData_isCellularDataEnabled_Async_0100
     * @tc.name     Call the enableCellularData interface and then call the isCellularDataEnabled interface.
     *              The result is true
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_isCellularDataEnabled_Async_0100", 0, async function (done) {
        cellular.isCellularDataEnabled((err, data) => {
            if (err) {
                expect().assertFail();
                console.log("Telephony_CellularData_isCellularDataEnabled_Async_0100 fail");
                done();
                return;
            }
            console.log("Telephony_CellularData_isCellularDataEnabled_Async_0100 finish");
            done();
        })
    })

    /*
     * @tc.number   Telephony_CellularData_getCellularDataState_Async_0100
     * @tc.name     Call the enableCellularData interface, call the getCellularDataState interface,
     *              and get the result 1 or 2
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_getCellularDataState_Async_0100", 0, async function (done) {
        cellular.getCellularDataState((err, data) => {
            if (err) {
                expect().assertFail();
                console.log("Telephony_CellularData_getCellularDataState_Async_0100 fail");
                done();
                return;
            }
            console.log("Telephony_CellularData_getCellularDataState_Async_0100 finish");
            done();
        })
    })

    /*
     * @tc.number   Telephony_CellularData_enableCellularData_Async_0100
     * @tc.name     Open the data switch and query the switch status and data status
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_enableCellularData_Async_0100", 0, async function (done) {
        cellular.enableCellularData((err) => {
            if (err) {
                console.log("Telephony_CellularData_enableCellularData_Async_0100 enableCellularData finish");
                done();
                return;
            }
            expect().assertFail();
            console.log("Telephony_CellularData_enableCellularData_Async_0100 enableCellularData fail");
            done();
        })
    })

    /*
     * @tc.number   Telephony_CellularData_disableCellularData_Async_0100
     * @tc.name     Turn off the data switch and query the switch status and data status
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_disableCellularData_Async_0100", 0, async function (done) {
        cellular.disableCellularData((err) => {
            if (err) {
                console.log("Telephony_CellularData_disableCellularData_Async_0100 disableCellularData finish");
                done();
                return;
            }
            expect().assertFail();
            console.log("Telephony_CellularData_disableCellularData_Async_0100 disableCellularData fail");
            done();
        })
    })

    /*
     * @tc.number   Telephony_CellularData_enableCellularDataRoaming_Async_0300
     * @tc.name     Call enableCellularDataRoaming interface, setting parameters as slotId error value (3),
     *              to check the correction results
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_enableCellularDataRoaming_Async_0300", 0, async function (done) {
        cellular.enableCellularDataRoaming(ERR_SLOT_ID, (err) => {
            if (err) {
                console.log("Telephony_CellularData_enableCellularDataRoaming_Async_0300 enableCellularData finish");
                done();
                return;
            }
            expect().assertFail();
            console.log("Telephony_CellularData_enableCellularDataRoaming_Async_0300 enableCellularData fail");
            done();
        })
    })

    /*
     * @tc.number   Telephony_CellularData_disableCellularDataRoaming_Async_0300
     * @tc.name     Call enableCellularDataRoaming interface, setting parameters as slotId error value (3), to check the
     *              correction results
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_disableCellularDataRoaming_Async_0300", 0, async function (done) {
        cellular.disableCellularDataRoaming(ERR_SLOT_ID, (err) => {
            if (err) {
                console.log(
                    "Telephony_CellularData_disableCellularDataRoaming_Async_0300 disableCellularDataRoaming finish"
                );
                done();
                return;
            }
            expect().assertFail();
            console.log("Telephony_CellularData_disableCellularDataRoaming_Async_0300 disableCellularDataRoaming fail");
            done();
        })
    })

    /*
     * @tc.number   Telephony_CellularData_getCellularDataFlowType_Async_0100
     * @tc.name     When cellular data is turned on, call getCellularDataFlowType() to get the data stream type and
     *              verify that the callback value is within the enumeration value range
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_getCellularDataFlowType_Async_0100", 0, async function (done) {
        cellular.getCellularDataFlowType((err, data) => {
            if (err) {
                expect().assertFail();
                console.log("Telephony_CellularData_getCellularDataFlowType_Async_0100 fail");
                done();
                return;
            }
            console.log("Telephony_CellularData_getCellularDataFlowType_Async_0100 finish");
            done();
        })
    })

    /*
     * @tc.number   Telephony_CellularData_isCellularDataRoamingEnabled_Async_0200
     * @tc.name     Call isCellularDataRoamingEnabled, set the slotId to error values (3), to check the correction
     *              results
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_isCellularDataRoamingEnabled_Async_0200", 0, async function (done) {
        cellular.isCellularDataRoamingEnabled(ERR_SLOT_ID, (err) => {
            if (err) {
                console.log("Telephony_CellularData_isCellularDataRoamingEnabled_Async_0200 finish");
                done();
                return;
            }
            expect().assertFail();
            console.log("Telephony_CellularData_isCellularDataRoamingEnabled_Async_0200 fail");
            done();
        })
    })

    /*
     * @tc.number   Telephony_CellularData_setDefaultCellularDataSlotId_Async_0500
     * @tc.name     Call setDefaultCellularDataSlotId() interface, set the default card slot to the number of exceptions
     *              (-1), and verify whether the setting failed (Async)
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_setDefaultCellularDataSlotId_Async_0500", 0, async function (done) {
        cellular.setDefaultCellularDataSlotId(ERR_SLOT_ID, (err) => {
            if (err) {
                console.log("Telephony_CellularData_setDefaultCellularDataSlotId_Async_0500 finish");
                done();
                return;
            }
            expect().assertFail();
            console.log("Telephony_CellularData_setDefaultCellularDataSlotId_Async_0500 fail");
            done();
        })
    })

    /*
     * @tc.number   Telephony_CellularData_getDefaultCellularDataSlotId_Async_0500
     * @tc.name     Call getDefaultCellularDataSlotId() interface and verify whether the setting failed (Async)
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_getDefaultCellularDataSlotId_Async_0500", 0, async function (done) {
        cellular.getDefaultCellularDataSlotId((err) => {
            if (err) {
                console.log("Telephony_CellularData_setDefaultCellularDataSlotId_Async_0500 finish");
                done();
                return;
            }
            expect().assertFail();
            console.log("Telephony_CellularData_setDefaultCellularDataSlotId_Async_0500 fail");
            done();
        })
    })

    /*
     * @tc.number   Telephony_CellularData_isCellularDataEnabled_Promise_0100
     * @tc.name     Call the enableCellularData interface and then call the isCellularDataEnabled interface.
     *              The result is true
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_isCellularDataEnabled_Promise_0100", 0, async function (done) {
        try {
            await cellular.isCellularDataEnabled();
            console.log("Telephony_CellularData_isCellularDataEnabled_Promise_0100 finish");
            done();
        } catch (err) {
            expect().assertFail();
            console.log("Telephony_CellularData_isCellularDataEnabled_Promise_0100 fail");
            done();
        }
    })

    /*
     * @tc.number   Telephony_CellularData_getCellularDataState_Promise_0100
     * @tc.name     Call the enableCellularData interface, call the getCellularDataState interface,
     *              and get the result 1 or 2
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_getCellularDataState_Promise_0100", 0, async function (done) {
        try {
            await cellular.getCellularDataState();
            console.log("Telephony_CellularData_getCellularDataState_Promise_0100 finish");
            done();
        } catch (err) {
            expect().assertFail();
            console.log("Telephony_CellularData_getCellularDataState_Promise_0100 fail");
            done();
        }
    })

    /*
     * @tc.number   Telephony_CellularData_enableCellularData_Promise_0100
     * @tc.name     Open the data switch and query the switch status and data status
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_enableCellularData_Promise_0100", 0, async function (done) {
        try {
            await cellular.enableCellularData();
            expect().assertFail();
            console.log("Telephony_CellularData_enableCellularData_Promise_0100 fail");
            done();
        } catch (err) {
            console.log("Telephony_CellularData_enableCellularData_Promise_0100 finish");
            done();
        }
    })

    /*
     * @tc.number   Telephony_CellularData_disableCellularData_Promise_0100
     * @tc.name     Turn off the data switch and query the switch status and data status
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_disableCellularData_Promise_0100", 0, async function (done) {
        try {
            await cellular.disableCellularData();
            expect().assertFail();
            console.log("Telephony_CellularData_disableCellularData_Promise_0100 fail");
            done();
        } catch (err) {
            console.log("Telephony_CellularData_disableCellularData_Promise_0100 finish");
            done();
        }
    })

    /*
     * @tc.number   Telephony_CellularData_enableCellularDataRoaming_Promise_0300
     * @tc.name     Call enableCellularDataRoaming interface, setting parameters as slotId error value (3),
     *              to check the correction results
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_enableCellularDataRoaming_Promise_0300", 0, async function (done) {
        try {
            await cellular.enableCellularDataRoaming(ERR_SLOT_ID);
            expect().assertFail();
            console.log("Telephony_CellularData_enableCellularDataRoaming_Promise_0300 fail");
            done();
        } catch (err) {
            console.log("Telephony_CellularData_enableCellularDataRoaming_Promise_0300 finish");
            done();
        }
    })

    /*
     * @tc.number   Telephony_CellularData_disableCellularDataRoaming_Promise_0300
     * @tc.name     Call enableCellularDataRoaming interface, setting parameters as slotId error value (3), to check the
     *              correction results
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_disableCellularDataRoaming_Promise_0300", 0, async function (done) {
        try {
            await cellular.disableCellularDataRoaming(ERR_SLOT_ID);
            expect().assertFail();
            console.log("Telephony_CellularData_disableCellularDataRoaming_Promise_0300 fail");
            done();
        } catch (err) {
            console.log("Telephony_CellularData_disableCellularDataRoaming_Promise_0300 finish");
            done();
        }
    })

    /*
     * @tc.number   Telephony_CellularData_getCellularDataFlowType_Promise_0100
     * @tc.name     When cellular data is turned on, call getCellularDataFlowType() to get the data stream type and
     *              verify that the callback value is within the enumeration value range
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_getCellularDataFlowType_Promise_0100", 0, async function (done) {
        try {
            await cellular.getCellularDataFlowType();
            console.log("Telephony_CellularData_getCellularDataFlowType_Promise_0100 finish");
            done();
        } catch (err) {
            expect().assertFail();
            console.log("Telephony_CellularData_getCellularDataFlowType_Promise_0100 fail");
            done();
        }
    })

    /*
     * @tc.number   Telephony_CellularData_isCellularDataRoamingEnabled_Promise_0200
     * @tc.name     Call isCellularDataRoamingEnabled, set the slotId to error values (3), to check the correction
     *              results
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_isCellularDataRoamingEnabled_Promise_0200", 0, async function (done) {
        try {
            await cellular.isCellularDataRoamingEnabled(ERR_SLOT_ID);
            expect().assertFail();
            console.log("Telephony_CellularData_isCellularDataRoamingEnabled_Promise_0200 fail");
            done();
        } catch (err) {
            console.log("Telephony_CellularData_isCellularDataRoamingEnabled_Promise_0200 finish");
            done();
        }
    })

    /*
     * @tc.number   Telephony_CellularData_setDefaultCellularDataSlotId_Promise_0500
     * @tc.name     Call setDefaultCellularDataSlotId() interface, set the default card slot to the number of exceptions
     *              (-1), and verify whether the setting failed (Async)
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_setDefaultCellularDataSlotId_Promise_0500", 0, async function (done) {
        try {
            await cellular.setDefaultCellularDataSlotId(ERR_SLOT_ID);
            expect().assertFail();
            console.log("Telephony_CellularData_setDefaultCellularDataSlotId_Promise_0500 fail");
            done();
        } catch (err) {
            console.log("Telephony_CellularData_setDefaultCellularDataSlotId_Promise_0500 finish");
            done();
        }
    })

    /*
     * @tc.number   Telephony_CellularData_getDefaultCellularDataSlotId_Promise_0500
     * @tc.name     Call getDefaultCellularDataSlotId() interface and verify whether the setting failed (Async)
     * @tc.desc     Function test
     */
    it("Telephony_CellularData_getDefaultCellularDataSlotId_Promise_0500", 0, async function (done) {
        try {
            await cellular.getDefaultCellularDataSlotId();
            expect().assertFail();
            console.log("Telephony_CellularData_getDefaultCellularDataSlotId_Promise_0500 fail");
            done();
        } catch (err) {
            console.log("Telephony_CellularData_getDefaultCellularDataSlotId_Promise_0500 finish");
            done();
        }
    })
})
