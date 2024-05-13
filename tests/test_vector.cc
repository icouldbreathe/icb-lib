#include "icb/vector.h"

#include "common_test_setup.h"

// Fixture for general icb::Vector<int> tests
class ICBVectorIntTestFixture : public ICBTestFixture {
protected:
    icb::Vector<int> test_vector; 
};

// Fixture for EmplaceBack with TestStruct
class ICBVectorStructTestFixture : public ICBTestFixture {
protected:
    icb::Vector<TestStruct> test_vector; 
};

TEST_F(ICBVectorIntTestFixture, DefaultConstructor) {
    EXPECT_EQ(test_vector.Size(), 0);
    EXPECT_EQ(test_vector.Capacity(), 0);
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

    EXPECT_EQ(test_vector.At(0), 5);

    EXPECT_THROW(test_vector.At(2), std::out_of_range); 
}

TEST_F(ICBVectorIntTestFixture, PushBack) {
    test_vector.PushBack(10);  
    EXPECT_EQ(test_vector.Size(), 1); 
}

TEST_F(ICBVectorStructTestFixture, EmplaceBack) {

    test_vector.EmplaceBack(10, 20);
    EXPECT_EQ(test_vector.Size(), 1);  
    EXPECT_EQ(test_vector[0].x, 10);
    EXPECT_EQ(test_vector[0].y, 20);
}

TEST_F(ICBTestFixture, RandomAccessIteratorConcept)
{
    static_assert(std::random_access_iterator<icb::Vector<int>::Iterator>);
    EXPECT_TRUE(std::random_access_iterator<icb::Vector<int>::Iterator>);
    EXPECT_TRUE(std::random_access_iterator<icb::Vector<int>::ConstIterator>);
}