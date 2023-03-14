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
import java.io.File;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileOutputStream;
import java.io.BufferedOutputStream;
import java.net.URISyntaxException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

import com.ibm.icu.util.ULocale;

/**
 * This class is used to generate i18n.dat file
 */
public class DataFetcher {
    private static final ReentrantLock LOCK = new ReentrantLock();
    private static final ArrayList<Fetcher> FETCHERS = new ArrayList<>();
    private static final HashMap<String, Integer> ID_MAP = new HashMap<>(64);
    private static final HashMap<String, Integer> LOCALES = new HashMap<>();
    private static final HashMap<Integer, ArrayList<LocaleConfig>> LOCALE_CONFIGS = new HashMap<>(64);
    private static final Logger LOG = Logger.getLogger("DataFetcher");
    private static int validLocales = 0;
    private static int sStatus = 0;
    private static final Pattern RE_LANGUAGE = Pattern.compile("^([a-z]{2,3})-\\*$");
    private static final int MAX_TIME_TO_WAIT = 10;

    static {
        addFetchers();
    }

    /**
     *
     * Add all required locales from locale.txt and fetch its related data.
     */
    private static void addFetchers() {
        try ( BufferedReader fLocales = new BufferedReader(new
            FileReader(new File(DataFetcher.class.getResource("/resource/locales.txt").toURI())))) {
            String line = "";
            int count = 0;
            ULocale[] availableLocales = ULocale.getAvailableLocales();
            while ((line = fLocales.readLine()) != null) {
                String tag = line.trim();
                if (LOCALES.containsKey(tag)) {
                    continue;
                }
                // special treatment to wildcard
                int tempCount = processWildcard(line, availableLocales, count);
                if (tempCount > count) {
                    count = tempCount;
                    continue;
                }
                if (!Utils.isValidLanguageTag(tag)) {
                    LOG.log(Level.SEVERE, String.format("wrong languageTag %s", tag));
                    sStatus = 1;
                    return;
                }
                FETCHERS.add(new Fetcher(tag, LOCK, ID_MAP));
                LOCALES.put(tag, count);
                ++count;
            }
        } catch (URISyntaxException e) {
            LOG.log(Level.SEVERE, "Add fetchers failed: Url syntax exception");
            sStatus = 1;
        } catch (IOException e) {
            LOG.log(Level.SEVERE, "Add fetchers failed: Io exception");
            sStatus = 1;
        }
    }

    private static int processWildcard(String line, ULocale[] availableLocales, int count) {
        String tag = line.trim();
        int tempCount = count;
        Matcher matcher = RE_LANGUAGE.matcher(line);
        if ("*".equals(line)) { // special treatment to wildcard xx-*
            for (ULocale loc : availableLocales) {
                String finalLanguageTag = loc.toLanguageTag();
                // now we assume en-001 as invalid locale,
                if (!LOCALES.containsKey(finalLanguageTag) && Utils.isValidLanguageTag(finalLanguageTag)) {
                    FETCHERS.add(new Fetcher(finalLanguageTag, LOCK, ID_MAP));
                    LOCALES.put(tag, tempCount);
                    ++tempCount;
                }
            }
            return tempCount;
        }
        if (matcher.matches()) { // special treatment to wildcard language-*
            String baseName = matcher.group(1);
            for (ULocale loc : availableLocales) {
                String finalLanguageTag = loc.toLanguageTag();
                if (loc.getLanguage().equals(baseName) && !LOCALES.containsKey(finalLanguageTag) &&
                    Utils.isValidLanguageTag(finalLanguageTag)) {
                    FETCHERS.add(new Fetcher(finalLanguageTag, LOCK, ID_MAP));
                    LOCALES.put(tag, tempCount);
                    ++tempCount;
                }
            }
        }
        return tempCount;
    }

    private static boolean checkStatus() {
        return sStatus == 0;
    }

    private DataFetcher() {}

    /**
     * If a locale's data equals to its fallback's data, this locale is excluded
     * if a meta data of a locale equals to its fallback's data, this meta data is excluded
     * validLocales keep track of how many locales will be available in dat file.
     * count indicates how many metaData in total will be available in dat file.
     *
     * @return Total number of meta data count
     */
    private static int buildLocaleConfigs() {
        Fetcher fallbackFetcher = null;
        String fallbackData = null;
        int count = 0;
        for (Map.Entry<String, Integer> entry : LOCALES.entrySet()) {
            String languageTag = entry.getKey();
            int index = entry.getValue();
            Fetcher currentFetcher = FETCHERS.get(index);
            ArrayList<LocaleConfig> temp = new ArrayList<>();
            LOCALE_CONFIGS.put(index, temp);
            String fallbackLanguageTag = Utils.getFallback(languageTag);
            // now we need to confirm whether current fetcher's data should be write to i18n.dat
            // if current fetcher's fallback contains equivalent data, then we don't need current fetcher's data.
            if (!LOCALES.containsKey(fallbackLanguageTag) || fallbackLanguageTag.equals(languageTag)) {
                fallbackFetcher = null;
            } else {
                fallbackFetcher = FETCHERS.get(LOCALES.get(fallbackLanguageTag));
            }
            if (currentFetcher.equals(fallbackFetcher)) {
                currentFetcher.included = false;
            } else {
                ++validLocales;
                for (int i = 0; i < Fetcher.getResourceCount(); i++) {
                    String targetMetaData = Fetcher.getInt2Str().get(i);
                    String myData = currentFetcher.datas.get(i);
                    if (fallbackFetcher != null) {
                        fallbackData = fallbackFetcher.datas.get(i);
                    } else {
                        fallbackData = null;
                    }
                    if (!myData.equals(fallbackData)) {
                        temp.add(new LocaleConfig(targetMetaData, i, ID_MAP.get(myData)));
                        ++count;
                    }
                }
            }
        }
        return count;
    }

    /**
     * Main function used to generate i18n.dat file
     *
     * @param args Main function's argument
     */
    public static void main(String args[]) {
        if (!Fetcher.isFetcherStatusOk() || !checkStatus()) {
            return;
        }
        ExecutorService exec = Executors.newCachedThreadPool();
        for (Fetcher fe : FETCHERS) {
            exec.execute(fe);
        }
        exec.shutdown();
        try {
            exec.awaitTermination(MAX_TIME_TO_WAIT, TimeUnit.SECONDS);
        } catch (InterruptedException e) {
            LOG.log(Level.SEVERE, "main class in DataFetcher interrupted");
        }
        int metaDataCount = buildLocaleConfigs(); // every metaData needs 6 bytes
        int localesCount = validLocales; // every locale need 8 bytes
        for (Fetcher fetcher : FETCHERS) {
            if (!fetcher.included) {
                LOCALES.remove(fetcher.languageTag);
            }
        }
        FETCHERS.sort(null);
        try (DataOutputStream out = new DataOutputStream(new BufferedOutputStream(
                new FileOutputStream(new File("./i18n.dat"))))) {
            Utils.writeHeader(out, 0, localesCount, metaDataCount);
            StringPool pool = new StringPool(ID_MAP, FileConfig.HEADER_SIZE + localesCount *
                FileConfig.LOCALE_MASK_ITEM_SIZE + metaDataCount * FileConfig.CONFIG_SIZE);
            LocaleList list = new LocaleList(FileConfig.HEADER_SIZE, LOCALES, LOCALE_CONFIGS, pool);
            list.write(out);
        } catch (IOException e) {
            LOG.log(Level.SEVERE, "exception in writing i18n.dat file");
        }
    }
}