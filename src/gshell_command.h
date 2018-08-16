#include "gadtlib.h"
#include "gshell_args.hpp"

#pragma once

namespace gadt
{
	namespace shell
	{
		using DirList = std::list<std::string>;
		using ParamsList = std::vector<std::string>;
		using ParamsCheckFunc = std::function<bool(const ParamsList&)>;

		//shell defination.
		namespace define
		{
			constexpr const size_t GADT_SHELL_COMMAND_TYPE_COUNT = 8;		//command type number.
			constexpr const size_t GADT_SHELL_COMMAND_MAX_NAME_LENGTH = 20;	//max length of the command. 
			constexpr const size_t GADT_SHELL_COMMAND_MAX_DESC_LENGTH = 40;	//max length of the command. 
			constexpr const size_t GADT_SHELL_MAX_PAGE_LAYER = 256;			//max page layer. 

			constexpr const console::ConsoleColor GADT_SHELL_COLOR_PAGE = console::ConsoleColor::Blue;
			constexpr const console::ConsoleColor GADT_SHELL_COLOR_COMMAND = console::ConsoleColor::Red;

			constexpr const char*  GADT_SHELL_PAGE_LAST_STR = "..";
			constexpr const char*  GADT_SHELL_PAGE_THIS_STR = ".";

			constexpr const char*  GADT_SHELL_COMMAND_SYMBOL = "[F]";
			constexpr const char*  GADT_SHELL_PAGE_SYMBOL = "[P]";

			//list command, default is 'ls'
			constexpr const char* GADT_SHELL_COMMAND_LIST_NAME = "ls";
			constexpr const char* GADT_SHELL_COMMAND_LIST_DESC = "get command list";

			//help command, default is 'help'
			constexpr const char* GADT_SHELL_COMMAND_HELP_NAME = "help";
			constexpr const char* GADT_SHELL_COMMAND_HELP_DESC = "get all shell command";

			//clean screen command, default is 'clear'
			constexpr const char* GADT_SHELL_COMMAND_CLEAR_NAME = "clear";
			constexpr const char* GADT_SHELL_COMMAND_CLEAR_DESC = "clean screen.";

			//command that exit the program, default is 'exit'
			constexpr const char* GADT_SHELL_COMMAND_EXIT_NAME = "exit";
			constexpr const char* GADT_SHELL_COMMAND_EXIT_DESC = "exit program.";

			//command that is uese to change directory, default is 'cd'
			constexpr const char* GADT_SHELL_COMMAND_CD_NAME = "cd";
			constexpr const char* GADT_SHELL_COMMAND_CD_DESC = "change directory.";

			//command that is uese to run batch file, default is 'bat'
			constexpr const char* GADT_SHELL_COMMAND_BAT_NAME = "bat";
			constexpr const char* GADT_SHELL_COMMAND_BAT_DESC = "run batch file.";

			constexpr const char* GADT_SHELL_SEPARATOR_PATH = "/";
			constexpr const char* GADT_SHELL_SEPARATOR_PARAMETER = " ";
			constexpr const char* GADT_SHELL_SEPARATOR_COMMAND = ";";

			//default params check func.
			bool DefaultParamsCheck(const ParamsList& list);

			//default params count check func.
			template<size_t COUNT>
			bool DefaultParamsCountCheck(const ParamsList& list)
			{
				return list.size() == COUNT ? true : false;
			}

			//default no params check func.
			bool DefaultNoParamsCheck(const ParamsList& list);
		}

		//shell command class.
		namespace command
		{
			//command type
			enum class CommandType :uint8_t
			{
				DEFAULT_COMMAND = 0,
				DATA_COMMAND = 1,
				PARAMS_COMMAND = 2,
				DATA_AND_PARAMS_COMMAND = 3,
				BOOL_PARAMS_COMMAND = 4,
				BOOL_DATA_AND_PARAMS_COMMAND = 5,
				DEFAULT_ARGS_COMMAND = 6,
				DATA_ARGS_COMMAND = 7
			};

			//get the name of command type.
			std::string GetCommandTypeName(CommandType type);

			//get the symbol of command type.
			std::string GetCommandTypeSymbol(CommandType type);

			//Command Parser.
			class CommandParser
			{
			private:
				bool		_is_legal;
				bool		_is_relative;
				DirList		_commands;
				ParamsList	_params;

			private:

				//parse the command.
				bool ParseParameters(std::string params);

				//parse command
				bool ParseCommands(std::string commands);

				//init by command
				bool ParseOriginalCommand(std::string original_command);

				//constructor by initialized.
				CommandParser(bool is_legal, bool is_relative, const std::list<std::string>& commands, const std::vector<std::string>& params) :
					_is_legal(is_legal),
					_is_relative(is_relative),
					_commands(commands),
					_params(params)
				{
				}

			public:

				//remove space behind or after the string.
				static std::string RemoveSpace(std::string str);

				//divide single string into multi string by separator.
				static std::vector<std::string> DivideString(std::string str, std::string separator);

				//constructor function by parent command parser
				inline void to_next_command()
				{
					_commands.pop_front();
				}

				//clear data.
				inline void clear()
				{
					_is_legal = false;
					_commands.clear();
					_params.clear();
				}

				//return true if the parser is legal.
				inline bool is_legal() const
				{
					return _is_legal;
				}

				//return true if the parser is relative path.
				inline bool is_relative() const
				{
					return _is_relative;
				}

				//refresh parser.
				inline void refresh(std::string original_command)
				{
					clear();
					_is_legal = ParseOriginalCommand(original_command);
				}

				//add command if it is not empty
				inline void add_command(std::string cmd)
				{
					if (cmd.length() > 0)
						_commands.push_back(cmd);
				}

				//add parameter if it is not empty
				inline void add_parameter(std::string param)
				{
					if (param.length() > 0)
						_params.push_back(param);
				}

				//return true if the command is the only command
				inline bool is_last_command() const
				{
					return _commands.size() == 1;
				}

				//return true if there are no commands.
				inline bool no_commands() const
				{
					return _commands.size() == 0;
				}

				//return true if there are no parameters.
				inline bool no_params() const
				{
					return _params.size() == 0;
				}

				//print details
				void print_details() const
				{
					std::cout << "commands:" << std::endl;
					for (auto cmd : _commands)
					{
						std::cout << "  <" << cmd << ">" << std::endl;
					}
					std::cout << "parameters:" << std::endl;
					for (auto param : _params)
					{
						std::cout << "  <" << param << ">" << std::endl;
					}
				}

				//get the params in this command.
				inline const std::vector<std::string>& params() const
				{
					return _params;
				}

				//get the first command.
				inline std::string fir_command() const
				{
					if (_is_legal && _commands.size() > 0)
					{
						return _commands.front();
					}
					return "";
				}

				//get the last command.
				inline std::string last_command() const
				{
					if (_is_legal && _commands.size() > 0)
					{
						return _commands.back();
					}
					return "";
				}

			public:

				//default constructor.
				CommandParser();

				//constructor function by command string.
				CommandParser(std::string original_command);

				//return true if the string is legal
				static bool CheckStringLegality(std::string str);

				//get next state of current parser.
				CommandParser GetNext() const;

				//get parser of the path.
				CommandParser GetPathParser() const;
			};

			//Command Data Base
			template<typename DataType>
			class CommandBase
			{
			public:

				using DefaultCommandFunc = std::function<void()>;
				using DataCommandFunc = std::function<void(DataType&)>;
				using ParamsCommandFunc = std::function<void(const ParamsList&)>;
				using DataAndParamsCommandFunc = std::function<void(DataType&, const ParamsList&)>;
				using BoolParamsCommandFunc = std::function<bool(const ParamsList&)>;
				using BoolDataAndParamsCommandFunc = std::function<bool(DataType&, const ParamsList&)>;

				template<class... Args>
				using DefaultArgsCommandFunc = std::function<void(Args...)>;

				template<class... Args>
				using DataArgsCommandFunc = std::function<void(DataType&, Args...)>;

			private:

				const CommandType		_type;
				const std::string		_name;
				const std::string		_desc;
				const ParamsCheckFunc	_params_check;

			protected:
				//constructor.
				CommandBase(CommandType type, std::string name, std::string desc, ParamsCheckFunc params_check) :
					_type(type),
					_name(name),
					_desc(desc),
					_params_check(params_check)
				{
				}

			public:

				//waning if excute failed.
				void wrong_params_warning() const
				{
					console::PrintError("illegal parameters");
				}

				//excute params check.
				inline bool params_check(const ParamsList& list)
				{
					return _params_check(list);
				}

				//get type.
				CommandType type() const
				{
					return _type;
				}

				//get name.
				std::string name() const
				{
					return _name;
				}

				//get desc.
				std::string desc() const
				{
					return _desc;
				}

				//excute interface.
				virtual void Run(DataType&, const ParamsList&) = 0;
			};

			//default func command record.
			template<typename DataType>
			class DefaultCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::DefaultCommandFunc;

			private:
				CommandFunc _default_command_func;

			public:

				DefaultCommand(std::string name, std::string desc, CommandFunc default_command_func) :
					CommandBase<DataType>(CommandType::DEFAULT_COMMAND, name, desc, define::DefaultNoParamsCheck),
					_default_command_func(default_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					_default_command_func();
				}
			};

			//data only command record.
			template<typename DataType>
			class DataCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::DataCommandFunc;

			private:
				CommandFunc _no_params_command_func;

			public:
				DataCommand(std::string name, std::string desc, CommandFunc no_params_command_func) :
					CommandBase<DataType>(CommandType::DATA_COMMAND, name, desc, define::DefaultNoParamsCheck),
					_no_params_command_func(no_params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					_no_params_command_func(data);
				}
			};

			//default func command record.
			template<typename DataType>
			class ParamsCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::ParamsCommandFunc;

			private:
				CommandFunc _params_command_func;

			public:

				ParamsCommand(std::string name, std::string desc, CommandFunc params_command_func, ParamsCheckFunc check) :
					CommandBase<DataType>(CommandType::PARAMS_COMMAND, name, desc, check),
					_params_command_func(params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					_params_command_func(params);
				}
			};

			//data and params command record.
			template<typename DataType>
			class DataAndParamsCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::DataAndParamsCommandFunc;

			private:
				CommandFunc _data_params_command_func;

			public:
				DataAndParamsCommand(std::string name, std::string desc, CommandFunc data_params_command_func, ParamsCheckFunc params_check) :
					CommandBase<DataType>(CommandType::DATA_AND_PARAMS_COMMAND, name, desc, params_check),
					_data_params_command_func(data_params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					_data_params_command_func(data, params);
				}
			};

			//default func command record.
			template<typename DataType>
			class BoolParamsCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::BoolParamsCommandFunc;

			private:
				CommandFunc _bool_params_command_func;

			public:

				BoolParamsCommand(std::string name, std::string desc, CommandFunc bool_params_command_func) :
					CommandBase<DataType>(CommandType::BOOL_PARAMS_COMMAND, name, desc, define::DefaultParamsCheck),
					_bool_params_command_func(bool_params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					bool result = _bool_params_command_func(params);
					if (result == false)
					{
						gadt::console::PrintError("run " + this->name() + " failed.");
					}
				}
			};

			//data and params command record.
			template<typename DataType>
			class BoolDataAndParamsCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::BoolDataAndParamsCommandFunc;

			private:
				CommandFunc _bool_data_params_command_func;

			public:
				BoolDataAndParamsCommand(std::string name, std::string desc, CommandFunc bool_data_params_command_func) :
					CommandBase<DataType>(CommandType::BOOL_DATA_AND_PARAMS_COMMAND, name, desc, define::DefaultParamsCheck),
					_bool_data_params_command_func(bool_data_params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					bool result = _bool_data_params_command_func(data, params);
					if (result == false)
					{
						console::PrintError("run " + this->name() + " failed.");
					}
				}
			};

			//data and params command record.
			template<typename DataType, class... Args>
			class DefaultArgsCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::template DefaultArgsCommandFunc<Args...>;

			private:
				CommandFunc _default_args_command_func;

			public:
				DefaultArgsCommand(std::string name, std::string desc, CommandFunc default_args_command_func) :
					CommandBase<DataType>(CommandType::DEFAULT_ARGS_COMMAND, name, desc, define::DefaultParamsCheck),
					_default_args_command_func(default_args_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					args::ArgConvertor<Args...> args(params);
					if (args.parse_success())
					{
						args.ExecuteFunction(_default_args_command_func);
					}
					else
					{
						console::PrintError(args.get_error());
					}
				}
			};

			//data and params command record.
			template<typename DataType, class... Args>
			class DataArgsCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::template DataArgsCommandFunc<Args...>;

			private:
				CommandFunc _data_args_command_func;

			public:
				DataArgsCommand(std::string name, std::string desc, CommandFunc data_args_command_func) :
					CommandBase<DataType>(CommandType::DEFAULT_ARGS_COMMAND, name, desc, define::DefaultParamsCheck),
					_data_args_command_func(data_args_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					args::ArgConvertor<Args...> args(params);
					if (args.parse_success())
					{
						args.template ExecuteFunction<DataType&>(_data_args_command_func, data);
					}
					else
					{
						console::PrintError(args.get_error());
					}
				}
			};

			//information of single command.
			template<typename DataType>
			struct CommandInfo
			{
				CommandType type;
				std::string desc;
				std::string help_desc;
				std::unique_ptr<CommandBase<DataType>> ptr;
			};
		}

	}
}