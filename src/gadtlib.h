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
* LIABILITY, WHETHER IN AN T OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "gadt_config.h"

//a marco use for parameters check.
#ifdef GADT_WARNING
	#define GADT_WARNING_IF(enable_condition, condition, reason) ::gadt::console::WarningCheck(enable_condition && condition, reason, __FILE__, __LINE__, __FUNCTION__);
#else
	#define GADT_WARNING_IF(enable_condition, condition, reason)
#endif

#pragma once

namespace gadt
{
	using AgentIndex = int8_t;//AgentIndex is the index of each player, default is int8_t. 0 is the default no-winner index.
	using UcbValue = double;
	using EvalValue = double;

	//extern BasicUPoint.
	template<typename IntType = uint64_t, typename std::enable_if<std::is_unsigned<IntType>::value, int>::type = 0>
	struct BasicUPoint;

	//struct BasicPoint is used to express a plane Point of signed integer.
	template<typename IntType = int64_t, typename std::enable_if<std::is_signed<IntType>::value, int>::type = 0>
	struct BasicPoint
	{
		IntType x;
		IntType y;

		constexpr BasicPoint() noexcept: x(0), y(0) {}

		constexpr BasicPoint(IntType _x, IntType _y) noexcept : x(_x), y(_y) {}

		//bool operation is banned.
		inline operator bool() = delete;

		//convert to singed point
		template<typename T = uint64_t, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline BasicUPoint<T> to_unsigned() const
		{
			return { T(x),T(y) };
		}

		//comparison operation
		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline bool operator==(BasicPoint<T> point) const
		{
			return x == point.x && y == point.y;
		}

		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline bool operator!=(BasicPoint<T> point) const
		{
			return x != point.x || y != point.y;
		}

		//with integer.(multiply or divide)
		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline BasicPoint<IntType> operator*(T i) const
		{
			return BasicPoint<IntType>(x*i, y*i);
		}

		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline BasicPoint<IntType> operator/(T i) const
		{
			return BasicPoint<IntType>(x / i, y / i);
		}

		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline void operator*=(T i)
		{
			x *= i;
			y *= i;
		}

		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline void operator/=(T i)
		{
			x /= i;
			y /= i;
		}

		//with point(plus or reduce).
		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline BasicPoint<IntType> operator+(BasicPoint<T> point) const
		{
			return BasicPoint<IntType>(x + point.x, y + point.y);
		}

		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline BasicPoint<IntType> operator-(BasicPoint<T> point) const
		{
			return BasicPoint<IntType>(x - point.x, y - point.y);
		}

		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline void operator+=(BasicPoint<T> point)
		{
			x += point.x;
			y += point.y;
		}

		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline void operator-=(BasicPoint<T> point)
		{
			x -= point.x;
			y -= point.y;
		}

		//multiply with another point(the result would be a integer).
		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline IntType operator*(BasicPoint<T> point) const
		{
			return IntType((x * point.x) + (y* point.y));
		}

		//swap
		inline void swap()
		{
			IntType t = x; x = y; y = t;
		}

		//swap with another point.
		inline void swap(BasicPoint<IntType>& point)
		{
			BasicPoint<IntType> t = point; *this = point; point = *this;
		}

		//convert to  
		std::string to_string() const
		{
			std::stringstream ss;
			ss << "[" << x << "," << y << "]";
			return ss.str();
		}
	};

	//struct BasicUPoint is used to express a plane Point of unsigned integer.
	template<typename IntType, typename std::enable_if<std::is_unsigned<IntType>::value, int>::type>
	struct BasicUPoint
	{
		IntType x;
		IntType y;

		constexpr BasicUPoint() noexcept : x(0), y(0) {}

		constexpr BasicUPoint(IntType _x, IntType _y) noexcept : x(_x), y(_y) {}

		//bool operation is banned.
		inline operator bool() = delete;

		//convert to singed point
		template<typename T = int, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline constexpr BasicPoint<T> to_signed() const
		{
			return { T(x),T(y) };
		}

		//comparison operation
		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline bool operator==(BasicUPoint<T> point) const
		{
			return x == point.x && y == point.y;
		}

		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline bool operator!=(BasicUPoint<T> point) const
		{
			return x != point.x || y != point.y;
		}

		//with integer.(multiply or divide)
		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline BasicUPoint<IntType> operator*(T i) const
		{
			return BasicUPoint<IntType>(x*i, y*i);
		}

		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline BasicUPoint<IntType> operator/(T i) const
		{
			return BasicUPoint<IntType>(x /i, y /i);
		}

		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline void operator*=(T i)
		{
			x *= i;
			y *= i;
		}

		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline void operator/=(T i)
		{
			x /= i;
			y /= i;
		}

		//with point(plus or reduce).
		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline BasicUPoint<IntType> operator+(BasicUPoint<T> point) const
		{
			return BasicUPoint<IntType>(x + point.x, y + point.y);
		}

		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline BasicUPoint<IntType> operator-(BasicUPoint<T> point) const
		{
			return BasicUPoint<IntType>(x - point.x, y - point.y);
		}

		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline void operator+=(BasicUPoint<T> point)
		{
			x += point.x;
			y += point.y;
		}

		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline void operator-=(BasicUPoint<T> point)
		{
			x -= point.x;
			y -= point.y;
		}

		//multiply with another point(the result would be a integer).
		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline IntType operator*(BasicUPoint<T> point) const
		{
			return IntType((x * point.x) + (y* point.y));
		}

		//swap
		inline void swap()
		{
			IntType t = x; x = y; y = t;
		}

		//swap with another point.
		inline void swap(BasicUPoint<IntType>& point)
		{
			BasicUPoint<IntType> t = point; *this = point; point = *this;
		}

		//convert to  
		std::string to_string() const
		{
			std::stringstream ss;
			ss << "[" << x << "," << y << "]";
			return ss.str();
		}
	};

	//default point.
	using Point = BasicPoint<int>;

	//default unsigned point.
	using UPoint = BasicUPoint<size_t>;

	//srting to interger.
	template<typename T, typename ReturnType = int, typename std::enable_if<std::is_integral<ReturnType>::value, int>::type = 0>
	inline ReturnType ToInt(const T& data)
	{
		std::stringstream ss;
		ss << data;
		ReturnType res = 0;
		ss >> res;
		return res;
	}

	//convert to int8_t
	template<typename T>
	inline int8_t ToInt8(const T& data)
	{
		return ToInt<T, int8_t>(data);
	}

	//convert to int16_t
	template<typename T>
	inline int16_t ToInt16(const T& data)
	{
		return ToInt<T, int16_t>(data);
	}

	//convert to int32_t
	template<typename T>
	inline int32_t ToInt32(const T& data)
	{
		return ToInt<T, int32_t>(data);
	}

	//convert to int64_t
	template<typename T>
	inline int64_t ToInt64(const T& data)
	{
		return ToInt<T, int64_t>(data);
	}

	//convert to uint8_t
	template<typename T>
	inline uint8_t ToUInt8(const T& data)
	{
		return ToInt<T, uint8_t>(data);
	}

	//convert to uint16_t
	template<typename T>
	inline uint16_t ToUInt16(const T& data)
	{
		return ToInt<T, uint16_t>(data);
	}

	//convert to uint32_t
	template<typename T>
	inline uint32_t ToUInt32(const T& data)
	{
		return ToInt<T, uint32_t>(data);
	}

	//convert to uint64_t
	template<typename T>
	inline uint64_t ToUInt64(const T& data)
	{
		return ToInt<T, uint64_t>(data);
	}

	//convert to size_t
	template<typename T>
	inline size_t ToSizeT(const T& data)
	{
		return ToInt<T, size_t>(data);
	}

	//convert to string.
	template<typename T>
	inline std::string ToString(T data)
	{
		std::stringstream ss;
		ss << data;
		return ss.str();
	}

	namespace console
	{
		//various delimiter string
		const std::string DELIMITER_STR_TAB = "	";
		const std::string DELIMITER_STR_SPACE = " ";
		const std::string DELIMITER_STR_EMPTY = "";

		//console color type
		enum class ConsoleColor
		{
			DeepBlue = 1,
			DeepGreen = 2,
			DeepCyan = 3,
			Brown = 4,
			Purple = 5,
			DeepYellow = 6,
			Default = 7,
			Gray = 8,
			Blue = 9,
			Green = 10,
			Cyan = 11,
			Red = 12,
			Pink = 13,
			Yellow = 14,
			White = 15
		};

		//color ostream.
		class costream
		{
		private:
			static ConsoleColor _current_color;

			//change colors by setting using winapi in windows or output string in linux.
			static void change_color(ConsoleColor color);

		public:
			costream& operator<<(ConsoleColor color)
			{
				change_color(color);
				return *this;
			}

			template<typename datatype>
			inline costream& operator<<(datatype d)
			{
				std::cout << d;
				return *this;
			}

			template <typename datatype>
			static inline void print(datatype data, ConsoleColor color)
			{
				ConsoleColor temp_color = _current_color;
				change_color(color);
				std::cout << data;
				change_color(temp_color);
			}
		};

		//color allocator is a class that can allocate different colors.
		class ColorAllocator
		{
		private:

			size_t _index;

		private:

			//to next index
			inline void to_next()
			{
				_index++;
				if (_index == 7)
					_index = 9;//pass white and gray.
				if (_index >= 16)
					_index = 1;
			}

			//return true if current color is tint
			inline bool is_tint() const
			{
				return _index >= static_cast<size_t>(ConsoleColor::Blue) && _index <= static_cast<size_t>(ConsoleColor::Yellow);
			}

			//return true if current color is deep
			inline bool is_deep() const
			{
				return _index >= static_cast<size_t>(ConsoleColor::DeepBlue) && _index <= static_cast<size_t>(ConsoleColor::DeepYellow);
			}

			//convert index to color.
			inline ConsoleColor get_color(size_t index) const
			{
				return static_cast<ConsoleColor>(index);
			}

			//get current color
			inline ConsoleColor current_color() const
			{
				return get_color(_index);
			}

		public:

			//default constructor.
			inline ColorAllocator() :
				_index(16)
			{
			}

			//get next tint color
			inline ConsoleColor GetTint()
			{
				to_next();
				while (is_tint() == false)
					to_next();
				return current_color();
			}

			//get next deep color
			inline ConsoleColor GetDeep()
			{
				to_next();
				while (is_deep() == false)
					to_next();
				return current_color();
			}

			//get next any color but white and gray.
			inline ConsoleColor GetAny()
			{
				to_next();
				return current_color();
			}

			//get random tint color
			inline ConsoleColor GetRandomTint() const
			{
				size_t rnd = rand() % 6;
				return get_color(static_cast<size_t>(ConsoleColor::Blue) + rnd);
			}

			//get random deep color
			inline ConsoleColor GetRandomDeep() const
			{
				size_t rnd = rand() % 6;
				return get_color(static_cast<size_t>(ConsoleColor::DeepBlue) + rnd);
			}

			//get random any color
			inline ConsoleColor GetRandomAny() const
			{
				size_t rnd = rand() % 12;
				if (rnd >= 6)
					rnd += 2;
				return get_color(static_cast<size_t>(ConsoleColor::DeepBlue) + rnd);
			}

			//get tint color by index.
			template<typename Index, typename std::enable_if<std::is_integral<Index>::value ,int>::type = 0>
			inline ConsoleColor GetTintByIndex(Index index) const
			{
				size_t remain = static_cast<size_t>(index) % 6;
				return get_color(ConsoleColor::Blue + remain);
			}

			//get deep color by index.
			template<typename Index, typename std::enable_if<std::is_integral<Index>::value, int>::type = 0>
			inline ConsoleColor GetDeepByIndex(Index index) const
			{
				size_t remain = static_cast<size_t>(index) % 6;
				return get_color(ConsoleColor::DeepBlue + remain);
			}

			//get any color by index.
			template<typename Index, typename std::enable_if<std::is_integral<Index>::value, int>::type = 0>
			inline ConsoleColor GetAnyByIndex(Index index) const
			{
				size_t remain = static_cast<size_t>(index) % 12;
				if (remain >= 6)
					remain += 2;
				return get_color(ConsoleColor::DeepBlue + remain);
			}
		};

		//colorful print.
		template<typename T>
		inline void Cprintf(T data, ConsoleColor color)
		{
			costream::print<T>(data, color);
		}

		//get input from keyboard as string.
		template<typename T = std::string>
		T GetInput(std::string tip = "Input >>")
		{
			std::cout << tip;
			T input;
			std::cin.clear();
			std::cin.sync();
			std::cin >> input;
			return input;
		}

		//get user confirm(Y/N or y/n)
		bool GetUserConfirm(std::string tip);

		//print PrintEndLine.
		template<size_t COUNT = 1>
		inline void PrintEndLine()
		{
			for (size_t i = 0; i < COUNT; i++)
				std::cout << std::endl;
		}

		//show error in terminal.
		void PrintError(std::string reason);

		//show message in terminal.
		void PrintMessage(std::string message);

		//if 'condition' is true that report detail.
		void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function);

		//system pause.
		void SystemPause();

		//system clear
		void SystemClear();
	}

	namespace timer
	{
		class TimePoint
		{
		private:
			clock_t _clock;
			time_t _time;
		public:

			inline TimePoint()
			{
				reset();
			}

			//get string of this time point.
			std::string get_string(std::string format = "%Y.%m.%d-%H:%M:%S") const;

			//set this time point to current time.
			inline void reset()
			{
				_clock = clock();
				_time = time(NULL);
			}

			//get the seconds since this time point was created.
			inline double time_since_created() const
			{
				return (double)(clock() - _clock) / CLOCKS_PER_SEC;
			}
		};
	}

	namespace func
	{
		//get the index of max element in vector.
		template<typename T>
		size_t GetMaxElementIndex(const std::vector<T>& vec)
		{
			GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, vec.size() == 0, "empty vector in " + std::string(__FUNCTION__));
			size_t best_index = 0;
			const T* best_ele = &vec[0];
			for (size_t i = 1; i < vec.size(); i++)
			{
				if (vec[i] > *best_ele)
				{
					best_ele = &vec[i];
					best_index = i;
				}
			}
			return best_index;
		}

		//get the index of max element in vector.
		template<typename T>
		size_t GetMaxElementIndex(const std::vector<T>& vec, std::function<bool(const T&, const T&)> more_than)
		{
			GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, vec.size() == 0, "empty vector in " + std::string(__FUNCTION__));
			size_t best_index = 0;
			T* best_ele = &vec[0];
			for (size_t i = 1; i < vec.size(); i++)
			{
				if (more_than(vec[i], *best_ele))
				{
					best_ele = &vec[i];
					best_index = i;
				}
			}
			return best_index;
		}

		//get max element in vector.
		template<typename T>
		const T& GetMaxElement(const std::vector<T>& vec)
		{
			GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, vec.size() == 0, "empty vector in " + std::string(__FUNCTION__));
			return vec[GetMaxElementIndex<T>(vec)];
		}

		//get max element in vector.
		template<typename T>
		const T& GetMaxElement(const std::vector<T>& vec, std::function<bool(const T&, const T&)> more_than)
		{
			GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, vec.size() == 0, "empty vector in " + std::string(__FUNCTION__));
			return vec[GetMaxElementIndex<T>(vec, more_than)];
		}

		//get the index of Min element in vector.
		template<typename T>
		size_t GetMinElementIndex(const std::vector<T>& vec)
		{
			GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, vec.size() == 0, "empty vector in " + std::string(__FUNCTION__));
			size_t best_index = 0;
			const T* best_ele = &vec[0];
			for (size_t i = 1; i < vec.size(); i++)
			{
				if (vec[i] < *best_ele)
				{
					best_ele = &vec[i];
					best_index = i;
				}
			}
			return best_index;
		}

		//get the index of Min element in vector.
		template<typename T>
		size_t GetMinElementIndex(const std::vector<T>& vec, std::function<bool(const T&, const T&)> less_than)
		{
			GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, vec.size() == 0, "empty vector in " + std::string(__FUNCTION__));
			size_t best_index = 0;
			T* best_ele = &vec[0];
			for (size_t i = 1; i < vec.size(); i++)
			{
				if (less_than(vec[i], *best_ele))
				{
					best_ele = &vec[i];
					best_index = i;
				}
			}
			return best_index;
		}

		//get Min element in vector.
		template<typename T>
		const T& GetMinElement(const std::vector<T>& vec)
		{
			GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, vec.size() == 0, "empty vector in " + std::string(__FUNCTION__));
			return vec[GetMinElementIndex<T>(vec)];
		}

		//get Min element in vector.
		template<typename T>
		const T& GetMinElement(const std::vector<T>& vec, std::function<bool(const T&, const T&)> less_than)
		{
			GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, vec.size() == 0, "empty vector in " + std::string(__FUNCTION__));
			return vec[GetMinElementIndex<T>(vec, less_than)];
		}

		//get random elements from a vector.
		template<typename T>
		const T& GetRandomElement(const std::vector<T>& vec)
		{
			GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, vec.size() == 0, "empty container");
			size_t rnd = rand() % vec.size();
			return vec[rnd];
		}
		
		//get sum of a container.
		template<typename SumType, typename ContainerType>
		SumType GetElementSum(const ContainerType& container)
		{
			SumType sum = SumType();
			for (auto elem : container)
				sum += elem;
			return sum;
		}

		//get manhattan distance between two point.
		size_t GetManhattanDistance(Point fir, Point sec);
		
	}
}