/* Copyright (c) 2018 Junkai Lu <junkai-lu@outlook.com>.
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

#include "game_shell.h"

namespace gadt
{
	namespace shell
	{
		//GameShell
		GameShell* GameShell::_g_focus_game = nullptr;

		//public constructor function
		GameShell::GameShell(std::string name) :
			_name(name),
			_shell_cmd(nullptr, this, "shell_cmd", []() {}),
			_root_page(nullptr, this, "root", DefaultInfoFunc),
			_focus_page(&_root_page)
		{
			InitializeShellCommands();
		}

		//default info func of game shell.
		void GameShell::DefaultInfoFunc()
		{
			timer::TimePoint tp;
			console::Cprintf("=============================================\n", console::ConsoleColor::Gray);
			console::Cprintf("       Game AI Development Toolkit\n", console::ConsoleColor::Yellow);
			console::Cprintf("       Copyright @ Junkai-Lu 2018 \n", console::ConsoleColor::Yellow);
			console::Cprintf("=============================================", console::ConsoleColor::Gray);
			console::PrintEndLine<2>();
		}

		//load and run (multi) batch command
		void GameShell::LoadBatCommand(const ParamsList & params)
		{
			for (auto file_path : params)
			{
				if (filesystem::exist_file(file_path))
				{
					std::string cmd = filesystem::load_file_as_string(file_path);
					for (auto& c : cmd)
					{
						if (c == '\r')
							c = ' ';
						if (c == '\n')
							c = ' ';
					}
					if (cmd.empty())//do not execute empty file.
						continue;
					bool res = RunMultiCommand(cmd);
					if (res == false)
						return;
				}
				else
				{
					console::PrintError("file '" + file_path + "' not found.");
					return;
				}
			}
		}

		//print current dir.
		void GameShell::PrintFocusPath() const
		{
			//print shell name.
			console::Cprintf(GameShell::focus_game()->name(), console::ConsoleColor::Purple);
			std::cout << " @ ";
			focus_page()->PrintPath();
		}

		//print input tips.
		void GameShell::InputTip(std::string tip)
		{
			if (focus_game() != nullptr)
			{
				focus_game()->PrintFocusPath();
				if (tip != "")
				{
					std::cout << "/";
				}
				console::Cprintf(tip, console::ConsoleColor::Green);
				std::cout << ": >> ";
			}
			else
			{
				console::Cprintf("ERROR: focus game not exist", console::ConsoleColor::Purple);
			}
		}

		std::string GameShell::GetInput()
		{
			char buffer[256];
			std::cin.clear();
			std::cin.getline(buffer, 256);
			return std::string(buffer);
		}

		//clear screen
		void GameShell::ClearScreen() const
		{
			console::SystemClear();
			focus_page()->_info_func();
			std::cout << ">> ";
			console::Cprintf("[ Shell ", console::ConsoleColor::DeepYellow);
			console::Cprintf("<" + focus_page()->_name + ">", console::ConsoleColor::Yellow);
			console::Cprintf(" ]\n", console::ConsoleColor::DeepYellow);
			std::cout << ">> ";
			console::Cprintf(std::string("use '") + std::string(define::GADT_SHELL_COMMAND_HELP_NAME) + std::string("' to get more command\n\n"), console::ConsoleColor::DeepGreen);
		}

		//initialize shell commands.
		void GameShell::InitializeShellCommands()
		{
			using CommandPtr = typename page::ShellPage<int>::CommandPtr;
			using ParamsCommand = typename page::ShellPage<int>::ParamsCommand;

			auto ListCommandFunc = [&](const ParamsList& params)->void {
				if (params.size() == 0)
				{
					focus_page()->PrintCommandList("-n");
				}
				else
				{
					focus_page()->PrintCommandList(params[0]);
				}
			};
			auto ListCommandCond = [](const ParamsList& params)->bool {
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
					console::PrintMessage("'" + std::string(define::GADT_SHELL_COMMAND_LIST_NAME) + "' only accept -n or -t as parameter");
					return false;
				}
				console::PrintMessage("'" + std::string(define::GADT_SHELL_COMMAND_LIST_NAME) + "' only accept one paramater");
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
					if (this->focus_page()->ExistCommand(name) || this->focus_page()->ExistChildPage(name))
					{
						return true;
					}
					else
					{
						console::PrintMessage(std::string("command '") + params.front() + "' not found.");
						return false;
					}
				}
				console::PrintMessage(std::string("'") + define::GADT_SHELL_COMMAND_HELP_NAME + "' only accept one parameter.");
				return false;
			};

			auto cd_command = CommandPtr(new ParamsCommand(
				define::GADT_SHELL_COMMAND_CD_NAME,
				define::GADT_SHELL_COMMAND_CD_DESC,
				[&](const ParamsList& params)->void {
				std::stringstream ss;
				for (auto p : params)
					ss << p;
				ChangeDirectory(ss.str());
			},
				define::DefaultParamsCheck
				));
			auto bat_command = CommandPtr(new ParamsCommand(
				define::GADT_SHELL_COMMAND_BAT_NAME,
				define::GADT_SHELL_COMMAND_BAT_DESC,
				[&](const ParamsList& params)->void { LoadBatCommand(params); },
				define::DefaultParamsCheck
			));
			auto exit_command = CommandPtr(new ParamsCommand(
				define::GADT_SHELL_COMMAND_EXIT_NAME,
				define::GADT_SHELL_COMMAND_EXIT_DESC,
				[&](const ParamsList& params)->void { set_focus_page(nullptr); },
				define::DefaultNoParamsCheck
			));
			auto list_command = CommandPtr(new ParamsCommand(
				define::GADT_SHELL_COMMAND_LIST_NAME,
				define::GADT_SHELL_COMMAND_LIST_DESC,
				ListCommandFunc,
				ListCommandCond
			));
			auto help_command = CommandPtr(new ParamsCommand(
				define::GADT_SHELL_COMMAND_HELP_NAME,
				define::GADT_SHELL_COMMAND_HELP_DESC,
				HelpCommandFunc,
				HelpCommandCond
			));
			auto clear_command = CommandPtr(new ParamsCommand(
				define::GADT_SHELL_COMMAND_CLEAR_NAME,
				define::GADT_SHELL_COMMAND_CLEAR_DESC,
				[&](const ParamsList& data)->void { this->ClearScreen(); },
				define::DefaultNoParamsCheck
			));

			_shell_cmd.add_command(define::GADT_SHELL_COMMAND_CD_NAME, cd_command);
			_shell_cmd.add_command(define::GADT_SHELL_COMMAND_BAT_NAME, bat_command);
			_shell_cmd.add_command(define::GADT_SHELL_COMMAND_EXIT_NAME, exit_command);
			_shell_cmd.add_command(define::GADT_SHELL_COMMAND_LIST_NAME, list_command);
			_shell_cmd.add_command(define::GADT_SHELL_COMMAND_HELP_NAME, help_command);
			_shell_cmd.add_command(define::GADT_SHELL_COMMAND_CLEAR_NAME, clear_command);
		}

		//execute command in focus page.
		void GameShell::ExecuteCommand(page::PageBasePtr page_ptr, std::string command, const ParamsList& params)
		{
			if (_shell_cmd.ExistCommand(command))
			{
				_shell_cmd.ExecuteCommand(command, params);
			}
			else
			{
				page_ptr->ExecuteCommand(command, params);
			}
		}

		//run single command.
		bool GameShell::RunSingleCommand(std::string command_str)
		{
			command::CommandParser parser(command_str);
			if (parser.no_commands() || !parser.is_legal())
			{
				console::PrintError("inavailable command!");
				return false;
			}
			auto target_page_ptr = focus_page()->GetRelativePathPage(parser.GetPathParser());
			if (target_page_ptr != nullptr)
			{
				std::string cmd_name = parser.fir_command();
				if (target_page_ptr->ExistCommand(cmd_name) || exist_shell_cmd(cmd_name))
				{
					ExecuteCommand(target_page_ptr, parser.fir_command(), parser.params());
					//if (!no_focus_page())//exit command was executed.
					//	set_focus_page(temp_page_ptr);
					return true;
				}
				else
				{
					console::PrintError("command " + cmd_name + " not found");
				}
			}
			else
			{
				console::PrintError(name() + ": unexcepted command: " + command_str);
			}
			return false;
		}

		//run multi command.
		bool GameShell::RunMultiCommand(std::string command_str)
		{
			auto multi_cmd = CommandParser::DivideString(command_str, define::GADT_SHELL_SEPARATOR_COMMAND);
			for (auto single_command : multi_cmd)
			{
				if (RunSingleCommand(single_command) == false)
					return false;
			}
			return true;
		}

		//start from root.
		void GameShell::Run(std::string init_command)
		{
			be_focus();
			ClearScreen();

			std::string command = init_command;
			for (;;)
			{
				if (no_focus_page())
				{
					return;//exit command was executed.
				}
				if (command.empty())
				{
					InputTip();
					command = GetInput();
				}
				else
				{
					RunMultiCommand(command);
					command.clear();
				}
			}
		}
	}
}