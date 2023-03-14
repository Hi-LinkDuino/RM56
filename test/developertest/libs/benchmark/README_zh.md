# C++ benchmark测试指导

基准测试（benchmarking）是一种测量和评估软件性能指标的活动，可以在某个时候通过基准测试建立一个已知的性能水平（称为基准线），当系统的软硬件环境发生变化之后再进行一次基准测试以确定那些变化对性能的影响。这是基准测试最常见的用途，其他用途包括测定某种负载水平下的性能极限、管理系统或环境的变化、发现可能导致性能问题的条件等。

## 用例编写

```
/*
 * Copyright (c) 2021 XXXX Device Co., Ltd.
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
```

详细内容介绍：

1. 添加测试用例文件头注释信息

   ```
   /*
    * Copyright (c) 2021 XXXX Device Co., Ltd.
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
   ```

2. 引用测试框架头文件和命名空间

   ```
   #include <benchmark/benchmark.h>
   
   using namespace std;
   ```

3. 添加测试套引用的头文件

   ```
   #include <string>
   #include <vector>
   ```

4. 测试用例实现

   ```
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
   ```

5. 向benchmark注册用例

   ```
   /* Register the function as a benchmark */
   BENCHMARK(BenchmarkTestExample);
   /* Register benchmark and explicitly set the fix iterations */
   BENCHMARK(BenchmarkTestExample)->Iterations(1000);
   ```

   注册用例同时可以指定用例的执行参数，常用的参数如下表

   | 参数                 | 说明                     |
   | -------------------- | ------------------------ |
   | Iterations           | 测试用例迭代次数         |
   | Repetitions          | 测试用例重复执行次数     |
   | ReportAggregatesOnly | 计算平均值、中值、标准差 |
   | arg                  | 向用例函数传入参数       |

   > **说明：**
   >
   > Iterations和Repetitions看似重复概念，实则存在差异，此处说明
   >
   > **Iterations**：benchmark注册的用例函数实现中均存在for循环，如demo用例BenchmarkTestExample中的for，**Iterations表示这个for循环迭代次数**
   >
   > **Repetitions**：表示注册的用例重复执行次数
   >
   > 因此若一个用例两个参数均指定，理论上用例中的函数实际执行次数为**Repetitions与Iterations的乘积**

   benchmark还支持其他多种参数，具体介绍和使用参考[benchmark](https://gitee.com/openharmony/third_party_benchmark/blob/master/README.md)

﻿
## 用例编译

```  
# Copyright (c) 2021 XXXX Device Co., Ltd.  
# Licensed under the Apache License, Version 2.0 (the "License");  
# you may not use this file except in compliance with the License.  
# You may obtain a copy of the License at  
#  
# http://www.apache.org/licenses/LICENSE-2.0  
#  
# Unless required by applicable law or agreed to in writing, software  
# distributed under the License is distributed on an "AS IS" BASIS,  
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
# See the License for the specific language governing permissions and  
# limitations under the License.  
  
import("//build/test.gni")  
  
module_output_path = "developertest/calculator"  
   
ohos_benchmarktest("BenchmarkDemoTest") {
  module_out_path = module_output_path
  sources = [ "benchmark_demo_test.cpp" ]
}

group("benchmarktest") {
  testonly = true
  deps = []

  deps += [
    # deps file
    ":BenchmarkDemoTest",
  ]
} 
```

详细内容如下：  

1. 添加文件头注释信息  

   ```  
   # Copyright (c) 2021 XXXX Device Co., Ltd.  
   # Licensed under the Apache License, Version 2.0 (the "License");  
   # you may not use this file except in compliance with the License.  
   # You may obtain a copy of the License at  
   #  
   # http://www.apache.org/licenses/LICENSE-2.0  
   #  
   # Unless required by applicable law or agreed to in writing, software  
   # distributed under the License is distributed on an "AS IS" BASIS,  
   # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
   # See the License for the specific language governing permissions and  
   # limitations under the License.    
   ```

2. 导入编译模板文件  

   ```  
   import("//build/test.gni")  
   ```

3. 指定文件输出路径  

   ```  
   module_output_path = "developertest/calculator"  
   ```

> **说明：** 此处输出路径为部件/模块名。  

4. 指定测试套名称  

   ```  
   ohos_benchmarktest("BenchmarkDemoTest") {
     module_out_path = module_output_path
     sources = [ "benchmark_demo_test.cpp" ]
   }
   ```

5. 对目标测试套进行分组  

   ```  
   group("benchmarktest") {  
     testonly = true  
     deps = []  
     
     deps += [  
   	# deps file  
   	":BenchmarkDemoTest",  
     ] 
   }    
   ```

   > **说明：** 进行条件分组的目的在于执行用例时可以选择性的执行某一种特定类型的用例。  

