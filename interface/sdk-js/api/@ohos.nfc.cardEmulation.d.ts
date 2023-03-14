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

/**
 * Provides methods to operate or manage NFC card emulation.
 *
 * @import import cardEmulation from '@ohos.nfc.cardEmulation';
 *
 * @since 6
 * @syscap SystemCapability.Communication.NFC.Core
 */
declare namespace cardEmulation {
  enum FeatureType {
    /** This constant is used to check whether HCE card emulation is supported. */
    HCE = 0,

    /** This constant is used to check whether SIM card emulation is supported. */
    UICC = 1,

    /** This constant is used to check whether eSE card emulation is supported. */
    ESE = 2,
  }

  /**
   * Checks whether a specified type of card emulation is supported.
   *
   * <p>This method is used to check Whether the host or secure element supports card emulation.
   *
   * @param feature Indicates the card emulation type, {@code HCE}, {@code UICC}, or {@code ESE}.
   * @return Returns {@code true} if the specified type of card emulation is supported; returns
   * {@code false} otherwise.
   *
   * @since 6
   */
  function isSupported(feature: number): boolean;

  /**
   * A class for NFC host application.
   *
   * <p>The NFC host application use this class, then Nfc service can access the application
   * installation information and connect to services of the application.
   *
   * @since 8
   * @syscap SystemCapability.Communication.NFC.Core
   */
  export class HceService {
    /**
     * start HCE
     *
     * @return Returns {@code true} if HCE is enabled or has been enabled; returns {@code false} otherwise.
     * @permission ohos.permission.NFC_CARD_EMULATION
     *
     * @since 8
     */
    startHCE(aidList: string[]): boolean;

    /**
     * stop HCE
     *
     * @return Returns {@code true} if HCE is disabled or has been disabled; returns {@code false} otherwise.
     * @permission ohos.permission.NFC_CARD_EMULATION
     *
     * @since 8
     */
    stopHCE(): boolean;


    /**
     * register HCE event to receive the APDU data.
     *
     * @param type the type to register.
     * @param callback Callback used to listen for HCE data device received.
     * @permission ohos.permission.NFC_CARD_EMULATION
     *
     * @since 8
     */

    on(type: "hceCmd", callback: AsyncCallback<number[]>): void;

    /**
     * Sends a response APDU to the remote device.
     *
     * <p>This method is used by a host application when swiping card.
     *
     * @param responseApdu Indicates the response, which is a byte array.
     * @permission ohos.permission.NFC_CARD_EMULATION
     *
     * @since 8
     */
    sendResponse(responseApdu: number[]): void;
  }
}
export default cardEmulation;
