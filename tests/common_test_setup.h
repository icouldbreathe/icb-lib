#pragma once

#include <gtest/gtest.h>
#include <iostream>

#include "../utils/Timer.h"

class ICBTestFixture : public ::testing::Test
{
protected:
    Timer timer;
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

class Entity
{
public:
    Entity() = default;
    Entity(int num)
        : m_Number(num)
    {
        std::cout << "Constructing Entity: " << m_Number << std::endl;
    }
    ~Entity()
    {
        std::cout << "Destructing Entity: " << m_Number << std::endl;
    }

    int Get() const
    {
        return m_Number;
    }

private:
    int m_Number;
};

struct TestStruct
{
    int x, y;
    TestStruct(int a, int b) : x(a), y(b) {}
};
