//
// Created by Harrand on 21/12/2019.
//

#include "test_framework.hpp"
#include "memory/block.hpp"
#include <cstring>

TZ_TEST_BEGIN(dist)
	// Here's some data, see if the block knows its own size.
	char data[8];
	tz::mem::Block b1(data, data + 5);
	topaz_expect(b1.size() == 5, "tz::mem::Block had incorrect size. Expected ", 5, " got ", b1.size());
	
	// chars are sizeof 1, so maybe it was getting lucky. size() should return number of bytes, not elements.
	std::uint64_t big_data[8];
	tz::mem::Block b2(big_data, big_data + 5);
	constexpr std::size_t sz = sizeof(std::uint64_t) * 5;
	topaz_expect(b2.size() == sz, "tz::mem::Block had incorrect size for non-char type. Expected ", sz, " got ", b2.size());
TZ_TEST_END

TZ_TEST_BEGIN(auto_block)
	tz::mem::AutoBlock blk{sizeof(int)};
	topaz_expect(blk.size() == sizeof(int), "tz::mem::AutoBlock::size(): Unexpected size. Expected ", sizeof(int), ", got ", blk.size());
TZ_TEST_END

TZ_TEST_BEGIN(block_cpy)
	int data[3] = {0, 1, 2};
	tz::mem::AutoBlock b1{sizeof(int) * 3};
	std::memcpy(b1.begin, data, b1.size());
	tz::mem::AutoBlock b2{sizeof(int) * 3};
	b1.copy_to(b2);
	for(std::size_t i = 0; i < 3; i++)
	{
		int expected = data[i];
		int actual = *(reinterpret_cast<int*>(b1.begin) + i);
		topaz_expectf(expected == actual, "tz::mem::Block::copy_to test did not copy as expected. Element %zu -- expected (%d) != actual (%d)", i, expected, actual);
	}
TZ_TEST_END

TZ_TEST_BEGIN(block_to_autoblock)
	int* memory = static_cast<int*>(std::malloc(sizeof(int)));
	tz::mem::Block unmanaged_blk{memory, sizeof(int)};
	auto blk = tz::mem::AutoBlock::take(unmanaged_blk);
TZ_TEST_END

int main()
{
	tz::test::Unit block;
	
	block.add(dist());
	block.add(auto_block());
	block.add(block_cpy());
	block.add(block_to_autoblock());

	return block.result();
}