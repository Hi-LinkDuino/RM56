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
#include "sorted_vector.h"

#include <gtest/gtest.h>
#include <iostream>

using namespace testing::ext;
using namespace OHOS;
using namespace std;

class UtilsSortedVector : public testing::Test {
};

HWTEST_F(UtilsSortedVector, testDefaultConsAndAddAndSort, TestSize.Level0)
{
    SortedVector<int> svec;
    std::vector<int> vec;

    for (int i = 0; i < 10; i++) {
        vec.push_back(i);
    }

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    auto it = svec.Begin();
    auto it2 = vec.begin();

    for (; (it != svec.End()) and (it2 != vec.end()); it2++, it++) {
        ASSERT_EQ(*it, *it2);
    }
}

HWTEST_F(UtilsSortedVector, testConsFromSortedAllowDup, TestSize.Level0)
{
    SortedVector<int> svec;
    std::vector<int> vec;

    for (int i = 0; i < 10; i++) {
        vec.push_back(i);
    }

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    SortedVector<int> newSvec(svec);

    auto it = newSvec.Begin();
    auto it2 = vec.begin();

    for (; (it != newSvec.End()) and (it2 != vec.end()); it2++, it++) {
        ASSERT_EQ(*it, *it2);
    }
}

HWTEST_F(UtilsSortedVector, testConsFromSortedNotAllowDuplicate, TestSize.Level0)
{
    SortedVector<int> svec;
    std::vector<int> vec;

    for (int i = 0; i < 20; i++) {
        vec.push_back(i);
    }

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    for (int i = 0; i < 20; i++) {
        svec.Add(i);
    }

    ASSERT_EQ(static_cast<size_t>(30), svec.Size());

    SortedVector<int, false> newSvec(svec);

    ASSERT_EQ(static_cast<size_t>(20), newSvec.Size());

    for (int i = 0; i < 20; i++) {
        ASSERT_EQ(vec[i], newSvec[i]);
    }
}

HWTEST_F(UtilsSortedVector, testConsFromSortedNotAllowToAlloworNotAllow, TestSize.Level0)
{
    SortedVector<int, false> svec;
    std::vector<int> vec;

    for (int i = 0; i < 20; i++) {
        vec.push_back(i);
    }

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    for (int i = 0; i < 20; i++) {
        svec.Add(i);
    }

    ASSERT_EQ(static_cast<size_t>(20), svec.Size());

    SortedVector<int> newSvecTrue(svec);

    ASSERT_EQ(static_cast<size_t>(20), newSvecTrue.Size());

    for (int i = 0; i < 20; i++) {
        ASSERT_EQ(vec[i], newSvecTrue[i]);
    }

    SortedVector<int> newSvecFalse(svec);
    ASSERT_EQ(static_cast<size_t>(20), newSvecFalse.Size());

    for (int i = 0; i < 20; i++) {
        ASSERT_EQ(vec[i], newSvecFalse[i]);
    }
}

HWTEST_F(UtilsSortedVector, testoperatoreq, TestSize.Level0)
{
    SortedVector<int> svec;
    std::vector<int> vec;

    for (int i = 0; i < 10; i++) {
        vec.push_back(i);
    }

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    SortedVector<int> newSvec = svec;

    auto it = newSvec.Begin();
    auto it2 = svec.Begin();

    for (; (it != newSvec.End()) and (it2 != svec.End()); it2++, it++) {
        ASSERT_EQ(*it, *it2);
    }
}

HWTEST_F(UtilsSortedVector, testOperatorEqAllowToNotAllow, TestSize.Level0)
{
    SortedVector<int> svec;
    std::vector<int> vec;

    for (int i = 0; i < 20; i++) {
        vec.push_back(i);
    }

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    for (int i = 0; i < 20; i++) {
        svec.Add(i);
    }

    ASSERT_EQ(static_cast<size_t>(30), svec.Size());
    SortedVector<int, false> newSvec = svec;

    ASSERT_EQ(static_cast<size_t>(20), newSvec.Size());

    for (int i = 0; i < 20; i++) {
        ASSERT_EQ(vec[i], newSvec[i]);
    }
}

HWTEST_F(UtilsSortedVector, testOperatorEqNotAllowToAllowOrNotAllow, TestSize.Level0)
{
    SortedVector<int, false> svec;
    std::vector<int> vec;

    for (int i = 0; i < 20; i++) {
        vec.push_back(i);
    }

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    for (int i = 0; i < 20; i++) {
        svec.Add(i);
    }

    ASSERT_EQ(static_cast<size_t>(20), svec.Size());
    SortedVector<int, false> newSvecFalse = svec;
    SortedVector<int, true> newSvecTrue = svec;
    ASSERT_EQ(static_cast<size_t>(20), newSvecFalse.Size());
    ASSERT_EQ(static_cast<size_t>(20), newSvecTrue.Size());
    for (int i = 0; i < 20; i++) {
        ASSERT_EQ(vec[i], newSvecFalse[i]);
        ASSERT_EQ(vec[i], newSvecTrue[i]);
    }
}

HWTEST_F(UtilsSortedVector, testOperatorEqAssignmentTwice, TestSize.Level0)
{
    SortedVector<int, true> svec;

    std::vector<int> vec;
    for (int i = 20; i < 30; i++) {
        vec.push_back(i);
    }

    for (int i = 0; i < 20; i++) {
        svec.Add(i);
    }

    SortedVector<int, false> newSvecFalse;

    for (int i = 20; i < 30; i++) {
        newSvecFalse.Add(i);
    }

    svec = newSvecFalse;

    ASSERT_EQ(static_cast<size_t>(10), svec.Size());
    for (int i = 0; i < 10; i++) {
        ASSERT_EQ(vec[i], svec[i]);
    }
}

HWTEST_F(UtilsSortedVector, testoperatorconsteq, TestSize.Level0)
{
    SortedVector<int> svec;
    std::vector<int> vec;

    for (int i = 0; i < 10; i++) {
        vec.push_back(i);
    }

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    const SortedVector<int> newSvec = svec;

    auto it = newSvec.Begin();
    auto it2 = svec.Begin();

    for (; (it != newSvec.End()) and (it2 != svec.End()); it2++, it++) {
        ASSERT_EQ(*it, *it2);
    }
}

HWTEST_F(UtilsSortedVector, testsizeclearIsEmpty, TestSize.Level0)
{
    SortedVector<int> svec;

    ASSERT_TRUE(svec.IsEmpty());
    ASSERT_EQ(svec.Size(), static_cast<size_t>(0));

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    ASSERT_EQ(svec.Size(), static_cast<const unsigned long>(10));

    ASSERT_FALSE(svec.IsEmpty());

    svec.Clear();

    ASSERT_TRUE(svec.IsEmpty());
    ASSERT_EQ(svec.Size(), static_cast<size_t>(0));
}

HWTEST_F(UtilsSortedVector, testCapasityandSetcapasity, TestSize.Level0)
{
    SortedVector<int> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    svec.SetCapcity(1000);

    ASSERT_EQ(svec.Capacity(), static_cast<size_t>(1000));

    ASSERT_LT(svec.SetCapcity(500), static_cast<ssize_t>(0));
    ASSERT_EQ(svec.Capacity(), static_cast<size_t>(1000));
}

HWTEST_F(UtilsSortedVector, testconstArray, TestSize.Level0)
{
    SortedVector<int> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    const int* pi = svec.Array();
    int arraySize = svec.Size();

    auto it = svec.Begin();
    int i = 0;
    for (; (i < arraySize) && (it != svec.End()); ++it, ++i) {
        ASSERT_EQ(pi[i], *it);
    }

    ASSERT_EQ(i, arraySize);
    ASSERT_EQ(it, svec.End());
}

HWTEST_F(UtilsSortedVector, testeditArray, TestSize.Level0)
{
    SortedVector<int> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    int* pi = svec.EditArray();
    int arraySize = svec.Size();

    // compare and equal
    auto it = svec.Begin();
    int i = 0;
    for (; (i < arraySize) && (it != svec.End()); ++it, ++i) {
        ASSERT_EQ(pi[i], *it);
    }

    SortedVector<int> copyvec;

    //  size equal
    ASSERT_EQ(i, arraySize);
    ASSERT_EQ(it, svec.End());

    //  fix value

    for (int t = 0; t < arraySize; ++t) {
        pi[t] += 1;
    }

    // compare  and equal add 1

    it = copyvec.Begin();
    i = 0;
    for (; (i < arraySize) && (it != copyvec.End()); ++it, ++i) {
        ASSERT_EQ(pi[i], *it + 1);
    }

    // all renewed
    it = svec.Begin();
    i = 0;
    for (; (i < arraySize) && (it != svec.End()); ++it, ++i) {
        ASSERT_EQ(pi[i], *it);
    }
}

HWTEST_F(UtilsSortedVector, testIndexOf, TestSize.Level0)
{
    SortedVector<int> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    int arraySize = svec.Size();

    auto it = svec.Begin();
    int i = 0;
    for (; (i < arraySize) && (it != svec.End()); ++it, ++i) {
        ASSERT_EQ(i, svec.IndexOf(i));
    }

    ASSERT_EQ(-1, svec.IndexOf(10));
}

HWTEST_F(UtilsSortedVector, testOrderof, TestSize.Level0)
{
    SortedVector<int> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    size_t count = svec.Size();
    size_t order = 0;
    for (size_t i = 0; i < count; i++) {
        order = svec.OrderOf(i);
        ASSERT_EQ(i + 1, order);
    }

    ASSERT_EQ(static_cast<size_t>(0), svec.OrderOf(-2));
    ASSERT_EQ(static_cast<size_t>(0), svec.OrderOf(-1));
    ASSERT_EQ(static_cast<size_t>(10), svec.OrderOf(9));
    ASSERT_EQ(static_cast<size_t>(10), svec.OrderOf(10));
}

HWTEST_F(UtilsSortedVector, testoperatorAccess, TestSize.Level0)
{
    SortedVector<int> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    for (int i = 0; i <= 9; i++) {
        ASSERT_EQ(i, svec[i]);
    }
}

HWTEST_F(UtilsSortedVector, testBack, TestSize.Level0)
{
    SortedVector<int> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    ASSERT_EQ(9, svec.Back());
}

HWTEST_F(UtilsSortedVector, testMirrorItemAt, TestSize.Level0)
{
    SortedVector<int> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    for (ssize_t i = -1; i >= -10; i--) {
        ASSERT_EQ(i + 10, svec.MirrorItemAt(i));
    }

    for (ssize_t i = 0; i <= 9; i++) {
        ASSERT_EQ(i, svec.MirrorItemAt(i));
    }
}

HWTEST_F(UtilsSortedVector, testEditItemAt, TestSize.Level0)
{
    SortedVector<int> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    for (ssize_t i = 0; i <= 9; i++) {
        svec.EditItemAt(i) += 1;
        ASSERT_EQ(i + 1, svec.EditItemAt(i));
    }
}

HWTEST_F(UtilsSortedVector, testCopyCtorFromVector, TestSize.Level0)
{
    std::vector<int> vec;

    for (int i = 9; i >= 0; i--) {
        vec.push_back(i);
    }

    SortedVector<int> svec(vec);

    for (ssize_t i = 0; i <= 9; i++) {
        ASSERT_EQ(i, svec[i]);
    }
}

HWTEST_F(UtilsSortedVector, testConsFromVectorToNotAllowDuplicate, TestSize.Level0)
{
    std::vector<int> vec;

    for (int i = 9; i >= 0; i--) {
        vec.push_back(i);
    }

    for (int i = 19; i >= 0; i--) {
        vec.push_back(i);
    }

    ASSERT_EQ(static_cast<size_t>(30), vec.size());
    SortedVector<int, false> svec(vec);

    ASSERT_EQ(static_cast<size_t>(20), svec.Size());

    for (ssize_t i = 0; i <= 19; i++) {
        ASSERT_EQ(i, svec[i]);
    }
}

HWTEST_F(UtilsSortedVector, testMergevector, TestSize.Level0)
{
    SortedVector<int> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    std::vector<int> vec;
    for (int i = 10; i < 20; i++) {
        vec.push_back(i);
    }

    svec.Merge(vec);

    for (ssize_t i = 0; i < 20; i++) {
        ASSERT_EQ(i, svec[i]);
    }
}

HWTEST_F(UtilsSortedVector, testMergevectorNoduplicate, TestSize.Level0)
{

    SortedVector<int, false> svec;

    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
    }

    std::vector<int> vec;
    for (int i = 0; i < 20; i++) {
        vec.push_back(i);
    }

    for (int i = 10; i < 30; i++) {
        vec.push_back(i);
    }

    svec.Merge(vec);
    ASSERT_EQ(svec.Size(), static_cast<size_t>(30));

    for (ssize_t i = 0; i < 30; i++) {
        ASSERT_EQ(i, svec[i]);
    }
}

HWTEST_F(UtilsSortedVector, testMergesortedvectorNoduplicate, TestSize.Level0)
{
    SortedVector<int, false> svec;
    SortedVector<int, false> svec2;
    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
        svec2.Add(i);
    }

    svec.Merge(svec2);
    ASSERT_EQ(static_cast<size_t>(10), svec.Size());
    // 0,1,2,3,4,5,6,7,8,9
    for (ssize_t i = 0; i < 10; i++) {
        ASSERT_EQ(i, svec[i]);
    }
}

HWTEST_F(UtilsSortedVector, testMergesortedvector, TestSize.Level0)
{
    SortedVector<int> svec;
    SortedVector<int> svec2;
    for (int i = 9; i >= 0; i--) {
        svec.Add(i);
        svec2.Add(i);
    }

    svec.Merge(svec2);

    // 0,0,1,1,2,2,3,3...
    for (ssize_t i = 0; i < 20; i++) {
        ASSERT_EQ(i / 2, svec[i]);
    }
}

HWTEST_F(UtilsSortedVector, testAddNotAllowDuplicate, TestSize.Level0)
{
    SortedVector<int, false> svec;

    for (int i = 9; i >= 0; i--) {

        ASSERT_NE(svec.Add(i), static_cast<ssize_t>(-1));
    }
    ASSERT_EQ(static_cast<size_t>(10), svec.Size());

    // duplicate
    for (int i = 9; i >= 0; i--) {
        ASSERT_EQ(svec.Add(i), static_cast<ssize_t>(-1));
        ASSERT_EQ(static_cast<size_t>(10), svec.Size());
    }

    for (ssize_t i = 0; i < 10; i++) {
        ASSERT_EQ(i, svec[i]);
    }
}
