/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * HDF is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 * See the LICENSE file in the root of this repository for complete details.
 */

#ifndef HC_STARTUP_CFG_GEN_H
#define HC_STARTUP_CFG_GEN_H

#include <fstream>
#include <map>
#include "generator.h"

namespace OHOS {
namespace Hardware {
struct HostInfo {
    std::string hostCaps;
    std::string hostUID;
    std::string hostGID;
    uint32_t hostPriority;
    uint32_t hostId;
    bool dynamicLoad;
};
enum EXPECT_TYPE {
    EXPECT_HOST_NAME,
    EXPECT_HOST_PRIORITY,
    EXPECT_HOST_CAPS,
    EXPECT_DEVICE_PROLOAD,
    EXPECT_TYPE_BUTT,
};
class StartupCfgGen : public Generator {
public:
    explicit StartupCfgGen(std::shared_ptr<Ast> ast);

    ~StartupCfgGen() override = default;

    bool Output() override;

private:
    bool Initialize();

    bool TemplateNodeSeparate();

    bool GetHostInfo();

    void HostInfoOutput(const std::string &name, bool end);

    void HostInfoOutputMusl(const std::string &name, bool end);

    void HeaderTopOutput();

    void HeaderBottomOutput();

    void HostInfosOutput();

    void GetHostCaps(const std::shared_ptr<AstObject> &capsTerm);

    void GetHostLoadMode(const std::shared_ptr<AstObject> &hostInfo);

    void InitHostInfo();

    std::ofstream ofs_;
    std::string outFileName_;
    bool useMusl = false;
    struct HostInfo hostData;
    std::map<std::string, struct HostInfo> hostInfoMap_;
};
} // namespace Hardware
} // namespace OHOS
#endif // HC_STARTUP_CFG_GEN_H
