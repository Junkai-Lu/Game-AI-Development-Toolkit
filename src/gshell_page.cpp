#include "gshell_page.h"

namespace gadt
{
	namespace shell
	{
		//shell pages class
		namespace page
		{
			//ShellPageBase
			ShellPageBase::ShellPageBase(PageBasePtr parent_page, ShellPageBase::ShellPtr belonging_shell, std::string name, InfoFunc info_func) :
				_parent_page(parent_page),
				_belonging_shell(belonging_shell),
				_name(name),
				_info_func(info_func),
				_index(AllocPageIndex()),
				_child_pages()
			{
			}

			//get root page of current page.
			PageBasePtr ShellPageBase::GetRootPage()
			{
				if (parent_page() == nullptr)
					return this;
				return parent_page()->GetRootPage();
			}

			//get relative path page. return nullptr if page not found.
			PageBasePtr ShellPageBase::GetRelativePathPage(command::CommandParser parser)
			{
				PageBasePtr temp_page_ptr = this;
				if (parser.is_relative() == false)
				{
					temp_page_ptr = GetRootPage();
				}
				for (size_t i = 0; i < define::GADT_SHELL_MAX_PAGE_LAYER; i++)
				{
					if (parser.no_commands())
					{
						return temp_page_ptr;
					}
					else
					{
						std::string page_name = parser.fir_command();
						if (page_name == define::GADT_SHELL_PAGE_LAST_STR && temp_page_ptr->parent_page() != nullptr)
						{
							temp_page_ptr = temp_page_ptr->parent_page();
							parser.to_next_command();
						}
						else if (page_name == define::GADT_SHELL_PAGE_THIS_STR)
						{
							//temp_page_ptr = temp_page_ptr->parent_page();
							parser.to_next_command();
						}
						else if (temp_page_ptr->ExistChildPage(page_name))
						{
							temp_page_ptr = temp_page_ptr->GetChildPagePtr(page_name);
							parser.to_next_command();
						}
						else
						{
							parser.clear();
							return nullptr;
						}
					}
				}
				return nullptr;
			}

			//print path of current page 
			void ShellPageBase::PrintPath() const
			{
				if (parent_page() == nullptr)
				{
					console::Cprintf(name(), console::ConsoleColor::Yellow);
				}
				else
				{
					parent_page()->PrintPath();
					console::Cprintf(name(), console::ConsoleColor::Green);
				}
				std::cout << "/";
			}

			//return true if the page exist.
			bool ShellPageBase::ExistChildPage(std::string name) const
			{
				if (_child_pages.count(name) > 0)
					return true;
				return false;
			}

			//get child page pointer by name.
			PageBasePtr ShellPageBase::GetChildPagePtr(std::string name) const
			{
				if (ExistChildPage(name))
				{
					return _child_pages.at(name).ptr.get();
				}
				return nullptr;
			}

			//get child page description by name.
			std::string ShellPageBase::GetChildPageDesc(std::string name) const
			{
				if (ExistChildPage(name))
				{
					return _child_pages.at(name).desc;
				}
				return "";
			}

			//get child page help description by name.
			std::string ShellPageBase::GetChildPageHelpDesc(std::string name) const
			{
				if (ExistChildPage(name))
				{
					return _child_pages.at(name).help_desc;
				}
				return "";
			}

			//return true if command name is legal.
			bool ShellPageBase::CheckCommandNameLegality(std::string command) const
			{
				if (command.size() > define::GADT_SHELL_COMMAND_MAX_NAME_LENGTH)
				{
					console::PrintError("command '" + command + "' out of max length");
					return false;
				}
				if (!command::CommandParser::CheckStringLegality(command))
				{
					console::PrintError("illegal command name '" + command + "'.");
					return false;
				}
				return true;
			}
		}
	}
}
