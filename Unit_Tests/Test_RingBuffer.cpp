#include <gtest/gtest.h>
#include <RingBuffer.h>
#include "TestConfig.h"

#if RunTests

TEST(RingBuffer, creation)
{
	RingBuffer rbuf = RingBuffer<int>(3);
}

TEST(RingBuffer, addWorks)
{
	RingBuffer rbuf = RingBuffer<int>(3);
	rbuf.add(1);

	ASSERT_EQ(1, rbuf[0]);
}

TEST(RingBuffer, addMultipleWorks)
{
	RingBuffer rbuf = RingBuffer<int>(3);
	std::vector<int> vec{ 1,2,3 };
	rbuf.add(vec);

	ASSERT_EQ(3, rbuf[2]);
}

TEST(RingBuffer, wrapAroundByOne)
{
	RingBuffer rbuf = RingBuffer<int>(3);
	std::vector<int> vec{ 1,2,3,4 };
	rbuf.add(vec);

	ASSERT_EQ(rbuf[0], 4);
}

TEST(RingBuffer, wrapAroundByTwo)
{
	RingBuffer rbuf = RingBuffer<int>(3);
	std::vector<int> vec{ 1,2,3,4,5 };
	rbuf.add(vec);

	ASSERT_EQ(rbuf[1], 5);
}

TEST(RingBuffer, RValueReferenceWrapAroundByTwo)
{
	RingBuffer rbuf = RingBuffer<int>(3);
	std::vector<int> vec{ 1,2,3,4,5 };
	rbuf.add(std::move(vec));

	ASSERT_EQ(rbuf[1], 5);
}

TEST(RingBuffer, getSample)
{
	RingBuffer rbuf = RingBuffer<int>(10);
	std::vector<int> vec{ 1,2,3,4,5 };
	rbuf.add(vec);

	auto sample = rbuf.getRandomSample(2);

	ASSERT_EQ(sample.size(), 2);
}

#endif //RunTests
