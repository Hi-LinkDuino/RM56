/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_TOKEN_H
#define OHOS_HDI_TOKEN_H

namespace OHOS {
namespace HDI {
enum class Token {
    UNKNOWN = 0,
    // types
    VOID,
    BOOLEAN,
    BYTE,
    SHORT,
    INTEGER,
    LONG,
    STRING,
    FLOAT,
    DOUBLE,
    FILEDESCRIPTOR,
    LIST,
    MAP,
    SMEMQUEUE,
    CHAR,
    // qualifier
    UNSIGNED,
    // custom types
    STRUCT,
    ENUM,
    UNION,
    // keywords
    PACKAGE,
    SEQUENCEABLE,
    IMPORT,
    INTERFACE,
    EXTENDS,
    ONEWAY,
    CALLBACK,
    FULL,
    LITE,
    IN,
    OUT,
    DOT,                  // '.'
    COMMA,                // ','
    COLON,                // ':'
    ASSIGN,               // '='
    SEMICOLON,            // ';'
    BRACES_LEFT,          // '{'
    BRACES_RIGHT,         // '}'
    BRACKETS_LEFT,        // '['
    BRACKETS_RIGHT,       // ']'
    PARENTHESES_LEFT,     // '('
    PARENTHESES_RIGHT,    // ')'
    ANGLE_BRACKETS_LEFT,  // '<'
    ANGLE_BRACKETS_RIGHT, // '>'
    // others
    IDENTIFIER,
    DIGIT,
    COMMENT_BLOCK,
    COMMENT_LINE,
    END_OF_FILE,
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_TOKEN_H