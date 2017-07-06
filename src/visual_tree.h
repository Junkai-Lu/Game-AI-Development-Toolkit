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
	namespace visual_tree
	{
		using NodeDict = std::map<std::string, std::string>;

		class TreeNode
		{
		private:
			using ChildPtrSet = std::vector<TreeNode*>;

		private:
			ChildPtrSet _childs;
			size_t		_depth;
			NodeDict	_dict;

		private:



		public:
			inline TreeNode()
			{

			}

			//return true if the keye exist in the dict.
			inline bool exist(std::string key)
			{
				return _dict.count(key) != 0;
			}

			//add key to node.
			inline void add_value(std::string key, std::string value)
			{
				_dict[key] = value;
			}

			inline void create_child()
			{
				
			}

			inline TreeNode* last_child()
			{

			}

			size_t depth() const
			{
				return _depth;
			}
		};


	}
}