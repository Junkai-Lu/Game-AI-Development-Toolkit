/*
* bitboard include bitboard and bitpoker.
*
* bitboard equal to a bool array which size is 64 and the size can not be change.
* bitpoker equal to a unsigned int array which size is 16, the value of each one is limit between 0 and 15.
* in poker games, we have cards with 15 different numbers(assume black joker and red joker is different),and the cards can be saved by bitpokers.
*
* version: 2017/1/13
* copyright: Junkai Lu
* email: Junkai-Lu@outlook.com
*/

#include "stdafx.h"
#include <string>
#include "gadtlib.h"

#pragma once

namespace gadt
{
#ifndef GADT_GCC
	typedef __int64 gadt_int64;
#else
	typedef long long gadt_int64;
#endif

	//bit board.
	class BitBoard
	{
	private:
		long long _data;

	public:
		inline BitBoard() :
			_data(0)
		{

		}
		inline BitBoard(long long board) :
			_data(board)
		{
		}

		//equal to the appointed value.
		inline void operator=(long long board)
		{
			_data = board;
		}

		//return whether any bit is true.
		inline bool any() const
		{
			return _data != 0;
		}

		//return whether no any bit is true.
		inline bool none() const
		{
			return _data == 0;
		}

		//set appointed bit to true.
		inline void set(size_t index)
		{
			GADT_WARNING_CHECK(index >= 64, "out of range.");
			long long temp = 1;
			temp = temp << index;
			_data = _data | temp;
		}

		//reset appointed bit.
		inline void reset(size_t index)
		{
			GADT_WARNING_CHECK(index >= 64, "out of range.");
			long long temp = 1;
			temp = ~(temp << index);
			_data = _data & temp;
		}

		//reset all bits.
		inline void reset()
		{
			_data = 0;
		}

		//write value to appointed bit.
		inline void write(size_t index, int value)
		{
			GADT_WARNING_CHECK(index >= 64, "out of range.");
			if (value)
			{
				set(index);
			}
			else
			{
				reset(index);
			}
		}

		//get bit.
		inline bool operator[](size_t index) const
		{
			GADT_WARNING_CHECK(index >= 64, "out of range.");
			return ((_data >> index) & 0x1) == 1;
		}

		//get bit.
		inline bool get(size_t index) const
		{
			GADT_WARNING_CHECK(index >= 64, "out of range.");
			return ((_data >> index) & 0x1) == 1;
		}

		//get value
		inline BitBoard to_ullong() const
		{
			return _data;
		}

		//get string
		inline std::string to_string() const
		{
			char c[64];
			for (size_t i = 64 - 1; i >= 0 && i <= 64; i--)
			{
				if (get(i))
				{
					c[64 - 1 - i] = '1';
				}
				else
				{
					c[64 - 1 - i] = '0';
				}
			}
			return std::string(c);
		}

		inline bool operator<(const BitBoard& target)
		{
			return _data < target._data;
		}
		inline bool operator>(const BitBoard& target)
		{
			return _data > target._data;
		}
		inline bool operator<=(const BitBoard& target)
		{
			return _data <= target._data;
		}
		inline bool operator>=(const BitBoard& target)
		{
			return _data >= target._data;
		}
		inline bool operator==(const BitBoard& target)
		{
			return _data == target._data;
		}
	};

	//bit poker.
	class BitPoker
	{
	private:
		gadt_int64 _data;
	public:
		inline BitPoker() :
			_data(0)
		{

		}
		inline BitPoker(long long board) :
			_data(board)
		{
		}
		//equal to the appointed value.
		inline void operator=(long long board)
		{
			_data = board;
		}

		//return whether any bit is true.
		inline bool any() const
		{
			return _data != 0;
		}

		//return whether no any bit is true.
		inline bool none() const
		{
			return _data == 0;
		}

		//set appointed bit to true.
		inline void set(size_t index,size_t value)
		{
			GADT_WARNING_CHECK(index >= 16, "out of range.");
			GADT_WARNING_CHECK(value >= 16, "out of value.");
			_data = (_data & (~((long long)0xF << index * 4))) | ((value & 0xF) << (index * 4));
		}

		//reset appointed bit.
		inline void reset(size_t index)
		{
			GADT_WARNING_CHECK(index >= 64, "out of range.");
			long long temp = 15;
			temp = ~(temp << index);
			_data = _data & temp;
		}

		//reset all bits.
		inline void reset()
		{
			_data = 0;
		}

		//get bit.
		inline bool operator[](size_t index) const
		{
			return get(index);
		}

		//get bit.
		inline size_t get(size_t index) const
		{
			GADT_WARNING_CHECK(index >= 16, "out of range.");
			return (_data >> (index * 4)) & 0xF;
		}

		//get total
		inline size_t total() const
		{
			gadt_int64 temp = _data;
			size_t t = 0;
			for (size_t i = 0; i < 16; i++)
			{
				t += (temp & 0xF);
				temp = temp >> 4;
			}
			return t;
		}

		//add one card in this card group.
		inline void Push(size_t index)
		{
			GADT_WARNING_CHECK(index >= 16, "out of range.");
			gadt_int64 value = get(index);
			GADT_WARNING_CHECK(value == 15, "overflow.");
			set(index, value + 1);
		}

		//get value
		inline BitBoard to_ullong() const
		{
			return _data;
		}

		//get string
		inline std::string to_string() const
		{
			std::cout << "{ ";
			for (int i = 0; i < 64; i += 4)
			{
				std::cout << ((_data >> i) & 0xF) << " ";
			}
			std::cout << "}" << std::endl;
		}

		inline std::string to_bit_string() const
		{
			char c[64];
			for (size_t i = 64 - 1; i >= 0 && i <= 64; i--)
			{
				if (get(i))
				{
					c[64 - 1 - i] = '1';
				}
				else
				{
					c[64 - 1 - i] = '0';
				}
			}
			return std::string(c);
		}

		inline void operator+=(const BitPoker& target)
		{
#ifdef GADT_WARNING
			for (size_t i = 0; i < 16; i++)
			{
				GADT_WARNING_CHECK(get(i) + target.get(i) > 0xF, ">> WARNING:: function BITGROUP::Plus overflow.");
			}
#endif
			_data += target._data;
		}
		inline void operator-=(const BitPoker& target)
		{
#ifdef GADT_WARNING
			for (size_t i = 0; i < 16; i++)
			{
				GADT_WARNING_CHECK(get(i) < target.get(i), ">> WARNING:: function BITGROUP::Plus overflow.");
			}
#endif
			_data -= target._data;
		}
		inline BitPoker operator+(const BitPoker& target) const
		{
#ifdef GADT_WARNING
			for (size_t i = 0; i < 16; i++)
			{
				GADT_WARNING_CHECK(get(i) + target.get(i) > 0xF, ">> WARNING:: function BITGROUP::Plus overflow.");
			}
#endif
			return BitPoker(_data + target._data);
		}
		inline BitPoker operator-(const BitPoker& target) const
		{
#ifdef GADT_WARNING
			for (size_t i = 0; i < 16; i++)
			{
				GADT_WARNING_CHECK(get(i) < target.get(i), ">> WARNING:: function BITGROUP::Plus overflow.");
			}
#endif
			return BitPoker(_data - target._data);
		}
		inline bool operator<(const BitPoker& target)
		{
			return _data < target._data;
		}
		inline bool operator>(const BitPoker& target)
		{
			return _data > target._data;
		}
		inline bool operator<=(const BitPoker& target)
		{
			return _data <= target._data;
		}
		inline bool operator>=(const BitPoker& target)
		{
			return _data >= target._data;
		}
		inline bool operator==(const BitPoker& target)
		{
			return _data == target._data;
		}
	};
}
