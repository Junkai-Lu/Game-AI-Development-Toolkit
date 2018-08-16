#include "gshell_command.h"
#include "gadt_table.h"

#pragma once

namespace gadt
{
	namespace shell
	{
		class GameShell;
		class TestPage;

		//shell pages class
		namespace page
		{
			template<typename DataType>
			class ShellPage;

			class ShellPageBase;
			using PageBasePtr = ShellPageBase * ;
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

				using ShellPtr = GameShell * ;
				using PageTable = std::map<std::string, PageInfo>;
				using InfoFunc = std::function<void()>;
				using CommandParser = command::CommandParser;

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
				ShellPageBase(PageBasePtr parent_page, ShellPtr belonging_shell, std::string name, InfoFunc info_func);

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

				using CommandBase = command::CommandBase<DataType>;
				using DefaultCommand = command::DefaultCommand<DataType>;
				using DataCommand = command::DataCommand<DataType>;
				using ParamsCommand = command::ParamsCommand<DataType>;
				using DataAndParamsCommand = command::DataAndParamsCommand<DataType>;
				using BoolParamsCommand = command::BoolParamsCommand<DataType>;
				using BoolDataAndParamsCommand = command::BoolDataAndParamsCommand<DataType>;
				using CommandPtr = std::unique_ptr<CommandBase>;
				using DefaultCommandFunc = typename CommandBase::DefaultCommandFunc;
				using DataCommandFunc = typename CommandBase::DataCommandFunc;
				using ParamsCommandFunc = typename CommandBase::ParamsCommandFunc;
				using DataAndParamsCommandFunc = typename CommandBase::DataAndParamsCommandFunc;
				using BoolParamsCommandFunc = typename CommandBase::BoolParamsCommandFunc;
				using BoolDataAndParamsCommandFunc = typename CommandBase::BoolDataAndParamsCommandFunc;

				template<class... Args>
				using DefaultArgsCommandFunc = typename CommandBase::template DefaultArgsCommandFunc<Args...>;

				template<class... Args>
				using DataArgsCommandFunc = typename CommandBase::template DataArgsCommandFunc<Args...>;

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
									tb.set_cell_in_row(n, {
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
								{ cmds[i].name,(cmds[i].is_page ? console::ConsoleColor::Blue : console::ConsoleColor::Red) },
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
				* Add DefaultCommand which do not have any parameters.
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
				* Add DataCommand that allows to operator binded data without parameters
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
				* Add ParamsCommand which allows to be executed with parameters.
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
				* Add DataAndParamsCommand which allows to operator binded data with parameters.
				*
				* [name] is the name of command.
				* [func] is the added function, type = void(DataType&, const ParamsList&)
				* [desc] is description of the command.
				* [check] is the parameters check function, type = bool(const ParamsList&)
				*/
				void AddFunction(std::string name, std::string desc, DataAndParamsCommandFunc func, ParamsCheckFunc check)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new DataAndParamsCommand(name, desc, func, check));
						add_command(name, command_ptr);
					}
				}

				/*
				* Add BoolParamsCommand which allows to be executed with parameters with a boolean return value.
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
				* Add BoolDataAndParamsCommand which allows to operator binded data with parameters with a boolean return value.
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

				template<class... Args>
				void AddFunctionWithArgs(std::string name, std::string desc, typename Identity<std::function<void(Args...)>>::type func)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new command::DefaultArgsCommand<DataType, Args...>(name, desc, func));
						add_command(name, command_ptr);
					}
				}

				template<class... Args>
				void AddFunctionWithArgs(std::string name, std::string desc, typename Identity<std::function<void(DataType&, Args...)>>::type func)
				{
					if (CheckCommandNameLegality(name))
					{
						auto command_ptr = CommandPtr(new command::DataArgsCommand<DataType, Args...>(name, desc, func));
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
			using ShellPagePtr = ShellPage<DataType>*;
		}

		/*
		* TestPage is a specified page that can be used by unit test.
		*
		* [parent_page] is parent page of generated test page.
		* [name] is the name of generated test page.
		*/
		class TestPage
		{
		private:

			using FuncType = std::function<void()>;
			using FuncItem = std::pair<std::string, FuncType>;
			using FuncList = std::vector<FuncItem>;
			using FuncItemInitList = std::initializer_list<FuncItem>;
			using PagePtr = page::ShellPage<FuncList>*;

			PagePtr _test_page;

		private:

			//run test and report time.
			static void RunTest(const FuncItem& item)
			{
				using namespace console;
				console::PrintEndLine();
				console::Cprintf(">> test start, target = ", ConsoleColor::White);
				console::Cprintf(item.first, console::ConsoleColor::Green);
				timer::TimePoint tp;
				console::PrintEndLine();
				item.second();
				console::Cprintf(">> test complete, time = ", ConsoleColor::White);
				console::Cprintf(tp.time_since_created(), console::ConsoleColor::Red);
				console::PrintEndLine();
			}

			//add 'all' test function.
			void AddTestAll()
			{
				if (is_initialized())
				{
					_test_page->AddFunction("all", "run all test function", [&](FuncList& functions)->void {
						for (const auto& p : functions)
						{
							RunTest(p);
						}
					});
				}
			}

		public:

			//return true if test page is not a nullptr.
			inline bool is_initialized() const
			{
				return _test_page != nullptr;
			}

		public:

			template<typename DataType>
			TestPage(page::ShellPagePtr<DataType> parent_page, std::string page_name, std::string page_desc) :
				_test_page(nullptr)
			{
				if (parent_page != nullptr)
					_test_page = (parent_page->template CreateChildPage<FuncList>(page_name, page_desc));
				AddTestAll();
			}

			TestPage(const TestPage& tp) = delete;

			//add function by name and function, which would generate a default description. 
			void AddFunction(std::string name, FuncType func)
			{
				AddFunction(name, "test " + name, func);
			}

			//add function by name, description and function.
			void AddFunction(std::string name, std::string desc, FuncType func)
			{
				if (is_initialized())
				{
					_test_page->data().push_back({ name, func });
					const size_t size = _test_page->data().size();
					_test_page->AddFunction(name, desc, [=](FuncList& func_list)->void {
						RunTest(func_list[size - 1]);
					});
				}
			}

			//add multi functions by name and function, which would generate a default description. 
			void AddFunctionList(FuncItemInitList init_list)
			{
				for (auto pair : init_list)
				{
					AddFunction(pair.first, pair.second);
				}
			}
		};
	}
}