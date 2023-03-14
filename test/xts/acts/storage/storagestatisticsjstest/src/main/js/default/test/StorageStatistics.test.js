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
    isNegativeNum,
    isArray,
    isInclude,
    describe,
    it,
    expect,
    storageStatistics,
    volumeManager,
    getPackageName,
} from "./Common";
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import bundle from '@ohos.bundle';

describe("storageStatistics", function () {

    beforeAll(async function (){
        let appInfo = await bundle.getApplicationInfo('ohos.acts.storage.statistics', 0, 100);
        let tokenID = appInfo.accessTokenId;
        let atManager = abilityAccessCtrl.createAtManager();
        let result = await atManager.grantUserGrantedPermission(tokenID, "ohos.permission.READ_MEDIA",1);
        console.log("tokenID:" + tokenID + "-result:" + result);
        let result1 = await atManager.verifyAccessToken(tokenID, "ohos.permission.READ_MEDIA");
        console.log("tokenID:" + tokenID + "-result:" + result1);
    });
    
    /**
     * @tc.number SUB_DF_VOLUME_GET_TOTAL_SIZE_OF_VOLUME_0000
     * @tc.name storage_statistics_test_get_total_size_of_volume_async_000
     * @tc.desc Test getTotalSizeOfVolume() interfaces.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_total_size_of_volume_async_000", 0, async function (done) {
        try {
            let volumes = await volumeManager.getAllVolumes();
            console.log(`promise getAllVolumes ===---=== ${JSON.stringify(volumes)}`);
            expect(isArray(volumes)).assertTrue();
            if (volumes.length > 0) {
                let totalSize = await storageStatistics.getTotalSizeOfVolume(volumes[0].uuid);
                console.log(`promise totalSize ===---=== ${totalSize}/1024 = ${totalSize / 1024}`);
                expect(isIntNum(totalSize) && !isNegativeNum(totalSize)).assertTrue();
            }
            done();
        } catch (e) {
            console.log("storage_statistics_test_get_total_size_of_volume_async_000 has failed for " + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_VOLUME_GET_TOTAL_SIZE_OF_VOLUME_0010
     * @tc.name storage_statistics_test_get_total_size_of_volume_async_001
     * @tc.desc Test getTotalSizeOfVolume() interfaces.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_total_size_of_volume_async_001", 0, async function (done) {
        try {
            let volumes = await volumeManager.getAllVolumes();
            expect(isArray(volumes)).assertTrue();
            if (volumes.length > 0) {
                storageStatistics.getTotalSizeOfVolume(volumes[0].uuid, (error, totalSize) => {
                    console.log(`callback totalSize ===---=== ${totalSize}/1024 = ${totalSize / 1024}`);
                    expect(isIntNum(totalSize) && !isNegativeNum(totalSize)).assertTrue();
                });
            }
            done();
        } catch (e) {
            console.log("storage_statistics_test_get_total_size_of_volume_async_001 has failed for " + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_VOLUME_GET_TOTAL_SIZE_OF_VOLUME_0020
     * @tc.name storage_statistics_test_get_total_size_of_volume_async_002
     * @tc.desc Test getTotalSizeOfVolume() interfaces.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_total_size_of_volume_async_002", 0, async function (done) {
        try {
            let totalSize = await storageStatistics.getTotalSizeOfVolume("1234");
            console.log(`async_002 totalSize ===---=== ${totalSize}`);
            expect(totalSize == -1).assertTrue();
            done();
        } catch (error) {
            console.log("storage_statistics_test_get_total_size_of_volume_async_002 has failed for " + error.message);
            expect(isInclude(error, "not a function") || isInclude(error, "is not callable")).assertTrue();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_VOLUME_GET_TOTAL_SIZE_OF_VOLUME_0030
     * @tc.name storage_statistics_test_get_total_size_of_volume_async_003
     * @tc.desc Test getTotalSizeOfVolume() interfaces.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_total_size_of_volume_async_003", 0, async function (done) {
        try {
            await storageStatistics.getTotalSizeOfVolume(1234);
            done();
        } catch (error) {
            console.log("storage_statistics_test_get_total_size_of_volume_async_003 has failed for " + error);
            expect(isInclude(error, "Invalid uuid")).assertTrue();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_VOLUME_GET_TOTAL_SIZE_OF_VOLUME_0040
     * @tc.name storage_statistics_test_get_total_size_of_volume_async_004
     * @tc.desc Test getTotalSizeOfVolume() interfaces.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_total_size_of_volume_async_004", 0, async function (done) {
        try {
            await storageStatistics.getTotalSizeOfVolume();
            done();
        } catch (error) {
            console.log("storage_statistics_test_get_total_size_of_volume_async_004 has failed for " + error);
            expect(isInclude(error, "Number of arguments unmatched")).assertTrue();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_VOLUME_GET_FREE_SIZE_OF_VOLUME_0000
     * @tc.name storage_statistics_test_get_free_size_of_volume_async_000
     * @tc.desc Test getFreeSizeOfVolume() interfaces.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_free_size_of_volume_async_000", 0, async function (done) {
        try {
            let volumes = await volumeManager.getAllVolumes();
            console.log(`promise getAllVolumes ===---=== ${JSON.stringify(volumes)}`);
            expect(isArray(volumes)).assertTrue();
            if (volumes.length > 0) {
                let freeSize = await storageStatistics.getFreeSizeOfVolume(volumes[0].id);
                console.log(`promise freeSize ===---=== ${freeSize}/1024 = ${freeSize / 1024}`);
                expect(isIntNum(freeSize) && !isNegativeNum(freeSize)).assertTrue();
            }
            done();
        } catch (e) {
            console.log("storage_statistics_test_get_free_size_of_volume_async_000 has failed for " + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_VOLUME_GET_FREE_SIZE_OF_VOLUME_0010
     * @tc.name storage_statistics_test_get_free_size_of_volume_async_001
     * @tc.desc Test getFreeSizeOfVolume() interfaces.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_free_size_of_volume_async_001", 0, async function (done) {
        try {
            let volumes = await volumeManager.getAllVolumes();
            expect(isArray(volumes)).assertTrue();
            if (volumes.length > 0) {
                storageStatistics.getFreeSizeOfVolume(volumes[0].uuid, (error, freeSize) => {
                    console.log(`callback freeSize ===---=== ${freeSize}/1024 = ${freeSize / 1024}`);
                    expect(isIntNum(freeSize) && !isNegativeNum(freeSize)).assertTrue();
                });
            }
            done();
        } catch (e) {
            console.log("storage_statistics_test_get_free_size_of_volume_async_001 has failed for " + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_VOLUME_GET_FREE_SIZE_OF_VOLUME_0020
     * @tc.name storage_statistics_test_get_free_size_of_volume_async_002
     * @tc.desc Test getFreeSizeOfVolume() interfaces.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_free_size_of_volume_async_002", 0, async function (done) {
        try {
            let freeSize = await storageStatistics.getFreeSizeOfVolume("1234");
            console.log(`async_002 freeSize ===---=== ${freeSize}`);
            expect(freeSize == -1).assertTrue();
            done();
        } catch (error) {
            console.log("storage_statistics_test_get_free_size_of_volume_async_002 has failed for " + error);
            expect(isInclude(error, "not a function") || isInclude(error, "is not callable")).assertTrue();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_VOLUME_GET_FREE_SIZE_OF_VOLUME_0030
     * @tc.name storage_statistics_test_get_free_size_of_volume_async_003
     * @tc.desc Test getFreeSizeOfVolume() interfaces.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_free_size_of_volume_async_003", 0, async function (done) {
        try {
            await storageStatistics.getFreeSizeOfVolume(1234);
            done();
        } catch (error) {
            console.log("storage_statistics_test_get_free_size_of_volume_async_003 has failed for " + error);
            expect(isInclude(error, "Invalid uuid")).assertTrue();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_VOLUME_GET_FREE_SIZE_OF_VOLUME_0040
     * @tc.name storage_statistics_test_get_free_size_of_volume_async_004
     * @tc.desc Test getFreeSizeOfVolume() interfaces.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_free_size_of_volume_async_004", 0, async function (done) {
        try {
            await storageStatistics.getFreeSizeOfVolume();
            done();
        } catch (error) {
            console.log("storage_statistics_test_get_free_size_of_volume_async_004 has failed for " + error);
            expect(isInclude(error, "Number of arguments unmatched")).assertTrue();
            done();
        }
    });

    /**
   	 * @tc.number SUB_DF_STORAGE_STATISTICS_GET_BUNDLE_STAT_0000
   	 * @tc.name storage_statistics_test_get_bundle_stat_async_000
   	 * @tc.desc Test getBundleStats() interfaces, returned in promise mode.
   	 * @tc.size MEDIUM
   	 * @tc.type Function
   	 * @tc.level Level 0
   	 * @tc.require
   	 */
    it("storage_statistics_test_get_bundle_stat_async_000", 0, async function (done) {
        try {
            let packageName = await getPackageName();
            let bundleStat = await storageStatistics.getBundleStats(packageName);
            expect(bundleStat != null).assertTrue();
            expect(isIntNum(bundleStat.appSize) && !isNegativeNum(bundleStat.appSize)).assertTrue();
            expect(isIntNum(bundleStat.cacheSize) && !isNegativeNum(bundleStat.cacheSize)).assertTrue();
            expect(isIntNum(bundleStat.dataSize) && !isNegativeNum(bundleStat.dataSize)).assertTrue();
            console.log(`promise bundleStat ===---=== ${JSON.stringify(bundleStat)}`);
            done();
        } catch (e) {
            console.log("storage_statistics_test_get_bundle_stat_async_000 has failed for " + e);
            expect(null).assertFail();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_STORAGE_STATISTICS_GET_BUNDLE_STAT_0010
     * @tc.name storage_statistics_test_get_bundle_stat_async_001
     * @tc.desc Test getBundleStats() interfaces , returned in callback mode.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_bundle_stat_async_001", 0, async function (done) {
        try {
            let packageName = await getPackageName();
            storageStatistics.getBundleStats(packageName, (error, bundleStat) => {
                expect(bundleStat != null).assertTrue();
                expect(isIntNum(bundleStat.appSize) && !isNegativeNum(bundleStat.appSize)).assertTrue();
                expect(isIntNum(bundleStat.cacheSize) && !isNegativeNum(bundleStat.cacheSize)).assertTrue();
                expect(isIntNum(bundleStat.dataSize) && !isNegativeNum(bundleStat.dataSize)).assertTrue();
                console.log(`callback bundleStat ===---=== ${JSON.stringify(bundleStat)}`);
                done();
            });
        } catch (e) {
            console.log("storage_statistics_test_get_bundle_stat_async_001 has failed for " + e);
            expect(null).assertFail();
        }
    });

    /**
     * @tc.number SUB_DF_STORAGE_STATISTICS_GET_BUNDLE_STAT_0020
     * @tc.name storage_statistics_test_get_bundle_stat_async_002
     * @tc.desc Test getBundleStats() interfaces, the parameter packagename is errors, returning error results.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_bundle_stat_async_002", 0, async function (done) {
        try {
            await storageStatistics.getBundleStats("packageName");
            done();
        } catch (e) {
            console.log("storage_statistics_test_get_bundle_stat_async_002 has failed for " + e);
            expect(e.message == "Invalid name").assertTrue();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_STORAGE_STATISTICS_GET_BUNDLE_STAT_0030
     * @tc.name storage_statistics_test_get_bundle_stat_async_003
     * @tc.desc Test getBundleStats() interfaces, When the parameter type is wrong, returns the correct result.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_bundle_stat_async_003", 0, async function (done) {
        try {
            await storageStatistics.getBundleStats(1);
        } catch (e) {
            console.log("storage_statistics_test_get_bundle_stat_async_003 has failed for " + e);
            expect(e.message == "Invalid name").assertTrue();
            done();
        }
    });

    /**
     * @tc.number SUB_DF_STORAGE_STATISTICS_GET_BUNDLE_STAT_0040
     * @tc.name storage_statistics_test_get_bundle_stat_async_004
     * @tc.desc Test getBundleStats() interfaces, When there are no parameters, returning error results.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     * @tc.require
     */
    it("storage_statistics_test_get_bundle_stat_async_004", 0, async function (done) {
        try {
            await storageStatistics.getBundleStats();
        } catch (e) {
            console.log("storage_statistics_test_get_bundle_stat_async_004 has failed for " + e);
            expect(e.message == "Number of arguments unmatched").assertTrue();
            done();
        }
    });
});
