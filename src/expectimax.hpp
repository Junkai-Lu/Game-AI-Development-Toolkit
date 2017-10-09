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
#include "gadt_algorithm.hpp"

#pragma once

namespace gadt
{
	namespace expectimax
	{
		/*
		* MctsSetting is the setting of MCTS.
		*
		* MctsSetting() would use default setting.
		* MctsSetting(params) would generate custom setting.
		*/
		struct ExpectimaxSetting final : public GameAlgorithmSettingBase
		{
			size_t max_depth;					//max depth of search

			//default setting constructor.
			ExpectimaxSetting() :
				GameAlgorithmSettingBase(),
				max_depth(0)
			{
			}

			//custom setting constructor.
			ExpectimaxSetting(
				double _timeout,
				AgentIndex _no_winner_index,
				size_t _max_depth
			) :
				GameAlgorithmSettingBase(_timeout,_no_winner_index),
				max_depth(_max_depth)
			{
			}

			//output print with str behind each line.
			std::string info() const override
			{
				log::ConsoleTable tb(2, 2);
				tb.set_width({ 12,6 });
				tb.enable_title({ "EXPECTIMAX SETTING" });
				tb.set_cell_in_row(0, { { "timeout" },{ ToString(timeout) } });
				tb.set_cell_in_row(1, { { "depth" },{ ToString(max_depth) } });
				return tb.output_string();
			}
		};

		class ExpectimaxNode
		{

		};
	}
}