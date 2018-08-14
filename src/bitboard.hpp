/* Copyright (c) 2018 Junkai Lu <junkai-lu@outlook.com>.
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

//enable constant-total option to include a constant total value in all types of bitboard.
#define GADT_BITBOARD_CONSTANT_TOTAL

//enable debug-info option to include extra info , this would lead to little performance penalties.
#ifdef GADT_WARNING
	#define GADT_BITBOARD_DEBUG_INFO
#endif

namespace gadt
{
	namespace bitboard
	{
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

			static const size_t data_ub = (ub / 16) + 1;
			uint16_t _data[data_ub];
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
			size_t _total;
#endif
#ifdef GADT_BITBOARD_DEBUG_INFO
			bool _debug_data[ub];
#endif

		private:

			//iter type.
			using Iter = BitIter<bool, BitBoard>;

		public:
			
			//default constructor
			inline BitBoard()
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				:_total(0)
#endif
			{
#ifdef GADT_BITBOARD_DEBUG_INFO
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
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				:_total(0)
#endif
			{
#ifdef GADT_BITBOARD_DEBUG_INFO
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
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				return _total != 0;
#else
				for (size_t i = 0; i < data_ub; i++)
				{
					if (_data[data_ub] != 0)
					{
						return true;
					}
				}
				return false;
#endif
			}

			//return whether no any bit is true.
			inline bool none() const
			{
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				return _total == 0;
#else
				for (size_t i = 0; i < data_ub; i++)
				{
					if (_data[data_ub] != 0)
					{
						return false;
					}
				}
				return true;
#endif
			}

			//set appointed bit to true.
			inline void set(size_t index)
			{
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= ub, "out of range.");
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				if (get(index) == false)
					_total++;
#endif

#ifdef GADT_BITBOARD_DEBUG_INFO
				_debug_data[index] = true;
#endif
				size_t data_index = index / 16;
				size_t bit_index = index % 16;
				uint16_t temp = 1;
				temp = uint16_t(temp << bit_index);
				_data[data_index] = _data[data_index] | temp;
			}

			//reset appointed bit.
			inline void reset(size_t index)
			{
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= ub, "out of range.");
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				if (get(index) == true)
					_total--;
#endif
#ifdef GADT_BITBOARD_DEBUG_INFO
				_debug_data[index] = false;
#endif
				uint16_t temp = 1;
				size_t data_index = index / 16;
				size_t bit_index = index % 16;
				temp = uint16_t(~(temp << bit_index));
				_data[data_index] = _data[data_index] & temp;
			}

			//reset all bits.
			inline void reset()
			{
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				_total = 0;
#endif
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < ub; i++)
				{
					_debug_data[i] = false;
				}
#endif
				for (size_t i = 0; i < data_ub; i++)
				{
					_data[data_ub] = 0;
				}
			}

			//write value to appointed bit.
			inline void write(size_t index, bool value)
			{
				if (value)
					set(index);
				else
					reset(index);
			}

			//get bit.
			inline bool get(size_t index) const
			{
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= ub, "out of range.");
				size_t data_index = index / 16;
				size_t bit_index = index % 16;
				return ((_data[data_index] >> bit_index) & 0x1) == 1;
			}

			//get total
			inline size_t total() const
			{
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				return _total;
#else
				size_t n = 0;
				for (size_t i = 0; i < ub; i++)
				{
					n += get(i);
				}
				return n;
#endif
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
			gadt_int64 _data;
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
			size_t _total;
#endif
#ifdef GADT_BITBOARD_DEBUG_INFO
			bool _debug_data[_upper_bound];
#endif
		private:

			//iter type.
			using Iter = BitIter<size_t, BitBoard64>;

			size_t get_total_by_count() const
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

			void refresh()
			{
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
				{
					_debug_data[i] = get(i);
				}
#endif
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				_total = get_total_by_count();
#endif
			}

		public:

			inline BitBoard64() :
				_data(0)
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				,_total(0)
#endif
			{
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < 64; i++)
				{
					_debug_data[i] = false;
				}
#endif
			}
			
			//init by 64-bit integer
			explicit inline BitBoard64(gadt_int64 board) :
				_data(board)
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				, _total(0)
#endif
			{
				refresh();
			}

			//initilize BitBoard by list
			explicit inline BitBoard64(std::initializer_list<size_t> init_list):
				_data(0)
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				, _total(0)
#endif
			{
#ifdef GADT_BITBOARD_DEBUG_INFO
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
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				_total = 0;
				for (size_t i = 0; i < upper_bound(); i++)
					if (get(i) == true)
						_total++;
#endif
#ifdef GADT_BITBOARD_DEBUG_INFO
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
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= 64, "out of range.");
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				if (get(index) == false)
					_total++;
#endif

#ifdef GADT_BITBOARD_DEBUG_INFO
				_debug_data[index] = true;
#endif
				gadt_int64 temp = 1;
				temp = temp << index;
				_data = _data | temp;

			}

			//reset appointed bit.
			inline void reset(size_t index)
			{
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= 64, "out of range.");
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				if (get(index) == true)
					_total--;
#endif

#ifdef GADT_BITBOARD_DEBUG_INFO
				_debug_data[index] = false;
#endif
				gadt_int64 temp = 1;
				temp = ~(temp << index);
				_data = _data & temp;

			}

			//reset all bits.
			inline void reset()
			{
				_data = 0;
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				_total = 0;
#endif
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < 64; i++)
				{
					_debug_data[i] = false;
				}
#endif
			}

			//write value to appointed bit.
			inline void write(size_t index, bool value)
			{
				if (value)
					set(index);
				else
					reset(index);
			}

			//get bit.
			inline bool get(size_t index) const
			{
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= 64, "out of range.");
				return ((_data >> index) & 0x1) == 1;
			}

			//total bit.
			inline size_t total() const
			{
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, get_total_by_count() != _total, "incorrect total value");
				return _total;
#else
				return get_total_by_count();
#endif	
			}

			//upper bound.
			constexpr inline static size_t upper_bound()
			{
				return _upper_bound;
			}

			//return true if target is subset of this.
			bool exist_subset(const BitBoard64& target) const
			{
				return (_data | target._data) == _data;
				/*for (size_t i = 0; i < target.upper_bound(); i++)
				{
					if (target[i] == true && get(i) == false)
					{
						return false;
					}
				}
				return true;*/
			}

			//return true if this is a subset of target.
			bool is_subset_of(const BitBoard64& target) const
			{
				return target.exist_subset(*this);
				/*for (size_t i = 0; i < upper_bound(); i++)
				{
					if (get(i) == true && target[i] == false)
					{
						return false;
					}
				}
				return true;*/
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
				refresh();
			}
			inline void operator|=(gadt_int64 target)
			{
				_data |= target;
				refresh();
			}
			inline void operator&=(const BitBoard64 target) 
			{
				_data &= target._data; 
				refresh();
			}
			inline void operator|=(const BitBoard64 target) 
			{
				_data |= target._data; 
				refresh();
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

		//an array which presented by a single 64-bit interger.
		template<size_t BIT_WIDTH, typename std::enable_if<(BIT_WIDTH > 0 && BIT_WIDTH <= 32) , int>::type = 0>
		class BitArray
		{
		private:

			static constexpr const size_t UPPER_BOUND = 64/BIT_WIDTH;
			static constexpr const gadt_int64 SINGLE_FEATURE = (UINT64_MAX >> (64 - BIT_WIDTH));

			gadt_int64 _data;
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
			size_t _total;
#endif
#ifdef GADT_BITBOARD_DEBUG_INFO
			size_t _debug_data[UPPER_BOUND];
#endif
		private:

			//iter type.
			using Iter = BitIter<size_t, BitArray>;

			void refresh()
			{
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
				{
					_debug_data[i] = get(i);
				}
#endif
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				_total = get_total_by_count();
#endif
			}

			size_t get_total_by_count() const
			{
				gadt_int64 temp = _data;
				size_t t = 0;
				for (size_t i = 0; i < upper_bound(); i++)
				{
					t += (temp & SINGLE_FEATURE);
					temp = temp >> BIT_WIDTH;
				}
				return t;
			}

		public:

			inline BitArray() :
				_data(0)
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				, _total(0)
#endif
			{
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
				{
					_debug_data[i] = 0;
				}
#endif
			}

			//init by 64-bit integer
			inline BitArray(gadt_int64 board) :
				_data(board)
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				, _total(0)
#endif
			{
				refresh();
			}

			//initilize BitBoard by list
			inline BitArray(std::initializer_list<size_t> init_list) :
				_data(0)
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				, _total(0)
#endif
			{
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
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
			inline BitArray(std::initializer_list<std::pair<size_t, gadt_int64>> init_list) :
				_data(0)
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				, _total(0)
#endif
			{
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
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
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				_total = 0;
				for (size_t i = 0; i < upper_bound(); i++)
					_total += get(i);
#endif

#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
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
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= upper_bound(), "out of range.");
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, value > SINGLE_FEATURE, "out of value.");
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				if (get(index) > value)
					_total -= (get(index) - (size_t)value);
				else
					_total += ((size_t)value - get(index));
#endif

#ifdef GADT_BITBOARD_DEBUG_INFO
				_debug_data[index] = (uint8_t)value;
#endif
				_data = (_data & (~(SINGLE_FEATURE << (index * BIT_WIDTH)))) | ((value & SINGLE_FEATURE) << (index * BIT_WIDTH));
			}

			//reset appointed bit.
			inline void reset(size_t index)
			{
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= upper_bound(), "out of range.");
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				_total -= get(index);
#endif

#ifdef GADT_BITBOARD_DEBUG_INFO
				_debug_data[index] = 0;
#endif
				gadt_int64 temp = SINGLE_FEATURE;
				temp = ~(temp << (index * BIT_WIDTH));
				_data = _data & temp;
			}

			//reset all bits.
			inline void reset()
			{
				_data = 0;
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
				{
					_debug_data[i] = 0;
				}
#endif
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				_total = 0;
#endif
			}

			//get bit.
			inline size_t get(size_t index) const
			{
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= upper_bound(), "out of range.");
				return (_data >> (index * BIT_WIDTH)) & SINGLE_FEATURE;
			}

			//get bit.
			inline size_t operator[](size_t index) const
			{
				return get(index);
			}

			//get total
			inline size_t total() const
			{
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				return _total;
#else
				return get_total_by_count();
#endif
			}

			//self increament.
			inline void increase(size_t index)
			{
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= upper_bound(), "out of range.");
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, get(index) == upper_bound() - 1, "overflow.");
#ifdef GADT_BITBOARD_DEBUG_INFO
				_debug_data[index] ++;
#endif
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				_total++;
#endif
				uint64_t temp = 1;
				temp = temp << (index * BIT_WIDTH);
				_data += temp;
			}

			//self decreament.
			inline void decrease(size_t index)
			{
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, index >= upper_bound(), "out of range.");
				GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, get(index) == 0, "overflow.");
#ifdef GADT_BITBOARD_DEBUG_INFO
				_debug_data[index] --;
#endif
#ifdef GADT_BITBOARD_CONSTANT_TOTAL
				_total--;
#endif
				uint64_t temp = 1;
				temp = temp << (index * BIT_WIDTH);
				_data -= temp;
			}

			//add one card in this card group.
			inline void push(size_t index)
			{
				increase(index);
			}

			//return true if target is subset of this.
			bool exist_subset(const BitArray<BIT_WIDTH>& target) const
			{
				for (size_t i = 0; i < target.upper_bound(); i++)
				{
					if (target[i] > get(i))
					{
						return false;
					}
				}
				return true;
			}

			//return true if this is a subset of target.
			bool is_subset_of(const BitArray<BIT_WIDTH>& target) const
			{
				for (size_t i = 0; i < upper_bound(); i++)
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
				for (int i = 0; i < 64; i += BIT_WIDTH)
					ss << ((_data >> i) & SINGLE_FEATURE) << ", ";
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
				return UPPER_BOUND;
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

			inline void operator+=(const BitArray<BIT_WIDTH> target)
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < upper_bound(); i++)
					GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, get(i) + target.get(i) > SINGLE_FEATURE, ">> WARNING:: function BITGROUP::Plus overflow.");
#endif
				_data += target._data;
				refresh();
			}
			inline void operator-=(const BitArray<BIT_WIDTH> target)
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < upper_bound(); i++)
					GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, get(i) < target.get(i), ">> WARNING:: function BITGROUP::Plus overflow.");
#endif
				_data -= target._data;
				refresh();
			}
			inline void operator&=(const BitArray<BIT_WIDTH> target)
			{
				_data &= target._data;
				refresh();
			}
			inline void operator|=(const BitArray<BIT_WIDTH> target)
			{
				_data |= target._data;
				refresh();
			}
			inline BitArray<BIT_WIDTH> operator+(const BitArray<BIT_WIDTH> target) const
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < upper_bound(); i++)
				{
					GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, get(i) + target.get(i) > SINGLE_FEATURE, "overflow.");
				}
#endif

				BitArray<BIT_WIDTH> temp(_data + target._data);
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
				{
					temp._debug_data[i] = _debug_data[i] + target._debug_data[i];
				}
#endif
				return temp;
			}
			inline BitArray<BIT_WIDTH> operator-(const BitArray<BIT_WIDTH> target) const
			{
#ifdef GADT_WARNING
				for (size_t i = 0; i < upper_bound(); i++)
				{
					GADT_WARNING_IF(GADT_BITBOARD_ENABLE_WARNING, get(i) < target.get(i), "overflow.");
				}
#endif
				BitArray<BIT_WIDTH> temp(_data - target._data);
#ifdef GADT_BITBOARD_DEBUG_INFO
				for (size_t i = 0; i < upper_bound(); i++)
				{
					temp._debug_data[i] = _debug_data[i] - target._debug_data[i];
				}
#endif
				return temp;
			}
			inline bool operator==(const BitArray<BIT_WIDTH> target) const
			{
				return _data == target._data;
			}
			inline bool operator<(const BitArray<BIT_WIDTH> target) const
			{
				return _data < target._data;
			}
			inline bool operator>(const BitArray<BIT_WIDTH> target) const
			{
				return _data > target._data;
			}
			inline bool operator<=(const BitArray<BIT_WIDTH> target) const
			{
				return _data <= target._data;
			}
			inline bool operator>=(const BitArray<BIT_WIDTH> target) const
			{
				return _data >= target._data;
			}
			inline size_t operator*(const BitArray<BIT_WIDTH> target) const
			{
				size_t t = 0;
				for (size_t i = 0; i < upper_bound(); i++)
				{
					t += (get(i) * target.get(i));
				}
				return t;
			}
			inline BitArray<BIT_WIDTH> operator&(const BitArray<BIT_WIDTH> target) const
			{
				return BitArray<BIT_WIDTH>(_data & target._data);
			}
			inline BitArray<BIT_WIDTH> operator|(const BitArray<BIT_WIDTH> target) const
			{
				return BitArray<BIT_WIDTH>(_data | target._data);
			}
		};

		//a set of multi bit array.
		template<size_t BIT_WIDTH, size_t ARRAY_COUNT, typename std::enable_if<(BIT_WIDTH > 0 && BIT_WIDTH <= 32 && ARRAY_COUNT > 0), int>::type = 0>
		class BitArraySet
		{
		public:

			using BArray = BitArray<BIT_WIDTH>;

		private:

			std::array<BArray, ARRAY_COUNT> _arrays;

		public:

			inline BitArraySet():
				_arrays()
			{
			}

			//initilize BitArraySet by single BitArray
			inline BitArraySet(const BArray& barr)
			{
				for (auto& arr : _arrays)
					arr = barr;
			}

			//initilize BitArraySet by multi BitArray
			inline BitArraySet(std::initializer_list<BArray> init_list)
			{
				size_t index = 0;
				for (auto arr : init_list)
				{
					_arrays[index] = arr;
					index++;
					if (index >= ARRAY_COUNT)
						return;
				}
			}

			//return true if any bit is true.
			inline bool any() const
			{
				bool res = false;
				for (size_t i = 0; i < ARRAY_COUNT; i++)
					res |= _arrays[i].any();
				return res;
			}

			//return true if all bits are false.
			inline bool none() const
			{
				bool res = true;
				for (size_t i = 0; i < ARRAY_COUNT; i++)
					res &= _arrays[i].none();
				return res;
			}

			//get bitarray.
			inline const BArray& get(size_t index) const
			{
				return _arrays[index];
			}

			//get bit.
			inline BArray& operator[](size_t index)
			{
				return _arrays[index];
			}

			//get total
			inline size_t total() const
			{
				size_t count = 0;
				for (const auto& arr : _arrays)
					count += arr.total();
				return count;
			}

			//return true if target is subset of this.
			inline bool exist_subset(const BitArraySet<BIT_WIDTH, ARRAY_COUNT>& target) const
			{
				for (size_t i = 0; i < ARRAY_COUNT; i++)
				{
					if (get(i).exist_subset(target[i]) == false)
						return false;
				}
				return true;
			}

			//return true if this is a subset of target.
			inline bool is_subset_of(const BitArraySet<BIT_WIDTH, ARRAY_COUNT>& target) const
			{
				for (size_t i = 0; i < ARRAY_COUNT; i++)
				{
					if (get(i).is_subset_of(target[i]) == false)
						return false;
				}
				return true;
			}

			//get string
			inline std::string to_string() const
			{
				std::stringstream ss;
				ss << "{" << _arrays[0].to_string();
				for (size_t i = 1; i < ARRAY_COUNT; i++)
					ss << ", " << _arrays[i].to_string();
				ss << "}";
				return ss.str();
			}

			//get ullong string.
			inline std::string to_ullong_string() const
			{
				std::stringstream ss;
				ss << "[" << _arrays[0].to_ullong();
				for (size_t i = 1; i < ARRAY_COUNT; i++)
					ss << ", " << _arrays[i].to_ullong();
				ss << "]";
				return ss.str();
			}

			//begin of the iter
			inline BArray* begin()
			{
				return &_arrays[0];
			}

			//begin of the iter
			inline const BArray* cbegin() const
			{
				return &_arrays[0];
			}

			//end of the iter
			inline BArray* end()
			{
				return begin() + ARRAY_COUNT;
			}

			//end of the iter
			inline const BArray* cend() const
			{
				return cbegin() + ARRAY_COUNT;
			}

			//operation.
			inline void operator+=(const BitArraySet<BIT_WIDTH, ARRAY_COUNT> target)
			{
				for (size_t i = 0; i < ARRAY_COUNT; i++)
					_arrays[i] += target.get(i);
			}
			inline void operator-=(const BitArraySet<BIT_WIDTH, ARRAY_COUNT> target)
			{
				for (size_t i = 0; i < ARRAY_COUNT; i++)
					_arrays[i] -= target.get(i);
			}
			inline BitArraySet<BIT_WIDTH, ARRAY_COUNT> operator+(const BitArraySet<BIT_WIDTH, ARRAY_COUNT> target) const
			{
				auto temp = *this;
				temp += target;
				return temp;
			}
			inline BitArraySet<BIT_WIDTH, ARRAY_COUNT> operator-(const BitArraySet<BIT_WIDTH, ARRAY_COUNT> target) const
			{
				auto temp = *this;
				temp -= target;
				return temp;
			}
			inline bool operator==(const BitArraySet<BIT_WIDTH, ARRAY_COUNT> target) const
			{
				for (size_t i = 0; i < ARRAY_COUNT; i++)
				{
					if ((get(i) == target.get(i)) == false)
						return false;
				}
				return true;
			}
		};

		//type define.
		using BitBoard128   = BitBoard<128>;
		using BitBoard256   = BitBoard<256>;
		using BitPoker		= BitArray<4>;
		using BitMahjong	= BitArraySet<4, 4>;
	}
}
