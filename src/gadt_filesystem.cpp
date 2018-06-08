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

#include "gadt_filesystem.h"

namespace gadt
{
	namespace filesystem
	{
#ifdef __GADT_MSVC 
		//convert string to wstring.
		std::wstring StringToWString(const std::string& str)
		{
			int size_str = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
			wchar_t *wide = new wchar_t[size_str];
			MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, size_str);
			std::wstring return_wstring(wide);
			delete[] wide;
			wide = NULL;
			return return_wstring;
		}
#endif

		//return true if the folder exists.
		bool exist_directory(std::string dir_path)
		{
#ifdef __GADT_GNUC
			return (access(dir_path.c_str(), 0) != -1);
#elif defined(__GADT_MSVC)
			return (_access(dir_path.c_str(), 0) != -1);
#endif
		}

		//create directory and return true if create successfully.
		bool create_directory(std::string dir_path)
		{
			if (exist_directory(dir_path))
				return false;
#ifdef __GADT_MSVC
			return ::CreateDirectoryW(StringToWString(dir_path).c_str(), 0);
#else
			return mkdir(dir_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1;
#endif
		}

		//remove dir and return true if remove successfully. 
		bool remove_directory(std::string dir_path)
		{
			if (!exist_directory(dir_path))
				return false;
#ifdef __GADT_MSVC
			if (::RemoveDirectoryW(StringToWString(dir_path).c_str()) != 0)
#else
			if (rmdir(dir_path.c_str()) == 0)
#endif
				return true;
			return false;
		}

		//return true if the file exists.
		bool exist_file(std::string file_path)
		{
			std::ifstream ifs(file_path, std::ios::in);
			return ifs.operator bool();
		}

		//remove file and return true if remove successfully.
		bool remove_file(std::string file_path)
		{
#ifdef __GADT_GNUC
			if (remove(file_path.c_str()) == 0)
				return true;
#else
			if (::DeleteFileW(StringToWString(file_path).c_str()) != 0)
				return true;
#endif
			return false;
		}

		//load file then convert it to string. return "" if convert failed.
		std::string load_file_as_string(std::string file_path)
		{
			std::filebuf *pbuf;
			std::ifstream filestr;
			long size;
			char * buffer;
			filestr.open(file_path, std::ios::binary);
			pbuf = filestr.rdbuf();
			size = (long)pbuf->pubseekoff(0, std::ios::end, std::ios::in);
			pbuf->pubseekpos(0, std::ios::in);
			buffer = new char[size];
			pbuf->sgetn(buffer, size);
			filestr.close();
			//cout.write(buffer, size);
			std::string temp(buffer, size);
			delete[] buffer;
			return temp;
		}

		//save string as file.
		bool save_string_as_file(std::string str, std::string file_path)
		{
			std::ofstream ofs(file_path, std::ios::trunc);
			if (!ofs)
				return false;
			ofs << str;
			return true;
		}

		/*
		//copy directory to target path.
		bool copy_directory(std::string from_path, std::string to_path)
		{
		if (exist_directory(from_path) == false)
		return false;
		#ifdef __GADT_MSVC
		return ::CreateDirectoryExW(StringToWString(from_path).c_str(), StringToWString(to_path).c_str(), 0) != 0;
		#else
		struct stat from_stat;
		bool get_from_stat = ::stat(from_path.c_str(), &from_stat) != 0;
		return mkdir(to_path.c_str(), from_stat.st_mode) != -1;
		#endif
		}

		//copy file to target path.
		bool copy_file(std::string from_path, std::string to_path, bool fail_if_exist)
		{

		}

		//move directory to target path
		bool move_directory(std::string from_path, std::string to_path)
		{

		}

		//move file to target path
		bool move_file(std::string from_path, std::string to_path)
		{

		}*/
	}
}
