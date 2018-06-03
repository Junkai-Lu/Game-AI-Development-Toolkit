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


//include some extra libaries.
#include "../lib/json11/json11.hpp"

// exclude unsupported compilers and define some marco by compiler.
#ifdef _MSC_VER
	#define __GADT_MSVC
	#include <SDKDDKVer.h>
	#include <Windows.h>
	#include <io.h>
	#include <direct.h>
	#include <tchar.h>
#elif defined(__GNUC__)
	#define __GADT_GNUC
	#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40900
		#error "unsupported GCC version"
	#endif
	#include <sys/stat.h>  
	#include <sys/types.h>
	#include <errno.h> 
	#include <unistd.h>
	#include <dirent.h>
#else
	#error "unsupported compiler"
#endif

//open debug-info option to include extra info , this would lead to little performance penalties.
#define GADT_DEBUG_INFO

//warning option allow the functions in GADT execute parameter check and report wrong parameters.
#define GADT_WARNING

//a marco use for parameters check.
#ifdef GADT_WARNING
	#define GADT_CHECK_WARNING(enable, condition, reason) gadt::console::WarningCheck(enable && condition, reason, __FILE__, __LINE__, __FUNCTION__);
#else
	#define GADT_CHECK_WARNING(enable, condition, reason)
#endif

#include <cstdio>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <memory>
#include <thread>
#include <functional>
#include <type_traits>

#pragma once

namespace gadt
{
#ifdef GADT_WARNING
	constexpr const bool GADT_STL_ENABLE_WARNING = true;
#else
	constexpr const bool GADT_STL_ENABLE_WARNING = false;
#endif

	using AgentIndex = int8_t;//AgentIndex is the index of each player, default is int8_t. 0 is the default no-winner index.
	using UcbValue = double;
	using EvalValue = double;

	//extern BasicUnsignedCoordinate.
	template<typename IntType = uint64_t, typename std::enable_if<std::is_unsigned<IntType>::value, int>::type = 0>
	struct BasicUnsignedCoordinate;

	//struct BasicCoordinate is used to express a plane coordinate of signed integer.
	template<typename IntType = int64_t, typename std::enable_if<std::is_signed<IntType>::value, int>::type = 0>
	struct BasicCoordinate
	{
		IntType x;
		IntType y;

		BasicCoordinate() : x(0), y(0) {}

		BasicCoordinate(IntType _x, IntType _y) : x(_x), y(_y) {}

		//bool operation is banned.
		inline operator bool() = delete;

		//convert to singed coordinate
		template<typename T = uint64_t, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline BasicUnsignedCoordinate<T> to_unsigned() const
		{
			return { T(x),T(y) };
		}

		//comparison operation
		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline bool operator==(BasicCoordinate<T> coord) const
		{
			return x == coord.x && y == coord.y;
		}

		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline bool operator!=(BasicCoordinate<T> coord) const
		{
			return x != coord.x || y != coord.y;
		}

		//with integer.(multiply or divide)
		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline BasicCoordinate<IntType> operator*(T i) const
		{
			return BasicCoordinate<IntType>(x*i, y*i);
		}

		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline BasicCoordinate<IntType> operator/(T i) const
		{
			return BasicCoordinate<IntType>(x / i, y / i);
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

		//with coordinate(plus or reduce).
		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline BasicCoordinate<IntType> operator+(BasicCoordinate<T> coord) const
		{
			return BasicCoordinate<IntType>(x + coord.x, y + coord.y);
		}

		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline BasicCoordinate<IntType> operator-(BasicCoordinate<T> coord) const
		{
			return BasicCoordinate<IntType>(x - coord.x, y - coord.y);
		}

		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline void operator+=(BasicCoordinate<T> coord)
		{
			x += coord.x;
			y += coord.y;
		}

		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline void operator-=(BasicCoordinate<T> coord)
		{
			x -= coord.x;
			y -= coord.y;
		}

		//multiply with another coordinate(the result would be a integer).
		template<typename T, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline IntType operator*(BasicCoordinate<T> coord) const
		{
			return IntType((x * coord.x) + (y* coord.y));
		}

		//swap
		inline void swap()
		{
			IntType t = x; x = y; y = t;
		}

		//swap with another coordinate.
		inline void swap(BasicCoordinate<IntType>& coord)
		{
			BasicCoordinate<IntType> t = coord; *this = coord; coord = *this;
		}

		//convert to  
		std::string to_string() const
		{
			std::stringstream ss;
			ss << "[" << x << "," << y << "]";
			return ss.str();
		}
	};

	//struct BasicUnsignedCoordinate is used to express a plane coordinate of unsigned integer.
	template<typename IntType, typename std::enable_if<std::is_unsigned<IntType>::value, int>::type>
	struct BasicUnsignedCoordinate
	{
		IntType x;
		IntType y;

		BasicUnsignedCoordinate() : x(0), y(0) {}

		BasicUnsignedCoordinate(IntType _x, IntType _y) : x(_x), y(_y) {}

		//bool operation is banned.
		inline operator bool() = delete;

		//convert to singed coordinate
		template<typename T = int, typename std::enable_if<std::is_signed<T>::value, int>::type = 0>
		inline BasicCoordinate<T> to_signed() const
		{
			return { T(x),T(y) };
		}

		//comparison operation
		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline bool operator==(BasicUnsignedCoordinate<T> coord) const
		{
			return x == coord.x && y == coord.y;
		}

		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline bool operator!=(BasicUnsignedCoordinate<T> coord) const
		{
			return x != coord.x || y != coord.y;
		}

		//with integer.(multiply or divide)
		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline BasicUnsignedCoordinate<IntType> operator*(T i) const
		{
			return BasicUnsignedCoordinate<IntType>(x*i, y*i);
		}

		template<typename T, typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
		inline BasicUnsignedCoordinate<IntType> operator/(T i) const
		{
			return BasicUnsignedCoordinate<IntType>(x /i, y /i);
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

		//with coordinate(plus or reduce).
		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline BasicUnsignedCoordinate<IntType> operator+(BasicUnsignedCoordinate<T> coord) const
		{
			return BasicUnsignedCoordinate<IntType>(x + coord.x, y + coord.y);
		}

		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline BasicUnsignedCoordinate<IntType> operator-(BasicUnsignedCoordinate<T> coord) const
		{
			return BasicUnsignedCoordinate<IntType>(x - coord.x, y - coord.y);
		}

		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline void operator+=(BasicUnsignedCoordinate<T> coord)
		{
			x += coord.x;
			y += coord.y;
		}

		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline void operator-=(BasicUnsignedCoordinate<T> coord)
		{
			x -= coord.x;
			y -= coord.y;
		}

		//multiply with another coordinate(the result would be a integer).
		template<typename T, typename std::enable_if<std::is_unsigned<T>::value, int>::type = 0>
		inline IntType operator*(BasicUnsignedCoordinate<T> coord) const
		{
			return IntType((x * coord.x) + (y* coord.y));
		}

		//swap
		inline void swap()
		{
			IntType t = x; x = y; y = t;
		}

		//swap with another coordinate.
		inline void swap(BasicUnsignedCoordinate<IntType>& coord)
		{
			BasicUnsignedCoordinate<IntType> t = coord; *this = coord; coord = *this;
		}

		//convert to  
		std::string to_string() const
		{
			std::stringstream ss;
			ss << "[" << x << "," << y << "]";
			return ss.str();
		}
	};

	//default coordinate.
	using Coordinate = BasicCoordinate<int>;

	//default unsigned coordinate.
	using UnsignedCoordinate = BasicUnsignedCoordinate<size_t>;

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

	//bool convert to string.
	inline std::string ToString(bool data)
	{
		return data ? "true" : "false";
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
		enum ConsoleColor
		{
			COLOR_DEEP_BLUE = 1,
			COLOR_DEEP_GREEN = 2,
			COLOR_DEEP_CYAN = 3,
			COLOR_BROWN = 4,
			COLOR_PURPLE = 5,
			COLOR_DEEP_YELLOW = 6,
			COLOR_DEFAULT = 7,
			COLOR_GRAY = 8,
			COLOR_BLUE = 9,
			COLOR_GREEN = 10,
			COLOR_CYAN = 11,
			COLOR_RED = 12,
			COLOR_PINK = 13,
			COLOR_YELLOW = 14,
			COLOR_WHITE = 15
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
				return _index >= COLOR_BLUE && _index <= COLOR_YELLOW;
			}

			//return true if current color is deep
			inline bool is_deep() const
			{
				return _index >= COLOR_DEEP_BLUE && _index <= COLOR_DEEP_YELLOW;
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
				return get_color(COLOR_BLUE + rnd);
			}

			//get random deep color
			inline ConsoleColor GetRandomDeep() const
			{
				size_t rnd = rand() % 6;
				return get_color(COLOR_DEEP_BLUE + rnd);
			}

			//get random any color
			inline ConsoleColor GetRandomAny() const
			{
				size_t rnd = rand() % 12;
				if (rnd >= 6)
					rnd += 2;
				return get_color(COLOR_DEEP_BLUE + rnd);
			}

			//get tint color by index.
			template<typename Index, typename std::enable_if<std::is_integral<Index>::value ,int>::type = 0>
			inline ConsoleColor GetTintByIndex(Index index) const
			{
				size_t remain = static_cast<size_t>(index) % 6;
				return get_color(COLOR_BLUE + remain);
			}

			//get deep color by index.
			template<typename Index, typename std::enable_if<std::is_integral<Index>::value, int>::type = 0>
			inline ConsoleColor GetDeepByIndex(Index index) const
			{
				size_t remain = static_cast<size_t>(index) % 6;
				return get_color(COLOR_DEEP_BLUE + remain);
			}

			//get any color by index.
			template<typename Index, typename std::enable_if<std::is_integral<Index>::value, int>::type = 0>
			inline ConsoleColor GetAnyByIndex(Index index) const
			{
				size_t remain = static_cast<size_t>(index) % 12;
				if (remain >= 6)
					remain += 2;
				return get_color(COLOR_DEEP_BLUE + remain);
			}
		};

		//print endline.
		template<size_t COUNT = 1>
		inline void EndLine()
		{
			for (size_t i = 0; i < COUNT; i++)
				std::cout << std::endl;
		}

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

		//show error in terminal.
		void ShowError(std::string reason);

		//show message in terminal.
		void ShowMessage(std::string message);

		//if 'condition' is true that report detail.
		void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function);

		//system pause.
		void SystemPause();

		//system clear
		void SystemClear();
	}

	namespace timer
	{
		std::string TimeString(std::string format = "%Y.%m.%d-%H:%M:%S");
		clock_t GetClock();
		double GetTimeDifference(const clock_t& start);
		
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

			//get the time since this time point was created.
			inline double time_since_created() const
			{
				return (double)(clock() - _clock) / CLOCKS_PER_SEC;
			}
		};
	}

	namespace file
	{
		//return true if the folder exists.
		bool DirExist(std::string dir_path);
		
		//create dir and return true if create successfully.
		bool MakeDir(std::string dir_path);

		//remove dir and return true if remove successfully. 
		bool RemoveDir(std::string dir_path);

		//return true if the file exists.
		bool FileExist(std::string file_path);

		//remove file and return true if remove successfully.
		bool RemoveFile(std::string file_path);

		//convert a file to string. return "" if convert failed.
		std::string FileToString(std::string file_path);

		//convert a string to file.
		bool StringToFile(std::string str, std::string file_path);
	}

	namespace func
	{
		//get mex element in vector.
		template<typename T>
		size_t GetMaxElement(const std::vector<T>& vec)
		{
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

		//get max element in vector.
		template<typename T>
		size_t GetMaxElement(const std::vector<T>& vec, std::function<bool(const T&, const T&)> more_than)
		{
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

		//get random elements from a vector.
		template<typename T>
		const T& GetRandomElement(const std::vector<T>& vec)
		{
			GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, vec.size() == 0, "empty container");
			size_t rnd = rand() % vec.size();
			return vec[rnd];
		}
		
		//get manhattan distance between two coordinate.
		size_t GetManhattanDistance(Coordinate fir, Coordinate sec);
		
	}
}