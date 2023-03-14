/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "parser/lexer.h"
#include "util/string_builder.h"

namespace OHOS {
namespace HDI {
std::unordered_map<String, Token, StringHashFunc, StringEqualFunc> Lexer::keywords_ = {
    {"boolean",        Token::BOOLEAN       },
    {"byte",           Token::BYTE          },
    {"callback",       Token::CALLBACK      },
    {"char",           Token::CHAR          },
    {"double",         Token::DOUBLE        },
    {"enum",           Token::ENUM          },
    {"extends",        Token::EXTENDS       },
    {"float",          Token::FLOAT         },
    {"full",           Token::FULL          },
    {"import",         Token::IMPORT        },
    {"in",             Token::IN            },
    {"int",            Token::INTEGER       },
    {"interface",      Token::INTERFACE     },
    {"List",           Token::LIST          },
    {"lite",           Token::LITE          },
    {"long",           Token::LONG          },
    {"Map",            Token::MAP           },
    {"oneway",         Token::ONEWAY        },
    {"out",            Token::OUT           },
    {"package",        Token::PACKAGE       },
    {"sequenceable",   Token::SEQUENCEABLE  },
    {"short",          Token::SHORT         },
    {"String",         Token::STRING        },
    {"struct",         Token::STRUCT        },
    {"union",          Token::UNION         },
    {"unsigned",       Token::UNSIGNED      },
    {"FileDescriptor", Token::FILEDESCRIPTOR},
    {"SharedMemQueue", Token::SMEMQUEUE     },
};

std::unordered_map<char, Token> Lexer::delimiters_ = {
    {'<', Token::ANGLE_BRACKETS_LEFT },
    {'>', Token::ANGLE_BRACKETS_RIGHT},
    {'{', Token::BRACES_LEFT         },
    {'}', Token::BRACES_RIGHT        },
    {'[', Token::BRACKETS_LEFT       },
    {']', Token::BRACKETS_RIGHT      },
    {',', Token::COMMA               },
    {'(', Token::PARENTHESES_LEFT    },
    {')', Token::PARENTHESES_RIGHT   },
    {'.', Token::DOT                 },
    {':', Token::COLON               },
    {';', Token::SEMICOLON           },
    {'=', Token::ASSIGN              }
};

std::unordered_map<Token, String> Lexer::tokenDumps_ = {
    {Token::ANGLE_BRACKETS_LEFT,  "<"             },
    {Token::ANGLE_BRACKETS_RIGHT, ">"             },
    {Token::BOOLEAN,              "boolean"       },
    {Token::BRACES_LEFT,          "{"             },
    {Token::BRACES_RIGHT,         "}"             },
    {Token::BRACKETS_LEFT,        "["             },
    {Token::BRACKETS_RIGHT,       "]"             },
    {Token::BYTE,                 "byte"          },
    {Token::CALLBACK,             "callback"      },
    {Token::CHAR,                 "char"          },
    {Token::COLON,                ":"             },
    {Token::COMMA,                ","             },
    {Token::DOT,                  "."             },
    {Token::DOUBLE,               "double"        },
    {Token::END_OF_FILE,          "eof"           },
    {Token::ENUM,                 "enum"          },
    {Token::EXTENDS,              "extends"       },
    {Token::FLOAT,                "float"         },
    {Token::FULL,                 "full"          },
    {Token::IMPORT,               "import"        },
    {Token::IN,                   "in"            },
    {Token::INTEGER,              "int"           },
    {Token::INTERFACE,            "interface"     },
    {Token::LITE,                 "lite"          },
    {Token::LIST,                 "List"          },
    {Token::LONG,                 "long"          },
    {Token::MAP,                  "Map"           },
    {Token::ONEWAY,               "oneway"        },
    {Token::OUT,                  "out"           },
    {Token::PACKAGE,              "package"       },
    {Token::SEQUENCEABLE,         "sequenceable"  },
    {Token::STRUCT,               "struct"        },
    {Token::PARENTHESES_LEFT,     "("             },
    {Token::PARENTHESES_RIGHT,    ")"             },
    {Token::SEMICOLON,            ";"             },
    {Token::SHORT,                "short"         },
    {Token::STRING,               "String"        },
    {Token::UNION,                "union"         },
    {Token::UNSIGNED,             "unsigned"      },
    {Token::FILEDESCRIPTOR,       "FileDescriptor"},
    {Token::SMEMQUEUE,            "SharedMemQueue"},
};

Lexer::~Lexer()
{
    if (currentFile_ != nullptr) {
        currentFile_->Close();
    }
}

bool Lexer::OpenSourceFile(const String &filePath)
{
    currentFile_ = std::make_unique<File>(filePath, int(File::READ));
    if (!currentFile_->IsValid()) {
        return false;
    }

    return true;
}

Token Lexer::GetToken(bool skipComment)
{
    if (!havePeek_) {
        currentToken_ = ReadToken(skipComment);
    }
    havePeek_ = false;
    return currentToken_;
}

Token Lexer::PeekToken(bool skipComment)
{
    if (!havePeek_) {
        currentToken_ = ReadToken(skipComment);
        havePeek_ = true;
    }
    return currentToken_;
}

Token Lexer::ReadToken(bool skipComment)
{
    while (!currentFile_->IsEof()) {
        char c = currentFile_->GetChar();
        tokenLineNo_ = currentFile_->GetCharLineNumber();
        tokenColumnNo_ = currentFile_->GetCharColumnNumber();
        if (IsSpace(c)) {
            continue;
        } else if (IsAlphabet(c) || c == '_') {
            Token t = ReadIdentifier(c);
            return t;
        } else if (IsDecimalDigital(c)) {
            Token t = ReadDecimalDigital(c);
            return t;
        }

        auto iter = delimiters_.find(c);
        if (iter != delimiters_.end()) {
            return iter->second;
        }

        if (c == '/') {
            if (currentFile_->PeekChar() == '/') {
                ReadLineComment(c);
            } else if (currentFile_->PeekChar() == '*') {
                ReadBlockComment(c);
            } else {
                currentToken_ = Token::UNKNOWN;
                return currentToken_;
            }

            if (skipComment) {
                continue;
            }

            return currentToken_;
        }

        currentToken_ = Token::UNKNOWN;
        return currentToken_;
    }
    currentToken_ = Token::END_OF_FILE;
    return currentToken_;
}

Token Lexer::ReadIdentifier(char c)
{
    StringBuilder sb;

    sb.Append(c);
    while (!currentFile_->IsEof()) {
        c = currentFile_->PeekChar();
        if (IsAlphabet(c) || c == '_' || IsDecimalDigital(c) || c == '.') {
            c = currentFile_->GetChar();
            sb.Append(c);
            continue;
        }
        if (IsSpace(c)) {
            currentFile_->GetChar();
        }
        break;
    }
    String key = sb.ToString();
    auto it = keywords_.find(key);
    if (it == keywords_.end()) {
        identifier_ = key;
        currentToken_ = Token::IDENTIFIER;
    } else {
        currentToken_ = it->second;
    }

    return currentToken_;
}

Token Lexer::ReadDecimalDigital(char c)
{
    StringBuilder sb;
    sb.Append(c);
    while (!currentFile_->IsEof()) {
        c = currentFile_->PeekChar();
        if (IsDecimalDigital(c)) {
            c = currentFile_->GetChar();
            sb.Append(c);
            continue;
        } else {
            break;
        }
    }
    digit_ = sb.ToString();
    currentToken_ = Token::DIGIT;
    return currentToken_;
}

Token Lexer::ReadLineComment(char c)
{
    StringBuilder sb;

    sb.Append(c);
    while (!currentFile_->IsEof()) {
        c = currentFile_->GetChar();
        if (c == '\n') {
            break;
        }
        sb.Append(c);
    }
    comment_ = sb.ToString();
    currentToken_ = Token::COMMENT_LINE;
    return currentToken_;
}

Token Lexer::ReadBlockComment(char c)
{
    StringBuilder sb;

    sb.Append(c);
    while (!currentFile_->IsEof()) {
        c = currentFile_->GetChar();
        sb.Append(c);
        if (c == '*' && currentFile_->PeekChar() == '/') {
            c = currentFile_->GetChar();
            sb.Append(c);
            break;
        }
    }
    comment_ = sb.ToString();
    currentToken_ = Token::COMMENT_BLOCK;
    return currentToken_;
}

void Lexer::SkipCurrentLine()
{
    while (!currentFile_->IsEof()) {
        char c = currentFile_->GetChar();
        if (c == '\n') {
            currentFile_->GetChar();
            return;
        }
    }
}

bool Lexer::SkipCurrentLine(char untilChar)
{
    while (!currentFile_->IsEof()) {
        int c = currentFile_->GetChar();
        if (c == untilChar) {
            return true;
        }
        if (c == '\n') {
            currentFile_->GetChar();
            return false;
        }
    }
    return true;
}

void Lexer::Skip(char untilChar)
{
    while (!currentFile_->IsEof()) {
        int c = currentFile_->GetChar();
        if (c == untilChar) {
            return;
        }
    }
}

void Lexer::SkipEof()
{
    while (!currentFile_->IsEof()) {}
}

int Lexer::TokenToChar(Token token)
{
    switch (token) {
        case Token::ANGLE_BRACKETS_LEFT:
            return '<';
        case Token::ANGLE_BRACKETS_RIGHT:
            return '>';
        case Token::BRACES_LEFT:
            return '{';
        case Token::BRACES_RIGHT:
            return '}';
        case Token::BRACKETS_LEFT:
            return '[';
        case Token::BRACKETS_RIGHT:
            return ']';
        case Token::COMMA:
            return ',';
        case Token::DOT:
            return '.';
        case Token::PARENTHESES_LEFT:
            return '(';
        case Token::PARENTHESES_RIGHT:
            return ')';
        case Token::COLON:
            return ':';
        case Token::SEMICOLON:
            return ';';
        default:
            return -1;
    }
}

String Lexer::DumpToken() const
{
    auto iter = tokenDumps_.find(currentToken_);
    if (iter != tokenDumps_.end()) {
        return iter->second;
    }

    if (currentToken_ == Token::COMMENT_BLOCK || currentToken_ == Token::COMMENT_LINE) {
        return comment_;
    }

    if (currentToken_ == Token::IDENTIFIER) {
        return identifier_;
    }

    return "unknown token";
}
} // namespace HDI
} // namespace OHOS