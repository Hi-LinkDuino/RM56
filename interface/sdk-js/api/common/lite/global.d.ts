/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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
 * Sets the interval for repeatedly calling a function.
 * @param handler Indicates the function to be called repeatedly at the interval.
 * @param delay Indicates the interval between each two calls, in milliseconds. The function will be called after this delay.
 * @param arguments Indicates additional arguments to pass to "handler" when the timer goes off.
 * @returns Returns the timer ID.
 */
export declare function setInterval(
  handler: Function,
  delay: number,
  ...arguments: any[]
): number;

/**
 * Sets a timer after which a function will be executed.
 * @param handler Indicates the function to be called after the timer goes off.
 * @param delay Indicates the delay (in milliseconds) after which the function will be called. If this parameter is left empty, default value "0" will be used, which means that the function will be called immediately or as soon as possible.
 * @param arguments Indicates additional arguments to pass to "handler" when the timer goes off.
 * @returns Returns the timer ID.
 */
export declare function setTimeout(
  handler: Function,
  delay?: number,
  ...arguments: any[]
): number;

/**
 * Cancels the interval set by " setInterval()".
 * @param intervalID Indicates the timer ID returned by "setInterval()".
 */
export declare function clearInterval(intervalID?: number): void;

/**
 * Cancels the timer set by " setTimeout()".
 * @param timeoutID Indicates the timer ID returned by "setTimeout()".
 */
export declare function clearTimeout(timeoutID?: number): void;

/**
 * Get the java interface instance. The java instance needs to register, otherwise it cannot be obtained.
 * After obtaining the instance, you can call the function with the same name on the Java side.
 * @param name Java interface name, including package path, such as com.example.test.timeinterfaceimpl.
 * @returns A promise object is returned. The resolve callback is the object of PA. The reject callback returns the object containing code and error data.
 * @since 5
 * @deprecated since 8
 */
export declare function createLocalParticleAbility(name?: string): any;

/**
 * Conditional compilation for rich equipment
 */
export declare const STANDARD: string;

/**
 * Conditional compilation for lite equipment
 */
export declare const LITE: string;

/**
 * Defining syscap function.
 * @since 8
 */
export declare function canIUse(syscap: string): boolean;

/**
 * @author yongxianglai@bestechnic.com
 * 
 * 显示一个 duckweed 消息。
 * option 对象字段如下：
 *   duation: number          //持续时间，单位为毫秒。例：2000
 *   side: string              //显示的屏幕位置，可取值为 'top'|'center'|'bottom'。例：'center'
 *   background: color        // 仅支持十六进制表示的颜色值字符串，支持 alpha 通道。例：'#ff123456'
 *   textColor: color         //仅支持十六进制表示的颜色值字符串，支持 alpha 通道。例：'#ff123456'
 *   textSize: size           //字体字号大小，单位为 pixel，不小于 12。例： 30
 *   fadeInOutTimes: array  //淡入 & 淡出时长，仅支持包含两个元素的数组。元素数值类型为整数，单位为毫秒，取值范围应不小于0。例：[1000, 2000]
 *   padding: size            //内容的边距大小，单位为 pixel
 *   textSytle: array         //富文本配置，值为一个数组。数组元素为长度为 3 的数组，这 3 个元素分别表示 样式（string），起始位置（number），结束位置（number）
 *                               //  样式可取的值为 'normal'|'bold'|'italic'|'bold_italic'，分别表示 普通样式 | 加粗 | 斜体 | 加粗&斜体。
 *                               //  默认不使用富文本。
 * 
 * @syscap SystemCapability.ArkUI.ArkUI.Lite
 * @param content 文本内容
 * @param option 样式配置
 * @returns duckweed 消息的唯一识别 id
 */
export declare function showDuckweed(content: string, option?: object): number;

/**
 * @author yongxianglai@bestechnic.com
 * 
 * 清除指定的 duckweed 消息。当 id 参数缺省时，视为清除所有的 duckweed 消息。
 * 
 * @syscap SystemCapability.ArkUI.ArkUI.Lite
 * @param id 目标 duckweed 消息的唯一识别 id
 * @returns duckweed 消息的唯一识别 id
 */
export declare function hideDuckweed(id?: number): void;
