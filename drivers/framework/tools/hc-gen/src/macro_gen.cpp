/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include <string>

#include "file.h"
#include "logger.h"
#include "macro_gen.h"

using namespace OHOS::Hardware;

constexpr static const char *FILE_HEAD_COMMENT =
    "/*\n"
    " * This is an automatically generated HDF config file. Do not modify it manually.\n"
    " */\n\n";

MacroGen::MacroGen(std::shared_ptr<Ast> ast) : Generator(ast) {}

const std::string &MacroGen::ToUpperString(std::string &str)
{
    for (char &c : str) {
        c = static_cast<char>(toupper(c));
    }

    return str;
}

bool MacroGen::HeaderTopOutput()
{
    ofs_ << FILE_HEAD_COMMENT;
    std::string defMacro = std::string().append("HCS_CONFIG_").append(ToUpperString(outFileName_)).append("_HEADER_H");
    ofs_ << "#ifndef " << defMacro << '\n';
    ofs_ << "#define " << defMacro << '\n';
    ofs_ << "\r\n#include \"hdf_base.h\"\n";

    return true;
}

bool MacroGen::HeaderBottomOutput()
{
    std::string defMacro = std::string().append("HCS_CONFIG_").append(ToUpperString(outFileName_)).append("_HEADER_H");
    ofs_ << "\n#endif // " << defMacro << '\n';
    ofs_.close();
    return true;
}

bool MacroGen::Output()
{
    if (!Initialize()) {
        return false;
    }
    if (!TemplateNodeSeparate()) {
        return false;
    }
    if (!HeaderTopOutput()) {
        return false;
    }

    if (!NodeWalk()) {
        return false;
    }
    if (!HeaderBottomOutput()) {
        return false;
    }
    return true;
}

bool MacroGen::Initialize()
{
    std::string outFileName = Option::Instance().GetOutputName();
    if (outFileName.empty()) {
        outFileName = Option::Instance().GetSourceNameBase();
    }
    outFileName = Util::File::StripSuffix(outFileName).append(".h");
    outFileName_ = Util::File::FileNameBase(outFileName);
    if (outFileName.find(".h") == std::string::npos) {
        outFileName.append(".h");
    }

    ofs_.open(outFileName, std::ofstream::out | std::ofstream::binary);
    if (!ofs_.is_open()) {
        Logger().Error() << "failed to open output file: " << outFileName;
        return false;
    }
    Logger().Debug() << "output: " << outFileName << outFileName_ << '\n';

    return true;
}

bool MacroGen::TemplateNodeSeparate()
{
    return ast_->WalkBackward([this](std::shared_ptr<AstObject> &object, int32_t depth) {
        if (object->IsNode() && ConfigNode::CastFrom(object)->GetNodeType() == NODE_TEMPLATE) {
            object->Separate();
            return NOERR;
        }
        return NOERR;
    });
}

std::string MacroGen::GenFullName(int32_t depth, const std::shared_ptr<AstObject> &node, const std::string &sep)
{
    std::string name;
    for (int i = 0; i < depth; i++) {
        name.append(nodeNameMap_[i]).append(sep);
    }
    name.append(node->Name());
    return name;
}

bool MacroGen::GenArray(
    const std::string &arrName, uint32_t &arrSize, uint32_t type, const std::shared_ptr<AstObject> &node)
{
    static uint32_t index = 0;
    const uint32_t ELEMENT_PER_LINE = 8;

    if (index == 0) {
        ofs_ << "#define " << arrName << "_data {";
    }

    if (index % ELEMENT_PER_LINE == 0) {
        ofs_ << " \\\n" << TAB;
    }

    if (type == PARSEROP_STRING) {
        ofs_ << '"' << node->StringValue() << '"';
    } else {
        ofs_ << node->IntegerValue();
    }

    index++;
    if (index == arrSize) {
        index = 0;
        arrSize = 0;
        ofs_ << " \\\n}\n";
    } else {
        if (index % ELEMENT_PER_LINE == 0) {
            ofs_ << ",";
        } else {
            ofs_ << ", ";
        }
    }

    return true;
}

bool MacroGen::GenNodeForeach(int32_t depth, const std::shared_ptr<AstObject> &node)
{
    std::list<std::string> subList;
    auto child = node->Child();
    uint32_t count = 0;

    Logger().Debug() << "node:" << node->Name() << " child:"
                     << " depth:" << depth;
    while (child != nullptr) {
        if (child->IsNode()) {
            Logger().Debug() << " " << child->Name();
            subList.push_back(GenFullName(depth + 1, child, "_"));
            count++;
        }
        child = child->Next();
    }

    ofs_ << "// hcs node macros: " << GenFullName(depth, node, "/") << std::endl;
    ofs_ << "#define " << GenFullName(depth, node, "_").append("_exists 1") << std::endl;
    if (count) {
        uint32_t index = count;
        std::list<std::string>::iterator iter;

        ofs_ << "#define " << GenFullName(depth, node, "_").append("_foreach_child(func)") << " \\\n";
        for (iter = subList.begin(); iter != subList.end(); ++iter) {
            if (--index) {
                ofs_ << TAB << "func(" << *iter << "); \\\n";
            } else {
                ofs_ << TAB << "func(" << *iter << ");\n";
            }
        }
        ofs_ << std::endl;

        index = count;
        ofs_ << "#define " << GenFullName(depth, node, "_").append("_foreach_child_vargs(func, ...)") << " \\\n";
        for (iter = subList.begin(); iter != subList.end(); ++iter) {
            if (--index) {
                ofs_ << TAB << "func(" << *iter << ", __VA_ARGS__); \\\n";
            } else {
                ofs_ << TAB << "func(" << *iter << ", __VA_ARGS__);\n";
            }
        }
        ofs_ << std::endl;
    }

    return true;
}

std::string MacroGen::GenRefObjName(int32_t depth, const std::shared_ptr<AstObject> &object)
{
    std::string name(object->StringValue());
    if (name.find(".") == std::string::npos) {
        name = GenFullName(depth - 1, object, "_").append(object->StringValue());
    } else {
        for (char &c : name) {
            if (c == '.') {
                c = '_';
            }
        }
    }
    return name;
}

bool MacroGen::NodeWalk()
{
    return ast_->WalkForward([this](std::shared_ptr<AstObject> &current, int32_t depth) {
        auto type = current->Type();
        static uint32_t arraySize = 0;

        Logger().Debug() << "name,type:[" << current->Name() << "," << type << "] depth:" << depth
                         << " arraySize:" << std::dec << arraySize << '\n';
        SetTypeData(type, current, arraySize, depth);

        return NOERR;
    });
}

void MacroGen::SetTypeData(uint32_t type, const std::shared_ptr<AstObject> &current, uint32_t &arraySize, int32_t depth)
{
    static std::string nodeName;
    static std::string arrayName;
    static uint32_t arrayType = 0;
    switch (type) {
        case PARSEROP_UINT8:
        case PARSEROP_UINT16:
        case PARSEROP_UINT32:
        case PARSEROP_UINT64: {
            SetTypeDataUinit64(arrayName, arraySize, arrayType, current);
            break;
        }
        case PARSEROP_STRING:
            if (arraySize != 0) {
                GenArray(arrayName, arraySize, arrayType, current);
            } else {
                ofs_ << " " << '"' << current->StringValue() << '"' << std::endl;
            }
            break;
        case PARSEROP_CONFTERM:
            ofs_ << "#define " << GenFullName(depth, current, "_") << "_exists 1" << std::endl;
            ofs_ << "#define " << GenFullName(depth, current, "_");
            break;
        case PARSEROP_CONFNODE: {
            if (nodeName != current->Name()) {
                ofs_ << std::endl;
            }
            nodeName = current->Name();
            nodeNameMap_[depth] = nodeName;
            GenNodeForeach(depth, current);
            break;
        }
        case PARSEROP_ARRAY: {
            std::shared_ptr<AstObject> parent(current->Parent());
            arraySize = ConfigArray::CastFrom(current)->ArraySize();
            arrayType = ConfigArray::CastFrom(current)->ArrayType();
            ofs_ << "_array_size " << arraySize << std::endl;
            arrayName = GenFullName(depth - 1, parent, "_");
            break;
        }
        case PARSEROP_NODEREF: {
            ofs_ << " " << GenRefObjName(depth, current) << std::endl;
            break;
        }
        default:
            break;
    }
}

void MacroGen::SetTypeDataUinit64(
    const std::string &arrayName, uint32_t &arraySize, uint32_t arrayType, const std::shared_ptr<AstObject> &current)
{
    if (arraySize != 0) {
        GenArray(arrayName, arraySize, arrayType, current);
    } else {
        ofs_ << " " << current->IntegerValue() << std::endl;
    }
}