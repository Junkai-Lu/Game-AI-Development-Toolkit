/*
* game shell is used for game AI developing and this shell is consist of the concept we called 'pages',
* each page is an single set of command and include data that binded to each page by use template.
*
* version: 2017/1/16
* copyright: Junkai Lu
* email: Junkai-Lu@outlook.com
*/

#include "gadtlib.h"
#include "gameshell.h"

namespace gadt
{
	//ShellPageBase
	ShellPageBase::ShellPageBase(GameShell* belonging_shell, std::string name) :
		_belonging_shell(belonging_shell),
		_name(name),
		_index(AllocPageIndex()),
		_info([]()->void {})
	{
	}
	void ShellPageBase::ShowPath() const
	{
		if (_call_source == nullptr)
		{
			console::Cprintf(GameShell::focus_game()->name(), console::color::purple);
			std::cout << " @ ";
			console::Cprintf(_name, console::color::yellow);
		}
		else
		{
			_call_source->ShowPath();
			std::cout << "/";
			console::Cprintf(_name, console::color::green);
		}
	}
	void ShellPageBase::CleanScreen() const
	{
		system("cls");
		_info();
		std::cout << ">> ";
		console::Cprintf("[ Shell ", console::color::deep_yellow);
		console::Cprintf("<" + _name + ">", console::color::yellow);
		console::Cprintf(" ]\n", console::color::deep_yellow);

		std::cout << ">> ";
		console::Cprintf("use 'help' to get more command\n\n", console::color::deep_green);
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
