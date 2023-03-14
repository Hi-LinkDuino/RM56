/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "lexer.h"
#include <sstream>
#include <string>
#include "logger.h"

using namespace OHOS::Hardware;

static constexpr int BINARY_NUM = 2;
static constexpr int OCTAL_NUM = 8;
static constexpr int DECIMAL_NUM = 10;
static constexpr int HEX_NUM = 16;

Lexer::Lexer() : lineno_(0), lineLoc_(0) {}

std::map<std::string, TokenType> Lexer::keyWords_ = {
    {"#include", INCLUDE },
    {"root",     ROOT    },
    {"delete",   DELETE  },
    {"template", TEMPLATE},
};

bool Lexer::Initialize(const std::string &sourceName)
{
    srcName_ = std::make_shared<std::string>(sourceName);

    if (src_.is_open()) {
        src_.close();
    }
    bufferStart_ = nullptr;
    bufferEnd_ = nullptr;
    lineno_ = 1;
    lineLoc_ = 1;
    src_.open(srcName_->c_str(), std::ifstream::binary);
    if (!src_.is_open()) {
        Logger().Error() << "Failed to open source file: " << srcName_->data();
        return false;
    }
    return true;
}

bool Lexer::SetTokenCharacter(char c, Token &token)
{
    switch (c) {
        case ';': /* fall-through */
        case ',': /* fall-through */
        case '[': /* fall-through */
        case ']': /* fall-through */
        case '{': /* fall-through */
        case '}': /* fall-through */
        case '=': /* fall-through */
        case '&': /* fall-through */
        case ':':
            ConsumeChar();
            token.type = c;
            token.lineNo = lineno_;
            break;
        case '"':
            return LexFromString(token);
        case '+': /* fall-through */
        case '-':
            return LexFromNumber(token);
        case EOF:
            token.type = EOF;
            break;
        default:
            Logger().Error() << *this << "can not recognized character '" << c << "'";
            return false;
    }
    return true;
}

bool Lexer::Lex(Token &token)
{
    char c;
    InitToken(token);
    do {
        if (!PeekChar(c, true)) {
            token.type = EOF;
            return true;
        }
        if (c == '#') {
            return LexInclude(token);
        }
        if (isalpha(c)) {
            LexFromLiteral(token);
            return true;
        }

        if (IsNum(c)) {
            return LexFromNumber(token);
        }

        if (c == '/') {
            if (!ProcessComment()) {
                return false;
            }
            continue;
        }

        return SetTokenCharacter(c, token);
    } while (true);

    return true;
}

char Lexer::GetRawChar()
{
    if (!FillBuffer()) {
        return EOF;
    }
    lineLoc_++;
    return *bufferStart_++;
}

bool Lexer::GetChar(char &c, bool skipSpace)
{
    char chr = GetRawChar();
    if (skipSpace) {
        while (IsSpace(chr)) {
            chr = GetRawChar();
        }
    }

    if (chr == '\n') {
        lineno_++;
        lineLoc_ = 0;
    }
    c = chr;
    return chr != EOF;
}

bool Lexer::PeekChar(char &c, bool skipSpace)
{
    if (!FillBuffer()) {
        return false;
    }

    if (skipSpace) {
        while (bufferStart_ <= bufferEnd_ && (IsSpace(*bufferStart_) || *bufferStart_ == '\n')) {
            lineLoc_++;
            if (*bufferStart_ == '\n') {
                lineLoc_ = 0;
                lineno_++;
            }
            bufferStart_++;
        }
    }

    if (bufferStart_ > bufferEnd_) {
        return false;
    }
    c = *bufferStart_;
    return true;
}

bool Lexer::IsSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\r';
}

bool Lexer::FillBuffer()
{
    if (bufferStart_ != nullptr && bufferStart_ <= bufferEnd_) {
        return true;
    }
    auto size = src_.readsome(buffer_, BUFFER_SIZE);
    if (size == 0) {
        return false;
    }
    bufferStart_ = buffer_;
    bufferEnd_ = bufferStart_ + size - 1;
    return true;
}

bool Lexer::ProcessComment()
{
    char c = 0;
    ConsumeChar(); // skip first '/'
    if (!GetChar(c)) {
        Logger().Error() << *this << "unterminated comment";
        return false;
    }

    if (c == '/') {
        while (c != '\n' && GetChar(c)) {}
        if (c != '\n' && c != EOF) {
            Logger().Error() << *this << "unterminated signal line comment";
            return false;
        }
    } else if (c == '*') {
        while (GetChar(c)) {
            if (c == '*' && GetChar(c) && c == '/') {
                return true;
            }
        }
        if (c != '/') {
            Logger().Error() << *this << "unterminated multi-line comment";
            return false;
        }
    } else {
        Logger().Error() << *this << "invalid character";
        return false;
    }

    return true;
}

std::shared_ptr<std::string> Lexer::GetSourceName() const
{
    return srcName_;
}

int32_t Lexer::GetLineno() const
{
    return lineno_;
}

int32_t Lexer::GetLineLoc() const
{
    return lineLoc_;
}

std::ostream &OHOS::Hardware::operator<<(std::ostream &stream, const Lexer &p)
{
    return stream << p.GetSourceName()->data() << ":" << p.GetLineno() << ":" << p.GetLineLoc() << ": ";
}

void Lexer::InitToken(Token &token)
{
    token.type = 0;
    token.numval = 0;
    token.strval.clear();
    token.src = srcName_;
    token.lineNo = lineno_;
}

bool Lexer::LexFromString(Token &token)
{
    char c;
    GetChar(c, false); // skip first '"'
    std::string value;
    while (GetChar(c, false) && c != '"') {
        value.push_back(c);
    }

    if (c != '"') {
        Logger().Error() << *this << "unterminated string";
        return false;
    }
    token.type = STRING;
    token.strval = std::move(value);
    token.lineNo = lineno_;
    return true;
}

void Lexer::LexHexAndBinaryNum(std::string &value, char &c, uint64_t &v)
{
    switch (c) {
        case 'x': // fall-through
        case 'X': // hex number
            ConsumeChar();
            while (PeekChar(c, false) && (IsNum(c) || (c >= 'a' && c <= 'f')
                                            || (c >= 'A' && c <= 'F'))) {
                value.push_back(c);
                ConsumeChar();
            }
            v = strtoll(value.data(), nullptr, HEX_NUM);
            break;
        case 'b': // binary number
            ConsumeChar();
            while (PeekChar(c, false) && (c == '0' || c == '1')) {
                value.push_back(c);
                ConsumeChar();
            }
            v = strtoll(value.data(), nullptr, BINARY_NUM);
            break;
        default:; // fall-through
    }
}

bool Lexer::LexFromNumber(Token &token)
{
    std::string value;
    char c = 0;
    uint64_t v = 0;
    errno = 0;

    GetChar(c, false);
    switch (c) {
        case '0':
            if (!PeekChar(c, true)) {
                break;
            }

            if (IsNum(c)) { // Octal number
                while (PeekChar(c) && IsNum(c)) {
                    ConsumeChar();
                    value.push_back(c);
                }
                v = static_cast<uint64_t>(strtoll(value.data(), nullptr, OCTAL_NUM));
                break;
            }
            LexHexAndBinaryNum(value, c, v);
            break;
        case '+': // fall-through
        case '-': // fall-through, signed decimal number
        default:  // unsigned decimal number
            value.push_back(c);
            while (PeekChar(c, true) && IsNum(c)) {
                ConsumeChar();
                value.push_back(c);
            }
            v = strtoll(value.data(), nullptr, DECIMAL_NUM);
            break;
    }

    if (errno != 0) {
        Logger().Error() << *this << "illegal number: " << value.data();
        return false;
    }
    token.type = NUMBER;
    token.numval = v;
    token.lineNo = lineno_;
    return true;
}

void Lexer::LexFromLiteral(Token &token)
{
    std::string value;
    char c;

    while (PeekChar(c, false) && !IsSpace(c)) {
        if (!isalnum(c) && c != '_' && c != '.' && c != '\\') {
            break;
        }
        value.push_back(c);
        ConsumeChar();
    }

    do {
        if (value == "true") {
            token.type = NUMBER;
            token.numval = 1;
            break;
        } else if (value == "false") {
            token.type = NUMBER;
            token.numval = 0;
            break;
        }
        auto keyword = keyWords_.find(value);
        if (keyword != keyWords_.end()) {
            token.type = keyword->second;
            break;
        }

        if (value.find('.') != std::string::npos) {
            token.type = REF_PATH;
        } else {
            token.type = LITERAL;
        }
    } while (false);

    token.strval = std::move(value);
    token.lineNo = lineno_;
}

void Lexer::ConsumeChar()
{
    char c;
    (void)GetChar(c, false);
}

bool Lexer::IsNum(char c)
{
    return c >= '0' && c <= '9';
}

bool Lexer::LexInclude(Token &token)
{
    ConsumeChar();
    LexFromLiteral(token);
    if (token.strval != "include") {
        return false;
    }

    token.type = INCLUDE;
    return true;
}
