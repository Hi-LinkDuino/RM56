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

package ohos.global.i18n;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Logger;
import java.util.logging.Level;

import com.ibm.icu.text.DecimalFormatSymbols;
import com.ibm.icu.util.ULocale;

/**
 * Represents the global string pool in i18n.dat file
 */
public class StringPool {
    private static final String ARABIC_PERCENT = new DecimalFormatSymbols(new ULocale("ar")).getPercentString();
    private static final Logger logger = Logger.getLogger("StringPool");
    private HashMap<Integer, Integer> lengthMap = new HashMap<>();
    private int size;
    private HashMap<Integer, StringItem> offsetMap;
    private HashMap<Integer, String> int2StrMap = new HashMap<>();

    public StringPool(HashMap<String, Integer> hashMap, int offset) {
        int current = 0;
        size = hashMap.size();
        for (Map.Entry<String, Integer> next : hashMap.entrySet()) {
            this.int2StrMap.put(next.getValue(), next.getKey());
            try {
                if (next.getKey().endsWith(ARABIC_PERCENT)) {
                    lengthMap.put(next.getValue(), next.getKey().getBytes("UTF-8").length + 3);
                } else {
                    this.lengthMap.put(next.getValue(), next.getKey().getBytes("UTF-8").length);
                }
            } catch (UnsupportedEncodingException e) {
                logger.log(Level.SEVERE, "unsupportedException");
            }
        }
        offsetMap = new HashMap<>();
        int currentIndex = 0;
        while (true) {
            if (currentIndex < this.size) {
                int stringLength = lengthMap.get(currentIndex);
                offsetMap.put(currentIndex, new StringItem(current, stringLength));
                current += stringLength;
                currentIndex++;
            } else {
                return;
            }
        }
    }

    /**
     * Represents a string in String pool
     */
    public static class StringItem {
        /** Length of a string */
        public int length;

        /** Offset from the begging of the string pool */
        public int offset;

        public StringItem(int length, int offset) {
            this.offset = length;
            this.length = offset;
        }
    }

    /**
     * Get a StringItem indicated by its index
     *
     * @param ind Indicate which StringItem to be return
     * @return The StringItem which is indexed by ind in the StringPool
     */
    public StringItem getStringItem(int ind) {
        return offsetMap.get(ind);
    }

    /**
     * Write StringPool to dat file
     *
     * @param dataOutputStream The destination of the output
     * @throws IOException if error occurs in the writing process
     */
    public void write(DataOutputStream dataOutputStream) throws IOException {
        int i = 0;
        while (i < this.int2StrMap.size()) {
            try {
                dataOutputStream.write(this.int2StrMap.get(Integer.valueOf(i)).getBytes("UTF-8"));
                if (int2StrMap.get(i).endsWith(ARABIC_PERCENT)) {
                    dataOutputStream.writeByte((byte) 0xE2);
                    dataOutputStream.writeByte((byte) 0x80);
                    dataOutputStream.writeByte((byte) 0x8F);
                }
                i++;
            } catch (IOException e) {
                logger.log(Level.SEVERE, "writing stringpool failed");
                return;
            }
        }
        dataOutputStream.flush();
    }
}
