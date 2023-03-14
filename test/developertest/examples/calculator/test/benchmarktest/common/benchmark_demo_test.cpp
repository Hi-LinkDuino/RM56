/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <benchmark/benchmark.h>
#include <string>
#include <vector>

using namespace std;

namespace {
    /**
     * @tc.name: BenchmarkTestExample
     * @tc.desc: Testcase for testing 'SimpleExample' function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    size_t SimpleExample()
    {
        string str = "benchmark test";
        return str.size();
    }

    static void BenchmarkTestExample(benchmark::State &state)
    {
        for (auto _ : state) {
            /* @tc.steps: step1.call SimpleExample in loop */
            SimpleExample();
        }
    }
    
    /* Register the function as a benchmark */
    BENCHMARK(BenchmarkTestExample);
    /* Register benchmark and explicitly set the fix iterations */
    BENCHMARK(BenchmarkTestExample)->Iterations(1000);

    /**
     * @tc.name: BenchmarkTestVectorOperator
     * @tc.desc: Testcase for testing "AccessVectorElementByOperator"
     *           function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    void AccessVectorElementByOperator()
    {
        constexpr int testLen = 5;
        std::vector<int> testVec(testLen, 0);
        for (int i = 0; i < testLen; i++) {
            testVec[i] = i * i;
        }
    }

    static void BenchmarkTestVectorOperator(benchmark::State &state)
    {
        for (auto _ : state) {
            /* @tc.steps: step1.call AccessVectorElementByOperator in loop */
            AccessVectorElementByOperator();
        }
    }

    /*
     * Register the function as a benchmark, set iterations repetitions.
     * And set "ReportAggregatesOnly", it will display the statistics Mean,
     * Median and Standard Deviation of Repeated Benchmarks.
     */
    BENCHMARK(BenchmarkTestVectorOperator)->Iterations(1000)->Repetitions(3)->
        ReportAggregatesOnly();

    /**
     * @tc.name: BenchmarkTestVectorAt
     * @tc.desc: Testcase for testing "AccessVectorElementByAt"
     *           function.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    void AccessVectorElementByAt()
    {
        constexpr int testLen = 5;
        std::vector<int> testVec(testLen, 0);
        for (int i = 0; i < testLen; i++) {
            testVec.at(i) = i * i;
        }
    }

    static void BenchmarkTestVectorAt(benchmark::State &state)
    {
        for (auto _ : state) {
            /* @tc.steps: step1.call AccessVectorElementByAt in loop */
            AccessVectorElementByAt();
        }
    }

    BENCHMARK(BenchmarkTestVectorAt)->Iterations(1000)->Repetitions(3)->
        ReportAggregatesOnly();

    /**
     * @tc.name: CalculatedAreaTestCase
     * @tc.desc: Define a testcase that accesses a class member
     *           variable.
     * @tc.type: FUNC
     * @tc.require: Issue Number
     */
    class BenchmarkDemoTest : public benchmark::Fixture {
    public:
        void SetUp(const ::benchmark::State &state)
        {
            /* @tc.setup: width and height assigned */
            phoneWidth_ = 1080;  /* 1080 is default width */
            phoneHeight_ = 2244; /* 2244 is default height */
        }

        void TearDown(const ::benchmark::State &state)
        {
        }
        
        int phoneWidth_;
        int phoneHeight_;
    };

    BENCHMARK_F(BenchmarkDemoTest, CalculatedAreaTestCase)(
        benchmark::State &st)
    {
        long int area = 0;
        for (auto _ : st) {
            /* @tc.steps: step1.calculate area */
            area = phoneWidth_ * phoneHeight_;
        }
    }
    
    BENCHMARK_REGISTER_F(BenchmarkDemoTest, CalculatedAreaTestCase);
}

// Run the benchmark
BENCHMARK_MAIN();
