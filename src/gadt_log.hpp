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
#include "visual_tree.h"
#include "gadt_filesystem.h"

#pragma once

namespace gadt
{
	namespace log
	{
		//error log class. 
		class ErrorLog
		{
		private:
			std::vector<std::string> _error_list;

		public:
			//default constructor.
			ErrorLog():
				_error_list()
			{
			}

			//constructor with init list.
			ErrorLog(std::initializer_list<std::string> init_list)
			{
				for (auto err : init_list)
				{
					_error_list.push_back(err);
				}
			}

			//copy constructor is banned
			ErrorLog(const ErrorLog&) = delete;

			//add a new error.
			inline void add(std::string err)
			{
				_error_list.push_back(err);
			}

			//return true if no error exist.
			inline bool is_empty() const
			{
				return _error_list.size() == 0;
			}

			//output as json format.
			inline std::string output() const
			{
				std::stringstream ss;
				ss << "[";
				for (size_t i = 0; i < _error_list.size(); i++)
				{
					ss << "\"" << _error_list[i] << "\"";
					if (i != _error_list.size() - 1)
					{
						ss << "," << std::endl;
					}
				}
				ss << "]";
				return ss.str();
			}
		};

		/*
		* SearchLogger is an template of log controller that is used for search logs.
		*
		* [State] is the game state type.
		* [Action] is the game action type.
		* [Result] is the game result type and could be ignore if it is unnecessary.
		*/
		template<typename State, typename Action, typename Result = int>
		class SearchLogger
		{
		private:
			using VisualTree = visual_tree::VisualTree;

		public:
			using StateToStrFunc = std::function<std::string(const State& state)>;
			using ActionToStrFunc = std::function<std::string(const Action& action)>;
			using ResultToStrFunc = std::function<std::string(const Result& result)>;

		private:
			//convert functions
			bool			_initialized;
			StateToStrFunc	_state_to_str_func;
			ActionToStrFunc	_action_to_str_func;
			ResultToStrFunc	_result_to_str_func;

			//log control
			bool			_enable_log;
			std::ostream*	_log_ostream;

			//json output control
			bool			_enable_json_output;
			std::string		_json_output_folder;
			VisualTree		_visual_tree;

		public:
			//default constructor.
			SearchLogger() :
				_initialized(false),
				_state_to_str_func([](const State&)->std::string {return ""; }),
				_action_to_str_func([](const Action&)->std::string {return ""; }),
				_enable_log(false),
				_log_ostream(&std::cout),
				_enable_json_output(false),
				_json_output_folder("JsonOutput"),
				_visual_tree()
			{
			}

			//initialized constructor.
			SearchLogger(
				StateToStrFunc state_to_str_func,
				ActionToStrFunc action_to_str_func,
				ResultToStrFunc result_to_str_func = [](const Result&)->std::string {return ""; }
			) :
				_initialized(true),
				_state_to_str_func(state_to_str_func),
				_action_to_str_func(action_to_str_func),
				_result_to_str_func(result_to_str_func),
				_enable_log(false),
				_log_ostream(std::cout),
				_enable_json_output(false),
				_json_output_folder("JsonOutput"),
				_visual_tree()
			{
			}

			//return true if log enabled.
			inline bool log_enabled() const { return _enable_log; }

			//return true if json output enabled
			inline bool json_output_enabled() const { return _enable_json_output; };

			//get log output stream.
			inline std::ostream& log_ostream() const
			{
				return *_log_ostream;
			}

			//get json output path
			inline std::string json_output_folder() const
			{
				return _json_output_folder;
			}

			//get ref of visual tree.
			inline VisualTree& visual_tree()
			{
				return _visual_tree;
			}

			//get state to string function.
			inline StateToStrFunc state_to_str_func() const
			{
				return _state_to_str_func;
			}

			//get action to string function.
			inline ActionToStrFunc action_to_str_func() const
			{
				return _action_to_str_func;
			}

			//get result to string function.
			inline ResultToStrFunc result_to_str_func() const
			{
				return _result_to_str_func;
			}

			//initialize logger.
			void Init(
				StateToStrFunc state_to_str_func,
				ActionToStrFunc action_to_str_func,
				ResultToStrFunc result_to_str_func
			)
			{
				_initialized = true;
				_state_to_str_func = state_to_str_func;
				_action_to_str_func = action_to_str_func;
				_result_to_str_func = result_to_str_func;
			}

			//enable log.
			inline void EnableLog(std::ostream& os)
			{
				_enable_log = true;
				_log_ostream = &os;
			}

			//disable log.
			inline void DisableLog()
			{
				_enable_log = false;
			}

			//enable json output
			inline void EnableJsonOutput(std::string json_output_folder)
			{
				_enable_json_output = true;
				_json_output_folder = json_output_folder;
			}

			//disable json output.
			inline void DisableJsonOutput()
			{
				_enable_json_output = false;
			}

			//output json to path.
			inline void OutputJson() const
			{
				std::string path = "./" + _json_output_folder;
				if (!filesystem::exist_directory(path))
				{
					filesystem::create_directory(path);
				}
				path += "/";
				path += timer::TimePoint().get_string();
				path += ".json";
				std::ofstream ofs(path);
				_visual_tree.output_json(ofs);
			}

		};

		//a lightweight string logger
		class StringLogger
		{
		private:

			bool _print_enabled;
			bool _file_enabled;
			bool _mem_enabled;

			std::string _file_path;
			std::stringstream _oss;
			std::ofstream _ofs;

		public:

			//return true if print enabled.
			inline bool print_enabled() const
			{
				return _print_enabled;
			}

			//return true if file enabled.
			inline bool file_enabled() const
			{
				return _file_enabled;
			}

			//return true if memory enabled.
			inline bool mem_enabled() const
			{
				return _mem_enabled;
			}

			//operator<<
			template<typename T>
			inline StringLogger& operator<<(T content)
			{
				if (_print_enabled)
					std::cout << content;
				if (_file_enabled)
					_ofs << content;
				if (_mem_enabled)
					_ofs << content;
				return *this;
			}

			//get string from string stream in memory
			inline std::string mem_string() const
			{
				return _oss.str();
			}

			//get file path.
			inline std::string file_path() const
			{
				return _file_path;
			}

			void operator=(const StringLogger& target);

		public:

			//constructor.
			StringLogger();

			//copy constructor.
			StringLogger(const StringLogger& target);

			//enable print log.
			void EnablePrint();

			//enable file log
			bool EnableFile(std::string file_path);

			//enable log memory.
			void EnableMem();

			//disable print log.
			void DisablePrint();

			//disable file log
			void DisableFile();

			//disable log memory.
			void DisableMem();

			//disable all log
			void Disable();
		};

		//JsonLoader is used to load json file.
		class JsonLoader
		{
		private:
			
			ErrorLog _err_log;

		public:

			//return true if error exist
			inline bool error_exist() const
			{
				return _err_log.is_empty() == false;
			}

			//return true if no error
			inline bool no_error() const
			{
				return _err_log.is_empty();
			}

			//get errors as string format.
			inline std::string get_error() const
			{
				return _err_log.output();
			}

		public:

			//convert json to int
			int JsonToInt(const json11::Json& json, std::string err_tag = "Int");

			//convert json to size_t
			size_t JsonToUInt(const json11::Json& json, std::string err_tag = "UInt");

			//convert json to string
			std::string JsonToString(const json11::Json& json, std::string err_tag = "String");

			//convert json to boolean
			bool JsonToBoolean(const json11::Json& json, std::string err_tag = "Boolean");

			//convert json to float
			float JsonToFloat(const json11::Json& json, std::string err_tag = "Float");

			//convert json to double
			double JsonToDouble(const json11::Json& json, std::string err_tag = "Double");

			//convert json to int vector
			inline std::vector<int> JsonToIntVector(const json11::Json& json, std::string err_tag = "IntVector")
			{
				std::string ele_err_tag = "Element in " + err_tag;
				return JsonToVector<int>(json, [&](const json11::Json& ele_json)->int {
					return JsonToInt(ele_json, ele_err_tag);
				}, err_tag);
			}

			//convert json to size vector
			inline std::vector<size_t> JsonToUIntVector(const json11::Json& json, std::string err_tag = "UintVector")
			{
				std::string ele_err_tag = "Element in " + err_tag;
				return JsonToVector<size_t>(json, [&](const json11::Json& ele_json)->size_t {
					return JsonToUInt(ele_json, ele_err_tag);
				}, err_tag);
			}

			//convert json to string vector
			inline std::vector<std::string> JsonToStringVector(const json11::Json& json, std::string err_tag = "StringVector")
			{
				std::string ele_err_tag = "Element in " + err_tag;
				return JsonToVector<std::string>(json, [&](const json11::Json& ele_json)->std::string {
					return JsonToString(ele_json, ele_err_tag);
				}, err_tag);
			}

			//convert json to bool vector
			inline std::vector<bool> JsonToBooleanVector(const json11::Json& json, std::string err_tag = "BooleanVector")
			{
				std::string ele_err_tag = "Element in " + err_tag;
				return JsonToVector<bool>(json, [&](const json11::Json& ele_json)->bool {
					return JsonToBoolean(ele_json, ele_err_tag);
				}, err_tag);
			}

			//convert json to float vector
			inline std::vector<float> JsonToFloatVector(const json11::Json& json, std::string err_tag = "FloatVector")
			{
				std::string ele_err_tag = "Element in " + err_tag;
				return JsonToVector<float>(json, [&](const json11::Json& ele_json)->float {
					return JsonToFloat(ele_json, ele_err_tag);
				}, err_tag);
			}

			//convert json to double vector
			inline std::vector<double> JsonToDoubleVector(const json11::Json& json, std::string err_tag = "DoubleVector")
			{
				std::string ele_err_tag = "Element in " + err_tag;
				return JsonToVector<double>(json, [&](const json11::Json& ele_json)->double {
					return JsonToDouble(ele_json, ele_err_tag);
				}, err_tag);
			}

			//convert json to array.
			template<typename T>
			std::vector<T> JsonToVector(const json11::Json& json, std::function<T(const json11::Json&)> JsonToElement, std::string err_tag = "Vector")
			{
				std::vector<T> vec;
				if (json.is_array())
					for (const json11::Json& ele_json : json.array_items())
						vec.push_back(JsonToElement(ele_json));
				else
					_err_log.add("[VECTOR] " + err_tag);
				return vec;
			}
		};
	}
}
