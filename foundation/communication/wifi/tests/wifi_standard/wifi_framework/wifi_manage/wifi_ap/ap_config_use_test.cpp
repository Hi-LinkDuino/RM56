/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mock_wifi_settings.h"
#include "mock_wifi_ap_hal_interface.h"
#include "operator_overload.h"
#include "ap_config_use.h"

using namespace OHOS;
using ::testing::_;
using ::testing::DoAll;
using ::testing::Eq;
using ::testing::Return;
using ::testing::StrEq;
using ::testing::ext::TestSize;

namespace OHOS {
namespace Wifi {
std::vector<int> allowed5GFreq, allowed2GFreq;
std::vector<int> allowed5GChan, allowed2GChan;
std::vector<int> allowedFreqCom;
std::map<BandType, std::vector<int32_t>> ChanTbs;

class ApConfigUse_Test : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    virtual void SetUp()
    {
        allowed5GFreq.clear();
        allowed2GFreq.clear();
        allowed2GChan.clear();
        allowed5GChan.clear();
        allowedFreqCom.clear();
        ChanTbs.clear();
        const int testFreq1 = 2412;
        const int testFreq2 = 2417;
        const int testFreq3 = 2472;
        const int testFreq4 = 2484;
        const int testFreq5 = 5170;
        const int testFreq6 = 5745;
        const int testFreq7 = 5825;

        allowed2GFreq.push_back(testFreq1);
        allowed2GFreq.push_back(testFreq2);
        allowed2GFreq.push_back(testFreq3);
        allowed2GFreq.push_back(testFreq4);
        allowed5GFreq.push_back(testFreq5);
        allowed5GFreq.push_back(testFreq6);
        allowed5GFreq.push_back(testFreq7);

        const int testChannel1 = 1;
        const int testChannel2 = 2;
        const int testChannel3 = 13;
        const int testChannel4 = 14;

        const int testChannel5 = 34;
        const int testChannel6 = 149;
        const int testChannel7 = 14;

        allowed2GChan.push_back(testChannel1);
        allowed2GChan.push_back(testChannel2);
        allowed2GChan.push_back(testChannel3);
        allowed2GChan.push_back(testChannel4);

        allowed5GChan.push_back(testChannel5);
        allowed5GChan.push_back(testChannel6);
        allowed5GChan.push_back(testChannel7);

        ChanTbs[BandType::BAND_2GHZ] = allowed2GChan;
        ChanTbs[BandType::BAND_5GHZ] = allowed5GChan;

        pApConfigUse = new ApConfigUse();
    }
    virtual void TearDown()
    {
        allowed5GFreq.clear();
        allowed2GFreq.clear();
        allowed2GChan.clear();
        allowed5GChan.clear();
        allowedFreqCom.clear();
        ChanTbs.clear();
        delete pApConfigUse;
        pApConfigUse = nullptr;
    }

public:
    ApConfigUse *pApConfigUse;
};

/* TransformFrequencyIntoChannel */
HWTEST_F(ApConfigUse_Test, TransformFrequencyIntoChannel, TestSize.Level1)
{
    const int testFreq1 = 2412;
    const int testFreq2 = 2417;
    const int testFreq13 = 2472;
    const int testFreq14 = 2484;

    const int testFreq01 = 2411;
    const int testFreq02 = 2485;
    const int testFreq03 = 2473;
    const int testFreq04 = 5826;
    const int testFreq05 = 5169;

    const int testChannel34 = 5170;
    const int testChannel149 = 5745;
    const int testChannel165 = 5825;

    /* 2.4G success */
    EXPECT_EQ(1, pApConfigUse->TransformFrequencyIntoChannel(testFreq1));
    EXPECT_EQ(2, pApConfigUse->TransformFrequencyIntoChannel(testFreq2));
    EXPECT_EQ(13, pApConfigUse->TransformFrequencyIntoChannel(testFreq13));
    EXPECT_EQ(14, pApConfigUse->TransformFrequencyIntoChannel(testFreq14));
    /* 5G  success */
    EXPECT_EQ(34, pApConfigUse->TransformFrequencyIntoChannel(testChannel34));
    EXPECT_EQ(149, pApConfigUse->TransformFrequencyIntoChannel(testChannel149));
    EXPECT_EQ(165, pApConfigUse->TransformFrequencyIntoChannel(testChannel165));
    /* 2.4G  failed */
    EXPECT_EQ(-1, pApConfigUse->TransformFrequencyIntoChannel(testFreq01));
    EXPECT_EQ(-1, pApConfigUse->TransformFrequencyIntoChannel(testFreq02));
    EXPECT_EQ(-1, pApConfigUse->TransformFrequencyIntoChannel(testFreq03));
    /* 5G   failed */
    EXPECT_EQ(-1, pApConfigUse->TransformFrequencyIntoChannel(testFreq04));
    EXPECT_EQ(-1, pApConfigUse->TransformFrequencyIntoChannel(testFreq05));
}
/* TransformFrequencyIntoChannel_overload */
HWTEST_F(ApConfigUse_Test, TransformFrequencyIntoChannel_1, TestSize.Level1)
{
    const int testFreq1 = 2412;
    const int testFreq2 = 2417;
    const int testFreq3 = 2472;
    const int testFreq4 = 2484;
    const int testFreq5 = 5170;
    const int testFreq6 = 5745;
    const int testFreq7 = 5825;
    const int testFreq = -1;
    std::vector<int> FreqVector;
    FreqVector.push_back(testFreq1);
    FreqVector.push_back(testFreq2);
    FreqVector.push_back(testFreq3);
    FreqVector.push_back(testFreq4);
    FreqVector.push_back(testFreq5);
    FreqVector.push_back(testFreq6);
    FreqVector.push_back(testFreq7);
    FreqVector.push_back(testFreq);
    std::vector<int> FreqVector1 = FreqVector;

    int buf[] = {1, 2, 13, 14, 34, 149, 165};
    std::vector<int> ChanVector;
    pApConfigUse->TransformFrequencyIntoChannel(FreqVector, ChanVector);
    for (unsigned long i = 0; i < (sizeof(buf) / 4); ++i) {
        EXPECT_EQ(buf[i], ChanVector[i]);
    }
    EXPECT_EQ(FreqVector1, FreqVector);
}
/* SetConfig */
HWTEST_F(ApConfigUse_Test, LogConfig_SUCCESS, TestSize.Level1)
{
    HotspotConfig apConfig;
    apConfig.SetBand(BandType::BAND_2GHZ);
    HotspotConfig apConfig1 = apConfig;
    pApConfigUse->LogConfig(apConfig);
    EXPECT_EQ(apConfig1, apConfig);
}

/* IsValid24GHz */
HWTEST_F(ApConfigUse_Test, IsValid24GHz, TestSize.Level1)
{
    EXPECT_FALSE(pApConfigUse->IsValid24GHz(2400));
    EXPECT_FALSE(pApConfigUse->IsValid24GHz(2500));
    EXPECT_TRUE(pApConfigUse->IsValid24GHz(2412));
    EXPECT_TRUE(pApConfigUse->IsValid24GHz(2484));
    EXPECT_FALSE(pApConfigUse->IsValid24GHz(2499));
}
/* IsValid5GHz */
HWTEST_F(ApConfigUse_Test, IsValid5GHz, TestSize.Level1)
{
    EXPECT_FALSE(pApConfigUse->IsValid5GHz(4900));
    EXPECT_FALSE(pApConfigUse->IsValid5GHz(5169));
    EXPECT_TRUE(pApConfigUse->IsValid5GHz(5170));
    EXPECT_TRUE(pApConfigUse->IsValid5GHz(5825));
    EXPECT_FALSE(pApConfigUse->IsValid5GHz(5827));
}

/* CheckBandChannel */
HWTEST_F(ApConfigUse_Test, CheckBandChannel_1, TestSize.Level1)
{
    HotspotConfig apConfig;
    apConfig.SetBand(BandType::BAND_2GHZ);
    apConfig.SetChannel(2);
    HotspotConfig apConfig1 = apConfig;
    std::vector<int32_t> band_2G_channel = { 1, 2, 3, 4, 5, 6, 7 };
    std::vector<int32_t> band_5G_channel = { 149, 168, 169 };
    ChannelsTable ChannelsTb = { { BandType::BAND_2GHZ, band_2G_channel }, { BandType::BAND_5GHZ, band_5G_channel } };

    pApConfigUse->CheckBandChannel(apConfig, ChannelsTb);
    EXPECT_EQ(apConfig1, apConfig);
}
HWTEST_F(ApConfigUse_Test, CheckBandChannel_2, TestSize.Level1)
{
    HotspotConfig apConfig;
    apConfig.SetBand(BandType::BAND_2GHZ);
    apConfig.SetChannel(9);
    std::vector<int32_t> band_2G_channel = { 1, 2, 3, 4, 5, 6, 7 };
    std::vector<int32_t> band_5G_channel = { 149, 168, 169 };
    ChannelsTable ChannelsTb = { { BandType::BAND_2GHZ, band_2G_channel }, { BandType::BAND_5GHZ, band_5G_channel } };
    pApConfigUse->CheckBandChannel(apConfig, ChannelsTb);
    EXPECT_EQ(apConfig.GetChannel(), 6);
    EXPECT_EQ(apConfig.GetBand(), BandType::BAND_2GHZ);
}
HWTEST_F(ApConfigUse_Test, CheckBandChannel_3, TestSize.Level1)
{
    HotspotConfig apConfig;
    ChannelsTable ChannelsTb;
    pApConfigUse->CheckBandChannel(apConfig, ChannelsTb);
    EXPECT_EQ(apConfig.GetChannel(), 6);
    EXPECT_EQ(apConfig.GetBand(), BandType::BAND_2GHZ);
}
} // namespace Wifi
} // namespace OHOS