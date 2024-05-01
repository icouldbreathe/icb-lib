#include <gtest/gtest.h>

#include "icb/vector.h"

struct TestStruct {
        int x, y;
        TestStruct(int a, int b) : x(a), y(b) {}
};

// Fixture for general icb::Vector<int> tests
class ICBVectorIntTestFixture : public ::testing::Test {
protected:
    icb::Vector<int> test_vector; 
};

// Fixture for EmplaceBack with TestStruct
class ICBVectorStructTestFixture : public ::testing::Test {
protected:
    icb::Vector<TestStruct> test_vector; 
};

TEST_F(ICBVectorIntTestFixture, DefaultConstructor) {
    EXPECT_EQ(test_vector.Size(), 0);
    EXPECT_EQ(test_vector.Capacity(), 0); // Assuming your initial capacity is 0
}

TEST_F(ICBVectorIntTestFixture, InitializerListConstructor) {
    icb::Vector<int> vec = {10, 20, 30};
    EXPECT_EQ(vec.Size(), 3);
    EXPECT_EQ(vec[0], 10);
    EXPECT_EQ(vec[1], 20);
    EXPECT_EQ(vec[2], 30);  
}

TEST_F(ICBVectorIntTestFixture, OperatorSubscript) {
    test_vector.PushBack(5);
    test_vector.PushBack(15);

    EXPECT_EQ(test_vector[0], 5);
    EXPECT_EQ(test_vector[1], 15);
    test_vector[1] = 25;
    EXPECT_EQ(test_vector[1], 25);
}

TEST_F(ICBVectorIntTestFixture, AtFunction) {
    test_vector.PushBack(5);
    test_vector.PushBack(15);

    EXPECT_EQ(test_vector.at(0), 5);

    // Example with out-of-bounds (assuming you have exception handling)
    EXPECT_THROW(test_vector.at(2), std::out_of_range); 
}

TEST_F(ICBVectorIntTestFixture, PushBack) {
    test_vector.PushBack(10);  
    EXPECT_EQ(test_vector.Size(), 1); 

    // ... more checks on element values and capacity growth
}

TEST_F(ICBVectorStructTestFixture, EmplaceBack) {

    test_vector.EmplaceBack(10, 20); // Construct TestStruct in-place
    EXPECT_EQ(test_vector.Size(), 1);  
    EXPECT_EQ(test_vector[0].x, 10);
    EXPECT_EQ(test_vector[0].y, 20);
}
