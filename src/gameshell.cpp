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
		//global variables
		const char* ShellPageBase::g_SHELL_HELP_COMMAND_STR = "ls";
		const char* ShellPageBase::g_SHELL_RETURN_COMMAND_STR = "..";
		const char* ShellPageBase::g_SHELL_ROOT_COMMAND_STR = ".";
		const char* ShellPageBase::g_SHELL_CLEAR_COMMAND_STR = "clear";
		const char* ShellPageBase::g_SHELL_EXIT_COMMAND_STR = "exit";
		const size_t ShellPageBase::g_SHELL_MAX_COMMAND_LENGTH = 15;

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

		//ShellPageBase
		ShellPageBase::ShellPageBase(GameShell* belonging_shell, std::string name) :
			_belonging_shell(belonging_shell),
			_name(name),
			_index(AllocPageIndex()),
			_info_func([]()->void {})
		{
		}
		void ShellPageBase::ClearScreen() const
		{
			console::SystemClear();
			_info_func();
			std::cout << ">> ";
			console::Cprintf("[ Shell ", console::DEEP_YELLOW);
			console::Cprintf("<" + _name + ">", console::YELLOW);
			console::Cprintf(" ]\n", console::DEEP_YELLOW);
			std::cout << ">> ";
			console::Cprintf(std::string("use '") + std::string(g_SHELL_HELP_COMMAND_STR) + std::string("' to get more command\n\n"), console::DEEP_GREEN);
		}

		//GameShell
		GameShell* GameShell::_g_focus_game = nullptr;
		void GameShell::ShowPath() const
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
		void GameShell::InputTip(std::string tip)
		{
			if (focus_game() != nullptr)
			{
				focus_game()->ShowPath();
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
		GameShell::GameShell(std::string name) :
			_page_table(),
			_name(name),
			_info_func([]()->void {})
		{
		}
		void GameShell::StartFromPage(std::string name)
		{
			if (page_exist(name))
			{
				be_focus();
				_dir_list.push_back(name);
				focus_page()->ClearScreen();
				CommandParser parser;
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
							focus_page()->ExecuteCommand(parser.fir_command(), parser.params());
							parser.clear();
						}
						else
						{
							//enter next page.
							std::string page_name = parser.fir_command();
							if (page_exist(page_name))
							{
								EnterPage(page_name);
								parser.to_next_command();
							}
							else
							{
								console::ShowError("page " + page_name + " not found 1");
								parser.clear();
							}
						}
					}
				}
			}
			console::ShowError("page " + name + "not found 2");
		}
		void GameShell::EnterPage(std::string name)
		{
			if (name == ShellPageBase::g_SHELL_RETURN_COMMAND_STR)
			{
				ReturnToPreviousPage();
			}
			else if(name == ShellPageBase::g_SHELL_ROOT_COMMAND_STR)
			{
				ReturnToRootPage();
			}
			else
			{
				_dir_list.push_back(name);
			}
		}
		void GameShell::ReturnToPreviousPage()
		{
			if (_dir_list.size() > 1)
			{
				_dir_list.pop_back();
			}
		}
	}
}
