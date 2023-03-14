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
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 * Represents a locale in dat file
 */
public class LocaleList {
    private ArrayList<LocaleMaskItem> list = new ArrayList<>(70);
    private StringPool pool;
    private int size;
    private int startOffset;

    public LocaleList(int startOffset, HashMap<String, Integer> locales, Map<Integer,
        ArrayList<LocaleConfig>> localeConfigs, StringPool stringPool) {
        this.startOffset = startOffset;
        this.size = locales.size();
        this.pool = stringPool;
        for (Map.Entry<String, Integer> next : locales.entrySet()) {
            this.list.add(new LocaleMaskItem(next.getKey(), localeConfigs.get(next.getValue())));
        }
        list.sort(null);
        for (int i = 0; i < list.size(); ++i) {
            LocaleMaskItem item = list.get(i);
            item.setRank(i);
            item.setOffset(startOffset + i * FileConfig.LOCALE_MASK_ITEM_SIZE);
        }
    }

    /**
     * Methods to write a localeList object to dat file
     *
     * @param dataOutputStream The destination of the output
     * @throws IOException throws IOException if an error occurs
     */
    public void write(DataOutputStream dataOutputStream) throws IOException {
        int acc = 0;
        for (int i = 0; i < list.size(); i++) {
            LocaleMaskItem localeMaskItem = list.get(i);
            // write Locale part
            dataOutputStream.writeInt((int) localeMaskItem.getMask());
            dataOutputStream.writeChar(startOffset + (this.size * FileConfig.LOCALE_MASK_ITEM_SIZE) +
                (acc * FileConfig.CONFIG_SIZE));
            dataOutputStream.writeChar(localeMaskItem.getConfigs().size());
            acc += localeMaskItem.getConfigs().size();
            dataOutputStream.flush();
        }
        // Write configs part
        for (int i = 0; i < this.list.size(); i++) {
            LocaleMaskItem localeMaskItem2 = this.list.get(i);
            ArrayList<LocaleConfig> configs = localeMaskItem2.getConfigs();
            // All configs are sorted already
            for (int j = 0; j < configs.size(); ++j) {
                LocaleConfig localeConfig = configs.get(j);
                int nameId = localeConfig.nameId;
                int stringId = localeConfig.stringId;
                dataOutputStream.writeChar(nameId);
                StringPool.StringItem stringItem = pool.getStringItem(stringId);
                dataOutputStream.writeChar(stringItem.offset);
                dataOutputStream.writeChar(stringItem.length);
            }
            dataOutputStream.flush();
        }
        pool.write(dataOutputStream);
        dataOutputStream.flush();
    }
}
