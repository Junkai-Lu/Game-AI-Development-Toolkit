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

#include "gadtlib.h"

#pragma once

namespace gadt
{
	namespace shell
	{
		//allow check warning if it is true.
		constexpr const bool g_ENABLE_SHELL_WARNING = true;
		constexpr const size_t g_SHELL_COMMAND_TYPE_NUMBER = 3;

		//Shell Page Interface
		class GameShell;

		//command type
		enum CommandType :uint8_t
		{
			NORMAL_COMMAND = 0,
			CHILD_PAGE_COMMAND = 1,
			DEFAULT_COMMAND = 2
		};

		//data of command
		template<typename DataType>
		struct CommandData
		{
			using CommandFunc = std::function<void(DataType&)>;

			const CommandType _type;
			const std::string _desc;
			const CommandFunc _command_func;
			const std::string _child_page_name;

			CommandData():
				_type(NORMAL_COMMAND),
				_desc(),
				_command_func(),
				_child_page_name()
			{
			}
			CommandData(CommandFunc command_func, std::string desc, CommandType type) :
				_type(type),
				_desc(desc),
				_command_func(command_func),
				_child_page_name()
			{
			}
			CommandData(std::string child_page_name, std::string desc) :
				_type(CHILD_PAGE_COMMAND),
				_desc(desc),
				_command_func(),
				_child_page_name(child_page_name)
			{
			}
		};

		//data of custom command
		template<typename DataType>
		struct CustomCommandData
		{
			using ConditionFunc = std::function<bool(std::string)>;
			using ActionFunc = std::function<void(DataType&)>;

			const std::string _name;
			const std::string _desc;
			const ConditionFunc _condition;
			const ActionFunc _action;

			CustomCommandData(std::string name, std::string desc, ConditionFunc condition, ActionFunc action) :
				_name(name),
				_desc(desc),
				_condition(condition),
				_action(action)
			{
			}
		};

		//ShellPageBase
		class ShellPageBase
		{
			friend class GameShell;
		public:
			using InfoFunc = std::function<void()>;

		private:
			GameShell*		_belonging_shell;	//the game shell this page belong to.
			std::string		_name;				//name, each name correspond to one page in a game shell.
			size_t			_index;				//page index, each page have a unique index.
			ShellPageBase*	_call_source;		//call source point to the page that call this page.

		protected:
			//static paramaters.
			static const char*  g_SHELL_HELP_COMMAND_STR;			//help command, default is 'help'
			static const char*  g_SHELL_RETURN_COMMAND_STR;			//command that return to previous page, default is 'return'
			static const char*  g_SHELL_CLEAN_COMMAND_STR;			//clean screen command, default is 'clear'
			static const char*  g_SHELL_EXIT_COMMAND_STR;			//command that exit the program, default is 'exit'
			static const size_t g_SHELL_MAX_COMMAND_LENGTH;			//max length of the command. 

		protected:
			InfoFunc			_info_func;			//info function. would be called before show menu.

			virtual void constructor() = 0;
			virtual void destructor() = 0;

			inline void set_call_source(ShellPageBase* call_source)
			{
				_call_source = call_source;
			}

			//get belonging shell.
			inline GameShell* belonging_shell() const
			{
				return _belonging_shell;
			}

			//get call source.
			inline ShellPageBase* call_source() const
			{
				return _call_source;
			}

			//get page name.
			inline std::string name() const
			{
				return _name;
			}

			//get page index.
			inline size_t index() const
			{
				return _index;
			}

			//print path.
			void ShowPath() const;

			//To be focus.
			void BeFocus();

			//allocate page index.
			static inline size_t AllocPageIndex()
			{
				static size_t page_index = 0;
				return page_index++;
			}

			//create a new page.
			ShellPageBase(GameShell* belonging_shell, std::string name);

			//copy constructor function is disallowed.
			ShellPageBase(ShellPageBase& sb) = delete;

			//a virtural function.
			virtual void Run(ShellPageBase* call_source) = 0;

		public:
			void CleanScreen() const;
			virtual ~ShellPageBase() = default;

			//add info function about this page, the func would be execute before the page works. 
			inline void SetInfoFunc(InfoFunc info_func)
			{
				_info_func = info_func;
			}
		};

		//Shell Page
		template<typename DataType>
		class ShellPage final :public ShellPageBase
		{
		public:
			using CommandDataType = CommandData<DataType>;
			using CustomCommandDataType = CustomCommandData<DataType>;
			using ConstructorFunc = std::function<void(DataType&)>;
			using DestructorFunc = std::function<void(DataType&)>;
			using CommandFunc = typename CommandDataType::CommandFunc;
			using ConditionFunc = typename CustomCommandDataType::ConditionFunc;
			using ActionFunc = typename CustomCommandDataType::ActionFunc;

		private:
			DataType _data;																			//data of the page.
			std::map<std::string, CommandDataType>	_command_list;									//command list
			std::vector<CustomCommandDataType>		_custom_func_list;								//custom func list
			std::vector<std::string>				_cmd_name_list[g_SHELL_COMMAND_TYPE_NUMBER];	//list of commands by names.

			ConstructorFunc							_constructor_func;	//constructor func. would be called when enter the page.
			DestructorFunc							_destructor_func;	//destructror func. would be called when quit the page.

		private:

			//print command list
			void PrintCommandList()
			{
				std::cout << std::endl;
				//console::Cprintf("[ COMMAND LIST ]\n\n", console::YELLOW);
				for (size_t i = 0 ;i < g_SHELL_COMMAND_TYPE_NUMBER;i++)
				{
					std::string type_name[g_SHELL_COMMAND_TYPE_NUMBER] = 
					{
						"COMMANDS",
						"CHILD PAGES",
						"SHELL COMMANDS"
					};
					if (_cmd_name_list[i].size() > 0)
					{
						std::cout << ">> ";
						console::Cprintf("[" + type_name[i] + "]\n", console::YELLOW);
						for (auto name : _cmd_name_list[i])
						{
							//const std::string& name = pair.first;
							const std::string& desc = get_command_data(name)._desc;
							std::cout << "   '";
							console::Cprintf(name, console::RED);
							std::cout << "'" << std::string(g_SHELL_MAX_COMMAND_LENGTH, ' ').substr(0, g_SHELL_MAX_COMMAND_LENGTH - name.length())
								<< desc << std::endl;
						}
						std::cout << std::endl;
					}
				}
			}

			//init shell and add default commands
			inline void ShellInit()
			{
				//add return describe
				InsertCommand(
					g_SHELL_RETURN_COMMAND_STR, 
					CommandDataType(
						[](DataType&)->void {}, 
						"return to previous menu.", 
						DEFAULT_COMMAND
					)
				);

				//add exit command
				InsertCommand(
					g_SHELL_EXIT_COMMAND_STR,
					CommandDataType(
						[](DataType&)->void {
							exit(0);
						},
						"exit program.",
						DEFAULT_COMMAND
					)
				);

				//add help command.
				InsertCommand(
					g_SHELL_HELP_COMMAND_STR,
					CommandDataType(
						[&](DataType& data)->void {
							PrintCommandList();
						},
						"get command list",
						DEFAULT_COMMAND
					)
				);

				//add clean command.
				InsertCommand(
					g_SHELL_CLEAN_COMMAND_STR,
					CommandDataType(
						[&](DataType& data)->void {
							this->CleanScreen();
						},
						"clean screen.",
						DEFAULT_COMMAND
					)
				);
			}

			//data operator
			inline DataType& data()
			{
				return _data;
			}

			//excute constructor func
			void constructor() override
			{
				_constructor_func(_data);
			}

			//excute destructor func
			void destructor() override
			{
				_destructor_func(_data);
			}

			//return true if the command name exist
			inline bool command_exist(std::string command) const
			{
				return _command_list.count(command) > 0;
			}

			//get func by name.
			inline const CommandDataType& get_command_data(std::string command)
			{
				if (!command_exist(command))
				{
					std::cerr << "function '" << command << "' in page '" << name() << "'not exist" << std::endl;
					console::SystemPause();
				}
				return _command_list[command];
			}

			//run page.
			void Run(ShellPageBase* call_source) override
			{
				set_call_source(call_source);
				BeFocus();
				CleanScreen();

				//excute constructor func.
				constructor();

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
					belonging_shell()->InputTip();
					std::string command;

					//get command that is not empty.
					for (;;)
					{
						command = belonging_shell()->GetInput();
						if (command != "")
						{
							break;
						}
					}

					//return command
					if (command == g_SHELL_RETURN_COMMAND_STR)
					{
						if (call_source != nullptr)
						{
							call_source->CleanScreen();
						}
						break;
					}

					//function command check
					if (command_exist(command))
					{
						CommandDataType& data = _command_list[command];
						if (data._type == NORMAL_COMMAND || data._type == DEFAULT_COMMAND)
						{
							data._command_func(_data);
							continue;
						}
						else if (data._type == CHILD_PAGE_COMMAND)
						{
							destructor();
							belonging_shell()->RunPage(command, this);
							constructor();
							continue;
						}
					}

					//custom command check
					for (const CustomCommandDataType& custom : _custom_func_list)
					{
						if (custom._condition(command) == true)
						{
							custom._action(_data);
							continue;
						}
					}

					//error
					console::Cprintf("ERROR: ", console::PURPLE);
					std::cout << "command not found." << std::endl;
				}

				destructor();	//excute destructor func.
			}

			//curtail command if the length of command is out of max length.
			inline std::string curtail_command(std::string command)
			{
				if (command.size() > g_SHELL_MAX_COMMAND_LENGTH)
				{
					return command.substr(0, g_SHELL_MAX_COMMAND_LENGTH);
				}
				return command;
			}

			//insert command data
			inline void InsertCommand(std::string command, CommandDataType&& data)
			{
				_cmd_name_list[data._type].push_back(command);
				_command_list.insert(std::pair<std::string, CommandDataType>(command, data));
			}

			//insert custom command
			inline void InsertCustomComand(CustomCommandDataType&& data)
			{
				_custom_func_list.push_back(data);
			}

		public:

			//default function.
			ShellPage(GameShell* belonging_shell, std::string name) :
				ShellPageBase(belonging_shell, name),
				_data(),
				_command_list(),
				_custom_func_list(),
				_constructor_func([](DataType&)->void {}),
				_destructor_func([](DataType&)->void {})
			{
				ShellInit();
			}

			//create a new shell page.
			template<class... Types>
			ShellPage(GameShell* belonging_shell, std::string name, Types&&... args) :
				ShellPageBase(belonging_shell, name),
				_data(std::forward<Types>(args)...),
				_command_list(),
				_custom_func_list(),
				_constructor_func([](DataType&)->void {}),
				_destructor_func([](DataType&)->void {})
			{
				ShellInit();
			}

			//copy constructor is disallowed.
			ShellPage(ShellPage&) = delete;

			//deconstor function, DO NOT EXECUTE ALONE.
			~ShellPage()
			{
			}

			//add child page of this page,the name page should exist in same shell and the name is also the command to enter this page.
			inline void AddChildPage(std::string child_name, std::string desc)
			{
				child_name = curtail_command(child_name);
				if (child_name != name())
				{
					InsertCommand(child_name, CommandDataType(child_name, desc));
				}
				else
				{
					console::Cprintf("INIT ERROR: ", console::PURPLE);
					console::Cprintf(" page ", console::GRAY);
					console::Cprintf(name(), console::RED);
					console::Cprintf(" add itself as child page!\n", console::GRAY);
					console::SystemPause();
				}
			}

			//add a function that can be execute by command and is allowed to visit the data binded in this page.
			inline void AddFunction(std::string command, CommandFunc func, std::string desc)
			{
				command = curtail_command(command);
				InsertCommand(command, CommandDataType(func, desc, NORMAL_COMMAND));
			}

			//use a extend function to check and execute command, if the command is legal that the function should return 'true'.
			inline void AddCustomCommand(std::string name, std::string desc, ConditionFunc condition, ActionFunc action)
			{
				InsertCustomComand(CustomCommandDataType(name, desc, condition, action));
			}

			//add constructor func, it would be called when enter page. 
			inline void SetConstructorFunc(ConstructorFunc constructor_func)
			{
				_constructor_func = constructor_func;
			}

			//add destructor func, it would be called when quit page.
			inline void SetDestructorFunc(DestructorFunc destructor_func)
			{
				_destructor_func = destructor_func;
			}

		};

		//Shell
		class GameShell final
		{
			friend ShellPageBase;
		public:
			using InfoFunc = typename ShellPageBase::InfoFunc;
			using PagePtr = std::shared_ptr<ShellPageBase>;

		private:
			//global variable
			static GameShell* _g_focus_game;	//focus page, that is used for show path.									
			std::map<std::string, PagePtr>	_page_table;	//page table
			std::string						_name;			//name of the shell
			ShellPageBase*					_focus_page;	//focus page.
			InfoFunc						_info_func;		//default info func.

		public:
			
			//global function
			static inline GameShell* focus_game()
			{
				return _g_focus_game;
			}

			//print input tips.
			void InputTip(std::string tip = "")
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
						console::Cprintf(tip, console::GREEN);
						std::cout << ": >> ";
					}
					else
					{
						console::Cprintf("ERROR: focus page not exist", console::PURPLE);
					}
				}
				else
				{
					console::Cprintf("ERROR: focus game not exist", console::PURPLE);
				}
			}

			//get input line by string format.
			std::string GetInput()
			{
				char buffer[50];
				std::cin.getline(buffer, 50);
				return std::string(buffer);
			}

			//public function
			GameShell(std::string name);

			//copy constructor is disallowed.
			GameShell(GameShell&) = delete;

			//get name of shell.
			inline std::string name() const
			{
				return _name;
			}

			//get focus_page of this shell.
			inline ShellPageBase* focus_page() const
			{
				return _focus_page;
			}

			//return true if the page name exist.
			inline bool page_exist(std::string name) const
			{
				return _page_table.count(name) > 0;
			}

			//add new page to this shell.
			inline void AddPage(std::string name, std::shared_ptr<ShellPageBase> new_page)
			{
				_page_table[name] = new_page;
			}

			//to be the focus.
			inline void BeFocus()
			{
				_g_focus_game = this;
			}

			//run page.
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

			//set info func for all pages
			inline void SetDefaultInfoFunc(InfoFunc info_func)
			{
				_info_func = info_func;
			}

			//create shell page, default data type is <int>
			template<typename DataType = int>
			ShellPage<DataType>* CreateShellPage(std::string page_name)
			{
				GADT_CHECK_WARNING(g_ENABLE_SHELL_WARNING, page_exist(page_name), "SHELL01: repeation of page name.");
				std::shared_ptr<ShellPage<DataType>> ptr(new ShellPage<DataType>(this, page_name));
				AddPage(page_name, ptr);
				ptr.get()->SetInfoFunc(_info_func);
				return ptr.get();
			}

			//create shell page with initilized value, default data type is <int>
			template<typename DataType, class... Types>
			ShellPage<DataType>* CreateShellPage(std::string page_name, Types&&... args)
			{
				GADT_CHECK_WARNING(g_ENABLE_SHELL_WARNING, page_exist(page_name), "SHELL01: repeation of page name.");
				std::shared_ptr<ShellPage<DataType>> ptr(new ShellPage<DataType>(this, page_name, std::forward<Types>(args)...));
				AddPage(page_name, ptr);
				ptr.get()->SetInfoFunc(_info_func);
				return ptr.get();
			}
		};
	}
}
