//
// Created by FernandoDamian on 29/6/2017.
//
#include <cppstreams.h>
#include <gtest/gtest.h>
#include <vector>
#include <string>

using ::testing::Test;
using namespace std;


class StreamsFromVectorTests : public Test {

protected:

    StreamsFromVectorTests() {}

    virtual ~StreamsFromVectorTests() {}

};

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_MapDoesNotChangeVectorSize_Test) {
    vector<int> testVector{0, 1, 2};
    std::vector<int> resultVector = Stream<int, std::vector>::makeStream(testVector)
            .map([](const int &value) { return value * 2; })
            .collect();
    ASSERT_EQ(resultVector.size(), testVector.size());
}

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_MapLambdaWorks_Test) {
    vector<int> testVector{0, 1, 2};
    std::vector<int> resultVector = Stream<int, std::vector>::makeStream(testVector)
            .map([](const int &value) { return value * 2; })
            .collect();

    ASSERT_EQ(resultVector[0], 0);
    ASSERT_EQ(resultVector[1], 2);
    ASSERT_EQ(resultVector[2], 4);
}

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_MapToNewType_Test) {
    vector<int> testVector{0, 1, 2};
    auto resultVector = Stream<int, std::vector>::makeStream(testVector)
            .map([](const int &value) { return std::to_string(value); })
            .collect();

    ASSERT_EQ(resultVector[0], "0");
    ASSERT_EQ(resultVector[1], "1");
    ASSERT_EQ(resultVector[2], "2");
}

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_findAnyWorks_Test) {
    vector<int> testVector{0, 1, 2};
    auto resultVector = Stream<int, std::vector>::makeStream(testVector)
            .map([](const int &value) { return value * 2; })
            .findAny();
    ASSERT_TRUE(resultVector);
    auto resultEmptyVector = Stream<int, std::vector>::makeStream(std::vector<int>())
            .map([](const int &value) { return value * 2; })
            .findAny();
    ASSERT_FALSE(resultEmptyVector);
}

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_FilterWorks_Test) {
    vector<int> testVector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> resultVector = Stream<int, std::vector>::makeStream(testVector)
        .filter([](const int & iValue) { return iValue % 2 == 0; } )
        .collect();

    ASSERT_EQ(resultVector.size(), 5UL);

    ASSERT_EQ(resultVector[0], 0);
    ASSERT_EQ(resultVector[1], 2);
    ASSERT_EQ(resultVector[2], 4);
    ASSERT_EQ(resultVector[3], 6);
    ASSERT_EQ(resultVector[4], 8);
}

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_CollectLimitWorks_Test) {
    vector<int> testVector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    std::vector<int> resultVector = Stream<int, std::vector>::makeStream(testVector)
        .collect(5);

    ASSERT_EQ(resultVector.size(), 5UL);

    ASSERT_EQ(resultVector[0], 0);
    ASSERT_EQ(resultVector[1], 1);
    ASSERT_EQ(resultVector[2], 2);
    ASSERT_EQ(resultVector[3], 3);
    ASSERT_EQ(resultVector[4], 4);
}

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_SumAVectorOfIntegers_Test) {
    vector<int> testVector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int sum = Stream<int, std::vector>::makeStream(testVector)
            .sum();

    ASSERT_EQ(sum, 45);
}

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_FindFirst_Test) {
    vector<int> testVector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int result = Stream<int, std::vector>::makeStream(testVector)
            .findFirst([] (const int& i) { return i > 0 && i % 2 == 0; }).value_or(0);

    ASSERT_EQ(result, 2);
}

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_FindFirstReturnDefault_Test) {
    vector<int> testVector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int result = Stream<int, std::vector>::makeStream(testVector)
            .findFirst([] (const int& i) { return i > 100; }).value_or(0);

    ASSERT_EQ(result, 0);
}

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_SumAfterMap_Test) {
    vector<int> testVector{0, 1, 2};
    int result = Stream<int, std::vector>::makeStream(testVector)
            .map([](const int &value) { return value * 2; })
            .sum(0);

    ASSERT_EQ(result, 6);
}

TEST_F(StreamsFromVectorTests, StreamsFromVectorTests_SumAfterFilter_Test) {
    vector<int> testVector{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int result = Stream<int, std::vector>::makeStream(testVector)
        .filter([](const int & iValue) { return iValue % 2 == 0; } )
        .sum(0);

    ASSERT_EQ(result, 20);
}
