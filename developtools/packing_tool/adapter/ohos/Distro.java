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
 * Distro info.
 *
 */
public class Distro {
    /**
     * Indicates the moduleName of Distro.
     */
    public String moduleName = "";

    /**
     * Indicates the moduleType of Distro.
     */
    public String moduleType = "";

    /**
     * Indicates the deliveryWithInstall of Distro.
     */
    public boolean deliveryWithInstall = false;

    /**
     *  Indicates the installationFree of Distro.
     */
    public int installationFree = 2;

    /**
     *  Indicates the virtualMachine of Distro.
     */
    public String virtualMachine = "default";
}