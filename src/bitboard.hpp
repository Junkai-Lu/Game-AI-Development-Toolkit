/* Copyright (c) 2017 Junkai Lu <junkai-lu@outlook.com>.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "gadtlib.h"

#pragma once

namespace gadt
{
	namespace bitboard
	{
		//allow check warning if it is true.
		constexpr const bool g_BITBOARD_ENABLE_WARNING = true;

		using gadt_int64 = uint64_t;

		template <typename VType, typename SType>
		class BitIter
		{
		private:
			const SType& _source;
			size_t _index;

		public:
			BitIter(size_t index, const SType& source) :
				_source(source),
				_index(index)
			{
			}

			bool operator!=(const BitIter& iter) const
			{
				if (_index != iter._index)
				{
					return true;
				}
				return false;
			}

			//
			void operator++()
			{
				_index++;
			}

			//operator self-increase
			VType operator* ()
			{
				return _source.get(_index);
			}
		};

		template<size_t ub>
		class BitBoard
		{
		private:
			//data array upper bound
			static const size_t data_ub = (ub / 16) + 1;

			//data array.
			uint16_t _data[data_ub];

			//debug info
#ifdef GADT_DEBUG_INFO
			bool _debug_data[ub];
#endif
			//iter type.
			using Iter = BitIter<bool, BitBoard>;

		public:
			
			//default constructor
			inline BitBoard()
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < ub; i++)
				{
					_debug_data[i] = false;
				}
#endif
				for (size_t i = 0; i < data_ub; i++)
				{
					_data[i] = 0;
				}
			}

			//initilize BitBoard by list
			inline BitBoard(std::initializer_list<size_t> init_list)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < ub; i++)
				{
					_debug_data[i] = false;
				}
#endif
				for (size_t i = 0; i < data_ub; i++)
				{
					_data[i] = 0;
				}
				for (size_t index : init_list)
				{
					set(index);
				}
			}

			//return whether any bit is true.
			inline bool any() const
			{
				for (size_t i = 0; i < data_ub; i++)
				{
					if (_data[data_ub] != 0)
					{
						return true;
					}
				}
				return false;
			}

			//return whether no any bit is true.
			inline bool none() const
			{
				for (size_t i = 0; i < data_ub; i++)
				{
					if (_data[data_ub] != 0)
					{
						return false;
					}
				}
				return true;
			}

			//set appointed bit to true.
			inline void set(size_t index)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= ub, "out of range.");
				size_t data_index = index / 16;
				size_t bit_index = index % 16;
				uint16_t temp = 1;
				temp = uint16_t(temp << bit_index);
				_data[data_index] = _data[data_index] | temp;

#ifdef GADT_DEBUG_INFO
				_debug_data[index] = true;
#endif
			}

			//reset appointed bit.
			inline void reset(size_t index)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= ub, "out of range.");
				uint16_t temp = 1;
				size_t data_index = index / 16;
				size_t bit_index = index % 16;
				temp = uint16_t(~(temp << bit_index));
				_data[data_index] = _data[data_index] & temp;

#ifdef GADT_DEBUG_INFO
				_debug_data[index] = false;
#endif
			}

			//reset all bits.
			inline void reset()
			{
				for (size_t i = 0; i < data_ub; i++)
				{
					_data[data_ub] = 0;
				}
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < ub; i++)
				{
					_debug_data[i] = false;
				}
#endif
			}

			//write value to appointed bit.
			inline void write(size_t index, int value)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= ub, "out of range.");
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
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= ub, "out of range.");
				size_t data_index = index / 16;
				size_t bit_index = index % 16;
				return ((_data[data_index] >> bit_index) & 0x1) == 1;
			}

			//get total
			inline size_t total() const
			{
				size_t n = 0;
				for (size_t i = 0; i < ub; i++)
				{
					n += get(i);
				}
				return n;
			}

			//upper bound
			constexpr inline static size_t upper_bound()
			{
				return ub;
			}

			//return true if target is subset of this.
			bool exist_subset(const BitBoard& target) const
			{
				for (size_t i = 0; i < target.upper_bound(); i++)
				{
					if (target[i] == true && get(i) == false)
					{
						return false;
					}
				}
				return true;
			}

			//return true if this is a subset of target.
			bool is_subset_of(const BitBoard& target) const
			{
				for (size_t i = 0; i < upper_bound(); i++)
				{
					if (get(i) == true && target[i] == false)
					{
						return false;
					}
				}
				return true;
			}

			//get data
			inline uint16_t to_ushort(size_t data_index) const
			{
				return _data[data_index];
			}

			//get ullong string.
			std::string to_ushort_string() const
			{
				std::stringstream ss;
				ss << "( ";
				for (size_t i = 0; i < data_ub - 1; i++)
				{
					ss << _data[i] << ", ";
				}
				ss << _data[data_ub - 1];
				ss << " )";
				return ss.str();
			}

			//get bit.
			inline bool operator[](size_t index) const
			{
				return get(index);
			}

			//equal
			inline bool operator==(const BitBoard& target)
			{
				for (size_t i = 0; i < data_ub; i++)
				{
					if (_data[data_ub] != target._data[data_ub])
					{
						return false;
					}
				}
				return true;
			}
						
			//begin of the iter
			inline Iter begin() const
			{
				return Iter(0, *this);
			}

			//end of the iter.
			inline Iter end() const
			{
				return Iter(ub, *this);
			}
		};

		//bit board64.
		class BitBoard64
		{
		private:
			static const size_t _upper_bound = 64;

#ifdef GADT_DEBUG_INFO
			bool _debug_data[_upper_bound];
#endif
			gadt_int64 _data;

			//iter type.
			using Iter = BitIter<size_t, BitBoard64>;

			void refresh()
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
				{
					_debug_data[i] = get(i);
				}
#endif
			}

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
			
			//init by 64-bit integer
			explicit inline BitBoard64(gadt_int64 board) :
				_data(board)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < 64; i++)
				{
					_debug_data[i] = get(i);
				}
#endif
			}

			//initilize BitBoard by list
			explicit inline BitBoard64(std::initializer_list<size_t> init_list):
				_data(0)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < 64; i++)
				{
					_debug_data[i] = false;
				}
#endif
				for (size_t index : init_list)
				{
					set(index);
				}
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
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= 64, "out of range.");
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
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= 64, "out of range.");
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
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= 64, "out of range.");
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
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= 64, "out of range.");
				return ((_data >> index) & 0x1) == 1;
			}

			//total bit.
			inline size_t total() const
			{
				size_t n = _data & 0x1;	//the velue of first pos.
				gadt_int64 temp = _data;
				for (size_t i = 1; i < 64; i++)
				{
					temp = temp >> 1;	//next pos;
					n += temp & 0x1;	//plus value of current pos.
				}
				return n;
			}

			//upper bound.
			constexpr inline static size_t upper_bound()
			{
				return _upper_bound;
			}

			//return true if target is subset of this.
			bool exist_subset(const BitBoard64& target)
			{
				for (size_t i = 0; i < target.upper_bound(); i++)
				{
					if (target[i] == true && get(i) == false)
					{
						return false;
					}
				}
				return true;
			}

			//return true if this is a subset of target.
			bool is_subset_of(const BitBoard64& target) const
			{
				for (size_t i = 0; i < upper_bound(); i++)
				{
					if (get(i) == true && target[i] == false)
					{
						return false;
					}
				}
				return true;
			}

			//get value
			inline gadt_int64 to_ullong() const
			{
				return _data;
			}

			//get string
			std::string to_string() const
			{
				char c[64];
				for (size_t i = 64 - 1; i <= 64; i--)
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
				return std::string(c, 64);
			}

			inline bool operator[](size_t index) const
			{
				return get(index);
			}
			inline bool operator<(const BitBoard64 target) const
			{
				return _data < target._data;
			}
			inline bool operator>(const BitBoard64 target) const
			{
				return _data > target._data;
			}
			inline bool operator<=(const BitBoard64 target) const
			{
				return _data <= target._data;
			}
			inline bool operator>=(const BitBoard64 target) const
			{
				return _data >= target._data;
			}
			inline bool operator==(const BitBoard64 target) const
			{
				return _data == target._data;
			}
			inline BitBoard64 operator&(gadt_int64 target) const
			{
				return BitBoard64(_data & target);
			}
			inline BitBoard64 operator|(gadt_int64 target) const
			{
				return BitBoard64(_data | target);
			}
			inline BitBoard64 operator^(gadt_int64 target) const
			{
				return BitBoard64(_data ^ target);
			}
			inline BitBoard64 operator&(const BitBoard64 target) const 
			{ 
				return BitBoard64(_data & target._data); 
			}
			inline BitBoard64 operator|(const BitBoard64 target) const 
			{
				return BitBoard64(_data | target._data); 
			}
			inline BitBoard64 operator^(const BitBoard64 target) const
			{
				return BitBoard64(_data ^ target._data);
			}
			inline BitBoard64 operator~() const
			{
				return BitBoard64(~_data);
			}
			inline void operator&=(gadt_int64 target)
			{
				_data &= target;
#ifdef GADT_DEBUG_INFO
				refresh();
#endif
			}
			inline void operator|=(gadt_int64 target)
			{
				_data |= target;
#ifdef GADT_DEBUG_INFO
				refresh();
#endif
			}
			inline void operator&=(const BitBoard64 target) 
			{
				_data &= target._data; 
#ifdef GADT_DEBUG_INFO
				refresh();
#endif
			}
			inline void operator|=(const BitBoard64 target) 
			{
				_data |= target._data; 
#ifdef GADT_DEBUG_INFO
				refresh();
#endif
			}
			inline size_t operator*(const BitBoard64 target) const
			{
				return this->operator&(target).total();
			}
			

			//begin of the iter
			inline Iter begin() const
			{
				return Iter(0, *this);
			}

			//end of the iter
			inline Iter end() const
			{
				return Iter(_upper_bound, *this);
			}
		};

		//bit poker.
		class BitPoker
		{
		private:
			static const size_t _upper_bound = 16;

#ifdef GADT_DEBUG_INFO
			size_t _debug_data[_upper_bound];
#endif
			gadt_int64 _data;

			//iter type.
			using Iter = BitIter<size_t, BitPoker>;

			void refresh()
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
				{
					_debug_data[i] = get(i);
				}
#endif
			}

		public:
			inline BitPoker() :
				_data(0)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] = 0;
				}
#endif
			}

			//init by 64-bit integer
			inline BitPoker(gadt_int64 board) :
				_data(board)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] = (uint8_t)get(i);
				}
#endif
			}

			//initilize BitBoard by list
			inline BitPoker(std::initializer_list<size_t> init_list):
				_data(0)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] = 0;
				}
#endif
				for (size_t index : init_list)
				{
					push(index);
				}
			}

			//initilize BitBoard by pair list
			inline BitPoker(std::initializer_list<std::pair<size_t,gadt_int64>> init_list):
				_data(0)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] = 0;
				}
#endif
				for (auto p : init_list)
				{
					set(p.first, p.second);
				}
			}

			//equal to the appointed value.
			inline void operator=(gadt_int64 board)
			{
				_data = board;
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
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
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= _upper_bound, "out of range.");
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, value >= _upper_bound, "out of value.");
				_data = (_data & (~((gadt_int64)0xF << index * 4))) | ((value & 0xF) << (index * 4));
#ifdef GADT_DEBUG_INFO
				_debug_data[index] = (uint8_t)value;
#endif
			}

			//reset appointed bit.
			inline void reset(size_t index)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= _upper_bound, "out of range.");
				gadt_int64 temp = 15;
				temp = ~(temp << (index * 4));
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
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] = 0;
				}
#endif
			}

			//get bit.
			inline size_t get(size_t index) const
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= _upper_bound, "out of range.");
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
				for (size_t i = 0; i < _upper_bound; i++)
				{
					t += (temp & 0xF);
					temp = temp >> 4;
				}
				return t;
			}

			//self increament.
			inline void increase(size_t index)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= _upper_bound, "out of range.");
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(index) == _upper_bound - 1, "overflow.");
#ifdef GADT_DEBUG_INFO
				_debug_data[index] ++;
#endif
				uint64_t temp = 1;
				temp = temp << (index * 4);
				_data += temp;
			}

			//self decreament.
			inline void decrease(size_t index)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= _upper_bound, "out of range.");
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(index) == 0, "overflow.");
#ifdef GADT_DEBUG_INFO
				_debug_data[index] --;
#endif
				uint64_t temp = 1;
				temp = temp << (index * 4);
				_data -= temp;
			}

			//add one card in this card group.
			inline void push(size_t index)
			{
				increase(index);
			}

			//return true if target is subset of this.
			bool exist_subset(const BitPoker& target) const
			{
				for (size_t i = 0; i < target._upper_bound; i++)
				{
					if (target[i] > get(i))
					{
						return false;
					}
				}
				return true;
			}

			//return true if this is a subset of target.
			bool is_subset_of(const BitPoker& target) const
			{
				for (size_t i = 0; i < _upper_bound; i++)
				{
					if (get(i) > target[i])
					{
						return false;
					}
				}
				return true;
			}

			//get value
			inline gadt_int64 to_ullong() const
			{
				return _data;
			}

			//get string
			std::string to_string() const
			{
				std::stringstream ss;
				ss << "[ ";
				for (int i = 0; i < 64; i += 4)
				{
					ss << ((_data >> i) & 0xF) << ", ";
				}
				ss << "]" << std::endl;
				return ss.str();
			}

			//bit string
			std::string to_bit_string() const
			{
				char c[64];
				for (size_t i = 64 - 1; i <= 64; i--)
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

			//upper bound.
			constexpr inline static size_t upper_bound()
			{
				return _upper_bound;
			}

			//begin of the iter
			inline Iter begin() const
			{
				return Iter(0, *this);
			}

			//end of the iter
			inline Iter end() const
			{
				return Iter(upper_bound(), *this);
			}

			inline void operator+=(const BitPoker target)
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < _upper_bound; i++)
				{
					GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(i) + target.get(i) > 0xF, ">> WARNING:: function BITGROUP::Plus overflow.");
				}
#endif
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] += target._debug_data[i];
				}
#endif
				_data += target._data;
			}
			inline void operator-=(const BitPoker target)
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < _upper_bound; i++)
				{
					GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(i) < target.get(i), ">> WARNING:: function BITGROUP::Plus overflow.");
				}
#endif
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] -= target._debug_data[i];
				}
#endif
				_data -= target._data;
			}
			inline BitPoker operator+(const BitPoker target) const
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < _upper_bound; i++)
				{
					GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(i) + target.get(i) > 0xF, ">> WARNING:: function BITGROUP::Plus overflow.");
				}
#endif

				BitPoker temp(_data + target._data);
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					temp._debug_data[i] = _debug_data[i] + target._debug_data[i];
				}
#endif
				return temp;
			}
			inline BitPoker operator-(const BitPoker target) const
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < _upper_bound; i++)
				{
					GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(i) < target.get(i), ">> WARNING:: function BITGROUP::Plus overflow.");
				}
#endif

				BitPoker temp(_data - target._data);
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					temp._debug_data[i] = _debug_data[i] - target._debug_data[i];
				}
#endif
				return temp;
			}
			inline bool operator==(const BitPoker target) const
			{
				return _data == target._data;
			}
			inline bool operator<(const BitPoker target) const
			{
				return _data < target._data;
			}
			inline bool operator>(const BitPoker target) const
			{
				return _data > target._data;
			}
			inline bool operator<=(const BitPoker target) const
			{
				return _data <= target._data;
			}
			inline bool operator>=(const BitPoker target) const
			{
				return _data >= target._data;
			}
			inline size_t operator*(const BitPoker target) const
			{
				size_t t = 0;
				for (size_t i = 0; i < upper_bound(); i++)
				{
					t += (get(i) * target.get(i));
				}
				return t;
			}
			inline BitPoker operator&(const BitPoker target) const
			{
				return BitPoker(_data & target._data);
			}
			inline BitPoker operator|(const BitPoker target) const
			{
				return BitPoker(_data | target._data);
			}
			inline void operator&=(const BitPoker target) 
			{
				_data &= target._data; 
#ifdef GADT_DEBUG_INFO
				refresh();
#endif
			}
			inline void operator|=(const BitPoker target) 
			{
				_data |= target._data; 
#ifdef GADT_DEBUG_INFO
				refresh();
#endif
			}
		};

		//bit Mahjong
		class BitMahjong
		{
		private:
			static const size_t _upper_bound = 42;
#ifdef GADT_DEBUG_INFO
			size_t _debug_data[_upper_bound];
#endif
			gadt_int64 _fir_data;
			gadt_int64 _sec_data;

			//iter type.
			using Iter = BitIter<size_t, BitMahjong>;

		public:
			inline BitMahjong() :
				_fir_data(0),
				_sec_data(0)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] = 0;
				}
#endif
			}

			//init by two 64-bit integer
			inline BitMahjong(gadt_int64 fir_data, gadt_int64 sec_data) :
				_fir_data(fir_data),
				_sec_data(sec_data)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] = (uint8_t)get(i);
				}
#endif
			}

			//initilize BitBoard by list
			inline BitMahjong(std::initializer_list<size_t> init_list) :
				_fir_data(0),
				_sec_data(0)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] = 0;
				}
#endif
				for (size_t index : init_list)
				{
					push(index);
				}
			}

			//initilize BitBoard by pair list
			inline BitMahjong(std::initializer_list<std::pair<size_t, gadt_int64>> init_list) :
				_fir_data(0),
				_sec_data(0)
			{
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] = 0;
				}
#endif
				for (auto p : init_list)
				{
					set(p.first, p.second);
				}
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
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= _upper_bound, "out of range.");
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, value >= 8, "out of value.");
				if (index >= 21)
				{
					_sec_data = (_sec_data & (~((gadt_int64)0x7 << (index - 21) * 3))) | ((value & 0x7) << ((index - 21) * 3));
				}
				else
				{
					_fir_data = (_fir_data & (~((gadt_int64)0x7 << index * 3))) | ((value & 0x7) << (index * 3));
				}
#ifdef GADT_DEBUG_INFO
				_debug_data[index] = (uint8_t)value;
#endif
			}

			//reset appointed bit.
			inline void reset(size_t index)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= _upper_bound, "out of range.");
				gadt_int64 temp = 7;
				if (index >= 21)
				{
					temp = ~(temp << ((index - 21) * 3));
					_sec_data = _sec_data & temp;
				}
				else
				{
					temp = ~(temp << (index * 3));
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
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] = 0;
				}
#endif
			}

			//get bit.
			inline size_t get(size_t index) const
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= _upper_bound, "out of range.");
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

			//self increament.
			inline void increase(size_t index)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= _upper_bound, "out of range.");
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(index) == 7, "overflow.");
#ifdef GADT_DEBUG_INFO
				_debug_data[index] ++;
#endif
				uint64_t temp = 1;
				if (index >= 21)
				{
					temp = temp << ((index - 21) * 3);
					_sec_data += temp;
				}
				else
				{
					temp = temp << (index * 3);
					_fir_data += temp;
				}

			}

			//self decreament.
			inline void decrease(size_t index)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(index) == 0, "overflow.");
#ifdef GADT_DEBUG_INFO
				_debug_data[index] --;
#endif
				uint64_t temp = 1;
				if (index >= 21)
				{
					temp = temp << ((index - 21) * 3);
					_sec_data -= temp;
				}
				else
				{
					temp = temp << (index * 3);
					_fir_data -= temp;
				}
			}

			//add one card in this card group.
			inline void push(size_t index)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index >= _upper_bound, "out of range.");
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(index) == 7, "overflow.");
				increase(index);
			}

			//return true if target is subset of this.
			inline bool exist_subset(const BitMahjong& target) const
			{
				for (size_t i = 0; i < target._upper_bound; i++)
				{
					if (target[i] > get(i))
					{
						return false;
					}
				}
				return true;
			}

			//return true if this is a subset of target.
			inline bool is_subset_of(const BitMahjong& target) const
			{
				for (size_t i = 0; i < _upper_bound; i++)
				{
					if (get(i) > target[i])
					{
						return false;
					}
				}
				return true;
			}

			//get string
			inline std::string to_string() const
			{
				std::stringstream ss;
				ss << "[ " << std::endl;
				for (int i = 0; i < 21; i++)
				{
					ss << get(i) << ", ";
				}
				ss << std::endl;
				for (int i = 21; i < 41; i++)
				{
					ss << get(i) << ", ";
				}
				ss << get(41) << std::endl;
				ss << "]" << std::endl;
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
				for (size_t i = 64 - 1; i <= 64; i--)
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
				for (size_t i = 64 - 1; i <= 64; i--)
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

			//upper bound.
			constexpr inline static size_t upper_bound()
			{
				return _upper_bound;
			}

			//begin of the iter
			inline Iter begin() const
			{
				return Iter(0, *this);
			}

			//end of the iter
			inline Iter end() const
			{
				return Iter(_upper_bound, *this);
			}

			//operation.
			inline void operator+=(const BitMahjong target)
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < _upper_bound; i++)
				{
					GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(i) + target.get(i) > 0x7, "overflow.");
				}
#endif
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] += target._debug_data[i];
				}
#endif
				_fir_data += target._fir_data;
				_sec_data += target._sec_data;
			}
			inline void operator-=(const BitMahjong target)
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < _upper_bound; i++)
				{
					GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(i) < target.get(i), "overflow.");
				}
#endif
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					_debug_data[i] -= target._debug_data[i];
				}
#endif
				_fir_data -= target._fir_data;
				_sec_data -= target._sec_data;
			}
			inline BitMahjong operator+(const BitMahjong target) const
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < _upper_bound; i++)
				{
					GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(i) + target.get(i) > 0xF, "overflow.");
				}
#endif

				BitMahjong temp(_fir_data + target._fir_data, _sec_data + target._sec_data);
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					temp._debug_data[i] = _debug_data[i] + target._debug_data[i];
				}
#endif
				return temp;
			}
			inline BitMahjong operator-(const BitMahjong target) const
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < _upper_bound; i++)
				{
					GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, get(i) < target.get(i), "overflow.");
				}
#endif
				BitMahjong temp(_fir_data - target._fir_data, _sec_data - target._sec_data);
#ifdef GADT_DEBUG_INFO
				for (size_t i = 0; i < _upper_bound; i++)
				{
					temp._debug_data[i] = _debug_data[i] - target._debug_data[i];
				}
#endif
				return temp;
			}
			inline bool operator==(const BitMahjong target) const
			{
				return _fir_data == target._fir_data && _sec_data == target._sec_data;
			}
			inline bool operator<(const BitMahjong target) const
			{
				if (_fir_data < target._fir_data)
				{
					return true;
				}
				else if (_fir_data == target._fir_data && _sec_data < target._sec_data)
				{
					return true;
				}
				return false;
			}
			inline bool operator>(const BitMahjong target) const
			{
				if (_fir_data > target._fir_data)
				{
					return true;
				}
				else if (_fir_data == target._fir_data && _sec_data > target._sec_data)
				{
					return true;
				}
				return false;
			}
			inline bool operator<=(const BitMahjong target) const
			{
				if (_fir_data < target._fir_data)
				{
					return true;
				}
				else if (_fir_data == target._fir_data && _sec_data <= target._sec_data)
				{
					return true;
				}
				return false;
			}
			inline bool operator>=(const BitMahjong target) const
			{
				if (_fir_data > target._fir_data)
				{
					return true;
				}
				else if (_fir_data == target._fir_data && _sec_data >= target._sec_data)
				{
					return true;
				}
				return false;
			}
			inline size_t operator*(const BitMahjong target) const
			{
				size_t t = 0;
				for (size_t i = 0; i < upper_bound(); i++)
				{
					t += (get(i) * target.get(i));
				}
				return t;
			}
		};

		//value vector.
		template<size_t ub>
		class ValueVector
		{
		private:
			uint8_t _values[ub];
			size_t _len;
			static const size_t _upper_bound = ub;

			//iter type.
			using Iter = BitIter<size_t, ValueVector<ub>>;

		public:
			//default constructor function
			inline ValueVector() :
				_len(0)
			{
			}

			//init with int vector.
			template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
			ValueVector(std::initializer_list<T> init_list) :
				_len(0)
			{
				for (T p : init_list)
				{
					if (is_full()) { break; }
					push((uint8_t)p);
				}
			}

			//copy constructor
			ValueVector(const ValueVector& v) :
				_len(v._len)
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
			inline static size_t upper_bound()
			{
				return _upper_bound;
			}

			//get element.
			inline uint8_t get(const size_t index) const
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, ((index < 0) || (index >= _len)), "overflow");
				return _values[index];
			}

			//visit operation.
			inline uint8_t operator[](const size_t index) const
			{
				return get(index);
			}

			//return return if the vector is full.
			inline bool is_full() const
			{
				return _len >= upper_bound();
			}

			//return ture if the vector is empty.
			inline bool is_empty() const
			{
				return _len == 0;
			}

			//get value operation.
			inline uint8_t value(size_t index) const
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, index < 0 || index >= _len, "overflow");
				return _values[index];
			}

			//push a new value in the end of array.
			inline void push(uint8_t index)
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, _len >= _upper_bound, "overflow");
				_values[_len] = index;
				_len++;
			}

			//get current length of array.
			inline size_t length() const
			{
				return _len;
			}

			//get random value and remove it.
			uint8_t draw_and_remove_value()
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, _len <= 0, "overflow");
				size_t rnd = rand() % _len;
				uint8_t temp = _values[rnd];
				_values[rnd] = _values[_len - 1];
				_len--;
				return temp;
			}

			//get random value but do not remove.
			uint8_t draw_value() const
			{
				GADT_CHECK_WARNING(g_BITBOARD_ENABLE_WARNING, _len <= 0, "overflow");
				size_t rnd = rand() % _len;
				return _values[rnd];
			}

			//to string format, e.g { 1, 2, 3}
			std::string to_string() const
			{
				std::stringstream ss;
				ss << "{ ";
				for (size_t i = 0; i < _len; i++)
				{
					ss << static_cast<uint16_t>(get(i)) << ", ";
				}
				ss << "}";
				return ss.str();
			}

			//begin of the iter
			inline Iter begin() const
			{
				return Iter(0, *this);
			}

			//end of the iter
			inline Iter end() const
			{
				return Iter(_len, *this);
			}
		};

		//type define.
		using PokerVector   = ValueVector<54>;
		using MahjongVector = ValueVector<144>;
		using BitBoard128   = BitBoard<128>;
		using BitBoard256   = BitBoard<256>;
	}
}
