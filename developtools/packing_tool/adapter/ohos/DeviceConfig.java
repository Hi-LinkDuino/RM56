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

package ohos;

/**
 * App DeviceConfig info.
 *
 */
public class DeviceConfig {
    /**
     * Indicates the target request Sdk Version of app DeviceConfig.
     */
    public String targetReqSdk = "";

    /**
     * Indicates the compatible request Sdk Version of app DeviceConfig.
     */
    public String compatibleReqSdk = "";

    /**
     * Indicates the jointUserid of app DeviceConfig.
     */
    public String jointUserid = "";

    /**
     * Indicates the process of app DeviceConfig.
     */
    public String process = "";

    /**
     * Indicates the arkFlag of app DeviceConfig.
     */
    public String arkFlag = "";

    /**
     * Indicates the targetArkVersion of app DeviceConfig.
     */
    public String targetArkVersion = "";

    /**
     * Indicates the compatibleArkVersion of app DeviceConfig.
     */
    public String compatibleArkVersion = "";

    /**
     * Indicates the directLaunch of app DeviceConfig.
     */
    public boolean directLaunch = false;
}
