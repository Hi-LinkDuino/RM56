/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef OHOS_HDI_PARSER_H
#define OHOS_HDI_PARSER_H

#include <memory>
#include <vector>

#include "ast/ast.h"
#include "ast/ast_interface_type.h"
#include "ast/ast_method.h"
#include "ast/ast_module.h"
#include "ast/ast_type.h"
#include "parser/file_detail.h"
#include "parser/lexer.h"
#include "parser/token.h"
#include "util/autoptr.h"
#include "util/light_refcount_base.h"
#include "util/options.h"
#include "util/string.h"

namespace OHOS {
namespace HDI {
class Parser {
public:
    Parser(const Options &options, const AutoPtr<ASTModule> &module = nullptr);

    ~Parser() = default;

    bool Parse(const String &sourceFile);

    bool Parse(const String &sourceFile, std::shared_ptr<FileDetail> &fileDetailPtr);

    inline AutoPtr<AST> GetAst() const
    {
        return ast_;
    }

private:
    class Attribute : public LightRefCountBase {
    public:
        bool isOneWay = false;
        bool isCallback = false;
        bool isFull = false;
        bool isLite = false;
    };

    class ErrorInfo : public LightRefCountBase {
    public:
        String file_;
        Token token_;
        int lineNo_;
        int columnNo_;
        String message_;
        AutoPtr<ErrorInfo> next_;
    };

    bool ParseIdlDetail(std::shared_ptr<FileDetail> &fileDetailPtr);

    bool ParseIdlPackage(std::shared_ptr<FileDetail> &fileDetailPtr);

    bool ParseIdlImport(std::shared_ptr<FileDetail> &fileDetailPtr);

    bool ParseFile();

    bool ParseLicense();

    bool ParsePackageName();

    bool ParserPackageInfo(const String &packageFullName);

    bool ParseImport();

    bool ParseSequenceable();

    bool ParseAttribute();

    bool ParseInterface(const AutoPtr<Attribute> &attributes = nullptr);

    bool ParseInterfaceBody(const AutoPtr<ASTInterfaceType> &interface);

    bool ParseMethod(const AutoPtr<ASTInterfaceType> &interface);

    void SetVersionInterfaceMethod(const AutoPtr<ASTInterfaceType> &interface);

    bool ParseAttributeBody(AutoPtr<Attribute> &attributes);

    bool ParseAttributeParam(AutoPtr<Attribute> &attributes);

    bool ParseParameterList(AutoPtr<ASTMethod> &method);

    bool ParseParameter(const AutoPtr<ASTMethod> &method);

    bool ParseParamAttr(const AutoPtr<ASTParameter> &parameter);

    AutoPtr<ASTType> ParseType();

    AutoPtr<ASTType> ParseUnsignedType(int typeLineNo, int typeColumnNo);

    AutoPtr<ASTType> ParseList();

    AutoPtr<ASTType> ParseMap();

    AutoPtr<ASTType> ParseSharedMemQueueMetaType();

    AutoPtr<ASTType> ParseArrayType(const AutoPtr<ASTType> &elementType);

    AutoPtr<ASTType> ParseCustomType();

    bool ParseEnumDefine(const AutoPtr<Attribute> &attributes = nullptr);

    bool ParseEnumBaseType(const AutoPtr<ASTEnumType> &type);

    bool ParseEnumMember(const AutoPtr<ASTEnumType> &type);

    bool ParseStructDefine(const AutoPtr<Attribute> &attributes = nullptr);

    bool ParseStructMember(const AutoPtr<ASTStructType> &type);

    bool ParseUnionDefine(const AutoPtr<Attribute> &attributes = nullptr);

    bool ParseUnionMember(const AutoPtr<ASTUnionType> &type);

    bool CheckType(int lineNo, int columnNo, const AutoPtr<ASTType> &type);

    void SetAstFileType();

    bool CheckIntegrity();

    bool CheckInterfaceAst();

    bool CheckCallbackAst();

    bool IsValidTypeName(const String &typeName);

    bool CheckPackageName(const String &filePath, const String &packageName);

    inline static bool IsPrimitiveType(Token token)
    {
        return token >= Token::BOOLEAN && token <= Token::FILEDESCRIPTOR;
    }

    bool AddAst();

    void LogError(const String &message);

    void LogError(int lineNo, int columnNo, const String &message);

    void ShowError();

    static const char *TAG;

    const Options &options_;
    std::shared_ptr<Lexer> lexer_;
    AutoPtr<ErrorInfo> errors_;
    AutoPtr<AST> ast_;
    AutoPtr<ASTModule> astModule_;
};
} // namespace HDI
} // namespace OHOS

#endif // OHOS_HDI_PARSER_H