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
 * Hap zip info
 *
 */
class HapZipInfo {
    private byte[] resDataBytes;
    private String harmonyProfileJsonStr = "";
    private String packInfoJsonStr = "";
    private String hapFileName = "";

    /**
     * Get resource data bytes.
     *
     * @return resource data bytes.
     */
    public byte[] getResDataBytes() {
        return resDataBytes;
    }

    /**
     * Set resource data bytes.
     *
     * @param resDataBytes Indicates the resource data bytes.
     */
    public void setResDataBytes(byte[] resDataBytes) {
        this.resDataBytes = resDataBytes;
    }

    /**
     * Get harmony profile json string.
     *
     * @return harmony profile json string.
     */
    public String getHarmonyProfileJsonStr() {
        return harmonyProfileJsonStr;
    }

    /**
     * Set harmony profile json string.
     *
     * @param harmonyProfileJsonStr Indicates harmony profile json string.
     */
    public void setHarmonyProfileJsonStr(String harmonyProfileJsonStr) {
        this.harmonyProfileJsonStr = harmonyProfileJsonStr;
    }

    /**
     * Get pack.info json string.
     *
     * @return pack.info json string.
     */
    public String getPackInfoJsonStr() {
        return packInfoJsonStr;
    }

    /**
     * Set pack.info json string.
     *
     * @param packInfoJsonStr Indicates the pack.info json string.
     */
    public void setPackInfoJsonStr(String packInfoJsonStr) {
        this.packInfoJsonStr = packInfoJsonStr;
    }

    /**
     * Get hap file name.
     *
     * @return hap file name.
     */
    public String getHapFileName() {
        return hapFileName;
    }

    /**
     * Set hap file name.
     *
     * @param hapFileName Indicates the hap file name.
     */
    public void setHapFileName(String hapFileName) {
        this.hapFileName = hapFileName;
    }
}
