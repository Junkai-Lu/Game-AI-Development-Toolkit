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

#pragma once

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
	#include "sys/sysinfo.h"
	#include <string.h>
	#include <errno.h> 
	#include <unistd.h>
	#include <dirent.h>
#else
	#error "unsupported compiler, please check complier or delete this error in 'gadt_config.h'"
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
#include <array>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <memory>
#include <thread>
#include <functional>
#include <algorithm>
#include <type_traits>
#include <chrono>
#include <unordered_map>

//extra libaries.
#include "../lib/json11/json11.hpp"

//warning option allow the functions in GADT execute parameter check and report wrong parameters.
#define GADT_WARNING

namespace gadt
{
	constexpr bool GADT_BITBOARD_ENABLE_WARNING		= true;	//enable warning in module 'bitboard'
	constexpr bool GADT_STL_ENABLE_WARNING			= true;	//enable warning in all modules in namespace 'stl'
	constexpr bool GADT_TABLE_ENABLE_WARNING		= true;	//enable warning in module 'table'.
	constexpr bool GADT_SHELL_ENABLE_WARNING		= true;	//enable warning in module 'shell'.
	constexpr bool GADT_VISUAL_TREE_ENABLE_WARNING	= true; //enable warning in module 'visual_tree'
}