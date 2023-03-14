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
import { TagSession } from './tagSession';

/**
 * Provides interfaces to control the read and write of tags that support the NFC-A technology.
 *
 * <p>This class is inherited from the {@link TagSession} abstract class, and provides methods to create
 * {@code NfcATag} objects and obtain the ATQA and SAK.
 *
 * @since 7
 * @syscap SystemCapability.Communication.NFC.Core
 */
export interface NfcATag extends TagSession {
  /**
   * Obtains the SAK of an NFC-A tag.
   *
   * @return Returns the SAK of the NFC-A tag.
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getSak(): number;

  /**
   * Obtains the ATQA of an NFC-A tag.
   *
   * @return Returns the ATQA of the NFC-A tag.
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getAtqa(): number[];
}

/**
 * Provides interfaces to create an {@code NfcBTag} and perform I/O operations on the tag.
 *
 * <p>This class inherits from the {@link TagSession} abstract class and provides interfaces to create an
 * {@code NfcBTag} and obtain the tag information.
 *
 * @since 7
 * @syscap SystemCapability.Communication.NFC.Core
 */
export interface NfcBTag extends TagSession {
  /**
   * Obtains the application data of a tag.
   *
   * @return Returns the application data of the tag.
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getRespAppData(): number[];

  /**
   * Obtains the protocol information of a tag.
   *
   * @return Returns the protocol information of the tag.
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getRespProtocol(): number[];
}

/**
 * Provides methods for creating an NFC-F tag, obtaining tag information, and controlling tag read and write.
 *
 * <p>This class inherits from the {@link TagSession} abstract class and provides interfaces to create an
 * {@code NfcFTag} and obtain the tag information.
 *
 * @since 7
 * @syscap SystemCapability.Communication.NFC.Core
 */
export interface NfcFTag extends TagSession {
  /**
   * Obtains the system code from this {@code NfcFTag} instance.
   *
   * @return Returns the system code.
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getSystemCode(): number[];

  /**
   * Obtains the PMm (consisting of the IC code and manufacturer parameters) from this {@code NfcFTag} instance.
   *
   * @return Returns the PMm.
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getPmm(): number[];
}

/**
 * Provides methods for creating an NFC-V tag, obtaining tag information, and controlling tag read and write.
 *
 * <p>This class inherits from the {@link TagSession} abstract class and provides interfaces to create an
 * {@code NfcVTag} and obtain the tag information.
 *
 * @since 7
 * @syscap SystemCapability.Communication.NFC.Core
 */
export interface NfcVTag extends TagSession {
  /**
   * Obtains the response flags from this {@code NfcVTag} instance.
   *
   * @return Returns the response flags.
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getResponseFlags(): number;

  /**
   * Obtains the data storage format identifier (DSFID) from this {@code NfcVTag} instance.
   *
   * @return Returns the DSFID.
   * @permission ohos.permission.NFC_TAG
   *
   * @since 7
   */
  getDsfId(): number;
}
