#include "gadt_log.hpp"

namespace gadt
{
	namespace log
	{
		StringLogger::StringLogger() :
			_print_enabled(false),
			_file_enabled(false),
			_mem_enabled(false),
			_file_path(),
			_oss(),
			_ofs()
		{
		}

		StringLogger::StringLogger(const StringLogger & target) :
			_print_enabled(target._print_enabled),
			_file_enabled(target._file_enabled),
			_mem_enabled(target._mem_enabled),
			_file_path(target._file_path),
			_oss(),
			_ofs()
		{
			_oss << target._oss.str();
			if (_file_enabled)
				_ofs.open(_file_path);
		}

		//enable print log.
		void StringLogger::EnablePrint()
		{
			_print_enabled = true;
		}

		//enable file log
		bool StringLogger::EnableFile(std::string file_path)
		{
			_ofs.open(file_path, std::ios::app);
			if (_ofs.is_open())
			{
				_file_enabled = true;
				_file_path = file_path;
			}
		}

		//enable log memory.
		void StringLogger::EnableMem()
		{
			_mem_enabled = true;
		}

		//disable print log.
		void StringLogger::DisablePrint()
		{
			_print_enabled = false;
		}

		//disable file log
		void StringLogger::DisableFile()
		{
			_file_enabled = false;
			_ofs.close();
			_file_path = "";
		}

		//disable log memory.
		void StringLogger::DisableMem()
		{
			_mem_enabled = false;
		}

		//disable all log
		void StringLogger::Disable()
		{
			DisablePrint();
			DisableFile();
			DisableMem();
		}
	}
}