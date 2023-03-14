/*
* Copyright (C) 2022 Huawei Device Co., Ltd.
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

/**
 * Provide interfaces related to debugger access and obtaining CPU, 
 * memory and other virtual machine information during runtime for JS programs
 *
 * @syscap SystemCapability.HiviewDFX.HiProfiler.HiDebug
 * @import import hidebug from '@ohos.hidebug'
 * @since 8
 */
declare namespace hidebug {
    /**
     * Get total native heap memory size
     * @param -
     * @return Returns total native heap memory size.
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiProfiler.HiDebug
     */
    function getNativeHeapSize() : bigint;

    /**
     * Get Native heap memory allocation size.
     * @param -
     * @return Returns native heap memory allocation size.
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiProfiler.HiDebug
     */
    function getNativeHeapAllocatedSize() : bigint;

    /**
     * Get Native heap memory free size
     * @param -
     * @return Returns native heap memory free size.
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiProfiler.HiDebug
     */
    function getNativeHeapFreeSize() : bigint;

    /**
     * Get application process proportional set size memory information
     * @param -
     * @return Returns application process proportional set size memory information.
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiProfiler.HiDebug
     */
    function getPss() : bigint;

    /**
     * Obtains the size of the shared dirty memory of a process.
     * @param -
     * @return Returns the size of the shared dirty memory.
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiProfiler.HiDebug
     */
    function getSharedDirty() : bigint;

    /**
     * Start CPU Profiling.
     * The input parameter is a user-defined file name, excluding the file suffix.
     * The generated file is in the files folder under the application directory.
     * such as "/data/accounts/account_0/appdata/[package name]/files/cpuprofiler-xxx.json"
     * 
     * @param filename Indicates the user-defined file name,  excluding the file suffix.
     * @return -
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiProfiler.HiDebug
     */
    function startProfiling(filename : string) : void;

    /**
     * Stop CPU Profiling.
     * It takes effect only when the CPU profiler is turned on
     * 
     * @param -
     * @return -
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiProfiler.HiDebug
     */
    function stopProfiling() : void;

    /**
     * Dump JS Virtual Machine Heap Snapshot.
     * The input parameter is a user-defined file name, excluding the file suffix.
     * The generated file is in the files folder under the application directory.
     * such as "/data/accounts/account_0/appdata/[package name]/files/xxx.heapsnapshot"
     * 
     * @param filename Indicates the user-defined file name, excluding the file suffix.
     * @return -
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiProfiler.HiDebug
     */
    function dumpHeapData(filename : string) : void;
}
export default hidebug;
