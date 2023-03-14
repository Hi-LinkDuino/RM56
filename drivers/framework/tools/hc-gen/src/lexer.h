/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_LEXER_H
#define HC_GEN_LEXER_H

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "token.h"

namespace OHOS {
namespace Hardware {
class Lexer {
public:
    Lexer();

    ~Lexer() = default;

    bool Initialize(const std::string &sourceName);

    bool Lex(Token &token);

    bool SetTokenCharacter(char c, Token &token);

    friend std::ostream &operator<<(std::ostream &stream, const Lexer &p);

    std::shared_ptr<std::string> GetSourceName() const;

    int32_t GetLineno() const;

    int32_t GetLineLoc() const;

private:
    static constexpr int BUFFER_SIZE = (1024 * 1024);

    void InitToken(Token &token);

    bool GetChar(char &c, bool skipSpace = true);

    void ConsumeChar();

    char GetRawChar();

    static bool IsSpace(char c);

    static bool IsNum(char c);

    bool FillBuffer();

    bool ProcessComment();

    bool LexInclude(Token &token);

    bool LexFromString(Token &token);

    bool LexFromNumber(Token &token);

    void LexHexAndBinaryNum(std::string &value, char &c, uint64_t &v);

    void LexFromLiteral(Token &token);

    bool PeekChar(char &c, bool skipSpace = true);

    static std::map<std::string, TokenType> keyWords_;

    std::ifstream src_;
    std::shared_ptr<std::string> srcName_;
    char buffer_[BUFFER_SIZE] {0};
    const char *bufferStart_ {nullptr};
    const char *bufferEnd_ {nullptr};
    int32_t lineno_;
    int32_t lineLoc_;
};

std::ostream &operator<<(std::ostream &s, const Lexer &lexer);
} // namespace Hardware
} // namespace OHOS

#endif // HC_GEN_LEXER_H
