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
* Provides options that can be set for the worker to create.
* @since 7
* @syscap SystemCapability.Utils.Lang
*/
export interface WorkerOptions {
  /**
   * Mode in which the worker executes the script.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  type?: "classic" | "module";

  /**
   * Name of the worker.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  name?: string;

  /**
   * Whether the worker is shared.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  shared?: boolean;
}

/**
 * Defines the event.
 * @since 7
 * @syscap SystemCapability.Utils.Lang
 */
export interface Event {
  /**
   * Type of the Event.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  readonly type: string;

  /**
   * Timestamp(accurate to millisecond) when the event is created.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  readonly timeStamp: number;
}

/**
 * Provides detailed information about the exception occurred during worker execution.
 * @since 7
 * @syscap SystemCapability.Utils.Lang
 */
 export interface ErrorEvent extends Event {
  /**
   * Information about the exception.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  readonly message: string;

  /**
   * File where the exception is located.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  readonly filename: string;

  /**
   * Number of the line where the exception is located.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  readonly lineno: number;

  /**
   * Number of the column where the exception is located.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  readonly colno: number;

  /**
   * Type of the exception.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  readonly error: Object;
}

/**
 * Holds the data transferred between worker threads.
 * @since 7
 * @syscap SystemCapability.Utils.Lang
 */
 export interface MessageEvent<T> extends Event {
  /**
   * Data transferred when an exception occurs.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  readonly data: T;
}

/**
 * Specifies the object whose ownership need to be transferred during data transfer.
 * The object must be ArrayBuffer.
 * @since 7
 * @syscap SystemCapability.Utils.Lang
 */
export interface PostMessageOptions {
  /**
   * ArrayBuffer array used to transfer the ownership.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  transfer?: Object[];
}

/**
 * Implements evemt listening.
 * @since 7
 * @syscap SystemCapability.Utils.Lang
 */
export interface EventListener {
  /**
   * Specifies the callback to invoke.
   * @param evt Event class for the callback to invoke.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  (evt: Event): void | Promise<void>;
}

/**
 * Type of message, only "message" and "messageerror".
 * @since 7
 * @syscap SystemCapability.Utils.Lang
 */
type MessageType = "message" | "messageerror";

/**
 * Specific event features.
 * @since 7
 * @syscap SystemCapability.Utils.Lang
 */
 export interface EventTarget {
  /**
   * Adds an event listener to the worker.
   * @param type  Type of the event to listen for.
   * @param listener Callback to invoke when an event of the specified type occurs.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  addEventListener(
    type: string,
    listener: EventListener
  ): void;

  /**
   * Dispatches the event defined for the worker.
   * @param event Event to dispatch.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  dispatchEvent(event: Event): boolean;

  /**
   * Removes an event defined for the worker.
   * @param type Type of the event for which the event listener is removed.
   * @param callback Callback of the event listener to remove.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  removeEventListener(
    type: string,
    callback?: EventListener
  ): void;

  /**
   * Removes all event listeners for the worker.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  removeAllListener(): void;
}

/**
 * Specifies the worker thread running environment, which is isolated from the host thread environment.
 * @since 7
 * @syscap SystemCapability.Utils.Lang
 */
declare interface WorkerGlobalScope extends EventTarget {
  /**
   * Worker name specified when there is a new worker.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  readonly name: string;

  /**
   * The onerror attribute of parentPort specifies
   * the event handler to be called when an exception occurs during worker execution.
   * The event handler is executed in the worker thread.
   * @param ev Error data.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  onerror?: (ev: ErrorEvent) => void;
  readonly self: WorkerGlobalScope & typeof globalThis;
}

/**
 * Specifies the worker thread running environment, which is isolated from the host thread environment
 * @since 7
 * @syscap SystemCapability.Utils.Lang
 */
 export interface DedicatedWorkerGlobalScope extends WorkerGlobalScope {
  /**
   * The onmessage attribute of parentPort specifies the event handler
   * to be called then the worker thread receives a message sent by
   * the host thread through worker postMessage.
   * The event handler is executed in the worker thread.
   * @param ev Message received.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  onmessage?: (this: DedicatedWorkerGlobalScope, ev: MessageEvent) => void;

  /**
   * The onmessage attribute of parentPort specifies the event handler
   * to be called then the worker receives a message that cannot be deserialized.
   * The event handler is executed in the worker thread.
   * @param ev Error data.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  onmessageerror?: (this: DedicatedWorkerGlobalScope, ev: MessageEvent) => void;

  /**
   * Close the worker thread to stop the worker from receiving messages
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  close(): void;

  /**
   * Send a message to be host thread from the worker
   * @param messageObject Data to be sent to the worker
   * @param transfer array cannot contain null.
   * @since 7
   * @syscap SystemCapability.Utils.Lang
   */
  postMessage(messageObject: Object, transfer: Transferable[]): void;
  postMessage(messageObject: Object, options?: PostMessageOptions): void;
}

/**
 * JS cross-thread communication tool
 * @since 7
 * @syscap SystemCapability.Utils.Lang
 */
declare namespace worker {
  class Worker extends EventTarget {
    /**
     * Creates a worker instance
     * @param scriptURL URL of the script to be executed by the worker
     * @param options Options that can be set for the worker
     * @since 7
     * @syscap SystemCapability.Utils.Lang
     */
    constructor(scriptURL: string, options?: WorkerOptions);

    /**
     * The onexit attribute of the worker specifies the event handler to be called
     * when the worker exits. The handler is executed in the host thread.
     * @param code Code indicating the worker exit state
     * @since 7
     * @syscap SystemCapability.Utils.Lang
     */
    onexit?: (code: number) => void;

    /**
     * The onerror attribute of the worker specifies the event handler to be called
     * when an exception occurs during worker execution.
     * The event handler is executed in the host thread.
     * @since 7
     * @syscap SystemCapability.Utils.Lang
     */
    onerror?: (err: ErrorEvent) => void;

    /**
     * The onmessage attribute of the worker specifies the event handler
     * to be called then the host thread receives a message created by itself
     * and sent by the worker through the parentPort.postMessage.
     * The event handler is executed in the host thread.
     * @param event Message received.
     * @since 7
     * @syscap SystemCapability.Utils.Lang
     */
    onmessage?: (event: MessageEvent) => void;

    /**
     * The onmessage attribute of the worker specifies the event handler
     * when the worker receives a message that cannot be serialized.
     * The event handler is executed in the host thread.
     * @since 7
     * @syscap SystemCapability.Utils.Lang
     */
    onmessageerror?: (event: MessageEvent) => void;

    /**
     * Sends a message to the worker thread.
     * The data is transferred using the structured clone algorithm.
     * @param message Data to be sent to the worker
     * @param transfer ArrayBuffer instance that can be transferred.
     * The transferList array cannot contain null.
     * @since 7
     * @syscap SystemCapability.Utils.Lang
     */
    postMessage(message: Object, transfer: ArrayBuffer[]): void;
    postMessage(message: Object, options?: PostMessageOptions): void;

    /**
     * Adds an event listener to the worker.
     * @param type Adds an event listener to the worker.
     * @param listener Callback to invoke when an event of the specified type occurs.
     * @since 7
     * @syscap SystemCapability.Utils.Lang
     */
    on(type: string, listener: EventListener): void;

    /**
     * Adds an event listener to the worker
     * and removes the event listener automatically after it is invoked once.
     * @param type Type of the event to listen for
     * @param listener Callback to invoke when an event of the specified type occurs
     * @since 7
     * @syscap SystemCapability.Utils.Lang
     */
    once(type: string, listener: EventListener): void;

    /**
     * Removes an event listener to the worker.
     * @param type Type of the event for which the event listener is removed.
     * @param listener Callback of the event listener to remove.
     * @since 7
     * @syscap SystemCapability.Utils.Lang
     */
    off(type: string, listener?: EventListener): void;

    /**
     * Terminates the worker thread to stop the worker from receiving messages
     * @since 7
     * @syscap SystemCapability.Utils.Lang
     */
    terminate(): void;
  }
  const parentPort: DedicatedWorkerGlobalScope;
}

export default worker;