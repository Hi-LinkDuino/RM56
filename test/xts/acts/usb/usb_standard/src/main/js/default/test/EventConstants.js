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

/*
 * Constants of events that will be registered to system.
 */
const EventConstants = {

  /**
   * Bitmask used for extracting the USBEndpoint type from it's address
   */
  USB_ENDPOINT_XFERTYPE_MASK: 0x03,

  /**
   * Control USBEndpoint type
   */
  USB_ENDPOINT_XFER_CONTROL: 0,

  /**
   * Isochronous USBEndpoint type
   */
  USB_ENDPOINT_XFER_ISOC: 1,

  /**
   * Bulk USBEndpoint type
   */
  USB_ENDPOINT_XFER_BULK: 2,

  /**
   * Interrupt USBEndpoint type
   */
  USB_ENDPOINT_XFER_INT: 3,

  /**
   * Bitmask used for extracting the USBEndpoint number from it's address
   */
  USB_ENDPOINT_NUMBER_MASK: 0x0f,

  /**
   * Bitmask used for extracting the USBEndpoint direction from it's address
   */
  USB_ENDPOINT_DIR_MASK: 0x80,

  /**
   * Used to signify direction of data for USBEndpoint is OUT, host to device
   */
  USB_ENDPOINT_DIR_OUT: 0,

  /**
   * Used to signify direction of data for USBEndpoint is IN, device to host
   */
  USB_ENDPOINT_DIR_IN: 0x80
}

export default EventConstants;