/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_PARSER_H
#define HC_GEN_PARSER_H

#include <memory>

#include "ast.h"
#include "lexer.h"

namespace OHOS {
namespace Hardware {

class Parser {
public:
    Parser() = default;

    ~Parser() = default;

    bool Parse();

    std::shared_ptr<Ast> ParseOne(const std::string &src, std::list<std::string> &includeList);

    std::shared_ptr<Ast> GetAst();

private:
    bool ProcessInclude(std::list<std::string> &includeList);

    std::shared_ptr<AstObject> ParseTemplate();

    std::shared_ptr<AstObject> ParseNodeAndTerm();

    std::shared_ptr<AstObject> ParseNodeCopy(Token &name);

    std::shared_ptr<AstObject> ParseNodeRef(Token &name);

    std::shared_ptr<AstObject> ParseNodeDelete(Token &name);

    std::shared_ptr<AstObject> ParseNodeInherit(Token &name);

    std::shared_ptr<AstObject> ParseNode(Token &name, bool bracesStart = false);

    std::shared_ptr<AstObject> ParseTerm(Token &name);

    std::shared_ptr<AstObject> ParseNodeWithRef(Token name);

    std::shared_ptr<AstObject> ParseArray();

    Lexer lexer_;
    Token current_;
    std::shared_ptr<Ast> ast_;
    std::list<std::string> srcQueue_;
};

} // namespace Hardware
} // namespace OHOS
#endif // HC_GEN_PARSER_H
