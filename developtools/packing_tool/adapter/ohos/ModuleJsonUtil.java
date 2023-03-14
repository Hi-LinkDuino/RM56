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

import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.Map;

import java.util.Locale;

public class ModuleJsonUtil {
    private static final String  VERSION_CODE = "\"versionCode\"";
    private static final String VERSION_NAME = "\"versionName\"";
    private static final Log LOG = new Log(ModuleJsonUtil.class.toString());

    /**
     * get the versionCode from json file.
     *
     * @param jsonStr uncompress json object
     * @return the result
     */
     public static Version getVersion(String jsonString) throws BundleException {
         Version version = new Version();
         try {
             if (jsonString != null) {
                // find versionName
                int indexOfVersionName = jsonString.indexOf(VERSION_NAME);
                if (indexOfVersionName <= 0) {
                    LOG.error("ModuleJsonUtil::getVersion obtain index of VERSION_NAME failed");
                    return version;
                }
                indexOfVersionName += VERSION_NAME.length();
                int nameStart = jsonString.indexOf("\"", indexOfVersionName);
                int nameEnd = nameStart + 1;
                while(jsonString.charAt(nameEnd) != '\"') {
                    ++nameEnd;
                }
                version.versionName = jsonString.substring(nameStart + 1, nameEnd).trim();
                //find versionCode
                int indexOfVersionCode = jsonString.indexOf(VERSION_CODE);
                if (indexOfVersionCode <= 0) {
                    LOG.error("ModuleJsonUtil::getVersion obtain index of VERSION_CODE failed");
                    return version;
                }
                indexOfVersionCode += VERSION_CODE.length();
                while(!Character.isDigit(jsonString.charAt(indexOfVersionCode))) {
                    ++indexOfVersionCode;
                }
                String versionCode = "";
                while(Character.isDigit(jsonString.charAt(indexOfVersionCode))) {
                    versionCode += jsonString.charAt(indexOfVersionCode);
                    ++indexOfVersionCode;
                }
                version.versionCode = Integer.parseInt(versionCode.trim());
            }
         } catch (NumberFormatException exception) {
             LOG.error("ModuleJsonUtil::getVersion NumberFormatException exception: " + exception.getMessage());
             throw new BundleException("ModuleJsonUtil::getVersion failed");
         }
         return version;
     }
}
