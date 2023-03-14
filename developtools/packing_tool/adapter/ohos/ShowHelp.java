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
 * showhelp class of compress tool.
 *
 */
public interface ShowHelp {
    Log log = new Log(ShowHelp.class.toString());
    /**
     * content of compress command help menu.
     */
    static void compressHelp() {
        log.info("\nHAP USAGE:\n" +
                "java -jar hmos_app_packing_tool.jar --mode <options> --json-path <option>\n" +
                "--profile-path [option] --maple-so-path [option] --maple-so-dir [option]\n" +
                "--dex-path [option] --lib-path [option] --resources-path [option] --assets-path [option]\n" +
                "--shell-apk-path [option] --index-path [option] --out-path <option> --force [option]\n" +
                "OPTIONS:\n" +
                "  --mode               not null  must be hap.\n" +
                "  --json-path                    config.json path.\n" +
                "  --profile-path                 CAPABILITY.profile path.\n" +
                "  --maple-so-path                maple-so/ path\n" +
                "  --maple-so-dir                 maple so path must end with .so,\n" +
                "                                 if has more than one so, use comma(,) to separate;\n" +
                "  --dex-path                     1.dex path must end with .dex,\n" +
                "                                 if has more than one dex, use comma(,) to separate;\n" +
                "                                 2.dex path also can be directory path.\n" +
                "  --lib-path                     lib/ path.\n" +
                "  --bin-path                     bin file path.\n" +
                "  --resources-path               resources/ path.\n" +
                "  --assets-path                  assets/ path.\n" +
                "  --shell-apk-path               1.shell apk path must end with .apk,\n" +
                "                                 if has more than one apk, use comma(,) to separate;\n" +
                "                                 2.shell apk path also can be directory path.\n" +
                "  --index-path                   resources.index path\n" +
                "  --out-path           not null  target hap binary path, must end with .hap.\n" +
                "  --force                        default false; if true, force delete destination\n" +
                "                                 out file if exists.\n" +
                "----------------------------------------------------------------------------------\n" +
                "HAR USAGE:\n" +
                "java -jar hmos_app_packing_tool.jar --mode <options> --json-path <option>\n" +
                "--jar-path <option> --lib-path [option] --resources-path [option] --txt-path [option]\n" +
                "--assets-path [option] --out-path <option> --force [option]\n" +
                "OPTIONS:\n" +
                "  --mode               not null  must be har.\n" +
                "  --json-path          not null  config.json path.\n" +
                "  --jar-path           not null  1.jar path must end with .jar,\n" +
                "                                 if has more than one jar, use comma(,) to separate;\n" +
                "                                 2.jar path also can be directory path.\n" +
                "  --lib-path                     lib/ path.\n" +
                "  --resources-path     not null  resources/ path.\n" +
                "  --txt-path                     1.txt path must end with .txt,\n" +
                "                                 if has more than one txt, use comma(,) to separate;\n" +
                "                                 2.txt path also can be directory path.\n" +
                "  --assets-path                  assets/ path.\n" +
                "  --out-path           not null  target har binary path, must end with .har.\n" +
                "  --force                        default false; if true, force delete destination\n" +
                "                                 out file if exists.\n" +
                "----------------------------------------------------------------------------------\n" +
                "APP USAGE:\n" +
                "java -jar hmos_app_packing_tool.jar --mode <options> --hap-path <option>\n" +
                "--pack-info-path [option] --out-path [option] --signature-path [option]\n" +
                "--certificate-path [option]  --force [option]\n" +
                "OPTIONS:\n" +
                "  --mode               not null  must be app.\n" +
                "  --hap-path           not null  1.hap path must end with .hap,\n" +
                "                                 if has more than one hap, use comma(,) to separate;\n" +
                "                                 2.hap path also can be directory path.\n" +
                "  --pack-info-path     not null  pack.info path\n" +
                "  --out-path           not null  target app binary path, must end with .app.\n" +
                "  --force                        default false; if true, force delete destination\n" +
                "                                 out file if exists.\n" +
                "  --signature-path               signature file path.\n" +
                "  --certificate-path             certificate file path.");
    }

    /**
     * content of uncompress command help menu.
     */
    static void uncompressHelp() {
        log.info("\nHAP USAGE:\n" +
                "java -jar hmos_app_unpacking_tool.jar --mode <options> --hap-path <option>\n" +
                "--out-path [option] --force [option]\n" +
                "OPTIONS:\n" +
                "  --mode               not null  must be hap.\n" +
                "  --hap-path           not null  hap path.\n" +
                "  --out-path                     target path of unzip.\n" +
                "  --force                        default false; if true, force delete destination\n" +
                "                                 out file if exists.\n" +
                "  --unpackapk                    default false; if true, unpack apk files from hap\n" +
                "                                 in the app.\n" +
                "----------------------------------------------------------------------------------\n" +
                "HAR USAGE:\n" +
                "java -jar hmos_app_unpacking_tool.jar --mode <options> --har-path <option>\n" +
                "--out-path [option] --force [option]\n" +
                "OPTIONS:\n" +
                "  --mode               not null  must be har.\n" +
                "  --har-path           not null  har path.\n" +
                "  --out-path           not null  target path of unzip.\n" +
                "  --force                        default false; if true, force delete destination\n" +
                "                                 out file if exists.\n" +
                "----------------------------------------------------------------------------------\n" +
                "APP USAGE:\n" +
                "java -jar hmos_app_unpacking_tool.jar --mode <options> --app-path <option>\n" +
                "--out-path [option] --force [option]\n" +
                "OPTIONS:\n" +
                "  --mode               not null  must be app.\n" +
                "  --app-path           not null  app path.\n" +
                "  --out-path                     target path of unzip.\n" +
                "  --force                        default false; if true, force delete destination\n" +
                "                                 out file if exists.\n" +
                "  --unpackapk                    default false; if true, unpack apk files from hap\n" +
                "                                 in the app.");
    }
}
