#include <list>
#include <string>
#include "icb/linkedlist.h"

#include "common_test_setup.h"

class ICBLinkedListIntFixture : public ICBTestFixture {
protected:
    icb::LinkedList<int> list;
};

class ICBLinkedListEntityFixture : public ICBTestFixture {
protected:
    icb::LinkedList<Entity> list;
};

class ICBLinkedListStringFixture : public ICBTestFixture {
protected:
    icb::LinkedList<std::string> list;
};

class ICBLinkedListCharFixture : public ICBTestFixture {
protected:
    icb::LinkedList<char> list;
};

class ICBLinkedListDoubleFixture : public ICBTestFixture {
protected:
    icb::LinkedList<double> list;
};

// ---

TEST_F(ICBLinkedListIntFixture, Clear)
{
    list = {1, 2, 3};

    list.Clear();

    EXPECT_TRUE(list.Empty());
    EXPECT_EQ(list.Size(), 0);

    EXPECT_EQ(*list.begin(), 0);
    EXPECT_EQ(*(--list.end()), 0);

    EXPECT_DEBUG_DEATH(list.Front(), "");
    EXPECT_DEBUG_DEATH(list.Back(), "");
    //EXPECT_DEBUG_DEATH(list.PopBack(), "");
    //EXPECT_DEBUG_DEATH(list.PopFront(), "");

    
}

TEST_F(ICBLinkedListIntFixture, EmptyList)
{
    EXPECT_TRUE(list.Empty());
    EXPECT_EQ(list.Size(), 0);
}

TEST_F(ICBLinkedListIntFixture, PushFront)
{
    list.PushFront(5);
    EXPECT_FALSE(list.Empty());
    EXPECT_EQ(list.Size(), 1);
    EXPECT_EQ(list.Front(), 5);
}

TEST_F(ICBLinkedListStringFixture, PushBack)
{
    list.PushBack("Hello");
    EXPECT_FALSE(list.Empty());
    EXPECT_EQ(list.Size(), 1);
    EXPECT_EQ(list.Back(), "Hello");
}

TEST_F(ICBLinkedListIntFixture, PopFront)
{
    list.PushFront(5);
    list.PushFront(10);
    EXPECT_EQ(list.Size(), 2);
    list.PopFront();
    EXPECT_EQ(list.Size(), 1);
    EXPECT_EQ(list.Front(), 5);
}

TEST_F(ICBLinkedListCharFixture, PopBack)
{
    list.PushBack('a');
    list.PushBack('b');
    EXPECT_EQ(list.Size(), 2);
    list.PopBack();
    EXPECT_EQ(list.Size(), 1);
    EXPECT_EQ(list.Back(), 'a');
}

TEST_F(ICBLinkedListIntFixture, CopyConstructor)
{
    for (int i = 0; i < 3; ++i)
    {
        list.PushBack(i);
    }

    icb::LinkedList<int> list2(list);
    EXPECT_EQ(list.Size(), list2.Size());

    int expected[] = {0, 1, 2};
    int i = 0;
    for (auto it = list2.begin(); it != list2.end(); ++it)
    {
        EXPECT_EQ(*it, expected[i++]);
    }

    // Modify list and verify changes aren't reflected in list2
    list.PopFront();
    EXPECT_NE(list.Size(), list2.Size());
}

TEST_F(ICBLinkedListDoubleFixture, CopyAssignment)
{
    for (int i = 0; i < 2; ++i)
    {
        list.PushBack(i * 0.5);
    }

    icb::LinkedList<double> list2;
    list2 = list;
    EXPECT_EQ(list.Size(), list2.Size());

    double expected[] = {0.0, 0.5};
    int i = 0;
    for (auto it = list2.begin(); it != list2.end(); ++it)
    {
        EXPECT_EQ(*it, expected[i++]);
    }

    // Modify list and verify changes are not reflected in list2
    list.PopBack();
    list.PopBack();
    list.PushBack(2.0);
    EXPECT_NE(list.Size(), list2.Size());
    EXPECT_EQ(list2.Back(), 0.5);
}

TEST_F(ICBLinkedListIntFixture, MoveConstructor)
{
    for (int i = 0; i < 3; ++i)
    {
        list.PushBack(i);
    }

    // Move constructor takes the elements from list by value (rvalue reference)
    icb::LinkedList<int> list2(std::move(list));

    // Verify list is now empty (moved elements)
    EXPECT_TRUE(list.Empty());
    EXPECT_EQ(list.Size(), 0);

    // Verify list2 has the elements and size
    EXPECT_EQ(list2.Size(), 3);
    int expected[] = {0, 1, 2};
    int i = 0;
    for (auto it = list2.begin(); it != list2.end(); ++it)
    {
        EXPECT_EQ(*it, expected[i++]);
    }
}

TEST_F(ICBLinkedListStringFixture, MoveAssignment)
{
    icb::LinkedList<std::string> list;
    list.PushBack("apple");
    list.PushBack("banana");

    icb::LinkedList<std::string> list2;
    list2.PushBack("orange");

    // Move assignment operator takes the elements from list by value (rvalue reference)
    list2 = std::move(list);

    // Verify list is now empty (moved elements)
    EXPECT_TRUE(list.Empty());
    EXPECT_EQ(list.Size(), 0);

    // Verify list2 has the elements from list (replaced)
    EXPECT_EQ(list2.Size(), 2);
    std::string expected[] = {"apple", "banana"};
    int i = 0;
    for (const auto &it : list2)
    {
        EXPECT_EQ(it, expected[i++]);
    }
}

// iterator

TEST(ICBLinkedList, IteratorConcept)
{
    EXPECT_TRUE(std::bidirectional_iterator<icb::LinkedList<int>::Iterator>);
    EXPECT_TRUE(std::bidirectional_iterator<icb::LinkedList<int>::ConstIterator>);
    static_assert(std::bidirectional_iterator<icb::LinkedList<int>::Iterator>);
}

TEST_F(ICBLinkedListIntFixture, Iterator)
{
    list.PushBack(1);
    list.PushBack(2);
    list.PushBack(3);

    int expected[] = {1, 2, 3};
    int i = 0;
    for (auto it = list.begin(); it != list.end(); ++it)
    {
        EXPECT_EQ(*it, expected[i++]);
    }
}

TEST_F(ICBLinkedListStringFixture, ConstIterator)
{
    list.PushBack("apple");
    list.PushBack("banana");

    std::string expected[] = {"apple", "banana"};
    int i = 0;
    for (const auto &it : list)
    {
        EXPECT_EQ(it, expected[i++]);
    }
}
