/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#include "startup_cfg_gen.h"
#include <algorithm>
#include <string>
#include "ast.h"
#include "file.h"
#include "logger.h"

using namespace OHOS::Hardware;

constexpr static const char *BOOT_CONFIG_TOP =
    "{\n"
    "    \"jobs\" : [{\n"
    "            \"name\" : \"post-fs-data\",\n"
    "            \"cmds\" : [\n"
    "                \"start hdf_devhost\"\n"
    "            ]\n"
    "        }\n"
    "    ],\n"
    "    \"services\" : [\n";
constexpr static const char *BOOT_CONFIG_BOTTOM =
    "    ]\n"
    "}\n";
constexpr static const char *SERVICE_TOP =
    "        {\n"
    "            \"name\" : ";
constexpr static const char *PATH_INFO = "            \"path\" : [\"/vendor/bin/hdf_devhost\", ";
constexpr static const char *UID_INFO_ROOT =
    "            \"uid\" : \"root\",\n"
    "            \"gid\" : [\"system\"]\n"
    "        }";
constexpr static const char *UID_INFO = "            \"uid\" : ";
constexpr static const char *GID_INFO = "            \"gid\" : [";
constexpr static const char *CAPS_INFO = "            \"caps\" : [";
constexpr static const char *DYNAMIC_INFO = "            \"dynamic\" : true,\n";

StartupCfgGen::StartupCfgGen(std::shared_ptr<Ast> ast) : Generator(ast)
{
    InitHostInfo();
}

void StartupCfgGen::HeaderTopOutput()
{
    ofs_ << BOOT_CONFIG_TOP;
}

void StartupCfgGen::HeaderBottomOutput()
{
    ofs_ << BOOT_CONFIG_BOTTOM;
    ofs_.close();
}

bool StartupCfgGen::Output()
{
    if (!Initialize()) {
        return false;
    }
    if (!TemplateNodeSeparate()) {
        return false;
    }
    HeaderTopOutput();

    if (!GetHostInfo()) {
        return false;
    }
    HostInfosOutput();

    HeaderBottomOutput();

    return true;
}

bool StartupCfgGen::Initialize()
{
    std::string outFileName = Option::Instance().GetOutputName();
    if (outFileName.empty()) {
        outFileName = Option::Instance().GetSourceNameBase();
    }
    outFileName = Util::File::StripSuffix(outFileName).append(".cfg");
    outFileName_ = Util::File::FileNameBase(outFileName);
    if (outFileName_.find("musl") != std::string::npos) {
        useMusl = true;
    }

    ofs_.open(outFileName, std::ofstream::out | std::ofstream::binary);
    if (!ofs_.is_open()) {
        Logger().Error() << "failed to open output file: " << outFileName;
        return false;
    }

    Logger().Debug() << "output: " << outFileName << outFileName_ << '\n';

    return true;
}

void StartupCfgGen::HostInfoOutputMusl(const std::string &name, bool end)
{
    std::string out = SERVICE_TOP;
    out.append("\"").append(name).append("\",\n");
    if (hostInfoMap_[name].dynamicLoad) {
        out.append(DYNAMIC_INFO);
    }
    out.append(PATH_INFO).append("\"").append(std::to_string(hostInfoMap_[name].hostId)).append("\"").append(", ");
    out.append("\"").append(name).append("\"").append("],\n");
    out.append(UID_INFO).append("\"").append(hostInfoMap_[name].hostUID).append("\"").append(",\n");
    out.append(GID_INFO).append("\"").append(hostInfoMap_[name].hostGID).append("\"").append("]");
    if (hostInfoMap_[name].hostCaps != "") {
        out.append(",\n");
        out.append(CAPS_INFO).append(hostInfoMap_[name].hostCaps).append("]");
    }
    out.append("\n").append(TAB).append(TAB).append("}");
    if (!end) {
        out.append(",");
    }
    ofs_ << out << '\n';
}

void StartupCfgGen::HostInfoOutput(const std::string &name, bool end)
{
    std::string out = SERVICE_TOP;
    out.append("\"").append(name).append("\",\n");
    if (hostInfoMap_[name].dynamicLoad) {
        out.append(DYNAMIC_INFO);
    }
    out.append(PATH_INFO).append("\"").append(std::to_string(hostInfoMap_[name].hostId)).append("\"").append(", ");
    out.append("\"").append(name).append("\"").append("],\n");
    out.append(UID_INFO_ROOT);
    if (!end) {
        out.append(",");
    }
    ofs_ << out << '\n';
}

void StartupCfgGen::InitHostInfo()
{
    hostData.dynamicLoad = true;
    hostData.hostCaps = "";
    hostData.hostUID = "";
    hostData.hostGID = "";
    hostData.hostPriority = 0;
    hostData.hostId = 0;
}

bool StartupCfgGen::TemplateNodeSeparate()
{
    return ast_->WalkBackward([this](std::shared_ptr<AstObject> &object, int32_t depth) {
        if (object->IsNode() && ConfigNode::CastFrom(object)->GetNodeType() == NODE_TEMPLATE) {
            object->Separate();
            return NOERR;
        }
        return NOERR;
    });
}

struct compare {
    bool operator()(const std::pair<std::string, struct HostInfo> &p1,
        const std::pair<std::string, struct HostInfo> &p2)
    {
        return (p1.second.hostPriority == p2.second.hostPriority) ?
            (p1.second.hostId < p2.second.hostId) : (p1.second.hostPriority < p2.second.hostPriority);
    }
};

void StartupCfgGen::HostInfosOutput()
{
    bool end = false;
    uint32_t cnt = 1;
    uint32_t size = hostInfoMap_.size();
    std::vector<std::pair<std::string, struct HostInfo>> vect(hostInfoMap_.begin(), hostInfoMap_.end());
    sort(vect.begin(), vect.end(), compare());
    std::vector<std::pair<std::string, struct HostInfo>>::iterator it = vect.begin();
    for (; it != vect.end(); ++it, ++cnt) {
        if (cnt == size) {
            end = true;
        }
        if (useMusl) {
            HostInfoOutputMusl(it->first, end);
        } else {
            HostInfoOutput(it->first, end);
        }
    }
}

void StartupCfgGen::GetHostCaps(const std::shared_ptr<AstObject> &capsTerm)
{
    if (capsTerm == nullptr) {
        return;
    }
    std::shared_ptr<AstObject> capsArray = capsTerm->Child();
    if (capsArray == nullptr) {
        return;
    }
    uint16_t capArraySize = ConfigArray::CastFrom(capsArray)->ArraySize();
    std::shared_ptr<AstObject> capsInfo = capsArray->Child();
    while (capArraySize && capsInfo != nullptr) {
        if (capsInfo->StringValue() != "") {
            hostData.hostCaps.append("\"").append(capsInfo->StringValue()).append("\"");
            if (capArraySize != 1) {
                hostData.hostCaps.append(", ");
            }
        }

        capsInfo = capsInfo->Next();
        capArraySize--;
    }
}

void StartupCfgGen::GetHostLoadMode(const std::shared_ptr<AstObject> &hostInfo)
{
    uint32_t preload;
    std::shared_ptr<AstObject> devInfo = nullptr;
    std::shared_ptr<AstObject> current = nullptr;
    std::shared_ptr<AstObject> devNodeInfo = nullptr;

    devInfo = hostInfo->Child();
    while (devInfo != nullptr) {
        if (!devInfo->IsNode()) {
            devInfo = devInfo->Next();
            continue;
        }
        devNodeInfo = devInfo->Child();
        while (devNodeInfo != nullptr) {
            current = devNodeInfo->Lookup("preload", PARSEROP_CONFTERM);
            if (current == nullptr) {
                devNodeInfo = devNodeInfo->Next();
                continue;
            }
            preload = current->Child()->IntegerValue();
            if (preload == 0 || preload == 1) {
                hostData.dynamicLoad = false;
            }
            devNodeInfo = devNodeInfo->Next();
        }
        devInfo = devInfo->Next();
    }
}

bool StartupCfgGen::GetHostInfo()
{
    std::shared_ptr<AstObject> deviceInfo = ast_->GetAstRoot()->Lookup("device_info", PARSEROP_CONFNODE);
    std::shared_ptr<AstObject> hostInfo = nullptr;
    std::shared_ptr<AstObject> object = nullptr;
    std::string serviceName;
    uint32_t hostId = 0;

    if (deviceInfo == nullptr) {
        Logger().Error() << "do not find device_info node";
        return false;
    }

    hostInfo = deviceInfo->Child();
    while (hostInfo != nullptr) {
        object = hostInfo->Lookup("hostName", PARSEROP_CONFTERM);
        if (object == nullptr) {
            hostInfo = hostInfo->Next();
            continue;
        }
        InitHostInfo();
        serviceName = object->Child()->StringValue();

        object = hostInfo->Lookup("priority", PARSEROP_CONFTERM);
        if (object != nullptr) {
            hostData.hostPriority = object->Child()->IntegerValue();
        }

        hostData.hostUID = serviceName;
        object = hostInfo->Lookup("uid", PARSEROP_CONFTERM);
        if (object != nullptr && object->Child()->StringValue() != "") {
            hostData.hostUID = object->Child()->StringValue();
        }

        hostData.hostGID = serviceName;
        object = hostInfo->Lookup("gid", PARSEROP_CONFTERM);
        if (object != nullptr && object->Child()->StringValue() != "") {
            hostData.hostGID = object->Child()->StringValue();
        }
        object = hostInfo->Lookup("caps", PARSEROP_CONFTERM);
        GetHostCaps(object);
        GetHostLoadMode(hostInfo);
        hostData.hostId = hostId;
        hostInfoMap_.insert(make_pair(serviceName, hostData));
        hostId++;
        hostInfo = hostInfo->Next();
    }
    return true;
}
