/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <set>

#include "file.h"
#include "logger.h"
#include "opcode.h"
#include "text_gen.h"

using namespace OHOS::Hardware;

constexpr static const char *FILE_HEAD_COMMENT =
    "/*\n"
    " * This is an automatically generated HDF config file. Do not modify it manually.\n"
    " */\n\n";

TextGen::TextGen(std::shared_ptr<Ast> ast) : Generator(ast) {}

bool TextGen::Output()
{
    if (!Initialize() || !DuplicateNodeNameCheck()) {
        return false;
    }

    auto ret = HeaderOutput();
    if (ret) {
        ret = ImplOutput();
    }

    if (!ret && (ofs_.is_open() && !ofs_.good())) {
        Logger().Error() << "failed to write file:" << outputFileName_;
    }

    return ret;
}

bool TextGen::Initialize()
{
    auto opt = Option::Instance();
    prefix_ = opt.GetSymbolPrefix();
    if (prefix_.empty()) {
        prefix_ = "HdfConfig";
    }

    prefix_ = ToLowerCamelString(prefix_);

    auto moduleTerm = ast_->GetAstRoot()->Lookup("module", PARSEROP_CONFTERM);
    if (moduleTerm == nullptr) {
        return false; // never hit
    }

    moduleName_ = ToUpperCamelString(moduleTerm->Child()->StringValue());
    rootVariableName_ = "g_" + prefix_ + moduleName_ + "ModuleRoot";
    return true;
}

bool TextGen::HeaderOutput()
{
    if (!InitOutput(".h")) {
        return false;
    }
    ofs_ << FILE_HEAD_COMMENT;

    std::string headerMacro = GetHeaderProtectMacro(outputFileName_);
    ofs_ << "#ifndef " << headerMacro << "\n";
    ofs_ << "#define " << headerMacro << "\n\n";
    ofs_ << "#include <stdint.h>\n\n";

    if (!HeaderOutputTraversal()) {
        return false;
    }

    ofs_ << "\n#endif // " << headerMacro << '\n';
    return ofs_.good();
}

bool TextGen::HeaderOutputTraversal()
{
    auto ret = ast_->WalkBackward([this](const std::shared_ptr<AstObject> &current, int32_t) -> uint32_t {
        if (!current->IsNode()) {
            return NOERR;
        }
        auto node = ConfigNode::CastFrom(current);
        if (node->GetNodeType() == NODE_INHERIT) {
            return NOERR;
        }

        return GenNodeDefinition(current);
    });
    if (!ret) {
        return false;
    }

    ofs_ << "const struct " << ToUpperCamelString(prefix_) << moduleName_ << "Root* HdfGet" << moduleName_
         << "ModuleConfigRoot(void);\n";
    return ofs_.good();
}

bool TextGen::ImplOutput()
{
    if (!InitOutput(".c")) {
        return false;
    }
    symMap.clear();
    ofs_ << FILE_HEAD_COMMENT;
    ofs_ << "#include \"" << outputNameBase_ << ".h\"\n\n";

    bool ret = OutputTemplateImpl();
    if (ret) {
        ret = OutputImplGlobalVariables();
    }

    if (!ret) {
        return ret;
    }

    ofs_ << "\nconst struct " << ToUpperCamelString(prefix_) << moduleName_ << "Root* HdfGet" << moduleName_
         << "ModuleConfigRoot(void)\n"
         << "{\n"
         << Indent() << "return &" << rootVariableName_ << ";\n"
         << "}\n";

    return ofs_.good();
}

std::string TextGen::ToUpperCamelString(const std::string &str)
{
    if (str.empty()) {
        return str;
    }
    auto out = ToCamelString(str);
    out[0] = static_cast<char>(toupper(out[0]));

    return out;
}

std::string TextGen::ToLowerCamelString(const std::string &str)
{
    if (str.empty()) {
        return str;
    }
    auto out = ToCamelString(str);
    out[0] = static_cast<char>(tolower(out[0]));

    return out;
}

std::string TextGen::ToCamelString(const std::string &str)
{
    if (str.empty()) {
        return str;
    }

    if (str.find('_') == std::string::npos) {
        return str;
    }

    std::string out;
    char cb = '\0';
    constexpr char underLine = '_';
    for (auto c : str) {
        if (c == '_') {
            cb = c;
            continue;
        }

        if (cb == underLine) {
            out.push_back(static_cast<char>(toupper(c)));
        } else {
            out.push_back(c);
        }

        cb = c;
    }

    return out;
}

bool TextGen::InitOutput(const std::string &fileSuffix)
{
    ofs_.close();
    outputFileName_ = Option::Instance().GetOutputName();
    if (outputFileName_.empty()) {
        outputFileName_ = Option::Instance().GetSourceNameBase();
    }
    outputFileName_ = Util::File::StripSuffix(outputFileName_).append(fileSuffix);
    outputNameBase_ = Util::File::FileNameBase(outputFileName_);

    ofs_.open(outputFileName_, std::ostream::out | std::ostream::binary);
    if (!ofs_.is_open()) {
        Logger().Error() << "failed to open output file: " << outputFileName_;
        return false;
    }

    return true;
}

const std::string &TextGen::ToUpperString(std::string &str)
{
    for (char &i : str) {
        i = static_cast<char>(toupper(i));
    }

    return str;
}

std::string TextGen::GetHeaderProtectMacro(const std::string &headerFileName)
{
    return std::string().append("HCS_CONFIG_").append(ToUpperString(outputNameBase_)).append("_HEADER_H");
}

uint32_t TextGen::GenNodeDefinition(const std::shared_ptr<AstObject> &node)
{
    auto structName = GenConfigStructName(node);
    static std::set<std::string> symbolSet;

    if (symbolSet.find(structName) != symbolSet.end()) {
        return NOERR;
    } else {
        symbolSet.insert(structName);
    }

    ofs_ << "struct " << structName << " {\n";

    auto termIt = node->Child();
    while (termIt != nullptr) {
        bool res = GenObjectDefinitionGen(termIt);
        if (!res) {
            return res;
        }
        termIt = termIt->Next();
    }

    ofs_ << "};\n\n";

    return ofs_.good() ? NOERR : EOUTPUT;
}

std::string TextGen::GenConfigStructName(const std::shared_ptr<AstObject> &node)
{
    return ToUpperCamelString(prefix_).append(ToUpperCamelString(moduleName_)).append(ToUpperCamelString(node->Name()));
}

bool TextGen::GenObjectDefinitionGen(const std::shared_ptr<AstObject> &object)
{
    if (!object->IsNode() && !object->IsTerm()) {
        return true;
    }

    switch (object->Type()) {
        case PARSEROP_CONFNODE: {
            auto structName = GenConfigStructName(object);
            auto node = ConfigNode::CastFrom(object);
            auto nodeName = ToLowerCamelString(node->Name());
            if (node->GetNodeType() == NODE_TEMPLATE) {
                ofs_ << Indent() << "const struct " << structName << "* " << nodeName << ";\n";
                ofs_ << Indent() << "uint16_t " << nodeName << "Size;\n";

            } else if (node->GetNodeType() == NODE_INHERIT) {
                return true;
            } else {
                ofs_ << Indent() << "struct " << structName << " " << nodeName << ";\n";
            }
            break;
        }
        case PARSEROP_CONFTERM:
            return GenTermDefinition(object);
        default:
            break;
    }

    return ofs_.good();
}

bool TextGen::GenTermDefinition(const std::shared_ptr<AstObject> &term)
{
    auto value = term->Child();
    switch (value->Type()) {
        case PARSEROP_ARRAY: {
            auto array = ConfigArray::CastFrom(value);
            if (IsInTemplate(term)) {
                ofs_ << TAB << "const " << TypeToStr(array->ArrayType()) << "* " << term->Name() << ";\n";
                ofs_ << TAB << "uint32_t " << term->Name() << "Size;\n";
            } else {
                ofs_ << TAB << TypeToStr(array->ArrayType()) << " " << term->Name() << "[" << array->ArraySize()
                     << "];\n";
            }
            break;
        }
        case PARSEROP_UINT8:
        case PARSEROP_UINT16:
        case PARSEROP_UINT32:
        case PARSEROP_UINT64:
        case PARSEROP_STRING:
            ofs_ << TAB << TypeToStr(value->Type()) << " " << term->Name() << ";\n";
            break;
        case PARSEROP_NODEREF: {
            auto structName = GenConfigStructName(ConfigTerm::CastFrom(term)->RefNode().lock());
            ofs_ << TAB << "const struct " << structName << "* " << term->Name() << ";\n";
            break;
        }
        default:
            break;
    }

    return ofs_.good();
}

bool TextGen::IsInTemplate(const std::shared_ptr<AstObject> &object)
{
    auto p = object->Parent();
    while (p != nullptr) {
        if (p->IsNode() && ConfigNode::CastFrom(p)->GetNodeType() == NODE_TEMPLATE) {
            return true;
        }
        p = p->Parent();
    }
    return false;
}

const std::string &TextGen::TypeToStr(uint32_t type)
{
    static std::map<uint32_t, std::string> typeMap = {
        {PARSEROP_UINT8,  "uint8_t"    },
        {PARSEROP_UINT16, "uint16_t"   },
        {PARSEROP_UINT32, "uint32_t"   },
        {PARSEROP_UINT64, "uint64_t"   },
        {PARSEROP_STRING, "const char*"},
    };
    return typeMap[type];
}

bool TextGen::OutputImplGlobalVariables()
{
    auto forwardWalkFunc = [this](const std::shared_ptr<AstObject> &current, int32_t depth) -> uint32_t {
        return ImplementGenTraversal(current, depth);
    };

    auto backwardWalkFunc = [this](const std::shared_ptr<AstObject> &current, int32_t depth) -> uint32_t {
        return ImplementCloseBraceGen(current, depth);
    };

    return ast_->WalkRound(forwardWalkFunc, backwardWalkFunc);
}

const std::string &TextGen::Indent(uint32_t times)
{
    static std::map<uint32_t, std::string> indentMap;
    auto indent = indentMap.find(times);
    if (indent == indentMap.end()) {
        auto str = std::string();
        for (uint32_t i = 0; i < times; ++i) {
            str.append(TAB);
        }
        indentMap.emplace(std::pair<uint32_t, std::string>(times, std::move(str)));
    }

    return indentMap.at(times);
}

uint32_t TextGen::ImplementCloseBraceGen(const std::shared_ptr<AstObject> &object, int32_t depth)
{
    if (!object->IsNode() || ConfigNode::CastFrom(object)->GetNodeType() == NODE_INHERIT) {
        return NOERR;
    }
    if (object == ast_->GetAstRoot()) {
        ofs_ << "};\n";
    } else {
        ofs_ << Indent(depth) << "},\n";
    }
    return ofs_.good() ? NOERR : EOUTPUT;
}

uint32_t TextGen::ImplementGenTraversal(const std::shared_ptr<AstObject> &object, int32_t depth)
{
    if (!object->IsNode() && !object->IsTerm()) {
        return NOERR;
    }

    if (object->IsTerm() && IsInSubClassNode(object)) {
        return NOERR;
    }

    if (object == ast_->GetAstRoot()) {
        auto structName = GenConfigStructName(object);
        ofs_ << "static const struct " << structName << " " << rootVariableName_ << " = {\n";
        if (object->Child() == nullptr) {
            ofs_ << "};\n";
        }
        return ofs_.good() ? NOERR : EOUTPUT;
    }
    return ObjectImplementGen(object, depth);
}

bool TextGen::IsInSubClassNode(const std::shared_ptr<AstObject> &object)
{
    std::shared_ptr<AstObject> obj = object;
    while (obj != nullptr) {
        if (obj->IsNode() && ConfigNode::CastFrom(obj)->GetNodeType() == NODE_INHERIT) {
            return true;
        }
        obj = obj->Parent();
    }
    return false;
}

uint32_t TextGen::ObjectImplementGen(const std::shared_ptr<AstObject> &object, int32_t depth)
{
    switch (object->Type()) {
        case PARSEROP_CONFNODE: {
            auto node = ConfigNode::CastFrom(object);
            if (node->GetNodeType() != NODE_NOREF) {
                return TemplateObjectImplGen(object, depth) ? EASTWALKBREAK : EOUTPUT;
            }
            ofs_ << Indent(depth) << '.' << node->Name() << " = {\n";
            if (node->Child() == nullptr) {
                ofs_ << Indent(depth) << "},\n";
            }
            break;
        }
        case PARSEROP_CONFTERM:
            return PrintTermImplement(object, depth);
        default:
            return NOERR;
    }

    return ofs_.good() ? NOERR : EOUTPUT;
}

bool TextGen::TemplateObjectImplGen(const std::shared_ptr<AstObject> &object, int32_t depth)
{
    auto node = ConfigNode::CastFrom(object);
    if (node->GetNodeType() != NODE_TEMPLATE) {
        return true;
    }

    std::string varName = GenTemplateVariableName(object);
    auto nodeName = ToLowerCamelString(node->Name());

    ofs_ << Indent(depth) << '.' << nodeName << " = ";
    if (node->InheritCount() != 0) {
        ofs_ << varName;
    } else {
        ofs_ << '0';
    }
    ofs_ << ",\n";
    ofs_ << Indent(depth) << '.' << nodeName << "Size = " << node->InheritCount() << ",\n";
    return ofs_.good();
}

std::string TextGen::GenTemplateVariableName(const std::shared_ptr<AstObject> &object)
{
    auto name = ToUpperCamelString(object->Name());
    auto sym = SymbolFind(name);
    auto node = ConfigNode::CastFrom(object);
    if (sym == nullptr) {
        SymbolAdd(name, object);
    } else if (sym->object != object && node->TemplateSignNum() == 0) {
        sym->duplicateCount++;
        node->SetTemplateSignNum(sym->duplicateCount);
    }

    return node->TemplateSignNum() != 0 ?
        std::string("g_").append(prefix_).append(name).append(std::to_string(node->TemplateSignNum())) :
        std::string("g_").append(prefix_).append(name);
}

std::shared_ptr<TextGen::Symbol> TextGen::SymbolFind(const std::string &name)
{
    auto sym = symMap.find(name);
    return sym == symMap.end() ? nullptr : sym->second;
}

void TextGen::SymbolAdd(const std::string &name, const std::shared_ptr<AstObject> &object)
{
    auto sym = symMap.find(name);
    if (sym != symMap.end()) {
        sym->second->duplicateCount++;
    }
    symMap.insert(std::make_pair(std::string(name), std::make_shared<Symbol>(object, 1)));
}

uint32_t TextGen::PrintTermImplement(const std::shared_ptr<AstObject> &object, int32_t depth)
{
    auto term = ConfigTerm::CastFrom(object);
    auto value = object->Child();
    switch (value->Type()) {
        case PARSEROP_UINT8:
        case PARSEROP_UINT16: /* fall-through */
        case PARSEROP_UINT32: /* fall-through */
        case PARSEROP_UINT64: /* fall-through */
        case PARSEROP_STRING:
            ofs_ << Indent(depth) << '.' << term->Name() << " = ";
            if (!PrintBaseTypeValue(value)) {
                return EOUTPUT;
            }
            ofs_ << ",\n";
            break;
        case PARSEROP_ARRAY:
            return PrintArrayImplement(object, depth);
        case PARSEROP_NODEREF: {
            std::string refPath = HcsBuildObjectPath(term->RefNode().lock());
            ofs_ << Indent(depth) << '.' << term->Name() << " = &" << refPath << ",\n";
            break;
        }
        default:
            break;
    }

    return ofs_.good() ? NOERR : EOUTPUT;
}

bool TextGen::PrintBaseTypeValue(const std::shared_ptr<AstObject> &object)
{
    switch (object->Type()) {
        case PARSEROP_UINT8:  /* fallthrough */
        case PARSEROP_UINT16: /* fallthrough */
        case PARSEROP_UINT32: /* fallthrough */
        case PARSEROP_UINT64:
            ofs_ << "0x" << std::hex << object->IntegerValue();
            break;
        case PARSEROP_STRING:
            ofs_ << '"' << object->StringValue() << '"';
            break;
        default:
            break;
    }

    return ofs_.good();
}

uint32_t TextGen::PrintArrayImplement(const std::shared_ptr<AstObject> &object, int32_t depth)
{
    if (IsInSubClassNode(object)) {
        return PrintArrayImplInSubClass(object, depth) ? NOERR : EOUTPUT;
    }

    auto termContext = object->Child();
    ofs_ << Indent(depth) << '.' << object->Name() << " = { ";

    if (!HcsPrintArrayContent(termContext, depth + 1)) {
        return EOUTPUT;
    }

    ofs_ << " },\n";

    return ofs_.good() ? NOERR : EOUTPUT;
}

bool TextGen::PrintArrayImplInSubClass(const std::shared_ptr<AstObject> &object, int32_t depth)
{
    auto array = ConfigArray::CastFrom(object->Child());
    auto arrayName = GenArrayName(object);

    ofs_ << Indent(depth) << '.' << object->Name() << " = " << arrayName << ",\n";
    ofs_ << Indent(depth) << '.' << object->Name() << "Size = " << std::to_string(array->ArraySize()) << ",\n";

    return ofs_.good();
}

bool TextGen::HcsPrintArrayContent(const std::shared_ptr<AstObject> &object, uint32_t indent)
{
    constexpr uint32_t ELEMENT_PER_LINE = 16;
    auto element = object->Child();
    uint32_t elementCount = 0;
    while (element != nullptr) {
        if (!PrintBaseTypeValue(element)) {
            return false;
        }
        if (elementCount++ >= ELEMENT_PER_LINE) {
            ofs_ << "\n" << Indent(indent);
        }
        element = element->Next();
        if (element != nullptr) {
            ofs_ << ", ";
        }
    }

    return ofs_.good();
}

std::string TextGen::HcsBuildObjectPath(std::shared_ptr<AstObject> object)
{
    std::list<std::shared_ptr<AstObject>> pathList;
    auto p = object;
    while (p != ast_->GetAstRoot()) {
        pathList.push_back(p);
        p = p->Parent();
    }

    pathList.reverse();

    std::string path = rootVariableName_;
    for (auto &it : pathList) {
        path.append(".").append(it->Name());
    }

    return path;
}

bool TextGen::OutputTemplateImpl()
{
    if (!OutputTemplateVariablesDeclare()) {
        return false;
    }

    return ast_->WalkBackward([this](const std::shared_ptr<AstObject> &object, int32_t) -> uint32_t {
        if (!object->IsNode() || (object->IsNode() && ConfigNode::CastFrom(object)->GetNodeType() != NODE_TEMPLATE)) {
            return NOERR;
        }
        auto node = ConfigNode::CastFrom(object);
        if (node->InheritCount() == 0) {
            return NOERR;
        }

        ofs_ << "static const struct " << GenConfigStructName(object) << ' ' << GenTemplateVariableName(object)
             << "[] = {\n";
        auto subClass = node->SubClasses();
        for (auto nodeObj : subClass) {
            std::shared_ptr<AstObject> obj = std::shared_ptr<AstObject>(nodeObj, [](auto p) {});
            ofs_ << Indent() << '[' << ConfigNode::CastFrom(obj)->InheritIndex() << "] = {\n";
            if (!TemplateVariableGen(obj)) {
                return EOUTPUT;
            }
            ofs_ << Indent() << "},\n";
        }

        ofs_ << "};\n\n";
        return ofs_.good() ? NOERR : EOUTPUT;
    });
}

bool TextGen::OutputTemplateVariablesDeclare()
{
    return ast_->WalkBackward([this](const std::shared_ptr<AstObject> &object, int32_t) -> uint32_t {
        if (object->IsTerm() && object->Child()->IsArray()) {
            return ArrayVariablesDeclareGen(object);
        } else if (!object->IsNode() ||
            (object->IsNode() && ConfigNode::CastFrom(object)->GetNodeType() != NODE_TEMPLATE)) {
            return NOERR;
        }
        auto node = ConfigNode::CastFrom(object);
        if (node->InheritCount() == 0) {
            return NOERR;
        }

        auto structName = GenConfigStructName(object);
        ofs_ << "static const struct " << GenConfigStructName(object) << ' ' << GenTemplateVariableName(object)
             << "[];\n\n";

        return ofs_.good() ? NOERR : EOUTPUT;
    });
}

uint32_t TextGen::ArrayVariablesDeclareGen(const std::shared_ptr<AstObject> &object)
{
    if (!IsInSubClassNode(object)) {
        return NOERR;
    }

    auto arrayName = GenArrayName(object);
    auto array = ConfigArray::CastFrom(object->Child());
    ofs_ << "static const " << TypeToStr(array->ArrayType()) << ' ' << arrayName << '[' << array->ArraySize()
         << "] = {\n"
         << Indent();
    HcsPrintArrayContent(object->Child(), 1);
    ofs_ << "\n};\n\n";

    return ofs_.good() ? NOERR : EOUTPUT;
}

std::string TextGen::GenArrayName(const std::shared_ptr<AstObject> &term)
{
    auto arrayName = std::string("g_hcsConfigArray").append(ToUpperCamelString(term->Name()));
    auto t = ConfigTerm::CastFrom(term);
    auto sym = SymbolFind(arrayName);
    if (sym == nullptr) {
        SymbolAdd(arrayName, term);
        t->SetSigNum(1);
    } else if (t->SigNum() == 0) {
        t->SetSigNum(sym->duplicateCount + 1);
        sym->duplicateCount++;
    }

    arrayName.append(std::to_string(t->SigNum()));
    return arrayName;
}

uint32_t TextGen::TemplateVariableGen(const std::shared_ptr<AstObject> &nodeObject)
{
    auto child = nodeObject->Child();
    while (child != nullptr) {
        auto res = Ast::WalkRound(
            child,
            [this](const std::shared_ptr<AstObject> &object, uint32_t depth) -> uint32_t {
                return ObjectImplementGen(object, depth + 2);
            },
            [this](const std::shared_ptr<AstObject> &object, uint32_t depth) -> uint32_t {
                return ImplementCloseBraceGen(object, depth + 2);
            });
        if (!res) {
            return false;
        }
        child = child->Next();
    }
    return true;
}

bool TextGen::DuplicateNodeNameCheck()
{
    std::map<std::string, std::shared_ptr<AstObject>> nodeMap;

    return ast_->WalkForward([&](const std::shared_ptr<AstObject> &current, uint32_t) -> uint32_t {
        if (!current->IsNode() || IsInSubClassNode(current)) {
            return NOERR;
        }

        auto node = nodeMap.find(current->Name());
        if (node == nodeMap.end()) {
            nodeMap[current->Name()] = current;
            return NOERR;
        }

        Logger().Error() << current->SourceInfo() << "duplicate node name at " << node->second->SourceInfo() << "\n"
                         << "To avoid redefining structures, not allow duplicate node name at text config mode";
        return EFAIL;
    });
}
