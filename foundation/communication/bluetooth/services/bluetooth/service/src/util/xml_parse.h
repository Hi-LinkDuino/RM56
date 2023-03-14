/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef XML_PARSE_H
#define XML_PARSE_H

#include <map>
#include <string>
#include <vector>
#include "base_def.h"

namespace utility {
#define SIZEOF_0X 2
#define BASE_16 16
class XmlParse {
public:
    /**
     * @brief Construct a new Xml Parse object
     *
     * @since 6
     */
    XmlParse();

    /**
     * @brief Destroy the Xml Parse object
     *
     * @since 6
     */
    ~XmlParse();

    /**
     * @brief Load XML document form file.
     *
     * @param path File path.
     * @return Success load xml document return true, else return false.
     * @since 6
     */
    bool Load(const std::string &path);

    /**
     * @brief Parse XML document.
     *
     * @return Success parse XML document return true, else return false.
     * @since 6
     */
    bool Parse();

    /**
     * @brief Store back to XML document.
     *
     * @return Success store back to XML document return true, else return false.
     * @since 6
     */
    bool Save();

    /**
     * @brief Get specified property value.
     *        Value type is int.
     *
     * @param section Xml section.
     * @param subSection Xml subSection.
     * @param property Xml property.
     * @param value Value type is int.
     * @return Success get specified property's value return true, else return false.
     * @since 6
     */
    bool GetValue(const std::string &section, const std::string &subSection, const std::string &property, int &value);

    /**
     * @brief Get specified property value.
     *        Value type is string.
     *
     * @param section Xml section.
     * @param subSection Xml subSection.
     * @param property Xml property.
     * @param value Value type is string.
     * @return Success get specified property's value return true, else return false.
     * @since 6
     */
    bool GetValue(
        const std::string &section, const std::string &subSection, const std::string &property, std::string &value);

    /**
     * @brief Get specified property value.
     *        Value type is bool.
     *
     * @param section Xml section.
     * @param subSection Xml subSection.
     * @param property Xml property.
     * @param value Value type is bool.
     * @return Success get specified property's value return true, else return false.
     * @since 6
     */
    bool GetValue(const std::string &section, const std::string &subSection, const std::string &property, bool &value);

    /**
     * @brief Set specified property value.
     *        Value type is int.
     *
     * @param section Xml section.
     * @param subSection Xml subSection.
     * @param property Xml property.
     * @param value Value type is const int.
     * @return Success set specified property's value return true, else return false.
     * @since 6
     */
    bool SetValue(
        const std::string &section, const std::string &subSection, const std::string &property, const int &value);

    /**
     * @brief Set specified property value. Value type is string.
     *
     * @param section Xml section.
     * @param subSection Xml subSection.
     * @param property Xml property.
     * @param value Value type is const string.
     * @return Success set specified property's value return true, else return false.
     * @since 6
     */
    bool SetValue(const std::string &section, const std::string &subSection, const std::string &property,
        const std::string &value);

    /**
     * @brief Set specified property value. Value type is bool.
     *
     * @param section Xml section.
     * @param subSection Xml subSection.
     * @param property Xml property.
     * @param value Value type is const bool.
     * @return Success set specified property's value return true, else return false.
     * @since 6
     */
    bool SetValue(
        const std::string &section, const std::string &subSection, const std::string &property, const bool &value);

    /**
     * @brief Whether XML document has specified section.
     *
     * @param section Xml section.
     * @param subSection Xml subSection.
     * @return XML document has specified section return true, else return false.
     * @since 6
     */
    bool HasSection(const std::string &section, const std::string &subSection);

    /**
     * @brief Get Address
     * @param section Xml section.
     * @param subSections
     * @return Specified section has one or Mutiple subSections return true, else return false.
     * @since 6
     */
    bool GetSubSections(const std::string &section, std::vector<std::string> &subSections);

    /**
     * @brief Whether XML document has specified property.
     *
     * @param section Xml section.
     * @param subSection
     * @param property
     * @return XML document has specified property return true, else return false.
     * @since 6
     */
    bool HasProperty(const std::string &section, const std::string &subSection, const std::string &property);

    /**
     * @brief Remove XML document specified section.
     *
     * @param section Xml section.
     * @param subSection Xml subSection.
     * @return Success remove XML document specified section return true, else return false.
     * @since 6
     */
    bool RemoveSection(const std::string &section, const std::string &subSection);

    /**
     * @brief Remove XML document specified property.
     *
     * @param section Xml section.
     * @param subSection Xml subSection.
     * @param property Xml property.
     * @return Success remove XML document specified property return true, else return false.
     * @since 6
     */
    bool RemoveProperty(const std::string &section, const std::string &subSection, const std::string &property);

    /**
     * @brief Get specified property value.
     *        Value type is int.
     *
     * @param section Xml section.
     * @param property Xml property.
     * @param value Value type is int.
     * @return Success get specified property's value return true, else return false.
     * @since 6
     */
    bool GetValue(const std::string &section, const std::string &property, int &value);

    /**
     * @brief Get specified property value.
     *        Value type is string.
     * @param section Xml section.
     * @param property Xml property.
     * @param value Value type is string.
     * @return Success get specified property's value return true, else return false.
     * @since 6
     */
    bool GetValue(const std::string &section, const std::string &property, std::string &value);

    /**
     * @brief Get specified property value.
     *        Value type is bool.
     * @param section Xml section.
     * @param property Xml property.
     * @param value Value type is bool.
     * @return Success get specified property's value return true, else return false.
     * @since 6
     */
    bool GetValue(const std::string &section, const std::string &property, bool &value);

    /**
     * @brief Set specified property value.
     *        Value type is int.
     *
     * @param section Xml section.
     * @param property Xml property.
     * @param value Value type is const int.
     * @return Success set specified property's value return true, else return false.
     * @since 6
     */
    bool SetValue(const std::string &section, const std::string &property, const int &value);

    /**
     * @brief Set specified property value.
     *        Value type is string.
     *
     * @param section Xml section.
     * @param property Xml property.
     * @param value Value type is const string.
     * @return Success set specified property's value return true, else return false.
     * @since 6
     */
    bool SetValue(const std::string &section, const std::string &property, const std::string &value);

    /**
     * @brief Set specified property value.
     *        Value type is bool.
     * @param section Xml section.
     * @param property Xml property.
     * @param value Value type is const bool.
     * @return Success set specified property's value return true, else return false.
     * @since 6
     */
    bool SetValue(const std::string &section, const std::string &property, const bool &value);

    /**
     * @brief Whether XML document has specified section.
     *
     * @param section Xml section.
     * @return XML document has specified section return true, else return false.
     * @since 6
     */
    bool HasSection(const std::string &section);

    /**
     * @brief Whether XML document has specified property.
     *
     * @param section Xml section.
     * @param property
     * @return XML document has specified property return true, else return false
     * @since 6
     */
    bool HasProperty(const std::string &section, const std::string &property);

    /**
     * @brief Remove XML document specified section.
     * @param section Xml section.
     * @return Success remove XML document specified section return true, else return false.
     * @since 6
     */
    bool RemoveSection(const std::string &section);

    /**
     * @brief Remove XML document specified property.
     * @param section Xml section.
     * @param property Xml property.
     * @return Success remove XML document specified property return true, else return false.
     * @since 6
     */
    bool RemoveProperty(const std::string &section, const std::string &property);

private:
    std::string filePath_ {""};

    DECLARE_IMPL();
};
};  // namespace utility

#endif  // XML_PARSE_H