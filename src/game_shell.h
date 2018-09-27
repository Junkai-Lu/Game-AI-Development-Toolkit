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

#include "gshell_args.hpp"
#include "gshell_command.h"
#include "gshell_page.h"

#pragma once

namespace gadt
{
	namespace shell
	{
		/*
		* GameShell a unix-style shell. allow user to add function and pages.
		* the root page would be created defaultly. use .root() member functuon to get it.
		* start shell by call Run() member function, whose parameter can be a command and it would be executed after shell start.
		* 
		* GameShell(std::string name) is the only constructor function, which need a name as its parameter.
		*/
		class GameShell final
		{
		private:

			using CommandParser = ::gadt::shell::command::CommandParser;

		private:
			//global variable
			static GameShell* _g_focus_game;		//focus page, that is used for show path.

			std::string				_name;			//name of the shell.
			page::ShellPage<int>	_shell_cmd;		//shell_cmd
			page::ShellPage<int>	_root_page;		//root_page.
			page::PageBasePtr		_focus_page;	//focus page.

			//std::map<std::string, PagePtr>	_page_table;	//page table.

		private:
			
			//return true if the page name exist.
			inline bool page_exist(std::string name) const
			{
				return focus_page()->ExistChildPage(name);
			}

			//to be the focus.
			inline void be_focus()
			{
				_g_focus_game = this;
			}

			//get focus_page of this shell.
			inline page::PageBasePtr focus_page() const
			{
				return _focus_page;
			}

			//return true if focus page is nullptr(means exit.)
			inline bool no_focus_page() const
			{
				return _focus_page == nullptr;
			}

			//set focus page.
			inline void set_focus_page(page::PageBasePtr page_ptr)
			{
				if (page_ptr->belonging_shell() == this)
				{
					_focus_page = page_ptr;
				}
			}

			//return true if command exist in shell command list.
			inline bool exist_shell_cmd(std::string name) const
			{
				return _shell_cmd.ExistCommand(name);
			}

			//global function
			static inline GameShell* focus_game()
			{
				return _g_focus_game;
			}

		private:

			//default info func of game shell.
			static void DefaultInfoFunc();

			void ChangeDirectory(std::string path)
			{
				command::CommandParser parser(path);
				auto temp_page_ptr = focus_page()->GetRelativePathPage(parser);
				if (temp_page_ptr != nullptr)
				{
					set_focus_page(temp_page_ptr);
				}
				else
				{
					console::Cprintf(name() + ": cd " + path + " : No such page.", console::ConsoleColor::White);
					console::PrintEndLine();
				}	
			}

			//load and run (multi) batch command
			void LoadBatCommand(const ParamsList& params);

			//print current dir.
			void PrintFocusPath() const;

			//print input tips.
			void InputTip(std::string tip = "");

			//get input line by string format.
			std::string GetInput();

			//clear screen
			void ClearScreen() const;

			//initialize shell commands.
			void InitializeShellCommands();

			//execute command in focus page.
			void ExecuteCommand(page::PageBasePtr page_ptr, std::string command, const ParamsList& params);

			//run single command.
			bool RunSingleCommand(std::string command_str);

			//run multi command.
			bool RunMultiCommand(std::string command_str);

		public:
			/*
			* Get the name of shell, which was decied when shell created.
			*
			* return: a string.
			*/
			inline std::string name() const
			{
				return _name;
			}

			/*
			* Get the root page of shell, which is a inner member of GameShell.
			*
			* return: a pointer to ::gadt::shell::page::ShellPage<int>.
			*/
			inline page::ShellPage<int>* root()
			{
				return &_root_page;
			}

		public:

			/*
			* Create game shell and give it a name.
			*/
			GameShell(std::string name);

			/*
			* Copy constructor is disallowed.
			*/
			GameShell(GameShell&) = delete;

			/*
			* Start shell.
			*
			* [init_command] is the default command that would be executed after shell started.
			*
			* No return value.
			*/
			void Run(std::string init_command = "");
		};

	}
}
