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

		//declartion.
		class ShellPageBase;
		class GameShell;
		using ParamsList = std::vector<std::string>;

		//Command Parser.
		class CommandParser
		{
		private:
			bool						_is_legal;
			std::list<std::string>		_commands;
			std::vector<std::string>	_params;

		private:

			//parse the command.
			bool ParseParameters(std::string params);

			//parse command
			bool ParseCommands(std::string commands);

			//init by command
			bool ParseOriginalCommand(std::string original_command);

			//constructor by initialized.
			CommandParser(bool is_legal, const std::list<std::string>& commands, const std::vector<std::string>& params) :
				_is_legal(is_legal),
				_commands(commands),
				_params(params)
			{
			}

		public:

			//return true if the string is legal
			static bool CheckStringLegality(std::string str);

			CommandParser():
				_is_legal(false),
				_commands(),
				_params()
			{
			}

			//constructor function by command string.
			CommandParser(std::string original_command) :
				_is_legal(false),
				_commands(),
				_params()
			{
				_is_legal = ParseOriginalCommand(original_command);
			}

			//constructor function by parent command parser
			void to_next_command()
			{
				_commands.pop_front();
			}

			//clear data.
			void clear()
			{
				_is_legal = false;
				_commands.clear();
				_params.clear();
			}

			//return true if the parser is legal.
			bool is_legal() const
			{
				return _is_legal;
			}

			//refresh parser.
			void refresh(std::string original_command)
			{
				clear();
				_is_legal = ParseOriginalCommand(original_command);
			}

			//return true if the command is the only command
			bool is_last_command() const
			{
				return _commands.size() == 1;
			}
			
			//return true if there are no commands.
			bool no_commands() const
			{
				return _commands.size() == 0;
			}

			//return true if there are no parameters.
			bool no_params() const
			{
				return _params.size() == 0;
			}

			//print details
			void print_details() const
			{
				std::cout << "commands:" << std::endl;
				for (auto cmd : _commands)
				{
					std::cout << "  <" << cmd << ">" << std::endl;
				}
				std::cout << "parameters:" << std::endl;
				for (auto param : _params)
				{
					std::cout << "  <" << param << ">" << std::endl;
				}
			}

			//get the params in this command.
			const std::vector<std::string>& params() const
			{
				return _params;
			}

			//get the first command.
			std::string fir_command() const
			{
				if (_is_legal && _commands.size() > 0)
				{
					return _commands.front();
				}
				return "";
			}
		};

		//command type
		enum CommandType :uint8_t
		{
			DEFAULT_COMMAND = 0,
			PARAMS_COMMAND = 1,
			DATA_COMMAND = 2,
			DATA_AND_PARAMS_COMMAND = 3,
			CHILD_PAGE_COMMAND = 4
		};

		//Command Data Base
		template<typename DataType>
		class CommandBase
		{
		public:
			using ParamsCheckFunc			= std::function<bool(const ParamsList&)>;
			using DefaultCommandFunc		= std::function<void()>;
			using DataCommandFunc			= std::function<void(DataType&)>;
			using ParamsCommandFunc			= std::function<void(const ParamsList&)>;
			using DataAndParamsCommandFunc	= std::function<void(DataType&, const ParamsList&)>;

		private:

			const CommandType		_type;
			const std::string		_name;
			const std::string		_desc;
			const ParamsCheckFunc	_params_check;

		protected:
			//constructor.
			CommandBase(CommandType type, std::string name, std::string desc, ParamsCheckFunc params_check) :
				_type(type),
				_name(name),
				_desc(desc),
				_params_check(params_check)
			{
			}

			//waning if excute failed.
			void wrong_params_warning() const
			{
				console::Cprintf("ERROR", console::PURPLE);
				std::cout << ": ";
				console::Cprintf("illegal parameters\n", console::RED);
			}

			//excute params check.
			bool params_check(const ParamsList& list)
			{
				return _params_check(list);
			}

		public:
			//get type.
			CommandType type() const
			{
				return _type;
			}

			//get name.
			std::string name() const
			{
				return _name;
			}

			//get desc.
			std::string desc() const
			{
				return _desc;
			}

			//excute interface.
			virtual void ExecuteCommand(DataType&, const ParamsList&) = 0;
		};

		//default func command record.
		template<typename DataType>
		class DefaultCommand
		{
		private:
			DefaultCommandFunc _no_params_command_func;
		};

		//data only command record.
		template<typename DataType>
		class DataCommand:public CommandBase<DataType>
		{
		private:
			DataCommandFunc _no_params_command_func;

		public:
			DataCommand(CommandType type, std::string name, std::string desc, DataCommandFunc no_params_command_func):
				CommandBase<DataType>(type, name, desc, [](const ParamsList& list)->bool {return list.size() == 0 ? true : false; }),
				_no_params_command_func(no_params_command_func)
			{
			}

			void ExecuteCommand(DataType& data, const ParamsList& params) override
			{
				bool is_legal_params = params_check(params);
				if (is_legal_params)
				{
					_no_params_command_func(data);
				}
				else
				{
					wrong_params_warning();
				}
			}
		};

		//data and params command record.
		template<typename DataType>
		class DataAndParamsCommand :public CommandBase<DataType>
		{
		private:
			DataAndParamsCommandFunc _params_command_func;

		public:
			DataAndParamsCommand(
				CommandType type,
				std::string name, 
				std::string desc, 
				DataAndParamsCommandFunc params_command_func, 
				ParamsCheckFunc params_check
			):
				CommandBase<DataType>(type, name, desc, params_check),
				_params_command_func(_params_command_func)
			{
			}

			void ExecuteCommand(DataType& data, const ParamsList& params) override
			{
				bool is_legal_params = _params_check(params);
				if (is_legal_params)
				{
					_params_command_func(data, params);
				}
				else
				{
					wrong_params_warning();
				}
			}
		};

		//child page command record
		template<typename DataType>
		class ChildPageCommand :public CommandBase<DataType>
		{
		private:
			GameShell* _belonging_shell;
			std::string _page_name;

		public:

			ChildPageCommand(std::string name, std::string desc, GameShell* belonging_shell, std::string page_name):
				CommandBase<DataType>(CHILD_PAGE_COMMAND, name, desc, [](const ParamsList& list)->bool {return list.size() == 0 ? true : false; }),
				_belonging_shell(belonging_shell),
				_page_name(page_name)
			{
			}

			void ExecuteCommand(DataType& data, const ParamsList& params) override
			{
				_belonging_shell->EnterPage(_page_name);
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

		public:
			//static paramaters.
			static const char*  g_SHELL_HELP_COMMAND_STR;			//help command, default is 'ls'
			static const char*  g_SHELL_RETURN_COMMAND_STR;			//command that return to previous page, default is '..'
			static const char*  g_SHELL_ROOT_COMMAND_STR;			//command that return to previous page, default is '.'
			static const char*  g_SHELL_CLEAR_COMMAND_STR;			//clean screen command, default is 'clear'
			static const char*  g_SHELL_EXIT_COMMAND_STR;			//command that exit the program, default is 'exit'
			static const size_t g_SHELL_MAX_COMMAND_LENGTH;			//max length of the command. 

		protected:

			InfoFunc			_info_func;			//info function. would be called before show menu.

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

			//allocate page index.
			static inline size_t AllocPageIndex()
			{
				static size_t page_index = 0;
				return page_index++;
			}

			//default params check func.
			static inline bool DefaultParamsCheck()
			{
				return true;
			}

			//create a new page.
			ShellPageBase(GameShell* belonging_shell, std::string name);

			//copy constructor function is disallowed.
			ShellPageBase(ShellPageBase& sb) = delete;

			//execute command.
			virtual void ExecuteCommand(std::string command, const ParamsList&) = 0;

		public:
			//clear
			void ClearScreen() const;
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
		private:

			friend class GameShell;
			using CommandBase				= CommandBase<DataType>;
			using CommandPtr			= std::unique_ptr<CommandBase>;
			using DataAndParamsCommand		= DataAndParamsCommand<DataType>;
			using DataCommand	= DataCommand<DataType>;
			using ChildPageCommand	= ChildPageCommand<DataType>;
			using ParamsList			= typename CommandBase::ParamsList;
			using ParamsCheckFunc		= typename CommandBase::ParamsCheckFunc;
			using DataAndParamsCommandFunc		= typename CommandBase::DataAndParamsCommandFunc;
			using DataCommandFunc	= typename CommandBase::DataCommandFunc;

		private:

			DataType _data;																			//data of the page.
			std::map<std::string, CommandPtr>	_command_list;									//command list
			std::vector<std::string>				_cmd_name_list[g_SHELL_COMMAND_TYPE_NUMBER];	//list of commands by names.

		private:

			//print command list
			void PrintCommandList()
			{
				std::cout << std::endl;
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
							const std::string& desc = _command_list[name].get()->desc();
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
			void DefaultCommandsInit()
			{
				auto root_command = CommandPtr(new ChildPageCommand(
					g_SHELL_ROOT_COMMAND_STR,
					"return to root page.",
					belonging_shell(),
					g_SHELL_ROOT_COMMAND_STR
				));

				auto return_command = CommandPtr(new ChildPageCommand(
					g_SHELL_RETURN_COMMAND_STR, 
					"return to previous menu.",
					belonging_shell(),
					g_SHELL_RETURN_COMMAND_STR
				));

				auto exit_command = CommandPtr(new DataCommand(
					DEFAULT_COMMAND,
					g_SHELL_EXIT_COMMAND_STR,
					"exit program.",
					[](DataType&)->void { exit(0);}
				));

				auto help_command = CommandPtr(new DataCommand(
					DEFAULT_COMMAND,
					g_SHELL_HELP_COMMAND_STR,
					"get command list",
					[&](DataType& data)->void { this->PrintCommandList();}
				));

				auto clear_command = CommandPtr(new DataCommand(
					DEFAULT_COMMAND,
					g_SHELL_CLEAR_COMMAND_STR,
					"clean screen.",
					[&](DataType& data)->void { this->ClearScreen(); }
				));

				InsertCommand(g_SHELL_ROOT_COMMAND_STR, root_command);
				InsertCommand(g_SHELL_RETURN_COMMAND_STR, return_command);
				InsertCommand(g_SHELL_EXIT_COMMAND_STR, exit_command);
				InsertCommand(g_SHELL_HELP_COMMAND_STR, help_command);
				InsertCommand(g_SHELL_CLEAR_COMMAND_STR, clear_command);
			}

			//data operator
			inline DataType& data()
			{
				return _data;
			}

			//return true if the command name exist
			inline bool command_exist(std::string command) const
			{
				return _command_list.count(command) > 0;
			}

			//return true if the command is legal.
			inline bool CheckCommandNameLegality(std::string command)
			{
				if (command.size() > g_SHELL_MAX_COMMAND_LENGTH)
				{
					console::ShowError("command '" + command + "' out of max length");
					return false;
				}
				if (!CommandParser::CheckStringLegality(command))
				{
					console::ShowError("illegal command name '" + command + "'.");
					return false;
				}
				return true;
			}

			//insert command.
			inline void InsertCommand(std::string name, CommandPtr& cmd_data_ptr)
			{
				_cmd_name_list[cmd_data_ptr.get()->type()].push_back(name);
				_command_list.insert(std::pair<std::string, CommandPtr>(name, std::move(cmd_data_ptr)));
			}

			//default function.
			ShellPage(GameShell* belonging_shell, std::string name) :
				ShellPageBase(belonging_shell, name),
				_data(),
				_command_list()
			{
				DefaultCommandsInit();
			}

			//create a new shell page.
			template<class... Types>
			ShellPage(GameShell* belonging_shell, std::string name, Types&&... args) :
				ShellPageBase(belonging_shell, name),
				_data(std::forward<Types>(args)...),
				_command_list()
			{
				DefaultCommandsInit();
			}

			//copy constructor is disallowed.
			ShellPage(ShellPage&) = delete;

		public:

			//execute command by name
			void ExecuteCommand(std::string command, const ParamsList& params) override
			{
				if (command_exist(command))
				{
					_command_list[command]->ExecuteCommand(_data, params);
				}
				else
				{
					console::ShowError("command " + command + " not found");
				}
			}

			//add child page of this page,the name page should exist in same shell and the name is also the command to enter this page.
			inline void AddChildPage(std::string child_name, std::string desc)
			{
				if (CheckCommandNameLegality(child_name))
				{
					auto child_page_ptr = CommandPtr(new ChildPageCommand(child_name, desc, belonging_shell(), child_name));
					InsertCommand(child_name, child_page_ptr);
				}
			}

			//add a function that can be execute by command and is allowed to visit the data binded in this page.
			void AddFunction(std::string command, DataCommandFunc func, std::string desc)
			{
				if (CheckCommandNameLegality(command))
				{
					auto command_ptr = CommandPtr(new DataCommand(DATA_COMMAND, command, desc, func));
					InsertCommand(command, command_ptr);
				}
			}

			//add a function that can be execute by command and is allowed to visit the data binded in this page.
			void AddFunction(std::string command, DataAndParamsCommandFunc func, std::string desc, ParamsCheckFunc check = DefaultParamsCheck)
			{
				if (CheckCommandNameLegality(command))
				{
					auto command_ptr = CommandPtr(new DataAndParamsCommand(DATA_AND_PARAMS_COMMAND, command, desc, func, check));
					InsertCommand(command, command_ptr);
				}
			}
		};

		//Shell
		class GameShell final
		{
			friend ShellPageBase;
		private:
			using InfoFunc = typename ShellPageBase::InfoFunc;
			using PagePtr = std::shared_ptr<ShellPageBase>;
			using DirList = std::list<std::string>;

		private:
			//global variable
			static GameShell* _g_focus_game;				//focus page, that is used for show path.									
			std::map<std::string, PagePtr>	_page_table;	//page table.
			DirList							_dir_list;		//list of dirs.
			std::string						_name;			//name of the shell.
			InfoFunc						_info_func;		//default info func.

		private:
			void ShowPath() const;

			//print input tips.
			void InputTip(std::string tip = "");

			//get input line by string format.
			std::string GetInput()
			{
				char buffer[256];
				std::cin.getline(buffer, 256);
				return std::string(buffer);
			}

			//return true if the page name exist.
			inline bool page_exist(std::string name) const
			{
				return _page_table.count(name) > 0 || name == ShellPageBase::g_SHELL_RETURN_COMMAND_STR;
			}

			//to be the focus.
			inline void be_focus()
			{
				_g_focus_game = this;
			}

			//add page
			inline void add_page(std::string name, std::shared_ptr<ShellPageBase> new_page)
			{
				_page_table[name] = new_page;
			}

			//return to previous page.
			void ReturnToPreviousPage();

			//return to root page
			inline void ReturnToRootPage()
			{
				std::string temp = _dir_list.front();
				_dir_list.clear();
				_dir_list.push_back(temp);
			}

		public:
			
			//get focus_page of this shell.
			inline ShellPageBase* focus_page() const
			{
				if (_dir_list.size() > 0)
				{
					return _page_table.at(_dir_list.back()).get();
				}
				return nullptr;

			}

			//global function
			static inline GameShell* focus_game()
			{
				return _g_focus_game;
			}

			//get name of shell.
			inline std::string name() const
			{
				return _name;
			}

			//public function
			GameShell(std::string name);

			//copy constructor is disallowed.
			GameShell(GameShell&) = delete;

			//start from appointed page.
			void StartFromPage(std::string name);

			//run page.
			void EnterPage(std::string name);

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
				add_page(page_name, ptr);
				ptr.get()->SetInfoFunc(_info_func);
				return ptr.get();
			}

			//create shell page with initilized value, default data type is <int>
			template<typename DataType, class... Types>
			ShellPage<DataType>* CreateShellPage(std::string page_name, Types&&... args)
			{
				GADT_CHECK_WARNING(g_ENABLE_SHELL_WARNING, page_exist(page_name), "SHELL01: repeation of page name.");
				std::shared_ptr<ShellPage<DataType>> ptr(new ShellPage<DataType>(this, page_name, std::forward<Types>(args)...));
				add_page(page_name, ptr);
				ptr.get()->SetInfoFunc(_info_func);
				return ptr.get();
			}
		};
	}
}
