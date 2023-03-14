/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import {
    isIntNum,
    describe,
    it,
    expect,
    volumeManager,
    isBoolean,
    isString,
    isInclude,
    isArray,
} from "./Common";

describe("volumeManager", function () {

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_GET_ALL_VOLUMES_0000
     * @tc.name volume_manager_test_get_all_volumes_async_000
     * @tc.desc Test getAllVolumes() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_get_all_volumes_async_000", 0, async function (done) {
        try {
            let volumes = await volumeManager.getAllVolumes();
            console.log(`promise getAllVolumes ===---=== ${JSON.stringify(volumes)}`);
            expect(isArray(volumes)).assertTrue();
            if (volumes.length > 0) {
                var id = volumes[0].id;
                var uuid = volumes[0].uuid;
                var description = volumes[0].description;
                var removable = volumes[0].removable;
                var state = volumes[0].state;
                var path = volumes[0].path;
                expect(isString(id)).assertTrue();
                expect(isString(uuid)).assertTrue();
                expect(isString(description)).assertTrue();
                expect(isBoolean(removable)).assertTrue();
                expect(isIntNum(state)).assertTrue();
                expect(isString(path)).assertTrue();
            }
        } catch (e) {
            console.log("volume_manager_test_get_all_volumes_async_000 has failed for :" + e);
            expect(null).assertFail();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_GET_ALL_VOLUMES_0010
     * @tc.name volume_manager_test_get_all_volumes_async_001
     * @tc.desc Test getAllVolumes() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_get_all_volumes_async_001", 0, async function (done) {
        try {
            volumeManager.getAllVolumes((error, volumes) => {
                console.log(`callback getAllVolumes ===---=== ${JSON.stringify(volumes)}`);
                expect(isArray(volumes)).assertTrue();
                if (volumes.length > 0) {
                    var id = volumes[0].id;
                    var uuid = volumes[0].uuid;
                    var description = volumes[0].description;
                    var removable = volumes[0].removable;
                    var state = volumes[0].state;
                    var path = volumes[0].path;
                    expect(isString(id)).assertTrue();
                    expect(isString(uuid)).assertTrue();
                    expect(isString(description)).assertTrue();
                    expect(isBoolean(removable)).assertTrue();
                    expect(isIntNum(state)).assertTrue();
                    expect(isString(path)).assertTrue();
                }
            });
        } catch (e) {
            console.log("volume_manager_test_get_all_volumes_async_001 has failed for " + e);
            expect(null).assertFail();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_GET_ALL_VOLUMES_0020
     * @tc.name volume_manager_test_get_all_Volumes_async_002
     * @tc.desc Test getAllVolumes() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_get_all_Volumes_async_002", 0, async function (done) {
        try {
            await volumeManager.getAllVolumes(2);
        } catch (error) {
            console.log(`volume_manager_test_get_all_Volumes_async_002 has failed for error ${error}`);
            expect(isInclude(error, "The callback shall be a function")).assertTrue();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_GET_ALL_VOLUMES_0030
     * @tc.name volume_manager_test_get_all_Volumes_async_003
     * @tc.desc Test getAllVolumes() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_get_all_Volumes_async_003", 0, async function (done) {
        try {
            await volumeManager.getAllVolumes("3");
            expect(null).assertFail();
        } catch (error) {
            console.log(`volume_manager_test_get_all_Volumes_async_003 has failed for error ${error}`);
            expect(isInclude(error, "The callback shall be a function")).assertTrue();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_GET_ALL_VOLUMES_0040
     * @tc.name volume_manager_test_get_all_Volumes_async_004
     * @tc.desc Test getAllVolumes() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_get_all_Volumes_async_004", 0, async function (done) {
        try {


            await volumeManager.getAllVolumes(false);
            expect(null).assertFail();
        } catch (error) {


            console.log(`volume_manager_test_get_all_Volumes_async_004 has failed f
            or error ${error}`);
            expect(isInclude(error, "The callback shall be a function")).assertTrue();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_MOUNT_0000
     * @tc.name volume_manager_test_mount_async_000
     * @tc.desc Test mount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_mount_async_000", 0, async function (done) {
        try {
            let volumes = await volumeManager.getAllVolumes();
            expect(isArray(volumes)).assertTrue();
            if (volumes.length > 0) {
                var id = volumes[0].id;
                let bool = await volumeManager.mount(id);
                console.log(`promise mount ===---=== ${bool}`);
                expect(isBoolean(bool));
            }
        } catch (e) {
            console.log(`volume_manager_test_mount_async_000 has failed for ${e}`);
            expect(null).assertFail();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_MOUNT_0010
     * @tc.name volume_manager_test_mount_async_001
     * @tc.desc Test mount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_mount_async_001", 0, async function (done) {
        try {
            let volumes = await volumeManager.getAllVolumes();
            expect(isArray(volumes)).assertTrue();
            if (volumes.length > 0) {
                var id = volumes[0].id;
                volumeManager.mount(id, (error, bool) => {
                    console.log(`callback mount ===---=== ${bool}`);
                    expect(isBoolean(bool)).assertTrue();
                });
            }
        } catch (e) {
            console.log(`volume_manager_test_mount_async_001 has failed for ${e}`);
            expect(null).assertFail();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_MOUNT_0020
     * @tc.name volume_manager_test_mount_async_002
     * @tc.desc Test mount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_mount_async_002", 0, async function (done) {
        try {
            await volumeManager.mount(2);
            expect(null).assertFail();
        } catch (error) {
            console.log(`volume_manager_test_mount_async_002 has failed for ${error}`);
            expect(isInclude(error, "Invalid volumeId")).assertTrue();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_MOUNT_0030
     * @tc.name volume_manager_test_mount_async_003
     * @tc.desc Test mount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_mount_async_003", 0, async function (done) {
        try {
            await volumeManager.mount("3");
            expect(null).assertFail();
        } catch (error) {
            console.log(`volume_manager_test_mount_async_003 has failed for ${error}`);
            expect(isInclude(error, "not a function") || isInclude(error, "is not callable")).assertTrue();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_MOUNT_0040
     * @tc.name volume_manager_test_mount_async_004
     * @tc.desc Test mount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_mount_async_004", 0, async function (done) {
        try {
            await volumeManager.mount(false);
            expect(null).assertFail();
        } catch (error) {
            console.log(`volume_manager_test_mount_async_004 has failed for ${error}`);
            expect(isInclude(error, "Invalid volumeId")).assertTrue();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_MOUNT_0050
     * @tc.name volume_manager_test_mount_async_005
     * @tc.desc Test mount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_mount_async_005", 0, async function (done) {
        try {
            await volumeManager.mount();
            expect(null).assertFail();
        } catch (error) {
            console.log(`volume_manager_test_mount_async_005 has failed for ${error}`);
            expect(isInclude(error, "Number of arguments unmatched")).assertTrue();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_UNMOUNT_0000
     * @tc.name volume_manager_test_unmount_async_000
     * @tc.desc Test unmount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_unmount_async_000", 0, async function (done) {
        try {
            let volumes = await volumeManager.getAllVolumes();
            expect(isArray(volumes)).assertTrue();
            if (volumes.length > 0) {
                var id = volumes[0].id;
                let bool = await volumeManager.unmount(id);
                console.log(`promise unmount ===---=== ${bool}`);
                expect(isBoolean(bool)).assertTrue();
            }
        } catch (e) {
            console.log(`volume_manager_test_unmount_async_000 has failed for ${e}`);
            expect(null).assertFail();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_UNMOUNT_0010
     * @tc.name volume_manager_test_unmount_async_001
     * @tc.desc Test unmount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_unmount_async_001", 0, async function (done) {
        try {
            let volumes = await volumeManager.getAllVolumes();
            expect(isArray(volumes)).assertTrue();
            if (volumes.length > 0) {
                var id = volumes[0].id;
                volumeManager.unmount(id, (error, bool) => {
                    console.log(`callback unmount ===---=== ${bool}`);
                    expect(isBoolean(bool)).assertTrue();
                });
            }
        } catch (e) {
            console.log(`volume_manager_test_unmount_async_001 has failed for ${e}`);
            expect(null).assertFail();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_UNMOUNT_0020
     * @tc.name volume_manager_test_unmount_async_002
     * @tc.desc Test unmount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_unmount_async_002", 0, async function (done) {
        try {
            await volumeManager.unmount();
            expect(null).assertFail();
        } catch (error) {
            console.log(`volume_manager_test_unmount_async_002 has failed for ${error}`);
            expect(isInclude(error, "Number of arguments unmatched")).assertTrue();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_UNMOUNT_0030
     * @tc.name volume_manager_test_unmount_async_003
     * @tc.desc Test unmount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_unmount_async_003", 0, async function (done) {
        try {
            await volumeManager.unmount(false);
            expect(null).assertFail();
        } catch (error) {
            console.log(`volume_manager_test_unmount_async_003 has failed for ${error}`);
            expect(isInclude(error, "Invalid volumeId")).assertTrue();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_UNMOUNT_0040
     * @tc.name volume_manager_test_unmount_async_004
     * @tc.desc Test unmount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_unmount_async_004", 0, async function (done) {
        try {
            await volumeManager.unmount("4");
            expect(null).assertFail();
        } catch (error) {
            console.log(`volume_manager_test_unmount_async_004 has failed for ${error}`);
            expect(isInclude(error, "not a function") || isInclude(error, "is not callable")).assertTrue();
        }
        done();
    });

    /**
     * @tc.number SUB_DF_VOLUME_MANAGER_UNMOUNT_0050
     * @tc.name volume_manager_test_unmount_async_005
     * @tc.desc Test unmount() interfaces
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("volume_manager_test_unmount_async_005", 0, async function (done) {
        try {
            await volumeManager.unmount(5);
            expect(null).assertFail();
        } catch (error) {
            console.log(`volume_manager_test_unmount_async_005 has failed for ${error}`);
            expect(isInclude(error, "Invalid volumeId")).assertTrue();
        }
        done();
    });
});
