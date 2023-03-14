/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_LEXER_H
#define OHOS_HDI_LEXER_H

#include <cstdlib>
#include <ctype.h>
#include <memory>
#include <unordered_map>

#include "parser/token.h"
#include "util/file.h"
#include "util/string.h"

namespace OHOS {
namespace HDI {
class Lexer {
public:
    Lexer() = default;

    ~Lexer();

    bool OpenSourceFile(const String &filePath);

    Token GetToken(bool skipComment = true);

    Token PeekToken(bool skipComment = true);

    inline String GetIdentifier() const
    {
        return identifier_;
    }

    inline unsigned long GetDigit() const
    {
        return std::atoll(digit_.string());
    }

    inline String GetComment() const
    {
        return comment_;
    }

    void SkipCurrentLine();

    bool SkipCurrentLine(char untilChar);

    void Skip(char untilChar);

    void SkipEof();

    inline String GetFilePath() const
    {
        if (currentFile_ == nullptr) {
            return String("");
        }

        return currentFile_->GetPath();
    }

    String DumpToken() const;

    inline int GetTokenLineNumber() const
    {
        return tokenLineNo_;
    }

    inline int GetTokenColumnNumber() const
    {
        return tokenColumnNo_;
    }

    static int TokenToChar(Token token);

private:
    Token ReadToken(bool skipComment);

    Token ReadIdentifier(char c);

    Token ReadDecimalDigital(char c);

    Token ReadLineComment(char c);

    Token ReadBlockComment(char c);

    inline static bool IsAlphabet(char c)
    {
        return isalpha(c);
    }

    inline static bool IsDecimalDigital(char c)
    {
        return isdigit(c);
    }

    inline static bool IsSpace(char c)
    {
        return isspace(c);
    }

    static const char *TAG;
    static std::unordered_map<String, Token, StringHashFunc, StringEqualFunc> keywords_;
    static std::unordered_map<char, Token> delimiters_;
    static std::unordered_map<Token, String> tokenDumps_;
    Token currentToken_ = Token::UNKNOWN;
    int tokenLineNo_ = 0;
    int tokenColumnNo_ = 0;
    String identifier_;
    String digit_;
    String comment_;
    bool havePeek_ = false;
    std::unique_ptr<File> currentFile_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_LEXER_H