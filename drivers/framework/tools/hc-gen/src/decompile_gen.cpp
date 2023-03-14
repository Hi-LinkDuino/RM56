/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "decompile_gen.h"
#include "file.h"
#include "logger.h"
#include <sstream>

using namespace OHOS::Hardware;

DecompileGen::DecompileGen(std::shared_ptr<Ast> ast, std::string outPutFileName) : ast_(ast)
{
    outPutFileName_ = Util::File::StripSuffix(std::move(outPutFileName)).append(".d.hcs");
    Logger().Debug() << "Decompile gen file: " << outPutFileName_;
}

bool DecompileGen::Init()
{
    file_.open(outPutFileName_, std::ostream::out | std::ostream::binary);
    if (!file_.is_open()) {
        Logger().Error() << "Failed to open decompile output file: " << outPutFileName_;
        return false;
    }
    return true;
}

void DecompileGen::WriteFile(const std::string &str)
{
    file_ << str;
}

std::string DecompileGen::GetNodeRefPath(uint32_t value)
{
    std::string refPath;
    std::shared_ptr<AstObject> astObject = ast_->GetAstRoot();
    if (astObject == nullptr) {
        return refPath;
    }
    while (astObject->Child() != nullptr) {
        refPath = astObject->Name() + ".";
        auto child = astObject->Child();
        bool deepIn = false;
        while (child != nullptr) {
            if (child->Type() != PARSEROP_CONFNODE) {
                child = child->Next();
                continue;
            }
            if (child->GetHash() == value) {
                return (refPath + child->Name());
            }
            if (value > child->GetHash() && value < (child->GetHash() + child->GetSize())) {
                astObject = child;
                deepIn = true;
                break;
            }
            child = child->Next();
        }
        if (!deepIn) {
            Logger().Error() << "ref unknown node, hash = " << value;
            break;
        }
    }
    return std::string();
}

int32_t DecompileGen::PrintArrayType(const std::shared_ptr<AstObject> &astObj)
{
    WriteFile("[");
    auto arrayElement = astObj->Child();
    if (arrayElement != nullptr) {
        while (arrayElement->Next()) {
            if (PrintBaseType(arrayElement) != NOERR) {
                return EOUTPUT;
            }
            WriteFile(", ");
            arrayElement = arrayElement->Next();
        }
        if (PrintBaseType(arrayElement) != NOERR) {
            return EOUTPUT;
        }
    }
    WriteFile("]");
    return NOERR;
}

int32_t DecompileGen::PrintBaseType(const std::shared_ptr<AstObject> &astObj)
{
    std::stringstream outStr;
    std::string refPath;
    switch (astObj->Type()) {
        case PARSEROP_UINT8:
        case PARSEROP_UINT16:
        case PARSEROP_UINT32:
        case PARSEROP_UINT64:
            outStr << "0x" << std::uppercase << std::hex << astObj->IntegerValue();
            WriteFile(outStr.str());
            break;
        case PARSEROP_STRING:
            outStr << "\"" << astObj->StringValue() << "\"";
            WriteFile(outStr.str());
            break;
        case PARSEROP_NODEREF:
            refPath = GetNodeRefPath(astObj->IntegerValue());
            if (refPath.empty()) {
                return EOUTPUT;
            }
            WriteFile("&" + refPath);
            break;
        case PARSEROP_ARRAY:
            return PrintArrayType(astObj);
        default:
            Logger().Error() << "unknown opcode = " << astObj->Type();
            return EFAIL;
    }
    return NOERR;
}

int32_t DecompileGen::OutPutWalk(const std::shared_ptr<AstObject> &astObj, int32_t walkDepth)
{
    if (astObj->Type() != PARSEROP_CONFNODE && astObj->Type() != PARSEROP_CONFTERM) {
        return NOERR;
    }
    int ret;
    std::string tabStr = std::string(TAB_SIZE * walkDepth, ' ');
    if (walkDepth != 0) {
        WriteFile(tabStr);
    }
    std::string str;
    switch (astObj->Type()) {
        case PARSEROP_CONFNODE:
            str = astObj->Name() + " {\n";
            WriteFile(str);
            if (astObj->Child() == nullptr) {
                tabStr += "}\n";
                WriteFile(tabStr);
            }
            break;
        case PARSEROP_CONFTERM:
            str = astObj->Name() + " = ";
            WriteFile(str);
            ret = PrintBaseType(astObj->Child());
            if (ret != NOERR) {
                return ret;
            }
            WriteFile(";\n");
            break;
        default:
            return EOUTPUT;
    }
    return 0;
}

int32_t DecompileGen::CloseBrace(const std::shared_ptr<AstObject> &astObj, int32_t walkDepth)
{
    if (astObj->Type() != PARSEROP_CONFNODE) {
        return NOERR;
    }
    std::string tabStr = std::string(TAB_SIZE * walkDepth, ' ');
    if (astObj != ast_->GetAstRoot()) {
        WriteFile(tabStr + "}\n");
    } else {
        WriteFile("}\n");
    }
    return file_.good() ? NOERR : EOUTPUT;
}

bool DecompileGen::OutPut()
{
    if (!Init()) {
        return false;
    }
    WriteFile(fileHeader_);
    if (!ast_->WalkRound(
        [this](std::shared_ptr<AstObject> &current, int32_t walkDepth) -> int32_t {
            return OutPutWalk(current, walkDepth);
        },
        [this](std::shared_ptr<AstObject> &current, int32_t walkDepth) -> int32_t {
            return CloseBrace(current, walkDepth);
        })) {
        return false;
    }

    return file_.good();
}
