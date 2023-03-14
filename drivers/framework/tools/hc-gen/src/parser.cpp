/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <memory>

#include "file.h"
#include "logger.h"
#include "parser.h"

using namespace OHOS::Hardware;

bool Parser::Parse()
{
    srcQueue_.push_back(Option::Instance().GetSourceName());
    std::list<std::shared_ptr<Ast>> astList;

    while (!srcQueue_.empty()) {
        std::list<std::string> includeList;
        auto oneAst = ParseOne(srcQueue_.front(), includeList);
        if (oneAst == nullptr) {
            return false;
        }

        astList.push_back(oneAst);
        srcQueue_.pop_front();
        srcQueue_.splice(srcQueue_.begin(), includeList);
    }

    astList.push_back(astList.front());
    astList.pop_front();
    ast_ = astList.front();
    astList.pop_front();

    if (!ast_->Merge(astList)) {
        Logger().Debug() << "failed to merge ast";
        return false;
    }
    if (!astList.empty()) {
        ast_->Dump("merged");
    }

    if (ast_->GetAstRoot() == nullptr) {
        Logger().Error() << Option::Instance().GetSourceName() << ": Empty hcs file";
        return false;
    }

    if (!ast_->Expand()) {
        return false;
    }
    return true;
}

std::shared_ptr<Ast> Parser::ParseOne(const std::string &src, std::list<std::string> &includeList)
{
    if (!lexer_.Initialize(src)) {
        return nullptr;
    }

    if (!lexer_.Lex(current_)) {
        return nullptr;
    }

    if (current_ == INCLUDE && !ProcessInclude(includeList)) {
        return nullptr;
    }

    std::shared_ptr<AstObject> rootNode = nullptr;
    if (current_ == ROOT) {
        auto preToken = current_;
        preToken.type = LITERAL;
        preToken.strval = "root";
        rootNode = ParseNode(preToken);
        if (rootNode == nullptr) {
            return nullptr;
        }
    } else if (current_ != EOF) {
        Logger().Error() << lexer_ << "syntax error, expect root node of end of file";
        return nullptr;
    }

    if (!lexer_.Lex(current_) || current_ != EOF) {
        Logger().Error() << lexer_ << "syntax error, expect EOF";
        return nullptr;
    }

    auto oneAst = std::make_shared<Ast>(rootNode);
    oneAst->Dump(*lexer_.GetSourceName());

    return oneAst;
}

bool Parser::ProcessInclude(std::list<std::string> &includeList)
{
    do {
        if (!lexer_.Lex(current_) || current_ != STRING) {
            Logger().Error() << lexer_ << "syntax error, expect include path after ’#include‘";
            return false;
        }

        auto includePath = current_.strval;
        if (includePath.empty()) {
            Logger().Error() << lexer_ << "include invalid file: \'" << includePath << '\'';
            return false;
        }
        if (includePath[0] != '/') {
            auto currentSrc = srcQueue_.front();
            auto currentSrcDir = Util::File::GetDir(currentSrc);
            includePath = currentSrcDir.append(includePath);
        }

        auto includeAbsPath = Util::File::AbsPath(includePath);
        if (includeAbsPath.empty()) {
            Logger().Error() << lexer_ << "include invalid file: \'" << current_.strval << '\'';
            return false;
        }

        includeList.push_back(includeAbsPath);

        if (!lexer_.Lex(current_)) {
            return false;
        }

        if (current_ == INCLUDE) {
            continue;
        }

        break;
    } while (true);

    return true;
}

std::shared_ptr<AstObject> Parser::ParseNode(Token &name, bool bracesStart)
{
    /* bracesStart： if true, current is '{' , else need to read next token and check with '}' */
    if (!bracesStart) {
        if (!lexer_.Lex(current_) || current_ != '{') {
            Logger().Error() << lexer_ << "syntax error, node miss '{'";
            return nullptr;
        }
    }

    auto node = std::make_shared<ConfigNode>(name, NODE_NOREF, "");
    std::shared_ptr<AstObject> child;
    while (lexer_.Lex(current_) && current_ != '}') {
        switch (current_.type) {
            case TEMPLATE:
                child = ParseTemplate();
                break;
            case LITERAL:
                child = ParseNodeAndTerm();
                break;
            default:
                Logger().Error() << lexer_ << "syntax error, except '}' or TEMPLATE or LITERAL for node '"
                                 << name.strval << '\'';
                return nullptr;
        }
        if (child == nullptr) {
            return nullptr;
        }

        node->AddChild(child);
    }

    if (current_ != '}') {
        Logger().Error() << lexer_ << "syntax error, node miss '}'";
        return nullptr;
    }
    return node;
}

std::shared_ptr<AstObject> Parser::ParseTerm(Token &name)
{
    if (!lexer_.Lex(current_)) {
        Logger().Error() << lexer_ << "syntax error, miss value of config term";
        return nullptr;
    }
    auto term = std::make_shared<ConfigTerm>(name, nullptr);
    switch (current_.type) {
        case STRING:
            term->AddChild(std::make_shared<AstObject>("", PARSEROP_STRING, current_.strval, current_));
            break;
        case NUMBER:
            term->AddChild(std::make_shared<AstObject>("", PARSEROP_UINT64, current_.numval, current_));
            break;
        case '[': {
            std::shared_ptr<AstObject> list = ParseArray();
            if (list == nullptr) {
                return nullptr;
            } else {
                term->AddChild(list);
            }
            break;
        }
        case '&':
            if (!lexer_.Lex(current_) || (current_ != LITERAL && current_ != REF_PATH)) {
                Logger().Error() << lexer_ << "syntax error, invalid config term definition";
                return nullptr;
            }
            term->AddChild(std::make_shared<AstObject>("", PARSEROP_NODEREF, current_.strval, current_));
            break;
        case DELETE:
            term->AddChild(std::make_shared<AstObject>("", PARSEROP_DELETE, current_.strval, current_));
            break;
        default:
            Logger().Error() << lexer_ << "syntax error, invalid config term definition";
            return nullptr;
    }

    if (!lexer_.Lex(current_) || current_ != ';') {
        Logger().Error() << lexer_ << "syntax error, miss ';'";
        return nullptr;
    }

    return term;
}

std::shared_ptr<AstObject> Parser::ParseTemplate()
{
    if (!lexer_.Lex(current_) || current_ != LITERAL) {
        Logger().Error() << lexer_ << "syntax error, template miss name";
        return nullptr;
    }

    auto name = current_;
    auto node = ParseNode(name, false);
    if (node == nullptr) {
        return node;
    }

    ConfigNode::CastFrom(node)->SetNodeType(NODE_TEMPLATE);
    return node;
}

std::shared_ptr<AstObject> Parser::ParseNodeAndTerm()
{
    auto name = current_;
    if (!lexer_.Lex(current_)) {
        Logger().Error() << lexer_ << "syntax error, broken term or node";
        return nullptr;
    }

    switch (current_.type) {
        case '=':
            return ParseTerm(name);
        case '{':
            return ParseNode(name, true);
        case ':':
            if (lexer_.Lex(current_)) {
                return ParseNodeWithRef(name);
            }
            Logger().Error() << lexer_ << "syntax error, unknown node reference type";
            break;
        default:
            Logger().Error() << lexer_ << "syntax error, except '=' or '{' or ':'";
            break;
    }

    return nullptr;
}

std::shared_ptr<AstObject> Parser::ParseNodeWithRef(Token name)
{
    std::shared_ptr<AstObject> node;
    switch (current_.type) {
        case REF_PATH:
        case LITERAL:
            return ParseNodeCopy(name);
        case '&':
            return ParseNodeRef(name);
        case DELETE:
            return ParseNodeDelete(name);
        case ':':
            return ParseNodeInherit(name);
        default:
            Logger().Error() << lexer_ << "syntax error, unknown node type";
            break;
    }

    return node;
}

/* started with NodePath on gramme : LITERAL ':' NodePath '{' ConfigTermList '}'*/
std::shared_ptr<AstObject> Parser::ParseNodeCopy(Token &name)
{
    auto nodePath = current_.strval;

    auto node = ParseNode(name);
    if (node == nullptr) {
        return nullptr;
    }

    auto nodeCopy = ConfigNode::CastFrom(node);
    nodeCopy->SetNodeType(NODE_COPY);
    nodeCopy->SetRefPath(nodePath);

    return node;
}

/* started with & on gramme : LITERAL ':' '&' NodePath '{' ConfigTermList '}'*/
std::shared_ptr<AstObject> Parser::ParseNodeRef(Token &name)
{
    if (!lexer_.Lex(current_) || (current_ != LITERAL && current_ != REF_PATH)) {
        Logger().Error() << lexer_ << "syntax error, miss node reference path";
        return nullptr;
    }
    auto refPath = current_.strval;
    auto node = ParseNode(name);
    if (node == nullptr) {
        return nullptr;
    }

    auto configNode = ConfigNode::CastFrom(node);
    configNode->SetNodeType(NODE_REF);
    configNode->SetRefPath(refPath);
    return node;
}

/* started with DELETE on gramme : LITERAL ':' DELETE '{' ConfigTermList '}'*/
std::shared_ptr<AstObject> Parser::ParseNodeDelete(Token &name)
{
    auto node = ParseNode(name);
    if (node == nullptr) {
        return nullptr;
    }

    /* maybe drop node context is better */
    auto configNode = ConfigNode::CastFrom(node);
    configNode->SetNodeType(NODE_DELETE);
    return node;
}

/* started with 2th ':' on gramme : LITERAL ':' ':' NodePath '{' ConfigTermList '}'*/
std::shared_ptr<AstObject> Parser::ParseNodeInherit(Token &name)
{
    if (!lexer_.Lex(current_) || (current_ != LITERAL && current_ != REF_PATH)) {
        Logger().Error() << lexer_ << "syntax error, miss node inherit path";
        return nullptr;
    }

    auto inheritPath = current_.strval;

    auto node = ParseNode(name);
    if (node == nullptr) {
        return nullptr;
    }

    auto configNode = ConfigNode::CastFrom(node);
    configNode->SetNodeType(NODE_INHERIT);
    configNode->SetRefPath(inheritPath);

    return node;
}

std::shared_ptr<AstObject> Parser::ParseArray()
{
    auto array = std::make_shared<ConfigArray>(current_);
    int32_t arrayType = 0;

    while (lexer_.Lex(current_) && current_ != ']') {
        if (current_.type == STRING) {
            array->AddChild(std::make_shared<AstObject>("", PARSEROP_STRING, current_.strval, current_));
        } else if (current_.type == NUMBER) {
            array->AddChild(std::make_shared<AstObject>("", PARSEROP_UINT64, current_.numval, current_));
        } else {
            Logger().Error() << lexer_ << "syntax error, except STRING or NUMBER in array";
            return nullptr;
        }

        if (arrayType == 0) {
            arrayType = current_.type;
        } else if (arrayType != current_.type) {
            Logger().Error() << lexer_ << "syntax error, not allow mix type array";
            return nullptr;
        }

        if (lexer_.Lex(current_)) {
            if (current_ == ',') {
                continue;
            } else if (current_ == ']') {
                break;
            } else {
                Logger().Error() << lexer_ << "syntax error, except ',' or ']'";
                return nullptr;
            }
        }
        return std::shared_ptr<AstObject>();
    }

    if (current_ != ']') {
        Logger().Error() << lexer_ << "syntax error, miss ']' at end of array";
        return nullptr;
    }

    return array;
}

std::shared_ptr<Ast> Parser::GetAst()
{
    return ast_;
}
