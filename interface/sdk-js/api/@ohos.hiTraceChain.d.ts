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

/**
 * Provides APIs to implement call chain tracing throughout a service process.
 * With HiTrace, you can quickly obtain the run log for the call chain of a 
 * specified service process and locate faults in cross-device, cross-process,
 * or cross-thread communications.
 *
 * @since 8
 * @syscap SystemCapability.HiviewDFX.HiTrace
 */
declare namespace hiTraceChain {
    /**
     * Enumerate trace flag
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     */
    enum HiTraceFlag {
        /**
         * default value
         */
        DEFAULT           = 0,

        /**
         * trace sync and async call. default: trace sync call only.
         */
        INCLUDE_ASYNC     = 1,

        /**
         * do not create child span. default: create child span.
         */
        DONOT_CREATE_SPAN = 1 << 1,

        /**
         * output tracepoint info in span. default: do not output tracepoint info.
         */
        TP_INFO           = 1 << 2,

        /**
         * do not output begin and end info. default: output begin and end info.
         */
        NO_BE_INFO        = 1 << 3,

        /**
         * do not add id to log. default: add id to log.
         */
        DISABLE_LOG  = 1 << 4,

        /**
         * the trace is triggered by fault.
         */
        FAILURE_TRIGGER   = 1 << 5,

        /**
         * output device-to-device tracepoint info in span only. default: do not output device-to-device tracepoint info.
         */
        D2D_TP_INFO       = 1 << 6,
    }

    /**
     * Enumerate trace point type
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     */
    enum HiTraceTracepointType {
        /**
         * client send
         */
        CS = 0,

        /**
         * client receive
         */
        CR = 1,

        /**
         * server send
         */
        SS = 2,

        /**
         * server receive
         */
        SR = 3,

        /**
         * general info
         */
        GENERAL = 4,
    }

    /**
     * Enumerate trace communication mode
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     */
    enum HiTraceCommunicationMode {
        /**
         * unspecified
         */
        DEFAULT = 0,

        /**
         * thread-to-thread
         */
        THREAD  = 1,

        /**
         * process-to-process
         */
        PROCESS = 2,

        /**
         * device-to-device
         */
        DEVICE  = 3,
    }

    /**
     * Trace id, for tracing process.
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     */
    interface HiTraceId {
        chainId: bigint; /* 0n: invalid */
        spanId?: number;
        parentSpanId?: number;
        flags?: number;
    }

    /**
     * Start tracing a process impl.
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     * @param {string} name Process name.
     * @param {number} flags Trace function flag.
     * @return {HiTraceId} Valid if first call, otherwise invalid.
     */
    function begin(name: string, flags: number = HiTraceFlag.DEFAULT): HiTraceId;

    /**
     * Stop process tracing and clear trace id of current thread if the given trace
     * id is valid, otherwise do nothing.
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     * @param {HiTraceId} id The trace id that need to stop.
     */
    function end(id: HiTraceId): void;

    /**
     * Get trace id of current thread, and return a invalid trace id if no
     * trace id belong to current thread
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     * @return {HiTraceId} Valid if current thread have a trace id, otherwise invalid.
     */
    function getId(): HiTraceId;

    /**
     * Set id as trace id of current thread. Do nothing if id is invalid.
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     * @param {HiTraceId} id Set id as trace id of current thread.
     */
    function setId(id: HiTraceId): void;

    /**
     * Clear trace id of current thread and set it invalid.
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     */
    function clearId(): void;

    /**
     * Create a new span id according to the trace id of current thread.
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     * @return {HiTraceId} A valid span trace id. Otherwise trace id of current thread if do not allow create span.
     */
    function createSpan(): HiTraceId;

    /**
     * Print hitrace info, include trace id info.
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     * @param {HiTraceCommunicationMode} mode Trace communication mode.
     * @param {HiTraceTracepointType} type Trace info type.
     * @param {HiTraceId} id Trace id that need to print.
     * @param {string} msg Customized info that need to print.
     */
    function tracepoint(mode: HiTraceCommunicationMode, type: HiTraceTracepointType, id: HiTraceId, msg?: string): void;

    /**
     * Judge whether the trace id is valid or not.
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     * @param {HiTraceId} id Trace id that need to judge.
     * @return {boolean} True for a valid trace id, otherwise false.
     */
    function isValid(id: HiTraceId): boolean;

    /**
     * Judge whether the trace id has enabled a trace flag or not.
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     * @param {HiTraceId} id Trace id that need to judge.
     * @param {HiTraceFlag} flag Trace flag that need to judge.
     * @return {boolean} true if the trace id has enabled the flag.
     */
    function isFlagEnabled(id: HiTraceId, flag: HiTraceFlag): boolean;

    /**
     * Enable the designative trace flag for the trace id.
     *
     * @since 8
     * @syscap SystemCapability.HiviewDFX.HiTrace
     * @param {HiTraceId} id Trace id that need to enable a flag. 
     * @param {HiTraceFlag} flag the designative trace flag that need to be enabled in the trace id.
     */
    function enableFlag(id: HiTraceId, flag: HiTraceFlag): void;
}

export default hiTraceChain;
