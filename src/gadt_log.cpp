#include "gadt_log.hpp"

namespace gadt
{
	namespace log
	{
		void StringLogger::operator=(const StringLogger & target)
		{
			_print_enabled = target._print_enabled;
			_file_enabled = target._file_enabled;
			_mem_enabled = target._mem_enabled;
			_file_path = target._file_path;
			if (file_enabled())
				_ofs.open(_file_path);
			if (mem_enabled())
			{
				_oss.clear();
				_oss << target._oss.str();
			}
		}

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
				return true;
			}
			return false;
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

		//convert json to int
		int JsonLoader::JsonToInt(const json11::Json & json, std::string err_tag)
		{
			if (json.is_number())
				return json.int_value();
			_err_log.add("[INT] " + err_tag);
			return 0;
		}

		//convert json to size_t
		size_t JsonLoader::JsonToUInt(const json11::Json & json, std::string err_tag)
		{
			if (json.is_number())
				return static_cast<size_t>(json.int_value());
			_err_log.add("[SIZE_T] " + err_tag);
			return size_t();
		}

		//convert json to string
		std::string JsonLoader::JsonToString(const json11::Json & json, std::string err_tag)
		{
			if (json.is_string())
				return json.string_value();
			_err_log.add("[STRING] " + err_tag);
			return std::string();
		}

		//convert json to boolean
		bool JsonLoader::JsonToBoolean(const json11::Json & json, std::string err_tag)
		{
			if (json.is_bool())
				return json.bool_value();
			_err_log.add("[BOOL] " + err_tag);
			return false;
		}

		//convert json to float
		float JsonLoader::JsonToFloat(const json11::Json & json, std::string err_tag)
		{
			if (json.is_number())
				return static_cast<float>(json.number_value());
			_err_log.add("[FLOAT] " + err_tag);
			return 0.0f;
		}

		//convert json to double
		double JsonLoader::JsonToDouble(const json11::Json & json, std::string err_tag)
		{
			if (json.is_number())
				return json.number_value();
			_err_log.add("[DOUBLE] " + err_tag);
			return 0.0;
		}
	}
}