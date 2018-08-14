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

#include "gadtlib.h"
#include "gadt_table.h"

#pragma once

namespace gadt
{
	namespace shell
	{
		//declartion.
		class GameShell;
		class ShellPageBase;
		class TestPage;

		using DirList = std::list<std::string>;
		using ParamsList = std::vector<std::string>;
		using ParamsCheckFunc = std::function<bool(const ParamsList&)>;

		//shell defination.
		namespace define
		{
			constexpr const size_t GADT_SHELL_COMMAND_TYPE_COUNT = 6;		//command type number.
			constexpr const size_t GADT_SHELL_COMMAND_MAX_NAME_LENGTH = 20;	//max length of the command. 
			constexpr const size_t GADT_SHELL_COMMAND_MAX_DESC_LENGTH = 40;	//max length of the command. 
			constexpr const size_t GADT_SHELL_MAX_PAGE_LAYER = 256;			//max page layer. 

			constexpr const char*  GADT_SHELL_PAGE_LAST_STR = "..";
			constexpr const char*  GADT_SHELL_PAGE_THIS_STR = ".";

			constexpr const char*  GADT_SHELL_COMMAND_SYMBOL = "[F]";
			constexpr const char*  GADT_SHELL_PAGE_SYMBOL = "[P]";

			//list command, default is 'ls'
			constexpr const char* GADT_SHELL_COMMAND_LIST_NAME = "ls";	
			constexpr const char* GADT_SHELL_COMMAND_LIST_DESC = "get command list";

			//help command, default is 'help'
			constexpr const char* GADT_SHELL_COMMAND_HELP_NAME = "help";	
			constexpr const char* GADT_SHELL_COMMAND_HELP_DESC = "get all shell command";

			//clean screen command, default is 'clear'
			constexpr const char* GADT_SHELL_COMMAND_CLEAR_NAME = "clear";	
			constexpr const char* GADT_SHELL_COMMAND_CLEAR_DESC = "clean screen.";

			//command that exit the program, default is 'exit'
			constexpr const char* GADT_SHELL_COMMAND_EXIT_NAME = "exit";
			constexpr const char* GADT_SHELL_COMMAND_EXIT_DESC = "exit program.";

			//command that is uese to change directory, default is 'cd'
			constexpr const char* GADT_SHELL_COMMAND_CD_NAME = "cd";
			constexpr const char* GADT_SHELL_COMMAND_CD_DESC = "change directory.";

			//command that is uese to run batch file, default is 'bat'
			constexpr const char* GADT_SHELL_COMMAND_BAT_NAME = "bat";
			constexpr const char* GADT_SHELL_COMMAND_BAT_DESC = "run batch file.";

			constexpr const char* GADT_SHELL_SEPARATOR_PATH = "/";
			constexpr const char* GADT_SHELL_SEPARATOR_PARAMETER = " ";
			constexpr const char* GADT_SHELL_SEPARATOR_COMMAND = ";";

			//default params check func.
			bool DefaultParamsCheck(const ParamsList& list);

			//default params count check func.
			template<size_t COUNT>
			bool DefaultParamsCountCheck(const ParamsList& list)
			{
				return list.size() == COUNT ? true : false;
			}

			//default no params check func.
			bool DefaultNoParamsCheck(const ParamsList& list);
		}

		//shell command class.
		namespace command
		{
			//command type
			enum class CommandType :uint8_t
			{
				DEFAULT_COMMAND = 0,
				DATA_COMMAND = 1,
				PARAMS_COMMAND = 2,
				DATA_AND_PARAMS_COMMAND = 3,
				BOOL_PARAMS_COMMAND = 4,
				BOOL_DATA_AND_PARAMS_COMMAND = 5
			};

			//get the name of command type.
			std::string GetCommandTypeName(CommandType type);

			//get the symbol of command type.
			std::string GetCommandTypeSymbol(CommandType type);

			//Command Parser.
			class CommandParser
			{
			private:
				bool		_is_legal;
				bool		_is_relative;
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
				CommandParser(bool is_legal, bool is_relative, const std::list<std::string>& commands, const std::vector<std::string>& params) :
					_is_legal(is_legal),
					_is_relative(is_relative),
					_commands(commands),
					_params(params)
				{
				}

			public:

				//remove space behind or after the string.
				static std::string RemoveSpace(std::string str);

				//divide single string into multi string by separator.
				static std::vector<std::string> DivideString(std::string str, std::string separator);

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

				//return true if the parser is relative path.
				inline bool is_relative() const
				{
					return _is_relative;
				}

				//refresh parser.
				inline void refresh(std::string original_command)
				{
					clear();
					_is_legal = ParseOriginalCommand(original_command);
				}

				//add command if it is not empty
				inline void add_command(std::string cmd)
				{
					if (cmd.length() > 0)
						_commands.push_back(cmd);
				}

				//add parameter if it is not empty
				inline void add_parameter(std::string param)
				{
					if (param.length() > 0)
						_params.push_back(param);
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

				//get the last command.
				inline std::string last_command() const
				{
					if (_is_legal && _commands.size() > 0)
					{
						return _commands.back();
					}
					return "";
				}

			public:

				//default constructor.
				CommandParser();

				//constructor function by command string.
				CommandParser(std::string original_command);

				//return true if the string is legal
				static bool CheckStringLegality(std::string str);

				//get next state of current parser.
				CommandParser GetNext() const;

				//get parser of the path.
				CommandParser GetPathParser() const;
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
				using BoolParamsCommandFunc = std::function<bool(const ParamsList&)>;
				using BoolDataAndParamsCommandFunc = std::function<bool(DataType&, const ParamsList&)>;

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
					console::PrintError("illegal parameters");
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

			//default func command with boolean return value record.
			template<typename DataType>
			class BoolParamsCommand;

			//data and params command with boolean return value record.
			template<typename DataType>
			class BoolDataAndParamsCommand;

			//information of single command.
			template<typename DataType>
			struct CommandInfo
			{
				CommandType type;
				std::string desc;
				std::string help_desc;
				std::unique_ptr<CommandBase<DataType>> ptr;
			};
		}

		//shell pages class
		namespace page
		{
			class ShellPageBase;

			template<typename DataType>
			class ShellPage;

			using PageBasePtr = ShellPageBase*;
			using PageBaseHandle = std::unique_ptr<ShellPageBase>;

			//information of single page.
			struct PageInfo
			{
				std::string desc;
				std::string help_desc;
				PageBaseHandle ptr;
			};

			//ShellPageBase
			class ShellPageBase
			{
				friend class ::gadt::shell::GameShell;
			
			protected:
				
				using ShellPtr		= GameShell * ;
				using PageTable		= std::map<std::string, PageInfo>;
				using InfoFunc		= std::function<void()>;
				using CommandParser	= command::CommandParser;

			private:

				const PageBasePtr	_parent_page;		//parent page of this page.
				const ShellPtr		_belonging_shell;	//the game shell this page belong to.
				std::string			_name;				//name, each name correspond to one page in a game shell.
				InfoFunc			_info_func;			//info function. would be called before show menu.
				size_t				_index;				//page index, each page have a unique index.
				PageTable			_child_pages;	//child pages of this page.

			protected:

				inline PageBasePtr parent_page() const
				{
					return _parent_page;
				}

				//get belonging shell.
				inline ShellPtr belonging_shell() const
				{
					return _belonging_shell;
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

				//information function
				inline InfoFunc info_func() const
				{
					return _info_func;
				}

				//add child page to child pages.
				inline void add_child_page(std::string page_name, PageBaseHandle& page_ptr, std::string desc, std::string help_desc)
				{
					_child_pages.insert(std::pair<std::string, PageInfo>(page_name, PageInfo{ desc, help_desc, std::move(page_ptr) }));
				}

				//return child pages.
				inline const PageTable& child_pages() const
				{
					return _child_pages;
				}

			protected:

				//create a new page.
				ShellPageBase(PageBasePtr parent_page ,ShellPtr belonging_shell, std::string name, InfoFunc info_func);

				//copy constructor function is disallowed.
				ShellPageBase(ShellPageBase& sb) = delete;

				//allocate page index.
				static inline size_t AllocPageIndex()
				{
					static size_t page_index = 0;
					return page_index++;
				}

				//get root page of current page.
				PageBasePtr GetRootPage();

				//get relative path page. return nullptr if page not found.
				PageBasePtr GetRelativePathPage(command::CommandParser parser);

				//print path of current page 
				void PrintPath() const;

				//return true if the page exist.
				bool ExistChildPage(std::string name) const;

				//get child page pointer by name.
				PageBasePtr GetChildPagePtr(std::string name) const;

				//get child page description by name.
				std::string GetChildPageDesc(std::string name) const;

				//get child page help description by name.
				std::string GetChildPageHelpDesc(std::string name) const;

				//return true if command name is legal.
				bool CheckCommandNameLegality(std::string command) const;

				//execute command.
				virtual void ExecuteCommand(std::string command, const ParamsList&) = 0;

				//print command list.
				virtual void PrintCommandList(std::string param) const = 0;

				//return true if the command exist.
				virtual bool ExistCommand(std::string name) const = 0;

			public:

				virtual ~ShellPageBase() = default;

				//add info function about this page, the func would be execute before the page works. 
				void SetInfoFunc(InfoFunc info_func, bool recursively = true)
				{
					_info_func = info_func;
					if (recursively)
						for (auto& child_page : _child_pages)
							child_page.second.ptr->SetInfoFunc(info_func, recursively);
				}
			};

			//Shell Page
			template<typename DataType>
			class ShellPage final :public ShellPageBase
			{
			private:
				friend class ::gadt::shell::GameShell;
				friend class ::gadt::shell::TestPage;

				using ShellPageBase::ShellPtr;
				using ShellPageBase::PageTable;
				
				using CommandBase					= command::CommandBase<DataType>;
				using DefaultCommand				= command::DefaultCommand<DataType>;
				using DataCommand					= command::DataCommand<DataType>;
				using ParamsCommand					= command::ParamsCommand<DataType>;
				using DataAndParamsCommand			= command::DataAndParamsCommand<DataType>;
				using BoolParamsCommand				= command::BoolParamsCommand<DataType>;
				using BoolDataAndParamsCommand		= command::BoolDataAndParamsCommand<DataType>;
				using CommandPtr					= std::unique_ptr<CommandBase>;
				using DefaultCommandFunc			= typename CommandBase::DefaultCommandFunc;
				using DataCommandFunc				= typename CommandBase::DataCommandFunc;
				using ParamsCommandFunc				= typename CommandBase::ParamsCommandFunc;
				using DataAndParamsCommandFunc		= typename CommandBase::DataAndParamsCommandFunc;
				using BoolParamsCommandFunc			= typename CommandBase::BoolParamsCommandFunc;
				using BoolDataAndParamsCommandFunc	= typename CommandBase::BoolDataAndParamsCommandFunc;

			private:

				DataType _data;											//data of the page.
				std::map<std::string, CommandPtr>		_command_list;	//command list
				std::vector<std::vector<std::string>>	_cmd_name_list;	//list of commands by names.
				
			private:

				//data operator
				inline DataType& data()
				{
					return _data;
				}

				
				//insert command.
				inline void add_command(std::string name, CommandPtr& cmd_data_ptr)
				{
					_cmd_name_list[static_cast<size_t>(cmd_data_ptr.get()->type())].push_back(name);
					_command_list.insert(std::pair<std::string, CommandPtr>(name, std::move(cmd_data_ptr)));
				}

			protected:

				//print command list
				void PrintCommandList(std::string param) const override
				{
					constexpr size_t SYMBOL_WIDTH = 3;
					constexpr size_t NAME_WIDTH = (define::GADT_SHELL_COMMAND_MAX_NAME_LENGTH + 1) / 2;
					constexpr size_t DESC_WIDTH = (define::GADT_SHELL_COMMAND_MAX_DESC_LENGTH + 1);

					std::cout << std::endl;
					if (param == "-t")
					{
						for (size_t i = 0; i < define::GADT_SHELL_COMMAND_TYPE_COUNT; i++)
						{
							command::CommandType cmd_type = static_cast<command::CommandType>(i);
							if (_cmd_name_list[i].size() > 0)
							{
								std::cout << ">> ";
								console::Cprintf("[" + command::GetCommandTypeName(cmd_type) + "]", console::ConsoleColor::Yellow);
								console::PrintEndLine();
								console::Table tb(3, _cmd_name_list[i].size());
								tb.set_width({ SYMBOL_WIDTH,NAME_WIDTH,DESC_WIDTH });
								for (size_t n = 0; n < _cmd_name_list[i].size(); n++)
								{
									std::string type = command::GetCommandTypeSymbol(cmd_type);
									std::string name = _cmd_name_list[i].at(n);
									std::string desc = _command_list.at(name)->desc();
									tb.set_cell_in_row(n,{
										{ type,console::ConsoleColor::Gray, console::TableAlign::Middle },
										{ name,console::ConsoleColor::Red, console::TableAlign::Left },
										{ desc,console::ConsoleColor::White, console::TableAlign::Left }
									});
								}
								tb.Print(console::TableFrame::CircleAndTight, console::TableIndex::Disable);
								std::cout << std::endl;
							}
						}
					}
					else//-n
					{
						std::cout << ">> ";
						console::Cprintf("[ COMMANDS ]", console::ConsoleColor::Yellow);
						console::PrintEndLine();

						struct CommandInfo
						{
							std::string name;
							std::string desc;
							std::string type;
							bool is_page;
						};
						std::vector<CommandInfo> cmds;

						for (const auto& pair : _command_list)
							cmds.push_back(CommandInfo{ pair.first, pair.second.get()->desc(), define::GADT_SHELL_COMMAND_SYMBOL, false });
		
						for (const auto& pair : child_pages())
							cmds.push_back(CommandInfo{ pair.first, pair.second.desc, define::GADT_SHELL_PAGE_SYMBOL, true });
						
						std::sort(cmds.begin(), cmds.end(), [&](const CommandInfo& fir, const CommandInfo sec)->bool {
							if (fir.name == sec.name)
							{
								if (sec.is_page)
									return false;
							}
							else if (fir.name > sec.name)
							{
								return false;
							}	
							return true;
						});

						console::Table tb(3, cmds.size());
						size_t max_desc_length = 0;
						for (size_t i = 0; i < cmds.size(); i++)
						{
							tb.set_cell_in_row(i, {
								{ cmds[i].type,console::ConsoleColor::Gray, console::TableAlign::Middle },
								{ cmds[i].name,(cmds[i].is_page ? console::ConsoleColor::Blue : console::ConsoleColor::Red)},
								{ cmds[i].desc,console::ConsoleColor::White }
							});
							if (cmds[i].desc.length() > max_desc_length)
								max_desc_length = cmds[i].desc.length();
						}
						max_desc_length = (max_desc_length + 1) / 2 + 2;
						if (max_desc_length > DESC_WIDTH) { max_desc_length = DESC_WIDTH; }
						tb.set_width({ SYMBOL_WIDTH,NAME_WIDTH, max_desc_length });
						tb.Print(console::TableFrame::CircleAndTight, console::TableIndex::Disable);
						std::cout << std::endl;
					}
				}

				//return true if the command name exist
				bool ExistCommand(std::string command) const override
				{
					return _command_list.count(command) > 0;
				}

			public:

				//default function.
				ShellPage(PageBasePtr parent_page, ShellPtr belonging_shell, std::string name, InfoFunc info_func) :
					ShellPageBase(parent_page, belonging_shell, name, info_func),
					_data(),
					_command_list(),
					_cmd_name_list(define::GADT_SHELL_COMMAND_TYPE_COUNT)
				{
				}

				//create a new shell page and initialize data.
				template<class... Types>
				ShellPage(PageBasePtr parent_page, ShellPtr belonging_shell, std::string name, InfoFunc info_func, Types&&... args) :
					ShellPageBase(parent_page, belonging_shell, name, info_func),
					_data(std::forward<Types>(args)...),
					_command_list(),
					_cmd_name_list(define::GADT_SHELL_COMMAND_TYPE_COUNT)
				{
				}

				//copy constructor is disallowed.
				ShellPage(ShellPage&) = delete;

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
						console::PrintError("command " + command + " not found");
					}
				}

				/*
				* overrided AddFunction.
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
						add_command(name, command_ptr);
					}
				}

				/*
				* overrided AddFunction.
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
						add_command(name, command_ptr);
					}
				}

				/*
				* overrided AddFunction.
				* add ParamsCommand which allows to be executed with parameters.
				*
				* [name] is the name of command.
				* [func] is the added function, type = void(const ParamsList&)
				* [desc] is description of the command.
				* [check] is the parameters check function, type = bool(const ParamsList&)
				*/
				void AddFunction(std::string name, std::string desc, ParamsCommandFunc func, ParamsCheckFunc check)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new ParamsCommand(name, desc, func, check));
						add_command(name, command_ptr);
					}
				}

				/*
				* overrided AddFunction.
				* add DataAndParamsCommand which allows to operator binded data with parameters.
				*
				* [name] is the name of command.
				* [func] is the added function, type = void(DataType&, const ParamsList&)
				* [desc] is description of the command.
				* [check] is the parameters check function, type = bool(const ParamsList&)
				*/
				void AddFunction(std::string name, std::string desc,DataAndParamsCommandFunc func, ParamsCheckFunc check)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new DataAndParamsCommand(name, desc, func, check));
						add_command(name, command_ptr);
					}
				}

				/*
				* overrided AddFunction.
				* add BoolParamsCommand which allows to be executed with parameters with a boolean return value.
				*
				* [name] is the name of command.
				* [func] is the added function, type = bool(const ParamsList&)
				* [desc] is description of the command.
				* [check] is the parameters check function, type = bool(const ParamsList&)
				*/
				void AddFunction(std::string name, std::string desc, BoolParamsCommandFunc func)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new BoolParamsCommand(name, desc, func));
						add_command(name, command_ptr);
					}
				}

				/*
				* overrided AddFunction.
				* add BoolDataAndParamsCommand which allows to operator binded data with parameters with a boolean return value.
				*
				* [name] is the name of command.
				* [func] is the added function, type = bool(DataType&, const ParamsList&)
				* [desc] is description of the command.
				* [check] is the parameters check function, type = bool(const ParamsList&)
				*/
				void AddFunction(std::string name, std::string desc, BoolDataAndParamsCommandFunc func)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new BoolDataAndParamsCommand(name, desc, func));
						add_command(name, command_ptr);
					}
				}

				/*
				* CreateChildPage.
				* create a page as the child page of current page.
				*
				* [child_name] is the name of child page.
				* [desc] is description of the command.
				*/
				template<typename ChildDataType = int>
				ShellPage<ChildDataType>* CreateChildPage(std::string page_name, std::string page_desc)
				{
					if (!ExistChildPage(page_name))
					{
						if (CheckCommandNameLegality(page_name))
						{
							auto ptr = new ShellPage<ChildDataType>(this, belonging_shell(), page_name, info_func());
							auto ptr_handle = PageBaseHandle(ptr);
							add_child_page(page_name, ptr_handle, page_desc, page_desc);
							return ptr;
						}
						else
						{
							console::PrintError("unexcepted page name: " + page_name);
						}
					}
					else
					{
						console::PrintError("repeatly create page " + page_name);
					}
					return nullptr;
				}

				/*
				* CreateChildPage.
				* create child page with initilized value, default data type is <int>
				*
				* [child_name] is the name of child page.
				* [desc] is description of the command.
				*/
				template<typename ChildDataType, class... Types>
				ShellPage<ChildDataType>* CreateChildPage(std::string page_name, std::string page_desc, Types&&... args)
				{
					if (!ExistChildPage(page_name))
					{
						if (CheckCommandNameLegality(page_name))
						{
							auto ptr = new ShellPage<ChildDataType>(this, belonging_shell(), page_name, info_func(), std::forward<Types>(args)...);
							auto ptr_handle = PageBaseHandle(ptr);
							add_child_page(page_name, ptr_handle, page_desc, page_desc);
							return ptr;
						}
						else
						{
							console::PrintError("repeatly create page " + page_name);
						}
					}
					else
					{
						console::PrintError("unexcepted page name: " + page_name);
					}
					return nullptr;
				}
			};

			template<typename DataType>
			using PagePtr = ShellPage<DataType>*;
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
			//get name of shell.
			inline std::string name() const
			{
				return _name;
			}

			//get root page.
			inline page::ShellPage<int>* root()
			{
				return &_root_page;
			}

			//public constructor function
			GameShell(std::string name);

			//copy constructor is disallowed.
			GameShell(GameShell&) = delete;

			//start from root.
			void Run(std::string init_command = "");
		};

		/*
		* TestPage is a specified page that can be used by unit test.
		*
		* [parent_page] is parent page of generated test page.
		* [name] is the name of generated test page.
		*/
		class TestPage
		{
		public:

			using FuncType = std::function<void()>;
			using FuncItem = std::pair<std::string, FuncType>;
			using FuncList = std::vector<FuncItem>;
			using PagePtr = page::ShellPage<FuncList>*;

			PagePtr _test_page;

		private:

			//run test and report time.
			static void RunTest(const FuncItem& item);

			//add 'all' test function.
			void AddTestAll();

		public:

			//return true if test page is not a nullptr.
			inline bool is_initialized() const
			{
				return _test_page != nullptr;
			}

		public:

			template<typename DataType>
			TestPage(page::ShellPage<DataType>* parent_page, std::string page_name, std::string page_desc):
				_test_page(nullptr)
			{
				if(parent_page != nullptr)
					_test_page = (parent_page->template CreateChildPage<FuncList>(page_name, page_desc));
				AddTestAll();
			}

			TestPage(const TestPage& tp) = delete;

			//add function by name and function, which would generate a default description. 
			void AddFunction(std::string name, FuncType func);

			//add function by name, description and function.
			void AddFunction(std::string name, std::string desc, FuncType func);

			//add multi functions by name and function, which would generate a default description. 
			void AddFunctionList(std::initializer_list<FuncItem> init_list);
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
					CommandBase<DataType>(CommandType::DEFAULT_COMMAND, name, desc, define::DefaultNoParamsCheck),
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
					CommandBase<DataType>(CommandType::DATA_COMMAND, name, desc, define::DefaultNoParamsCheck),
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
					CommandBase<DataType>(CommandType::PARAMS_COMMAND, name, desc, check),
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
					CommandBase<DataType>(CommandType::DATA_AND_PARAMS_COMMAND, name, desc, params_check),
					_data_params_command_func(data_params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					_data_params_command_func(data, params);
				}
			};

			//default func command record.
			template<typename DataType>
			class BoolParamsCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::BoolParamsCommandFunc;

			private:
				CommandFunc _bool_params_command_func;

			public:

				BoolParamsCommand(std::string name, std::string desc, CommandFunc bool_params_command_func) :
					CommandBase<DataType>(CommandType::BOOL_PARAMS_COMMAND, name, desc, define::DefaultParamsCheck),
					_bool_params_command_func(bool_params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					bool result = _bool_params_command_func(params);
					if (result == false)
					{
						gadt::console::PrintError("run " + this->name() + " failed.");
					}
				}
			};

			//data and params command record.
			template<typename DataType>
			class BoolDataAndParamsCommand :public CommandBase<DataType>
			{
			public:
				using CommandFunc = typename CommandBase<DataType>::BoolDataAndParamsCommandFunc;

			private:
				CommandFunc _bool_data_params_command_func;

			public:
				BoolDataAndParamsCommand(std::string name, std::string desc, CommandFunc bool_data_params_command_func) :
					CommandBase<DataType>(CommandType::BOOL_DATA_AND_PARAMS_COMMAND, name, desc, define::DefaultParamsCheck),
					_bool_data_params_command_func(bool_data_params_command_func)
				{
				}

				void Run(DataType& data, const ParamsList& params) override
				{
					bool result = _bool_data_params_command_func(data, params);
					if (result == false)
					{
						console::PrintError("run " + this->name() + " failed.");
					}
				}
			};
		}
	}
}
