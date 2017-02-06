/*
* bitboard include BitBoard64,BitBoard128, BitPoker and BitMahjong.
*
* BitBoard equal to a bool array which size is 64 and the size can not be change.
* BitPoker equal to a unsigned int array which size is 16, the value of each one is limit between 0 and 15.
* BitMahjong is a unsigned int vector whose size is 42, each value is limit between 0 and 7.
*
* in poker games, we have cards with 15 different numbers(assume black joker and red joker is different),and the cards can be saved by bitpokers.
* in mahjong games. there are <40 kinds of tile and each one is less than 8 in game, so we design a vector by using two 64-bit uint to save these info.
*
* version: 2017/2/6
* copyright: Junkai Lu
* email: Junkai-Lu@outlook.com
*/

#include <string>
#include <sstream>
#include "gadtlib.h"

#pragma once

namespace gadt
{
#ifndef GADT_UNIX
	typedef uint64_t gadt_int64;
#else
	typedef uint64_t gadt_int64;
#endif

	//bit board.
	class BitBoard64
	{
	private:
#ifdef GADT_DEBUG_INFO
		bool _debug_data[64];
#endif
		gadt_int64 _data;
	public:
		inline BitBoard64() :
			_data(0)

		{
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 64; i++)
			{
				_debug_data[i] = false;
			}
#endif
		}
		inline BitBoard64(gadt_int64 board) :
			_data(board)
		{
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 64; i++)
			{
				_debug_data[i] = get(i);
			}
#endif
		}

		//equal to the appointed value.
		inline void operator=(gadt_int64 board)
		{
			_data = board;
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 64; i++)
			{
				_debug_data[i] = get(i);
			}
#endif
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
			gadt_int64 temp = 1;
			temp = temp << index;
			_data = _data | temp;
#ifdef GADT_DEBUG_INFO
			_debug_data[index] = true;
#endif
		}

		//reset appointed bit.
		inline void reset(size_t index)
		{
			GADT_WARNING_CHECK(index >= 64, "out of range.");
			gadt_int64 temp = 1;
			temp = ~(temp << index);
			_data = _data & temp;
#ifdef GADT_DEBUG_INFO
			_debug_data[index] = false;
#endif
		}

		//reset all bits.
		inline void reset()
		{
			_data = 0;
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 64; i++)
			{
				_debug_data[i] = false;
			}
#endif
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
		inline bool get(size_t index) const
		{
			GADT_WARNING_CHECK(index >= 64, "out of range.");
			return ((_data >> index) & 0x1) == 1;
		}

		//get bit.
		inline bool operator[](size_t index) const
		{
			return get(index);
		}

		//get value
		inline gadt_int64 to_ullong() const
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

		inline bool operator<(const BitBoard64& target)
		{
			return _data < target._data;
		}
		inline bool operator>(const BitBoard64& target)
		{
			return _data > target._data;
		}
		inline bool operator<=(const BitBoard64& target)
		{
			return _data <= target._data;
		}
		inline bool operator>=(const BitBoard64& target)
		{
			return _data >= target._data;
		}
		inline bool operator==(const BitBoard64& target)
		{
			return _data == target._data;
		}
	};

	//bit board.
	class BitBoard128
	{
	private:
#ifdef GADT_DEBUG_INFO
		bool _debug_data[128];
#endif
		gadt_int64 _fir_data;
		gadt_int64 _sec_data;
	public:
		inline BitBoard128() :
			_fir_data(0),
			_sec_data(0)
		{
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 128; i++)
			{
				_debug_data[i] = false;
			}
#endif
		}
		inline BitBoard128(gadt_int64 fir_data,gadt_int64 sec_data) :
			_fir_data(fir_data),
			_sec_data(sec_data)
		{
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 128; i++)
			{
				_debug_data[i] = get(i);
			}
#endif
		}

		//return whether any bit is true.
		inline bool any() const
		{
			return _fir_data != 0 || _sec_data != 0;
		}

		//return whether no any bit is true.
		inline bool none() const
		{
			return _fir_data == 0 && _sec_data == 0;
		}

		//set appointed bit to true.
		inline void set(size_t index)
		{
			GADT_WARNING_CHECK(index >= 128, "out of range.");
			gadt_int64 temp = 1;
			if (index > 63)
			{
				temp = temp << (index - 64);
				_sec_data = _sec_data | temp;
			}
			else
			{
				temp = temp << index;
				_fir_data = _fir_data | temp;
			}
#ifdef GADT_DEBUG_INFO
			_debug_data[index] = true;
#endif
		}

		//reset appointed bit.
		inline void reset(size_t index)
		{
			GADT_WARNING_CHECK(index >= 128, "out of range.");
			gadt_int64 temp = 1;
			if (index >= 64)
			{
				temp = ~(temp << (index-64));
				_sec_data = _sec_data & temp;
			}
			else
			{
				temp = ~(temp << index);
				_fir_data = _fir_data & temp;
			}
#ifdef GADT_DEBUG_INFO
			_debug_data[index] = false;
#endif
		}

		//reset all bits.
		inline void reset()
		{
			_fir_data = 0;
			_sec_data = 0;
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 128; i++)
			{
				_debug_data[i] = false;
			}
#endif
		}

		//write value to appointed bit.
		inline void write(size_t index, int value)
		{
			GADT_WARNING_CHECK(index >= 128, "out of range.");
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
		inline bool get(size_t index) const
		{
			GADT_WARNING_CHECK(index >= 128, "out of range.");
			if (index >= 64)
			{
				return ((_sec_data >> (index - 64)) & 0x1) == 1;
			}
			return ((_fir_data >> index) & 0x1) == 1;
		}

		//get bit.
		inline bool operator[](size_t index) const
		{
			return get(index);
		}

		//get ullong string.
		inline std::string to_ullong_string() const
		{
			std::stringstream ss;
			ss << _fir_data << " " << _sec_data;
			return ss.str();
		}

		//get string format.
		inline std::string to_bit_string() const
		{
			char c[129];
			c[64] = 10;
			for (size_t i = 64 - 1; i >= 0 && i <= 64; i--)
			{
				if ((_fir_data >> i) & 1)
				{
					c[63 - i] = '1';
				}
				else
				{
					c[63 - i] = '0';
				}
			}
			for (size_t i = 64 - 1; i >= 0 && i <= 64; i--)
			{
				if ((_sec_data >> i) & 1)
				{
					c[128 - i] = '1';
				}
				else
				{
					c[128 - i] = '0';
				}
			}
			return std::string(c, 129);
		}

		inline bool operator==(const BitBoard128& target)
		{
			return _fir_data == target._fir_data && _sec_data == target._sec_data;
		}
	};

	//bit poker.
	class BitPoker
	{
	private:
#ifdef GADT_DEBUG_INFO
		uint8_t _debug_data[16];
#endif
		gadt_int64 _data;
	public:
		inline BitPoker() :
			_data(0)
		{
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 16; i++)
			{
				_debug_data[i] = 0;
			}
#endif
		}
		inline BitPoker(gadt_int64 board) :
			_data(board)
		{
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 16; i++)
			{
				_debug_data[i] = (uint8_t)get(i);
			}
#endif
		}

		//equal to the appointed value.
		inline void operator=(gadt_int64 board)
		{
			_data = board;
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 16; i++)
			{
				_debug_data[i] = (uint8_t)get(i);
			}
#endif
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
		inline void set(size_t index, gadt_int64 value)
		{
			GADT_WARNING_CHECK(index >= 16, "out of range.");
			GADT_WARNING_CHECK(value >= 16, "out of value.");
			_data = (_data & (~((gadt_int64)0xF << index * 4))) | ((value & 0xF) << (index * 4));
#ifdef GADT_DEBUG_INFO
			_debug_data[index] = (uint8_t)value;
#endif
		}

		//reset appointed bit.
		inline void reset(size_t index)
		{
			GADT_WARNING_CHECK(index >= 16, "out of range.");
			gadt_int64 temp = 15;
			temp = ~(temp << index);
			_data = _data & temp;
#ifdef GADT_DEBUG_INFO
			_debug_data[index] = 0;
#endif
		}

		//reset all bits.
		inline void reset()
		{
			_data = 0;
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 16; i++)
			{
				_debug_data[i] = 0;
			}
#endif
		}

		//get bit.
		inline size_t get(size_t index) const
		{
			GADT_WARNING_CHECK(index >= 16, "out of range.");
			return (_data >> (index * 4)) & 0xF;
		}

		//get bit.
		inline size_t operator[](size_t index) const
		{
			return get(index);
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
		inline void push(size_t index)
		{
			GADT_WARNING_CHECK(index >= 16, "out of range.");
			size_t value = get(index);
			GADT_WARNING_CHECK(value == 15, "overflow.");
			set(index, value + 1);
		}

		//get value
		inline gadt_int64 to_ullong() const
		{
			return _data;
		}

		//get string
		inline std::string to_string() const
		{
			std::stringstream ss;
			ss << "{ ";
			for (int i = 0; i < 64; i += 4)
			{
				ss << ((_data >> i) & 0xF) << " ";
			}
			ss << "}" << std::endl;
			return ss.str();
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
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 16; i++)
			{
				_debug_data[i] += target._debug_data[i];
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
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 16; i++)
			{
				_debug_data[i] -= target._debug_data[i];
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

			BitPoker temp(_data + target._data);
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 16; i++)
			{
				temp._debug_data[i] = get(i) + target.get(i);
			}
#endif
			return temp;
		}
		inline BitPoker operator-(const BitPoker& target) const
		{
#ifdef GADT_WARNING
			for (size_t i = 0; i < 16; i++)
			{
				GADT_WARNING_CHECK(get(i) < target.get(i), ">> WARNING:: function BITGROUP::Plus overflow.");
			}
#endif

			BitPoker temp(_data - target._data);
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 16; i++)
			{
				temp._debug_data[i] = get(i) - target.get(i);
			}
#endif
			return temp;
		}
		inline bool operator==(const BitPoker& target)
		{
			return _data == target._data;
		}
	};

	//bit Mahjong
	class BitMahjong
	{
	private:
#ifdef GADT_DEBUG_INFO
		uint8_t _debug_data[42];
#endif
		gadt_int64 _fir_data;
		gadt_int64 _sec_data;
	public:
		inline BitMahjong() :
			_fir_data(0),
			_sec_data(0)
		{
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 42; i++)
			{
				_debug_data[i] = 0;
			}
#endif
		}
		inline BitMahjong(gadt_int64 fir_data, gadt_int64 sec_data) :
			_fir_data(fir_data),
			_sec_data(sec_data)
		{
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 42; i++)
			{
				_debug_data[i] = (uint8_t)get(i);
			}
#endif
		}

		//return whether any bit is true.
		inline bool any() const
		{
			return _fir_data != 0 || _sec_data != 0;
		}

		//return whether no any bit is true.
		inline bool none() const
		{
			return _fir_data == 0 && _sec_data == 0;
		}

		//set appointed bit to true.
		inline void set(size_t index, gadt_int64 value)
		{
			GADT_WARNING_CHECK(index >= 42, "out of range.");
			GADT_WARNING_CHECK(value >= 8, "out of value.");
			if (index >= 21)
			{
				_sec_data = (_sec_data & (~((gadt_int64)0x7 << (index-21) * 3))) | ((value & 0x7) << ((index-21) * 3));
			}
			else
			{
				gadt_int64 t = (((gadt_int64)value & 0x7) << (index * 3));
				_fir_data = (_fir_data & (~((gadt_int64)0x7 << index * 3))) | ((value & 0x7) << (index * 3));
			}
#ifdef GADT_DEBUG_INFO
			_debug_data[index] = (uint8_t)value;
#endif
		}

		//reset appointed bit.
		inline void reset(size_t index)
		{
			GADT_WARNING_CHECK(index >= 42, "out of range.");
			gadt_int64 temp = 7;
			if (index >= 21)
			{
				temp = ~(temp << (index-21));
				_sec_data = _sec_data & temp;
			}
			else
			{
				temp = ~(temp << index);
				_fir_data = _fir_data & temp;
			}
#ifdef GADT_DEBUG_INFO
			_debug_data[index] = 0;
#endif
		}

		//reset all bits.
		inline void reset()
		{
			_fir_data = 0;
			_sec_data = 0;
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 42; i++)
			{
				_debug_data[i] = 0;
			}
#endif
		}

		//get bit.
		inline size_t get(size_t index) const
		{
			GADT_WARNING_CHECK(index >= 42, "out of range.");
			if (index >= 21)
			{
				return (_sec_data >> ((index - 21) * 3)) & 0x7;
			}
			return (_fir_data >> (index * 3)) & 0x7;
		}

		//get bit.
		inline size_t operator[](size_t index) const
		{
			return get(index);
		}

		//get total
		inline size_t total() const
		{
			gadt_int64 temp = _fir_data;
			size_t t = 0;
			for (size_t i = 0; i < 21; i++)
			{
				t += (temp & 0x7);
				temp = temp >> 3;
			}
			temp = _sec_data;
			for (size_t i = 0; i < 21; i++)
			{
				t += (temp & 0x7);
				temp = temp >> 3;
			}
			return t;
		}

		//add one card in this card group.
		inline void push(size_t index)
		{
			GADT_WARNING_CHECK(index >= 42, "out of range.");
			size_t value = get(index);
			GADT_WARNING_CHECK(value == 7, "overflow.");
			set(index, value + 1);
		}

		//get string
		inline std::string to_string() const
		{
			std::stringstream ss;
			ss << "{ " << std::endl;
			for (int i = 0; i < 21; i++)
			{
				ss << get(i) << ",";
			}
			ss << std::endl;
			for (int i = 21; i < 41; i++)
			{
				ss << get(i) << ",";
			}
			ss << get(41) << std::endl;
			ss << "}" << std::endl;
			return ss.str();
		}

		//get ullong string.
		inline std::string to_ullong_string() const
		{
			std::stringstream ss;
			ss << _fir_data << " " << _sec_data;
			return ss.str();
		}

		//get bits string.
		inline std::string to_bit_string() const
		{
			char c[129];
			c[64] = 10;
			for (size_t i = 64 - 1; i >= 0 && i <= 64; i--)
			{
				if ((_fir_data>>i) & 1)
				{
					c[63 - i] = '1';
				}
				else
				{
					c[63 - i] = '0';
				}
			}
			for (size_t i = 64 - 1; i >= 0 && i <= 64; i--)
			{
				if ((_sec_data >> i) & 1)
				{
					c[128 - i] = '1';
				}
				else
				{
					c[128 - i] = '0';
				}
			}
			return std::string(c,129);
		}

		//operation.
		inline void operator+=(const BitMahjong& target)
		{
#ifdef GADT_WARNING
			for (size_t i = 0; i < 42; i++)
			{
				GADT_WARNING_CHECK(get(i) + target.get(i) > 0x7, "overflow.");
			}
#endif
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 42; i++)
			{
				_debug_data[i] += target._debug_data[i];
			}
#endif
			_fir_data += target._fir_data;
			_sec_data += target._sec_data;
		}
		inline void operator-=(const BitMahjong& target)
		{
#ifdef GADT_WARNING
			for (size_t i = 0; i < 42; i++)
			{
				GADT_WARNING_CHECK(get(i) < target.get(i), "overflow.");
			}
#endif
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 42; i++)
			{
				_debug_data[i] -= target._debug_data[i];
			}
#endif
			_fir_data -= target._fir_data;
			_sec_data -= target._sec_data;
		}
		inline BitMahjong operator+(const BitMahjong& target) const
		{
#ifdef GADT_WARNING
			for (size_t i = 0; i < 42; i++)
			{
				GADT_WARNING_CHECK(get(i) + target.get(i) > 0xF, "overflow.");
			}
#endif

			BitMahjong temp(_fir_data + target._fir_data, _sec_data + target._sec_data);
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 42; i++)
			{
				temp._debug_data[i] = _debug_data[i] + target._debug_data[i];
			}
#endif
			return temp;
		}
		inline BitMahjong operator-(const BitMahjong& target) const
		{
#ifdef GADT_WARNING
			for (size_t i = 0; i < 42; i++)
			{
				GADT_WARNING_CHECK(get(i) < target.get(i), "overflow.");
			}
#endif
			BitMahjong temp(_fir_data - target._fir_data, _sec_data - target._sec_data);
#ifdef GADT_DEBUG_INFO
			for (size_t i = 0; i < 42; i++)
			{
				temp._debug_data[i] = _debug_data[i] - target._debug_data[i];
			}
#endif
			return temp;
		}
		inline bool operator==(const BitMahjong& target)
		{
			return _fir_data == target._fir_data && _sec_data == target._sec_data;
		}
	};

	//value vector.
	template<size_t ub>
	class ValueVector
	{
	private:
		uint8_t _values[ub];
		size_t _len;
		const size_t _upper_bound;
	public:
		//default constructor function
		inline ValueVector() :
			_len(0),
			_upper_bound(ub)
		{
		}

		//copy constructor
		ValueVector(const ValueVector& v) :
			_len(v._len),
			_upper_bound(ub)
		{
			for (size_t i = 0; i < ub; i++)
			{
				_values[i] = v._values[i];
			}
		}
		void operator=(const ValueVector& v)
		{
			for (size_t i = 0; i < ub; i++)
			{
				_values[i] = v._values[i];
			}
			_len = v._len;
		}
		template<size_t uub> ValueVector(const ValueVector<uub>& g) = delete;
		template<size_t uub> void operator=(const ValueVector<uub>& g) = delete;

		//get upper bound.
		inline size_t upper_bound()
		{
			return _upper_bound;
		}

		//visit operation.
		inline uint8_t operator[](const size_t index) const
		{
			GADT_WARNING_CHECK(index < 0 || index >= _len, "overflow");
			return _values[index];
		}

		//get value operation.
		inline uint8_t value(size_t index) const
		{
			GADT_WARNING_CHECK(index <0 || index >= _len, "overflow");
			return _values[index];
		}

		//push a new value in the end of array.
		inline void push(uint8_t index)
		{
			GADT_WARNING_CHECK(_len >= _upper_bound, "overflow");
			_values[_len] = index;
			_len++;
		}

		//get current length of array.
		inline size_t length() const
		{
			return _len;
		}

		//get random value and remove it.
		inline uint8_t draw_and_remove_value()
		{
			GADT_WARNING_CHECK(_len <= 0, "overflow");
			size_t rnd = rand() % _len;
			uint8_t temp = _values[rnd];
			_values[rnd] = _values[_len];
			_len--;
			return temp;
		}

		//get random value but do not remove.
		inline uint8_t draw_value()
		{
			GADT_WARNING_CHECK(_len <= 0, "overflow");
			size_t rnd = rand() % _len;
			return _values[rnd];
		}
	};

	typedef ValueVector<54> PokerVector;
	typedef ValueVector<144> MahjongVector;
}
