#include "icb/hash_table.h"

#include "common_test_setup.h"

class ICBHashTableTestFixture : public ICBTestFixture {
protected:
    icb::HashTable<std::string, int> table; 
};


TEST_F(ICBHashTableTestFixture, EmptyTable) {
    EXPECT_TRUE(table.Empty());
    EXPECT_EQ(table.Size(), 0);
}

TEST_F(ICBHashTableTestFixture, InsertionAndFind) {
    table.Insert("apple", 5);
    table.Insert("banana", 10);
    table.Insert("cherry", 15);

    EXPECT_EQ(table.Find("apple"), 5);
    EXPECT_EQ(table.Find("banana"), 10);
    EXPECT_EQ(table.Find("cherry"), 15);

    // Test move insertion
    std::string key = "date";
    table.Insert(std::move(key), 20);
    EXPECT_EQ(table.Find("date"), 20);
    EXPECT_TRUE(key.empty());

    // Test that existing values are not overwritten
    table.Insert("apple", 50);
    EXPECT_EQ(table.Find("apple"), 5); 
}

TEST_F(ICBHashTableTestFixture, Erase) {
    table.Insert("grape", 25);
    EXPECT_EQ(table.Find("grape"), 25);
    table.Erase("grape");
    EXPECT_EQ(table.Find("grape"), std::nullopt);

    // Test erasing a non-existent key
    table.Erase("fig");
    EXPECT_EQ(table.Size(), 0);
}

TEST_F(ICBHashTableTestFixture, Rehash) {
    table.Insert("lemon", 30);
    table.Insert("mango", 35);
    table.Insert("orange", 40);

    table.Rehash(20); 

    EXPECT_EQ(table.Find("lemon"), 30);
    EXPECT_EQ(table.Find("mango"), 35);
    EXPECT_EQ(table.Find("orange"), 40);
}

TEST_F(ICBHashTableTestFixture, Collisions) {
    // Test collision handling for keys with the same hash value
    table.Insert("pear", 45);
    table.Insert("plum", 50);  // These should hash to the same index

    EXPECT_EQ(table.Find("pear"), 45);
    EXPECT_EQ(table.Find("plum"), 50);
}

TEST_F(ICBHashTableTestFixture, DuplicateKeys) {
    table.Insert("apple", 5);
    table.Insert("apple", 10); // Insert with same key

    EXPECT_EQ(table.Size(), 1); // Size should not increase
    EXPECT_EQ(table.Find("apple"), 5); // Original value should be retained
}

TEST_F(ICBHashTableTestFixture, ManyInsertions) {
    // Test inserting a large number of elements
    for (int i = 0; i < 1000; ++i) {
        table.Insert(std::to_string(i), i);
    }

    EXPECT_EQ(table.Size(), 1000);

    // Verify all values can be found
    for (int i = 0; i < 1000; ++i) {
        EXPECT_EQ(table.Find(std::to_string(i)), i);
    }
}

TEST_F(ICBHashTableTestFixture, EraseAndRehash) {
    // Test erase after rehashing
    table.Insert("apple", 5);
    table.Insert("banana", 10);
    table.Insert("cherry", 15);

    table.Rehash(20);

    table.Erase("banana"); 
    EXPECT_EQ(table.Find("banana"), std::nullopt);
}

TEST_F(ICBHashTableTestFixture, RehashWithCollisions) {
    // Test rehashing a table with collisions
    table.Insert("pear", 45);
    table.Insert("plum", 50);  // These should hash to the same index

    table.Rehash(20);  // Rehash to a larger table

    EXPECT_EQ(table.Find("pear"), 45);
    EXPECT_EQ(table.Find("plum"), 50);
}

TEST_F(ICBHashTableTestFixture, InitializerListConstructor) {
    // Test the initializer list constructor if it's implemented in HashTable
    icb::HashTable<std::string, int> table2{
        {"apple", 5}, {"banana", 10}, {"cherry", 15}
    };

    EXPECT_EQ(table2.Size(), 3);
    EXPECT_EQ(table2.Find("banana"), 10);
}
