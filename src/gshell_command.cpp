#include "gshell_command.h"

namespace gadt
{
	namespace shell
	{
		//shell defination.
		namespace define
		{
			//default params check func.
			bool DefaultParamsCheck(const ParamsList & list)
			{
				return true;
			}

			//default no params check func.
			bool DefaultNoParamsCheck(const ParamsList & list)
			{
				return DefaultParamsCountCheck<0>(list);
			}
		}

		//shell command class.
		namespace command
		{
			//get the name of command type.
			std::string GetCommandTypeName(CommandType type)
			{
				static const char* type_name[define::GADT_SHELL_COMMAND_TYPE_COUNT] = {
					"DEFAULT COMMAND",
					"DATA COMMAND",
					"PARAMS COMMAND",
					"DATA_AND_PARAMS_COMMAND",
					"BOOL_PARAMS COMMAND",
					"BOOL_DATA_AND_PARAMS_COMMAND"
				};
				return std::string(type_name[static_cast<size_t>(type)]);
			}

			//get the symbol of command type.
			std::string GetCommandTypeSymbol(CommandType type)
			{
				static const char* type_symbol[define::GADT_SHELL_COMMAND_TYPE_COUNT] = {
					"[F]",
					"[F]",
					"[F]",
					"[F]",
					"[F]",
					"[F]"
				};
				return std::string(type_symbol[static_cast<size_t>(type)]);
			}

			//default constructor.
			CommandParser::CommandParser() :
				_is_legal(false),
				_is_relative(true),
				_commands(),
				_params()
			{
			}

			//constructor function by command string.
			CommandParser::CommandParser(std::string original_command) :
				_is_legal(false),
				_is_relative(true),
				_commands(),
				_params()
			{
				_is_legal = ParseOriginalCommand(original_command);
			}

			//CommandParser
			bool CommandParser::CheckStringLegality(std::string str)
			{
				for (auto c : str)
				{
					if (c == 92 || c == 47 || c == 58 || c == 42 || c == 34 || c == 60 || c == 62 || c == 124)
					{
						return false;
					}
				}
				return true;
			}

			//get next state of current parser.
			CommandParser CommandParser::GetNext() const
			{
				auto temp = _commands;
				if (temp.size() > 0)
				{
					temp.pop_front();
				}
				return CommandParser(_is_legal, _is_relative, temp, _params);
			}

			//get parser of the path.
			CommandParser CommandParser::GetPathParser() const
			{
				auto temp = _commands;
				if (temp.size() > 0)
				{
					temp.pop_back();
				}
				return CommandParser(_is_legal, _is_relative, temp, std::vector<std::string>());
			}

			//parse the command.
			bool CommandParser::ParseParameters(std::string params_str)
			{
				auto param_list = DivideString(params_str, define::GADT_SHELL_SEPARATOR_PARAMETER);
				for (auto param : param_list)
				{
					add_parameter(param);
				}
				return true;
			}

			//parse command
			bool CommandParser::ParseCommands(std::string cmd_str)
			{
				auto cmd_list = DivideString(cmd_str, define::GADT_SHELL_SEPARATOR_PATH);
				size_t begin = 0;
				if (cmd_list.size() != 0)
				{
					if (cmd_list[0].empty())
					{
						_is_relative = false;//absolute path.
						begin = 1;
					}
				}
				for (size_t i = begin; i < cmd_list.size(); i++)
				{
					if (CheckStringLegality(cmd_list[i]) == true)
					{
						add_command(cmd_list[i]);
					}
					else
					{
						return false;//find illegal command
					}
				}
				return true;
			}

			//divide origin command into command and parameters.
			bool CommandParser::ParseOriginalCommand(std::string original_command)
			{
				size_t space_pos = original_command.find(define::GADT_SHELL_SEPARATOR_PARAMETER);
				if (space_pos != std::string::npos)
				{
					//find space, divide command into command part and param part.
					std::string commands = original_command.substr(0, space_pos);
					std::string params = original_command.substr(space_pos, original_command.length() - space_pos);
					return ParseCommands(commands) && ParseParameters(params);
				}
				//no params, parse commands.
				return ParseCommands(original_command);
			}

			//remove space behind or after the string.
			std::string CommandParser::RemoveSpace(std::string str)
			{
				for (;;)
				{
					if (str.length() == 0)
						return str;
					else if (str[0] == ' ')
						str = str.substr(1, str.length() - 1);
					else if (str.back() == ' ')
						str.pop_back();
					else
						return str;
				}
				return str;
			}

			//divide single string into multi string by separator.
			std::vector<std::string> CommandParser::DivideString(std::string str, std::string separator)
			{
				std::vector<std::string> result;
				str = RemoveSpace(str);
				while (str.length() != 0)
				{
					size_t divide_pos = str.find(separator);
					if (divide_pos != std::string::npos)
					{
						std::string single_str = RemoveSpace(str.substr(0, divide_pos));
						if (single_str.empty() == false)
						{
							result.push_back(single_str);
						}
						str = str.substr(divide_pos + separator.length(), str.length() - divide_pos);
					}
					else
					{
						result.push_back(RemoveSpace(str));
						break;
					}
				}
				return result;
			}
		}
	}
}