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
	namespace filesystem
	{
#ifdef __GADT_MSVC
		//convert string to wstring.
		std::wstring StringToWString(const std::string& str);
#endif

		//return true if the folder exists.
		bool exist_directory(std::string dir_path);

		//create directory and return true if create successfully.
		bool create_directory(std::string dir_path);

		//remove dir and return true if remove successfully. 
		bool remove_directory(std::string dir_path);

		//return true if the file exists.
		bool exist_file(std::string file_path);

		//remove file and return true if remove successfully.
		bool remove_file(std::string file_path);

		//load file then convert it to string. return "" if convert failed.
		std::string load_file_as_string(std::string file_path);

		//save string as file.
		bool save_string_as_file(std::string str, std::string file_path);

		/*
		//copy directory to target path.
		bool copy_directory(std::string from_path, std::string to_path);

		//copy file to target path.
		bool copy_file(std::string from_path, std::string to_path, bool fail_if_exist);

		//move directory to target path
		bool move_directory(std::string from_path, std::string to_path);

		//move file to target path
		bool move_file(std::string from_path, std::string to_path);*/
	}
}
