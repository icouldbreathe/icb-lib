#include "icb/AVLSet.h"

#include "common_test_setup.h"

class ICBAVLSetEmptyFixture : public ICBTestFixture
{
  protected:
    icb::AVLSet<int> tree;
};

class ICBAVLSetNonEmptyFixture : public ICBAVLSetEmptyFixture
{
  protected:
    void SetUp() override
    {
        tree.Insert(5);
        tree.Insert(3);
        tree.Insert(8);
        tree.Insert(1);
        tree.Insert(4);
    }
};

// Empty tree tests
TEST_F(ICBAVLSetEmptyFixture, EmptyContains)
{
    EXPECT_FALSE(tree.Contains(5));
}

TEST_F(ICBAVLSetEmptyFixture, InsertSingle)
{
    tree.Insert(7);
    EXPECT_TRUE(tree.Contains(7));
    EXPECT_FALSE(tree.Contains(12));
}

TEST_F(ICBAVLSetEmptyFixture, InsertMultiple)
{
    tree.Insert(10);
    tree.Insert(2);
    tree.Insert(15);
    EXPECT_TRUE(tree.Contains(10));
    EXPECT_TRUE(tree.Contains(2));
    EXPECT_TRUE(tree.Contains(15));
}

// Non-empty tree tests
TEST_F(ICBAVLSetNonEmptyFixture, ContainsExisting)
{
    EXPECT_TRUE(tree.Contains(3));
    EXPECT_TRUE(tree.Contains(8));
}

TEST_F(ICBAVLSetNonEmptyFixture, ContainsNonExisting)
{
    EXPECT_FALSE(tree.Contains(6));
    EXPECT_FALSE(tree.Contains(100));
}

TEST_F(ICBAVLSetNonEmptyFixture, InsertExisting)
{
    tree.Insert(3); // Should not change the tree
    EXPECT_TRUE(tree.Contains(3));
}

TEST_F(ICBAVLSetNonEmptyFixture, InsertNew)
{
    tree.Insert(6);
    EXPECT_TRUE(tree.Contains(6));
}

// Traversal Tests
TEST_F(ICBAVLSetNonEmptyFixture, InorderTraversal)
{
    std::vector<int> expected = {1, 3, 4, 5, 8};
    std::vector<int> actual;

    tree.ForEachNode(icb::AVLSet<int>::TraversalOrder::INORDER, [&actual](const int &data) { actual.push_back(data); });

    EXPECT_EQ(expected, actual);
}

TEST_F(ICBAVLSetNonEmptyFixture, PreorderTraversal)
{
    std::vector<int> expected = {5, 3, 1, 4, 8};
    std::vector<int> actual;

    tree.ForEachNode(icb::AVLSet<int>::TraversalOrder::PREORDER,
                     [&actual](const int &data) { actual.push_back(data); });

    EXPECT_EQ(expected, actual);
}

TEST_F(ICBAVLSetNonEmptyFixture, PostorderTraversal)
{
    std::vector<int> expected = {1, 4, 3, 8, 5};
    std::vector<int> actual;

    tree.ForEachNode(icb::AVLSet<int>::TraversalOrder::POSTORDER,
                     [&actual](const int &data) { actual.push_back(data); });

    EXPECT_EQ(expected, actual);
}

// Edge Case Tests
TEST_F(ICBAVLSetEmptyFixture, CopyConstructor)
{
    icb::AVLSet<int> tree2(tree);
    EXPECT_FALSE(tree2.Contains(3));
    tree2.Insert(10);                // Modify the copied tree
    EXPECT_FALSE(tree.Contains(10)); // Original tree is unchanged
}

TEST_F(ICBAVLSetNonEmptyFixture, CopyConstructor)
{
    icb::AVLSet<int> tree2(tree);
    EXPECT_TRUE(tree2.Contains(3));
    EXPECT_TRUE(tree2.Contains(8));
    tree2.Insert(10);                // Modify the copied tree
    EXPECT_FALSE(tree.Contains(10)); // Original tree is unchanged
}

// Stress Test
TEST(ICBAVLSetStressTest, LargeInput)
{
    icb::AVLSet<int> tree;
    const int numElements = 10000;

    for (int i = 0; i < numElements; ++i)
    {
        tree.Insert(i);
    }

    for (int i = 0; i < numElements; ++i)
    {
        EXPECT_TRUE(tree.Contains(i));
    }
}

// Edge Case Tests
TEST_F(ICBAVLSetNonEmptyFixture, InsertDuplicate)
{
    tree.Insert(3);                // Insert a duplicate value
    EXPECT_TRUE(tree.Contains(3)); // Value should still be in the tree
    // Optionally, verify that the tree structure is unchanged
}

// Move Semantics Tests
TEST(ICBAVLSetMoveSemantics, MoveConstructor)
{
    icb::AVLSet<int> tree;
    tree.Insert(5);
    tree.Insert(3);
    icb::AVLSet<int> tree2(std::move(tree));
    EXPECT_TRUE(tree2.Contains(5));
    EXPECT_TRUE(tree2.Contains(3));
    // Original tree should be empty
}

TEST(ICBAVLSetMoveSemantics, MoveAssignment)
{
    icb::AVLSet<int> tree;
    tree.Insert(5);
    tree.Insert(3);
    icb::AVLSet<int> tree2;
    tree2 = std::move(tree);
    EXPECT_TRUE(tree2.Contains(5));
    EXPECT_TRUE(tree2.Contains(3));
    // Original tree should be empty
}

// Negative Values Test
TEST_F(ICBAVLSetEmptyFixture, NegativeValues)
{
    tree.Insert(-5);
    tree.Insert(-10);
    tree.Insert(-1);
    EXPECT_TRUE(tree.Contains(-5));
    EXPECT_TRUE(tree.Contains(-10));
    EXPECT_TRUE(tree.Contains(-1));
}

// Custom Type Test
struct CustomType
{
    int x;
    bool operator<(const CustomType &other) const
    {
        return x < other.x;
    }
    bool operator>(const CustomType &other) const
    {
        return x > other.x;
    }
    bool operator==(const CustomType &other) const
    {
        return x == other.x;
    }
    bool operator!=(const CustomType &other) const
    {
        return x != other.x;
    }
};

TEST(ICBAVLSetCustomType, InsertAndContains)
{
    icb::AVLSet<CustomType> tree;
    tree.Insert(CustomType{5});
    tree.Insert(CustomType{3});
    tree.Insert(CustomType{8});

    EXPECT_TRUE(tree.Contains(CustomType{5}));
    EXPECT_TRUE(tree.Contains(CustomType{3}));
    EXPECT_TRUE(tree.Contains(CustomType{8}));
    EXPECT_FALSE(tree.Contains(CustomType{10}));
}
