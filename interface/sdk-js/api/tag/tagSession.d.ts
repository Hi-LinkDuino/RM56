/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
import tag from '../ohos.nfc.tag';
import { AsyncCallback } from './basic';

/**
 * Controls tag read and write.
 *
 * <p>Classes for different types of tags inherit from this abstract class to control connections to
 * tags, read data from tags, and write data to tags.
 *
 * @since 7
 * @syscap SystemCapability.Communication.NFC.Core
 */
export interface TagSession {
  /**
   * Obtains the tag information.
   *
   * @return Returns the tag information, which is a {@link TagInfo} object.
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getTagInfo(): tag.TagInfo;

  /**
   * Connects to a tag.
   *
   * <p>This method must be called before data is read from or written to the tag.
   *
   * @return Returns {@code true} if the connection is set up; returns {@code false} otherwise.
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  connectTag(): boolean;

  /**
   * Resets a connection with a tag and restores the default timeout duration for writing data to the tag.
   *
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  reset(): void;

  /**
   * Checks whether a connection has been set up with a tag.
   *
   * @return Returns {@code true} if a connection has been set up with the tag;
   * returns {@code false} otherwise.
   *
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  isTagConnected(): boolean;

  /**
   * Sets the timeout duration (ms) for sending data to a tag.
   *
   * <p>If data is not sent to the tag within the duration, data sending fails.
   *
   * @param timeout Indicates the timeout duration to be set.
   * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
   *
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  setSendDataTimeout(timeout: number): boolean;

  /**
   * Queries the timeout duration (ms) for sending data to a tag.
   *
   * @return Returns the timeout duration.
   *
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getSendDataTimeout(): number;

  /**
   * Writes data to a tag.
   *
   * @param data Indicates the data to be written to the tag.
   * @return Returns bytes received in response. Or bytes with a length of 0 if the
   * data fails to be written to the tag.
   *
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  sendData(data: number[]): Promise<number[]>;

  /**
   * Writes data to a tag.
   *
   * @param data Indicates the data to be written to the tag.
   * @return Returns bytes received in response. Or bytes with a length of 0 if the
   * data fails to be written to the tag.
   *
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  sendData(data: number[], callback: AsyncCallback<number[]>): void;

  /**
   * Queries the maximum length of data that can be sent to a tag.
   *
   * @return Returns the maximum length of the data to be sent to the tag.
   *
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getMaxSendLength(): number;
}
