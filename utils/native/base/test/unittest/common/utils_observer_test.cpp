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
#include <gtest/gtest.h>
#include "observer.h"
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace testing::ext;
using namespace OHOS;
using namespace std;


class BookList: public Observable {
public:
    BookList() { books_.clear(); }
    void AddBook(const string& book)
    {
        books_.insert(book);
        SetChanged();
        NotifyObservers();
    }

    void RemoveBook(const string& book)
    {
        books_.erase(book);
        SetChanged();
        NotifyObservers();
    }

    const set<string>& GetBooks() { return books_; }
private:
    set<string> books_;

};

class BookObserver: public Observer {
public:
    virtual void Update(const Observable* o, const ObserverArg* /* arg */)
    {
        BookList* bookList = reinterpret_cast<BookList*>(const_cast<Observable*>(o));
        books_ = bookList->GetBooks();
    }

    int GetBooksCount() { return static_cast<int>(books_.size()); }
    bool BookExists(const string& book) { return books_.count(book) > 0;}
private:
    set<string> books_;
};


class UtilsObserverTest : public testing::Test {
public :
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void UtilsObserverTest::SetUpTestCase(void)
{
}

void UtilsObserverTest::TearDownTestCase(void)
{
}

void UtilsObserverTest::SetUp(void)
{
}

void UtilsObserverTest::TearDown(void)
{
}


HWTEST_F(UtilsObserverTest, test_ObserverNotify, TestSize.Level0)
{
    BookList bookList;
    shared_ptr<BookObserver> bookObserver1 = make_shared<BookObserver>();
    shared_ptr<BookObserver> bookObserver2 = make_shared<BookObserver>();
    shared_ptr<BookObserver> bookObserver3 = make_shared<BookObserver>();

    bookList.AddObserver(bookObserver1);
    bookList.AddObserver(bookObserver2);
    bookList.AddObserver(bookObserver3);
    bookList.AddBook("book1");

    EXPECT_EQ(bookObserver1->GetBooksCount(), 1);
    EXPECT_EQ(bookObserver2->GetBooksCount(), 1);
    EXPECT_EQ(bookObserver3->GetBooksCount(), 1);

    bookList.RemoveObserver(bookObserver1);
    bookList.RemoveBook("book1");
    EXPECT_EQ(bookObserver1->GetBooksCount(), 1);
    EXPECT_EQ(bookObserver2->GetBooksCount(), 0);
    EXPECT_EQ(bookObserver3->GetBooksCount(), 0);

    bookList.RemoveObserver(bookObserver2);
    bookList.AddBook("book2");
    bookList.AddBook("book3");
    EXPECT_EQ(bookObserver1->GetBooksCount(), 1);
    EXPECT_EQ(bookObserver2->GetBooksCount(), 0);
    EXPECT_EQ(bookObserver3->GetBooksCount(), 2);

}


HWTEST_F(UtilsObserverTest, test_RemoveAllObserver, TestSize.Level0)
{
    BookList bookList;
    shared_ptr<BookObserver> bookObserver1 = make_shared<BookObserver>();
    shared_ptr<BookObserver> bookObserver2 = make_shared<BookObserver>();
    shared_ptr<BookObserver> bookObserver3 = make_shared<BookObserver>();

    bookList.AddObserver(bookObserver1);
    bookList.AddObserver(bookObserver2);
    bookList.AddObserver(bookObserver3);
    bookList.AddBook("book1");

    EXPECT_EQ(bookObserver1->GetBooksCount(), 1);
    EXPECT_EQ(bookObserver2->GetBooksCount(), 1);
    EXPECT_EQ(bookObserver3->GetBooksCount(), 1);

    bookList.RemoveAllObservers();
    bookList.RemoveBook("book1");
    EXPECT_EQ(bookObserver1->GetBooksCount(), 1);
    EXPECT_EQ(bookObserver2->GetBooksCount(), 1);
    EXPECT_EQ(bookObserver3->GetBooksCount(), 1);
    EXPECT_EQ(bookList.GetObserversCount(), 0);
}

