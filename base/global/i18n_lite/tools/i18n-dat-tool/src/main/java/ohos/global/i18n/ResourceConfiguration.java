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

import org.codehaus.jackson.map.ObjectMapper;
import org.codehaus.jackson.JsonNode;
import org.codehaus.jackson.annotate.JsonIgnoreProperties;

import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Iterator;
import java.util.ArrayList;

/**
 * Resource configuration defined in the reource_items.json
 */
public class ResourceConfiguration {
    private ResourceConfiguration() {}

    /**
     * method used to parse resource_items.json
     *
     * @return the resource configuration extracted from resource_items.json
     */
    public static ArrayList<ResourceConfiguration.ConfigItem> parse() {
        ArrayList<ResourceConfiguration.ConfigItem> ret = new ArrayList<>();
        try {
            File json = new File(ResourceConfiguration.class.getResource("/resource/resource_items.json").toURI());
            ObjectMapper mapper = new ObjectMapper();
            JsonNode jsonNode = mapper.readTree(json);
            Iterator<JsonNode> jsonNodes = jsonNode.getElements();
            int count = 0;
            while (jsonNodes.hasNext()) {
                JsonNode node = jsonNodes.next();
                ConfigItem item = mapper.treeToValue(node, ConfigItem.class);
                if (count != item.index) {
                    throw new IllegalStateException("not consecutive index for index " + count);
                }
                ++count;
                ret.add(item);
            }
        } catch (IOException | URISyntaxException e) {
            e.printStackTrace();
        }
        return ret;
    }

    /**
     * Configuration item defined in resource_items.json
     */
    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class ConfigItem {
        int index;
        String method;
        int lengths;
        int offsets;
        String pub;
        String sep;
        String pointer;
        String type;
        Element[] elements;

        /**
         * get index
         *
         * @return index
         */
        public int getIndex() {
            return index;
        }

        /**
         * get method
         *
         * @return method
         */
        public String getMethod() {
            return method;
        }

        /**
         * get lengths
         *
         * @return length
         */
        public int getLengths() {
            return lengths;
        }

        /**
         * get offsets
         *
         * @return offsets
         */
        public int getOffsets() {
            return offsets;
        }

        /**
         * get pub
         *
         * @return pub
         */
        public String getPub() {
            return pub;
        }

        /**
         * get sep
         *
         * @return sep
         */
        public String getSep() {
            return sep;
        }

        /**
         * get pointer
         *
         * @return Pointer
         */
        public String getPointer() {
            return pointer;
        }

        /**
         * get type
         *
         * @return type
         */
        public String getType() {
            return type;
        }

        /**
         * get elements
         *
         * @return elements
         */
        public Element[] getElements() {
            return elements;
        }
    }

    /**
     * element pattern
     */
    public static class Element {
        String availableFormat;
        String skeleton;
        int enumIndex;
        int index;

        /**
         * get availableFormat
         *
         * @return availableFormat
         */
        public String getAvailableFormat() {
            return availableFormat;
        }

        /**
         * get skeleton
         *
         * @return return skeleton
         */
        public String getSkeleton() {
            return skeleton;
        }

        /**
         * get enumIndex
         *
         * @return enumIndex
         */
        public int getEnumIndex() {
            return enumIndex;
        }

        /**
         * get index
         *
         * @return index
         */
        public int getIndex() {
            return index;
        }
    }
}
