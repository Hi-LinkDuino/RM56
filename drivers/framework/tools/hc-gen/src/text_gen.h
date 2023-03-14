/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_GEN_TEXT_GENERATOR_H
#define HC_GEN_TEXT_GENERATOR_H

#include <fstream>
#include <map>

#include "generator.h"

namespace OHOS {
namespace Hardware {
class TextGen : public Generator {
public:
    explicit TextGen(std::shared_ptr<Ast> ast);

    ~TextGen() override = default;

    bool Output() override;

private:
    bool Initialize();

    bool HeaderOutput();

    bool HeaderOutputTraversal();

    bool ImplOutput();

    bool InitOutput(const std::string &fileSuffix);

    bool DuplicateNodeNameCheck();

    static std::string ToUpperCamelString(const std::string &str);

    static std::string ToLowerCamelString(const std::string &str);

    static std::string ToCamelString(const std::string &str);

    static const std::string &ToUpperString(std::string &str);

    std::string GetHeaderProtectMacro(const std::string &headerFileName);

    uint32_t GenNodeDefinition(const std::shared_ptr<AstObject> &node);

    std::string GenConfigStructName(const std::shared_ptr<AstObject> &node);

    bool GenObjectDefinitionGen(const std::shared_ptr<AstObject> &object);

    bool GenTermDefinition(const std::shared_ptr<AstObject> &term);

    static const std::string &TypeToStr(uint32_t type);

    static bool IsInTemplate(const std::shared_ptr<AstObject> &object);

    bool OutputImplGlobalVariables();

    uint32_t ImplementGenTraversal(const std::shared_ptr<AstObject> &object, int32_t depth);

    uint32_t ImplementCloseBraceGen(const std::shared_ptr<AstObject> &object, int32_t depth);

    static const std::string &Indent(uint32_t times = 1);

    static bool IsInSubClassNode(const std::shared_ptr<AstObject> &object);

    uint32_t ObjectImplementGen(const std::shared_ptr<AstObject> &object, int32_t depth);

    bool TemplateObjectImplGen(const std::shared_ptr<AstObject> &object, int32_t depth);

    std::string GenTemplateVariableName(const std::shared_ptr<AstObject> &object);

    struct Symbol {
        Symbol(std::shared_ptr<AstObject> obj, uint32_t c) : object(std::move(obj)), duplicateCount(c) {};
        ~Symbol() = default;
        std::shared_ptr<AstObject> object;
        uint32_t duplicateCount;
    };

    std::shared_ptr<Symbol> SymbolFind(const std::string &name);

    void SymbolAdd(const std::string &name, const std::shared_ptr<AstObject> &object);

    std::ofstream ofs_;
    std::string outputFileName_;
    std::string outputNameBase_;
    std::string prefix_;
    std::string moduleName_;
    std::string rootVariableName_;
    std::map<std::string, std::shared_ptr<Symbol>> symMap;

    uint32_t PrintTermImplement(const std::shared_ptr<AstObject> &object, int32_t depth);

    bool PrintBaseTypeValue(const std::shared_ptr<AstObject> &object);

    uint32_t PrintArrayImplement(const std::shared_ptr<AstObject> &object, int32_t depth);

    bool PrintArrayImplInSubClass(const std::shared_ptr<AstObject> &object, int32_t depth);

    bool HcsPrintArrayContent(const std::shared_ptr<AstObject> &object, uint32_t indent);

    std::string HcsBuildObjectPath(std::shared_ptr<AstObject> object);

    bool OutputTemplateImpl();

    bool OutputTemplateVariablesDeclare();

    uint32_t ArrayVariablesDeclareGen(const std::shared_ptr<AstObject> &object);

    std::string GenArrayName(const std::shared_ptr<AstObject> &term);

    uint32_t TemplateVariableGen(const std::shared_ptr<AstObject> &nodeObject);
};
} // namespace Hardware
} // namespace OHOS
#endif // HC_GEN_TEXT_GENERATOR_H
