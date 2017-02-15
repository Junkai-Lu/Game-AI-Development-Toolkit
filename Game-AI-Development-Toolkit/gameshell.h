/*
* game shell is used for game AI developing and this shell is consist of the concept we called 'pages',
* each page is an single set of command and include data that binded to each page by use template.
*
* version: 2017/2/15
* copyright: Junkai Lu
* email: Junkai-Lu@outlook.com
*/

#include "gadtlib.h"

#pragma once

namespace gadt
{
	//Shell Page Interface
	class GameShell;
	class ShellPageBase
	{
		friend class GameShell;
	private:
		std::string _name;				//name, each name correspond to one page in a game shell.
		size_t _index;					//page index, each page have a unique index.
		ShellPageBase* _call_source;	//call source point to the page that call this page.
		GameShell* _belonging_shell;	//the game shell this page belong to.
	protected:std::function<void()> _info;	//point to a function that shows infos about page. default is empty. 

	protected:

		inline void set_call_source(ShellPageBase* call_source)
		{
			_call_source = call_source;
		}
		inline void set_info_func(std::function<void()> info)
		{
			_info = info;
		}
		inline GameShell* belonging_shell() const
		{
			return _belonging_shell;
		}
		inline ShellPageBase* call_source() const
		{
			return _call_source;
		}
		inline std::string name() const
		{
			return _name;
		}
		inline size_t index() const
		{
			return _index;
		}

		void ShowPath() const;
		void BeFocus();
		static inline size_t AllocPageIndex()
		{
			static size_t page_index = 0;
			return page_index++;
		}
		ShellPageBase(GameShell* belonging_shell, std::string name);
		ShellPageBase(ShellPageBase& sb) = delete;
		virtual void Run(ShellPageBase* call_source) = 0;

	public:
		void CleanScreen() const;
		virtual ~ShellPageBase() = default;

	};

	//Shell Page
	template<typename datatype>
	class ShellPage :public ShellPageBase
	{
		friend class GameShell;

	private:
		std::map<std::string, std::function<void(datatype&)> > _func;
		std::map<std::string, std::string> _des;
		std::set<std::string> _child;
		std::function<bool(std::string, datatype&)> _extra_command;
		datatype _data;

	private:
		//default function.
		ShellPage(GameShell* belonging_shell, std::string name) :
			ShellPageBase(belonging_shell, name),
			_extra_command([](std::string a, datatype& b)->bool {return false; })
		{
			ShellInit();
		}
		ShellPage(GameShell* belonging_shell, std::string name, datatype data) :
			ShellPageBase(belonging_shell, name),
			_data(data),
			_extra_command([](std::string a, datatype& b)->bool {return false; })
		{
			ShellInit();
		}
		ShellPage(ShellPage&) = delete;

		inline void ShellInit()
		{
			AddDescript("return", "return to previous menu.");
			//add 'help' command.
			AddFunction("help", [&](datatype& data)->void {
				std::cout << std::endl << ">> ";
				console::Cprintf("[ COMMAND LIST ]\n\n", console::color::yellow);
				for (auto command : _des)
				{
					std::cout << "   '";
					console::Cprintf(command.first, console::color::red);
					std::cout << "'" << std::string("          ").substr(0, 10 - command.first.length()) << command.second << std::endl;
				}
				std::cout << std::endl << std::endl;
			}, "get command list");

			//add 'cls' command.
			AddFunction("cls", [&](datatype& data)->void {
				this->CleanScreen();
			}, "clean screen.");
		}

		//data operator
		inline datatype& data()
		{
			return _data;
		}
		inline bool func_exist(std::string command) const
		{
			return _func.count(command) > 0;
		}
		inline bool child_exist(std::string command) const
		{
			return _child.count(command) > 0;
		}
		inline bool des_exist(std::string command) const
		{
			return _des.count(command) > 0;
		}
		inline std::function<void(datatype&)> get_func(std::string command)
		{
			if (!func_exist(command))
			{
				std::cerr << "function '" << command << "' in page '" << name() << "'not exist" << std::endl;
				system("pause");
			}
			return _func[command];
		}
		inline std::string get_des(std::string command) const
		{
			if (!func_exist(command))
			{
				std::cerr << "descript '" << command << "' in page '" << name() << "'not exist" << std::endl;
				system("pause");
			}
			return _des[command];
		}

		void Run(ShellPageBase* call_source)
		{
			set_call_source(call_source);
			BeFocus();
			CleanScreen();

			for (;;)
			{
				/*
				* command check order
				* 1.return
				* 2.function
				* 3.child shell
				* 4.extra command
				*/
				BeFocus();	//ensure this page is always the focus.
				GameShell::InputTip();
				std::string command;

				//get command that is not empty.
				for (;;)
				{
					command = GameShell::GetInput();
					if (command != "")
					{
						break;
					}
				}

				//return command
				if (command == "return")
				{
					if (call_source != nullptr)
					{
						call_source->CleanScreen();
					}
					break;
				}

				//function command check
				if (func_exist(command))
				{
					get_func(command)(_data);
					continue;
				}

				//child shell check
				if (child_exist(command))
				{
					belonging_shell()->RunPage(command, this);
					continue;
				}

				//extra command check
				if (_extra_command(command, _data))
				{
					continue;
				}

				//error
				console::Cprintf("ERROR: ", console::color::purple);
				std::cout << "command not found." << std::endl;
			}
		}

	public:
		//deconstor function, DO NOT EXECUTE ALONE.
		~ShellPage()
		{

		}

		//add child page of this page,the name page should exist in same shell and the name is also the command to enter this page.
		inline void AddChildPage(std::string child_name, std::string des)
		{
			if (child_name != name())
			{
				_des[child_name] = des;
				_child.insert(child_name);
			}
			else
			{
				console::Cprintf("INIT ERROR: ", console::color::purple);
				console::Cprintf(" page ", console::color::gray);
				console::Cprintf(name(), console::color::red);
				console::Cprintf(" add itself as child page!\n", console::color::gray);
				system("pause");
			}
		}

		//add a function that can be execute by command and is allowed to visit the data binded in this page.
		inline void AddFunction(std::string command, std::function<void(datatype&)> func, std::string des)
		{
			_func[command] = func;
			_des[command] = des;
		}

		//add/cover descript, the command should be added in other place.
		inline void AddDescript(std::string command, std::string des)
		{
			_des[command] = des;
		}

		//use a extend function to check and execute command, if the command is legal that the function should return 'true'.
		inline void AddExtarCommand(std::function<bool(std::string, datatype&)> func, std::string command, std::string des)
		{
			AddDescript(command, des);
			_extra_command = func;
		}

		//add info function about this page, the func would be execute before the page works. 
		inline void AddInfoFunc(std::function<void()> info)
		{
			set_info_func(info);
		}


	};

	//Shell
	class GameShell
	{
		friend ShellPageBase;

	private:
		//global variable
		static GameShell* _g_focus_game;	//focus page, that is used for show path.

		//page table
		std::map<std::string, std::shared_ptr<ShellPageBase>> _page_table;
		std::string _name;
		ShellPageBase* _focus_page;

	public:
		//global function
		static inline GameShell* focus_game()
		{
			return _g_focus_game;
		}
		static inline void InputTip(std::string tip = "")
		{
			if (focus_game() != nullptr)
			{
				if (focus_game()->focus_page() != nullptr)
				{
					focus_game()->focus_page()->ShowPath();
					if (tip != "")
					{
						std::cout << "/";
					}
					console::Cprintf(tip, console::color::green);
					std::cout << ": >> ";
				}
				else
				{
					console::Cprintf("ERROR: focus page not exist", console::color::purple);
				}
			}
			else
			{
				console::Cprintf("ERROR: focus game not exist", console::color::purple);
			}
		}
		static inline std::string GetInput()
		{
			char buffer[50];
			std::cin.getline(buffer, 50);
			return std::string(buffer);
		}

		//public function
		GameShell(std::string name);
		GameShell(GameShell&) = delete;
		inline std::string name() const
		{
			return _name;
		}
		inline ShellPageBase* focus_page() const
		{
			return _focus_page;
		}
		inline bool page_exist(std::string name) const
		{
			return _page_table.count(name) > 0;
		}
		inline void BeFocus()
		{
			_g_focus_game = this;
		}
		inline void RunPage(std::string name, ShellPageBase* call_source = nullptr)
		{
			if (page_exist(name))
			{
				_page_table[name]->Run(call_source);
			}
			else
			{
				std::cerr << "page '" << name << "' not exist" << std::endl;
			}
		}
		template<typename datatype> ShellPage<datatype>* CreatePage(std::string name)
		{
			std::shared_ptr<ShellPage<datatype>> ptr(new ShellPage<datatype>(this, name));
			_page_table[name] = ptr;
			return ptr.get();
		}
		template<typename datatype> ShellPage<datatype>* CreatePage(std::string name, datatype data)
		{
			std::shared_ptr<ShellPage<datatype>> ptr(new ShellPage<datatype>(this, name, data));
			_page_table[name] = ptr;
			return ptr.get();
		}

	};
}
