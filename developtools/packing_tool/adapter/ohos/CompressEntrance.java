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

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * bundle tool main class.
 * step1: parse arguments
 * step2: verity arguments
 * step3: compress arguments
 *
 */
public class CompressEntrance {
    private static final int EXIT_STATUS_NORMAL = 0;
    private static final int EXIT_STATUS_EXCEPTION = 1;
    private static final Log LOG = new Log(CompressEntrance.class.toString());

    /**
     * Pack the app.
     *
     * @param hapPath Indicates the hap path.
     * @param packInfoPath Indicates the pack.info file path.
     * @param outPath Indicates the out path.
     * @return Returns {@code true} if the pack is successful; returns {@code false} otherwise.
     */
    public static boolean pack(String hapPath, String packInfoPath, String outPath) {
        if (hapPath == null || hapPath.isEmpty()) {
            LOG.error("CompressEntrance::pack hapPath is invalid!");
            return false;
        }

        if (packInfoPath == null || packInfoPath.isEmpty()) {
            LOG.error("CompressEntrance::pack packInfoPath is invalid!");
            return false;
        }

        if (outPath == null || outPath.isEmpty()) {
            LOG.error("CompressEntrance::pack outPath is invalid!");
            return false;
        }

        Utility utility = new Utility();
        utility.setMode(Utility.MODE_APP);
        utility.setHapPath(hapPath);
        utility.setPackInfoPath(packInfoPath);
        utility.setOutPath(outPath);
        utility.setForceRewrite("true");

        if (!CompressVerify.commandVerify(utility)) {
            LOG.error("CompressEntrance::startCompress verity failed");
            return false;
        }

        Compressor compressor = new Compressor();
        if (!compressor.compressProcess(utility)) {
            LOG.error("CompressEntrance::startCompress compress failed");
            return false;
        }

        return true;
    }

    /**
     * compress tool main function.
     *
     * @param args command line
     */
    public static void main(String[] args) {
        Utility utility = new Utility();

        if (!CommandParser.commandParser(utility, args)) {
            LOG.error("CompressEntrance::main exit, parser failed");
            ShowHelp.compressHelp();
            System.exit(EXIT_STATUS_EXCEPTION);
        }

        if (!CompressVerify.commandVerify(utility)) {
            LOG.error("CompressEntrance::main exit, verity failed");
            ShowHelp.compressHelp();
            System.exit(EXIT_STATUS_EXCEPTION);
        }

        Compressor compressor = new Compressor();
        if (!compressor.compressProcess(utility)) {
            LOG.error("CompressEntrance::main exit, compress failed");
            ShowHelp.compressHelp();
            System.exit(EXIT_STATUS_EXCEPTION);
        }

        System.exit(EXIT_STATUS_NORMAL);
    }
}