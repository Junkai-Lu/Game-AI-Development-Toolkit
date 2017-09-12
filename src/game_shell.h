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
		//declartion.
		class GameShell;
		class ShellPageBase;
		using DirList = std::list<std::string>;
		using ParamsList = std::vector<std::string>;
		using ParamsCheckFunc = std::function<bool(const ParamsList&)>;

		//shell defination.
		namespace define
		{
			constexpr const bool   g_SHELL_ENABLE_WARNING = true;			//enable warning.
			constexpr const size_t g_COMMAND_TYPE_NUMBER = 5;		//command type number.
			constexpr const size_t g_MAX_COMMAND_LENGTH = 15;		//max length of the command. 

			//list command, default is 'ls'
			constexpr const char*  g_LIST_COMMAND_NAME = "ls";	
			constexpr const char*  g_LIST_COMMAND_DESC = "get command list";

			//help command, default is 'help'
			constexpr const char*  g_HELP_COMMAND_NAME = "help";	
			constexpr const char*  g_HELP_COMMAND_DESC = "get all shell command";

			//command that return to previous page, default is '..'
			constexpr const char*  g_RETURN_COMMAND_NAME = "..";	
			constexpr const char*  g_RETURN_COMMAND_DESC = "return to previous menu.";

			//command that return to root page, default is '.'
			constexpr const char*  g_ROOT_COMMAND_NAME = ".";		
			constexpr const char*  g_ROOT_COMMAND_DESC = "return to root page.";

			//clean screen command, default is 'clear'
			constexpr const char*  g_CLEAR_COMMAND_NAME = "clear";	
			constexpr const char*  g_CLEAR_COMMAND_DESC = "clean screen.";

			//command that exit the program, default is 'exit'
			constexpr const char*  g_EXIT_COMMAND_NAME = "exit";
			constexpr const char*  g_EXIT_COMMAND_DESC = "exit program.";

			//get the name of command type.
			std::string GetCommandTypeName(size_t i);
			
			//get the symbol of command type.
			std::string GetCommandTypeSymbol(size_t i);

			//default params check func.
			bool DefaultParamsCheck(const ParamsList& list);

			//default no params check func.
			bool DefaultNoParamsCheck(const ParamsList& list);


		}

		//shell command class.
		namespace command
		{
			//command type
			enum CommandType :uint8_t
			{
				DEFAULT_COMMAND = 0,
				DATA_COMMAND = 1,
				PARAMS_COMMAND = 2,
				DATA_AND_PARAMS_COMMAND = 3,
				CHILD_PAGE_COMMAND = 4
			};

			//Command Parser.
			class CommandParser
			{
			private:
				bool		_is_legal;
				DirList		_commands;
				ParamsList	_params;

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

				//default constructor.
				CommandParser() :
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
				inline void to_next_command()
				{
					_commands.pop_front();
				}

				//clear data.
				inline void clear()
				{
					_is_legal = false;
					_commands.clear();
					_params.clear();
				}

				//return true if the parser is legal.
				inline bool is_legal() const
				{
					return _is_legal;
				}

				//refresh parser.
				inline void refresh(std::string original_command)
				{
					clear();
					_is_legal = ParseOriginalCommand(original_command);
				}

				//return true if the command is the only command
				inline bool is_last_command() const
				{
					return _commands.size() == 1;
				}

				//return true if there are no commands.
				inline bool no_commands() const
				{
					return _commands.size() == 0;
				}

				//return true if there are no parameters.
				inline bool no_params() const
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
				inline const std::vector<std::string>& params() const
				{
					return _params;
				}

				//get the first command.
				inline std::string fir_command() const
				{
					if (_is_legal && _commands.size() > 0)
					{
						return _commands.front();
					}
					return "";
				}

				//get next state of current parser.
				CommandParser GetNext() const
				{
					auto temp = _commands;
					if (temp.size() > 0)
					{
						temp.pop_front();
					}
					return CommandParser(_is_legal, temp, _params);
				}
			};

			//Command Data Base
			template<typename DataType>
			class CommandBase
			{
			public:
				using DefaultCommandFunc = std::function<void()>;
				using DataCommandFunc = std::function<void(DataType&)>;
				using ParamsCommandFunc = std::function<void(const ParamsList&)>;
				using DataAndParamsCommandFunc = std::function<void(DataType&, const ParamsList&)>;

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

			public:

				//waning if excute failed.
				void wrong_params_warning() const
				{
					console::ShowError("illegal parameters");
				}

				//excute params check.
				inline bool params_check(const ParamsList& list)
				{
					return _params_check(list);
				}

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
				virtual void Run(DataType&, const ParamsList&) = 0;
			};

			//default func command record.
			template<typename DataType>
			class DefaultCommand;

			//data only command record.
			template<typename DataType>
			class DataCommand;

			//default func command record.
			template<typename DataType>
			class ParamsCommand;

			//data and params command record.
			template<typename DataType>
			class DataAndParamsCommand;

			template<typename DataType>
			class ChildPageCommand;

		}

		//shell pages class
		namespace page
		{
			//ShellPageBase
			class ShellPageBase
			{
				friend class ::gadt::shell::GameShell;
			public:
				using InfoFunc		= std::function<void()>;
				using CommandParser	= command::CommandParser;

			private:
				GameShell*		_belonging_shell;	//the game shell this page belong to.
				std::string		_name;				//name, each name correspond to one page in a game shell.
				size_t			_index;				//page index, each page have a unique index.
				ShellPageBase*	_call_source;		//call source point to the page that call this page.

			protected:

				InfoFunc _info_func;			//info function. would be called before show menu.

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

				//create a new page.
				ShellPageBase(GameShell* belonging_shell, std::string name);

				//copy constructor function is disallowed.
				ShellPageBase(ShellPageBase& sb) = delete;

				//execute command.
				virtual void ExecuteCommand(std::string command, const ParamsList&) = 0;

				//print command list.
				virtual void PrintCommandList(std::string param) const = 0;

				//return true if the command exist.
				virtual bool ExistCommand(std::string name) const = 0;

				//return true if the function exist.
				virtual bool ExistFunc(std::string name) const = 0;

				//return true if the page exist.
				virtual bool ExistChildPage(std::string name) const = 0;

			public:

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
				friend class ::gadt::shell::GameShell;

				using CommandBase				= command::CommandBase<DataType>;
				using DefaultCommand			= command::DefaultCommand<DataType>;
				using DataCommand				= command::DataCommand<DataType>;
				using ParamsCommand				= command::ParamsCommand<DataType>;
				using DataAndParamsCommand		= command::DataAndParamsCommand<DataType>;
				using ChildPageCommand			= command::ChildPageCommand<DataType>;
				using CommandPtr				= std::unique_ptr<CommandBase>;
				using DefaultCommandFunc		= typename CommandBase::DefaultCommandFunc;
				using DataCommandFunc			= typename CommandBase::DataCommandFunc;
				using ParamsCommandFunc			= typename CommandBase::ParamsCommandFunc;
				using DataAndParamsCommandFunc	= typename CommandBase::DataAndParamsCommandFunc;
				
			private:

				DataType _data;											//data of the page.
				std::map<std::string, CommandPtr>		_command_list;	//command list
				std::vector<std::vector<std::string>>	_cmd_name_list;	//list of commands by names.

			private:

				//print command list
				void PrintCommandList(std::string param) const override
				{
					
					std::cout << std::endl;
					if (param == "-t")
					{
						for (size_t i = 0; i < define::g_COMMAND_TYPE_NUMBER; i++)
						{

							if (_cmd_name_list[i].size() > 0)
							{
								std::cout << ">> ";
								console::Cprintf("[" + define::GetCommandTypeName(i) + "]\n", console::YELLOW);
								for (std::string name : _cmd_name_list[i])
								{
									//const std::string& name = pair.first;
									std::string desc = _command_list.at(name)->desc();
									std::cout << "   '";
									console::Cprintf(name, console::RED);
									std::cout << "'" << std::string(define::g_MAX_COMMAND_LENGTH, ' ').substr(0, define::g_MAX_COMMAND_LENGTH - name.length())
										<< desc << std::endl;
								}
								std::cout << std::endl;
							}
						}
					}
					else//-n
					{
						std::cout << ">> ";
						console::Cprintf("[ COMMANDS ]\n", console::YELLOW);
						for (const auto& pair : _command_list)
						{
							//const std::string& name = pair.first;
							auto name = pair.first;
							auto desc = pair.second.get()->desc();
							auto type = pair.second.get()->type();
							//std::cout << "  ";
							//console::Cprintf(define::GetCommandTypeSymbol(type), console::GRAY);
							std::cout << "   '";
							console::Cprintf(name, console::RED);
							std::cout << "'" << std::string(define::g_MAX_COMMAND_LENGTH, ' ').substr(0, define::g_MAX_COMMAND_LENGTH - name.length())
								<< desc << std::endl;
						}
						std::cout << std::endl;
					}
				}

				//return true if the command name exist
				bool ExistCommand(std::string command) const override
				{
					return _command_list.count(command) > 0;
				}

				//return true if the function exist.
				bool ExistFunc(std::string name) const override
				{
					if (ExistCommand(name))
					{
						if (_command_list.at(name).get()->type() != command::CHILD_PAGE_COMMAND)
						{
							return true;
						}
					}
					return false;
				}

				//return true if the page exist.
				bool ExistChildPage(std::string name) const override
				{
					if (ExistCommand(name))
					{
						if (_command_list.at(name).get()->type() == command::CHILD_PAGE_COMMAND)
						{
							return true;
						}
					}
					return false;
				}

				//data operator
				inline DataType& data()
				{
					return _data;
				}

				//return true if the command is legal.
				inline bool CheckCommandNameLegality(std::string command)
				{
					if (command.size() > define::g_MAX_COMMAND_LENGTH)
					{
						console::ShowError("command '" + command + "' out of max length");
						return false;
					}
					if (!command::CommandParser::CheckStringLegality(command))
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
					_command_list(),
					_cmd_name_list(define::g_COMMAND_TYPE_NUMBER)
				{
				}

				//create a new shell page.
				template<class... Types>
				ShellPage(GameShell* belonging_shell, std::string name, Types&&... args) :
					ShellPageBase(belonging_shell, name),
					_data(std::forward<Types>(args)...),
					_command_list(),
					_cmd_name_list(define::g_COMMAND_TYPE_NUMBER)
				{
				}

				//copy constructor is disallowed.
				ShellPage(ShellPage&) = delete;

				

			public:

				

				//execute command by name
				void ExecuteCommand(std::string command, const ParamsList& params) override
				{
					if (ExistCommand(command))
					{
						auto& cmd = _command_list[command];
						if (cmd->params_check(params) == true)
						{
							cmd->Run(_data, params);
						}
						else
						{
							cmd->wrong_params_warning();
						}
					}
					else
					{
						console::ShowError("command " + command + " not found");
					}
				}

				/*
				* overloaded AddFunction.
				* add DefaultCommand which do not have any parameters.
				*
				* [name] is the name of command.
				* [func] is the added function, type = void()
				* [desc] is description of the command.
				*/
				void AddFunction(std::string name, std::string desc, DefaultCommandFunc func)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new DefaultCommand(name, desc, func));
						InsertCommand(name, command_ptr);
					}
				}

				/*
				* overloaded AddFunction.
				* add DataCommand that allows to operator binded data without parameters
				*
				* [name] is the name of command.
				* [func] is the added function, type = void(DataType&)
				* [desc] is description of the command.
				*/
				void AddFunction(std::string name, std::string desc, DataCommandFunc func)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new DataCommand(name, desc, func));
						InsertCommand(name, command_ptr);
					}
				}

				/*
				* overloaded AddFunction.
				* add ParamsCommand which allows to be executed with parameters.
				*
				* [name] is the name of command.
				* [func] is the added function, type = void(const ParamsList&)
				* [desc] is description of the command.
				* [check] is the parameters check function, type = bool(const ParamsList&)
				*/
				void AddFunction(std::string name, std::string desc, ParamsCommandFunc func, ParamsCheckFunc check = define::DefaultParamsCheck)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new ParamsCommand(name, desc, func, check));
						InsertCommand(name, command_ptr);
					}
				}

				/*
				* overloaded AddFunction.
				* add DataAndParamsCommand which allows to operator binded data with parameters.
				*
				* [name] is the name of command.
				* [func] is the added function, type = void(DataType&, const ParamsList&)
				* [desc] is description of the command.
				* [check] is the parameters check function, type = bool(const ParamsList&)
				*/
				void AddFunction(std::string name, std::string desc, DataAndParamsCommandFunc func, ParamsCheckFunc check = define::DefaultParamsCheck)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new DataAndParamsCommand(name, desc, func, check));
						InsertCommand(name, command_ptr);
					}
				}

				/*
				* AddChildPage.
				* add one page as the child page of current page.
				*
				* [child_name] is the name of child page.
				* [desc] is description of the command.
				*/
				inline void AddChildPage(std::string child_name, std::string desc)
				{
					if (CheckCommandNameLegality(child_name))
					{
						auto child_page_ptr = CommandPtr(new ChildPageCommand(child_name, desc, belonging_shell(), child_name));
						InsertCommand(child_name, child_page_ptr);
					}
				}
			};
		}


		/*
		* GameShell a unix-style shell. allow user to add function and pages.
		*
		* [name] is the name of the shell.
		*/
		class GameShell final
		{
			friend ::gadt::shell::page::ShellPageBase;
		private:
			using CommandParser = ::gadt::shell::command::CommandParser;
			using ShellPageBase = ::gadt::shell::page::ShellPageBase;

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
			page::ShellPage<int>			_shell_cmd;		//shell_cmd

		private:
			//print current dir.
			void PrintDir() const;

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
				return _page_table.count(name) > 0 || name == define::g_RETURN_COMMAND_NAME || name == define::g_ROOT_COMMAND_NAME;
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

			//clear screen
			void ClearScreen() const;

			//initialize shell commands.
			void ShellCmdInit();

			//execute command in focus page.
			void ExecuteCommandInFocusPage(std::string command, const ParamsList& params);

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

		public:
			//get name of shell.
			inline std::string name() const
			{
				return _name;
			}

			//public constructor function
			GameShell(std::string name);

			//copy constructor is disallowed.
			GameShell(GameShell&) = delete;

			//start from appointed page.
			void StartFromPage(std::string name);

			//enter page by name.
			void EnterPage(std::string name);

			//set info func for all pages
			inline void SetDefaultInfoFunc(InfoFunc info_func)
			{
				_info_func = info_func;
			}

			//create shell page, default data type is <int>
			template<typename DataType = int>
			page::ShellPage<DataType>* CreateShellPage(std::string page_name)
			{
				if (page_exist(page_name))
				{
					console::ShowError("repeation of create page " + page_name);
					console::SystemPause();
					return nullptr;
				}
				std::shared_ptr<page::ShellPage<DataType>> ptr(new page::ShellPage<DataType>(this, page_name));
				add_page(page_name, ptr);
				ptr.get()->SetInfoFunc(_info_func);
				return ptr.get();
			}

			//create shell page with initilized value, default data type is <int>
			template<typename DataType, class... Types>
			page::ShellPage<DataType>* CreateShellPage(std::string page_name, Types&&... args)
			{
				if (page_exist(page_name))
				{
					console::ShowError("repeation of create page " + page_name);
					console::SystemPause();
					return nullptr;
				}
				std::shared_ptr<page::ShellPage<DataType>> ptr(new page::ShellPage<DataType>(this, page_name, std::forward<Types>(args)...));
				add_page(page_name, ptr);
				ptr.get()->SetInfoFunc(_info_func);
				return ptr.get();
			}
		};

		namespace command
		{
			//default func command record.
			template<typename DataType>
			class DefaultCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::DefaultCommandFunc;

			private:
				CommandFunc _default_command_func;

			public:

				DefaultCommand(std::string name, std::string desc, CommandFunc default_command_func) :
					CommandBase<DataType>(DEFAULT_COMMAND, name, desc, define::DefaultNoParamsCheck),
					_default_command_func(default_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					_default_command_func();
				}
			};

			//data only command record.
			template<typename DataType>
			class DataCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::DataCommandFunc;

			private:
				CommandFunc _no_params_command_func;

			public:
				DataCommand(std::string name, std::string desc, CommandFunc no_params_command_func) :
					CommandBase<DataType>(DATA_COMMAND, name, desc, define::DefaultNoParamsCheck),
					_no_params_command_func(no_params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					_no_params_command_func(data);
				}
			};

			//default func command record.
			template<typename DataType>
			class ParamsCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::ParamsCommandFunc;

			private:
				CommandFunc _params_command_func;

			public:

				ParamsCommand(std::string name,std::string desc, CommandFunc params_command_func,ParamsCheckFunc check) :
					CommandBase<DataType>(PARAMS_COMMAND, name, desc, check),
					_params_command_func(params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					_params_command_func(params);
				}
			};

			//data and params command record.
			template<typename DataType>
			class DataAndParamsCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::DataAndParamsCommandFunc;

			private:
				CommandFunc _data_params_command_func;

			public:
				DataAndParamsCommand(std::string name, std::string desc, CommandFunc data_params_command_func, ParamsCheckFunc params_check ) :
					CommandBase<DataType>(DATA_AND_PARAMS_COMMAND, name, desc, params_check),
					_data_params_command_func(data_params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					_data_params_command_func(data, params);
				}
			};

			//child page command record
			template<typename DataType>
			class ChildPageCommand :public CommandBase<DataType>
			{
			private:
				::gadt::shell::GameShell* _belonging_shell;
				std::string _page_name;

			public:

				ChildPageCommand(std::string name, std::string desc, GameShell* belonging_shell, std::string page_name) :
					CommandBase<DataType>(CHILD_PAGE_COMMAND, name, desc, [](const ParamsList& list)->bool {return list.size() == 0 ? true : false; }),
					_belonging_shell(belonging_shell),
					_page_name(page_name)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					_belonging_shell->EnterPage(_page_name);
				}
			};
		}
	}
}
