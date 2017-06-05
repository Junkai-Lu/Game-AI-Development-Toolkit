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
	//ShellPageBase

	const char* ShellPageBase::g_HELP_COMMAND	= "help";
	const char* ShellPageBase::g_EXIT_COMMAND	= "return";
	const char* ShellPageBase::g_CLEAN_COMMAND	= "clean";

	ShellPageBase::ShellPageBase(GameShell* belonging_shell, std::string name) :
		_belonging_shell(belonging_shell),
		_name(name),
		_index(AllocPageIndex()),
		_info_func([]()->void {}),
		_constructor_func([]()->void {}),
		_destructor_func([]()->void {})
	{
	}
	void ShellPageBase::ShowPath() const
	{
		if (_call_source == nullptr)
		{
			console::Cprintf(GameShell::focus_game()->name(), console::PURPLE);
			std::cout << " @ ";
			console::Cprintf(_name, console::YELLOW);
		}
		else
		{
			_call_source->ShowPath();
			std::cout << "/";
			console::Cprintf(_name, console::GREEN);
		}
	}
	void ShellPageBase::CleanScreen() const
	{
		console::SystemClear();
		_info_func();
		std::cout << ">> ";
		console::Cprintf("[ Shell ", console::DEEP_YELLOW);
		console::Cprintf("<" + _name + ">", console::YELLOW);
		console::Cprintf(" ]\n", console::DEEP_YELLOW);
		std::cout << ">> ";
		console::Cprintf(std::string("use '") + std::string(g_HELP_COMMAND) + std::string("' to get more command\n\n"), console::DEEP_GREEN);
	}
	void ShellPageBase::BeFocus()
	{
		belonging_shell()->_focus_page = this;
	}

	//GameShell
	GameShell* GameShell::_g_focus_game = nullptr;
	GameShell::GameShell(std::string name) :
		_name(name)
	{
		BeFocus();
	}
}
