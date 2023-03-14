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

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

/**
 * Resources Parser.
 *
 */
public class ResourcesParser {
    /**
     * Parses resources default id.
     */
    public static final int RESOURCE_DEFAULT_ID = -1;

    private static final int VERSION_BYTE_LENGTH = 128;
    private static final int TAG_BYTE_LENGTH = 4;
    private static final Log LOG = new Log(ResourcesParser.class.toString());

    /**
     * Key Param.
     */
    static class KeyParam {
        int keyType;
        int value;
    }

    /**
     * Config Index.
     */
    static class ConfigIndex {
        String tag;
        int offset;
        int keyCount;
        KeyParam[] params;
    }

    /**
     * Data Item.
     */
    static class DataItem {
        int size;
        int type;
        int id;
        String value;
        String name;
    }

    /**
     * Get resource value by resource id.
     *
     * @param resourceId resource id
     * @param data resource index data
     * @return the resourceId value
     * @throws BundleException IOException.
     */
    static String getResourceById(int resourceId, byte[] data) throws BundleException {
        String resourceIdValue = "";
        if (data == null || data.length <= 0 || resourceId == RESOURCE_DEFAULT_ID) {
            LOG.error("ResourcesParser::getIconPath data byte or ResourceId is null");
            return resourceIdValue;
        }

        List<String> result = getResource(resourceId, data);
        if (result != null && result.size() > 0 && result.get(0) != null && !"".equals(result.get(0))) {
            resourceIdValue = result.get(0).substring(0, result.get(0).length() - 1);
        }
        return resourceIdValue;
    }

    /**
     * Get base resource value by resource id.
     *
     * @param resourceId resource id
     * @param data resource index data
     * @return the resource value
     * @throws BundleException IOException.
     */
    static String getBaseResourceById(int resourceId, byte[] data) throws BundleException {
        String resourceIdValue = "";
        if (data == null || data.length <= 0 || resourceId == RESOURCE_DEFAULT_ID) {
            LOG.error("ResourcesParser::getBaseResourceById data byte or ResourceId is null");
            return resourceIdValue;
        }
        resourceIdValue = getBaseResource(resourceId, data);
        if (resourceIdValue != null && !"".equals(resourceIdValue)) {
            resourceIdValue = resourceIdValue.substring(0, resourceIdValue.length() - 1);
        }
        return resourceIdValue;
    }

    /**
     * Get base resource.
     *
     * @param resId resource id
     * @param data resource index data array
     * @return the resource value
     * @throws BundleException IOException.
     */
    static String getBaseResource(int resId, byte[] data) {
        ByteBuffer byteBuf = ByteBuffer.wrap(data);
        byteBuf.order(ByteOrder.LITTLE_ENDIAN);
        byte[] version = new byte[VERSION_BYTE_LENGTH];
        byteBuf.get(version);
        byteBuf.getInt();
        int configCount = byteBuf.getInt();
        Optional<ConfigIndex> optionalConfigIndex = loadBaseConfig(byteBuf, configCount);
        if (!optionalConfigIndex.isPresent()) {
            LOG.error("ResourcesParser::getBaseResource configIndex is null");
            return "";
        }
        return readBaseItem(resId, optionalConfigIndex.get(), byteBuf);
    }

    /**
     * Load index config.
     *
     * @param bufBuf config byte buffer
     * @param count config count
     * @return the base config index
     * @throws BundleException IOException.
     */
    static  Optional<ConfigIndex> loadBaseConfig(ByteBuffer bufBuf, int count) {
        for (int i = 0; i < count; i++) {
            ConfigIndex cfg = new ConfigIndex();
            byte[] tag = new byte[TAG_BYTE_LENGTH];
            bufBuf.get(tag);
            cfg.tag = new String(tag, StandardCharsets.UTF_8);
            cfg.offset = bufBuf.getInt();
            cfg.keyCount = bufBuf.getInt();
            cfg.params = new KeyParam[cfg.keyCount];
            for (int j = 0; j < cfg.keyCount; j++) {
                cfg.params[j] = new KeyParam();
                cfg.params[j].keyType = bufBuf.getInt();
                cfg.params[j].value = bufBuf.getInt();
            }
            if (cfg.keyCount == 0) {
                return Optional.of(cfg);
            }
        }
        return Optional.empty();
    }

    /**
     * Read base config item.
     *
     * @param resId resource id
     * @param configIndex the config index
     * @param buf config byte buffer
     * @return the base item
     * @throws BundleException IOException.
     */
    static String readBaseItem(int resId, ConfigIndex configIndex, ByteBuffer buf) {
        buf.rewind();
        buf.position(configIndex.offset);
        byte[] tag = new byte[TAG_BYTE_LENGTH];
        buf.get(tag);
        int count = buf.getInt();
        for (int i = 0; i < count; i++) {
            int id = buf.getInt();
            int offset = buf.getInt();
            if (id == resId) {
                buf.rewind();
                buf.position(offset);
                DataItem item = readItem(buf);
                return item.value;
            }
        }
        return "";
    }

    /**
     * Get Icon resource.
     *
     * @param resId resource id
     * @param data resource index data array
     * @return the result
     * @throws BundleException IOException.
     */
    static List<String> getResource(int resId, byte[] data) {
        ByteBuffer byteBuf = ByteBuffer.wrap(data);
        byteBuf.order(ByteOrder.LITTLE_ENDIAN);
        byte[] version = new byte[VERSION_BYTE_LENGTH];
        byteBuf.get(version);
        byteBuf.getInt();
        int configCount = byteBuf.getInt();
        List<ConfigIndex> cfg = loadConfig(byteBuf, configCount);
        return readAllItem(resId, cfg, byteBuf);
    }

    /**
     * Load index config.
     *
     * @param bufBuf config byte buffer
     * @param count config count
     * @return the config list
     * @throws BundleException IOException.
     */
    static List<ConfigIndex> loadConfig(ByteBuffer bufBuf, int count) {
        List<ConfigIndex> configList = new ArrayList<>(count);
        for (int i = 0; i < count; i++) {
            ConfigIndex cfg = new ConfigIndex();
            byte[] tag = new byte[TAG_BYTE_LENGTH];
            bufBuf.get(tag);
            cfg.tag = new String(tag, StandardCharsets.UTF_8);
            cfg.offset = bufBuf.getInt();
            cfg.keyCount = bufBuf.getInt();
            cfg.params = new KeyParam[cfg.keyCount];
            for (int j = 0; j < cfg.keyCount; j++) {
                cfg.params[j] = new KeyParam();
                cfg.params[j].keyType = bufBuf.getInt();
                cfg.params[j].value = bufBuf.getInt();
            }

            configList.add(cfg);
        }
        return configList;
    }

    /**
     * Read all config item.
     *
     * @param resId resource id
     * @param configs the config list
     * @param buf config byte buffer
     * @return the item list
     * @throws BundleException IOException.
     */
    static List<String> readAllItem(int resId, List<ConfigIndex> configs, ByteBuffer buf) {
        List<String> result = new ArrayList<>();
        for (ConfigIndex index : configs) {
            buf.rewind();
            buf.position(index.offset);
            byte[] tag = new byte[TAG_BYTE_LENGTH];
            buf.get(tag);
            int count = buf.getInt();
            for (int i = 0; i < count; i++) {
                int id = buf.getInt();
                int offset = buf.getInt();
                if (id == resId) {
                    buf.rewind();
                    buf.position(offset);
                    DataItem item = readItem(buf);
                    result.add(item.value);
                    break;
                }
            }
        }
        return result;
    }

    /**
     * Read the config item.
     *
     * @param buf config byte buffer
     * @return the item info
     * @throws BundleException IOException.
     */
    static DataItem readItem(ByteBuffer buf) {
        DataItem item = new DataItem();
        item.size = buf.getInt();
        item.type = buf.getInt();
        item.id = buf.getInt();
        int len = buf.getShort();
        byte[] value = new byte[len];
        buf.get(value);
        item.value = new String(value, StandardCharsets.UTF_8);
        len = buf.getShort();
        byte[] name = new byte[len];
        buf.get(name);
        item.name = new String(name, StandardCharsets.UTF_8);
        return item;
    }
}

