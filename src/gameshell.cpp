/*
* game shell is used for game AI developing and this shell is consist of the concept we called 'pages',
* each page is an single set of command and include data that binded to each page by use template.
*/

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

#include "gameshell.h"

namespace gadt
{
	namespace shell
	{
		//shell defination.
		namespace define
		{
			//get the name of command type.
			std::string GetCommandTypeName(size_t i)
			{
				static const char* type_name[g_COMMAND_TYPE_NUMBER] = {
					"DEFAULT COMMAND",
					"DATA COMMAND",
					"PARAMS COMMAND",
					"DATA_AND_PARAMS_COMMAND",
					"CHILD_PAGE"
				};
				return std::string(type_name[i]);
			}

			//get the symbol of command type.
			std::string GetCommandTypeSymbol(size_t i)
			{
				static const char* type_symbol[g_COMMAND_TYPE_NUMBER] = {
					"[XX]",
					"[YX]",
					"[XY]",
					"[YY]",
					"[ P]"
				};
				return std::string(type_symbol[i]);
			}

			//default params check func.
			bool DefaultParamsCheck(const ParamsList & list)
			{
				return true;
			}

			//default no params check func.
			bool DefaultNoParamsCheck(const ParamsList& list)
			{
				return list.size() == 0 ? true : false;
			}
		}

		//shell command class.
		namespace command
		{
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
			bool CommandParser::ParseParameters(std::string params_str)
			{
				if (params_str.length() == 0) { return true; }
				if (params_str[0] == ' ') { return ParseParameters(params_str.substr(1, params_str.length() - 1)); }

				size_t divide_pos = params_str.find(" ");
				if (divide_pos != std::string::npos)
				{
					std::string param = params_str.substr(0, divide_pos);
					std::string remains = params_str.substr(divide_pos + 1, params_str.length() - divide_pos);
					if (CheckStringLegality(param) == false)
					{
						return false;
					}
					_params.push_back(param);
					return ParseParameters(remains);
				}
				if (CheckStringLegality(params_str))
				{
					_params.push_back(params_str);
					return true;
				}
				return false;
			}
			bool CommandParser::ParseCommands(std::string cmd_str)
			{
				if (cmd_str.length() == 0) { return true; }
				if (cmd_str[0] == ' ') { return ParseParameters(cmd_str.substr(1, cmd_str.length() - 1)); }
				size_t divide_pos = cmd_str.find("/");
				if (divide_pos != std::string::npos)
				{
					std::string cmd = cmd_str.substr(0, divide_pos);
					std::string remains = cmd_str.substr(divide_pos + 1, cmd_str.length() - divide_pos);
					if (CheckStringLegality(cmd) == false)
					{
						return false;
					}
					_commands.push_back(cmd);
					return ParseCommands(remains);
				}
				if (CheckStringLegality(cmd_str))
				{
					_commands.push_back(cmd_str);
					return true;
				}
				return false;
			}
			bool CommandParser::ParseOriginalCommand(std::string original_command)
			{
				size_t space_pos = original_command.find(" ");
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
		}

		//shell pages class
		namespace page
		{
			//ShellPageBase
			ShellPageBase::ShellPageBase(::gadt::shell::GameShell* belonging_shell, std::string name) :
				_belonging_shell(belonging_shell),
				_name(name),
				_index(AllocPageIndex()),
				_info_func([]()->void {})
			{
			}
		}

		//GameShell
		GameShell* GameShell::_g_focus_game = nullptr;

		//public constructor function
		GameShell::GameShell(std::string name) :
			_page_table(),
			_name(name),
			_info_func([]()->void {}),
			_shell_cmd(this,"shell_cmd")
		{
			ShellCmdInit();
		}

		//print current dir.
		void GameShell::PrintDir() const
		{
			//print shell name.
			console::Cprintf(GameShell::focus_game()->name(), console::PURPLE);
			std::cout << " @ ";
			//console::Cprintf(_name, console::YELLOW);

			//print dir
			bool is_first = false;
			for (auto page_name : _dir_list)
			{
				if (is_first == true)
				{
					std::cout << "/";
					console::Cprintf(page_name, console::GREEN);
				}
				else
				{
					console::Cprintf(page_name, console::YELLOW);
					is_first = true;
				}
			}
			std::cout << "/";
		}

		//print input tips.
		void GameShell::InputTip(std::string tip)
		{
			if (focus_game() != nullptr)
			{
				focus_game()->PrintDir();
				if (tip != "")
				{
					std::cout << "/";
				}
				console::Cprintf(tip, console::GREEN);
				std::cout << ": >> ";
			}
			else
			{
				console::Cprintf("ERROR: focus game not exist", console::PURPLE);
			}
		}

		//return to root page
		void GameShell::ReturnToPreviousPage()
		{
			if (_dir_list.size() > 1)
			{
				_dir_list.pop_back();
			}
		}

		//clear screen
		void GameShell::ClearScreen() const
		{
			console::SystemClear();
			focus_page()->_info_func();
			std::cout << ">> ";
			console::Cprintf("[ Shell ", console::DEEP_YELLOW);
			console::Cprintf("<" + focus_page()->_name + ">", console::YELLOW);
			console::Cprintf(" ]\n", console::DEEP_YELLOW);
			std::cout << ">> ";
			console::Cprintf(std::string("use '") + std::string(define::g_HELP_COMMAND_NAME) + std::string("' to get more command\n\n"), console::DEEP_GREEN);
		}

		//initialize shell commands.
		void GameShell::ShellCmdInit()
		{
			using CommandPtr = typename page::ShellPage<int>::CommandPtr;
			using ParamsCommand = typename page::ShellPage<int>::ParamsCommand;
			using ChildPageCommand = typename page::ShellPage<int>::ChildPageCommand;

			auto ListCommandFunc = [&](const ParamsList& params)->void{
				if (params.size() == 0)
				{
					focus_page()->PrintCommandList("-n");
				}
				else
				{
					focus_page()->PrintCommandList(params[0]);
				}
			};
			auto ListCommandCond = [](const ParamsList& params)->bool{
				if (params.size() == 0)
				{
					return true;
				}
				else if (params.size() == 1)
				{
					if (params[0] == "-n" || params[0] == "-t")
					{
						return true;
					}
					console::ShowMessage("'" + std::string(define::g_LIST_COMMAND_NAME) + "' only accept -n or -t as parameter");
					return false;
				}
				console::ShowMessage("'" + std::string(define::g_LIST_COMMAND_NAME) + "' only accept one paramater");
				return false;
			};

			auto HelpCommandFunc = [&](const ParamsList& params)->void {
				if (params.size() == 1)
				{

				}
				else
				{
					_shell_cmd.PrintCommandList(std::string("-n"));
				}
			};
			auto HelpCommandCond = [&](const ParamsList& params)->bool {
				if (params.size() == 0)
				{
					return true;
				}
				else if (params.size() == 1)
				{
					std::string name = params.front();
					if (this->focus_page()->ExistFunc(name))
					{
						return true;
					}
					else
					{
						console::ShowMessage(std::string("command '") + params.front() + "' not found.");
						return false;
					}
				}
				console::ShowMessage(std::string("'") + define::g_HELP_COMMAND_NAME + "' only accept one parameter.");
				return false;
			};

			auto root_command = CommandPtr(new ChildPageCommand(
				define::g_ROOT_COMMAND_NAME, 
				define::g_ROOT_COMMAND_DESC, 
				this, 
				define::g_ROOT_COMMAND_NAME
			));
			auto return_command = CommandPtr(new ChildPageCommand(
				define::g_RETURN_COMMAND_NAME, 
				define::g_RETURN_COMMAND_DESC, 
				this,
				define::g_RETURN_COMMAND_NAME
			));
			auto exit_command = CommandPtr(new ParamsCommand(
				define::g_EXIT_COMMAND_NAME, 
				define::g_EXIT_COMMAND_DESC, 
				[&](const ParamsList& params)->void { this->_dir_list.clear(); },
				define::DefaultNoParamsCheck
			));
			auto list_command = CommandPtr(new ParamsCommand(
				define::g_LIST_COMMAND_NAME, 
				define::g_LIST_COMMAND_DESC, 
				ListCommandFunc, 
				ListCommandCond
			));
			auto help_command = CommandPtr(new ParamsCommand(
				define::g_HELP_COMMAND_NAME,
				define::g_HELP_COMMAND_DESC,
				HelpCommandFunc,
				HelpCommandCond
			));
			auto clear_command = CommandPtr(new ParamsCommand(
				define::g_CLEAR_COMMAND_NAME, 
				define::g_CLEAR_COMMAND_DESC, 
				[&](const ParamsList& data)->void { this->ClearScreen(); }, 
				define::DefaultNoParamsCheck
			));

			_shell_cmd.InsertCommand(define::g_ROOT_COMMAND_NAME	, root_command);
			_shell_cmd.InsertCommand(define::g_RETURN_COMMAND_NAME	, return_command);
			_shell_cmd.InsertCommand(define::g_EXIT_COMMAND_NAME	, exit_command);
			_shell_cmd.InsertCommand(define::g_LIST_COMMAND_NAME	, list_command);
			_shell_cmd.InsertCommand(define::g_HELP_COMMAND_NAME	, help_command);
			_shell_cmd.InsertCommand(define::g_CLEAR_COMMAND_NAME	, clear_command);
		}

		//execute command in focus page.
		void GameShell::ExecuteCommandInFocusPage(std::string command, const ParamsList& params)
		{
			if (_shell_cmd.ExistCommand(command))
			{
				_shell_cmd.ExecuteCommand(command, params);
			}
			else
			{
				focus_page()->ExecuteCommand(command, params);
			}
		}

		//enter page by name.
		void GameShell::EnterPage(std::string name)
		{
			if (name == define::g_RETURN_COMMAND_NAME)
			{
				ReturnToPreviousPage();
			}
			else if (name == define::g_ROOT_COMMAND_NAME)
			{
				ReturnToRootPage();
			}
			else
			{
				_dir_list.push_back(name);
			}
		}

		//start from appointed page.
		void GameShell::StartFromPage(std::string name)
		{
			if (page_exist(name))
			{
				be_focus();
				_dir_list.push_back(name);
				ClearScreen();
				command::CommandParser parser;
				for (;;)
				{
					if (_dir_list.empty())
					{
						return;
					}
					if (parser.no_commands() || !parser.is_legal())
					{
						InputTip();
						std::string original_command = GetInput();
						parser.refresh(original_command);
						if (parser.is_legal() == false)
						{
							console::ShowError("inavailable command!");
							parser.clear();
						}
					}
					else
					{
						//is the last command, excute it.
						if (parser.is_last_command())
						{
							ExecuteCommandInFocusPage(parser.fir_command(), parser.params());
							parser.clear();
						}
						else
						{
							//enter next page.
							std::string page_name = parser.fir_command();
							if (page_exist(page_name) && 
								(
									focus_page()->ExistChildPage(page_name) ||
									page_name == define::g_RETURN_COMMAND_NAME ||
									page_name == define::g_ROOT_COMMAND_NAME
								)
								)
							{
								EnterPage(page_name);
								parser.to_next_command();
							}
							else
							{
								console::ShowError("page " + page_name + " not found");
								parser.clear();
							}
						}
					}
				}
			}
			console::ShowError("page " + name + "not found");
		}
	}
}