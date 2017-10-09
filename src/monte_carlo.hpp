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
#include "gadt_algorithm.hpp"

#pragma once

namespace gadt
{
	namespace monte_carlo
	{
		/*
		* MonteCarloSetting is the setting of Monte Carlo.
		*
		* MonteCarloSetting() would use default setting.
		* MonteCarloSetting(params) would generate custom setting.
		*
		* more details, see document.
		*/
		struct MonteCarloSetting final: public GameAlgorithmSettingBase
		{
			size_t		thread_num;					//thread num.
			bool		enable_action_policy;		//enable action policy(like Flat-UCB )
			size_t		simulation_times;			//simulation_time;
			size_t		simulation_warning_length;	//if the simulation length out of this value, it would throw a warning if is debug.
			

			//default setting constructor.
			MonteCarloSetting() :
				GameAlgorithmSettingBase(30,0),
				thread_num(1),
				enable_action_policy(true),
				simulation_warning_length(1000)
			{
			}

			//custom setting constructor.
			MonteCarloSetting(
				double _timeout,
				size_t _thread_num,
				size_t _enable_action_policy,
				AgentIndex _no_winner_index = 0,
				size_t _simulation_warning_length = 1000
			) :
				GameAlgorithmSettingBase(_timeout, no_winner_index),
				thread_num(_thread_num),
				enable_action_policy(_enable_action_policy),
				simulation_warning_length(_simulation_warning_length)
			{
			}

			//output print with str behind each line.
			std::string info() const override
			{
				size_t index = 0;
				log::ConsoleTable tb(2, 5);
				tb.set_width({ 12,6 });
				tb.enable_title({ "MONTE CARLO SETTING" });
				tb.set_cell_in_row(index++, { { "timeout" },{ ToString(timeout) } });
				tb.set_cell_in_row(index++, { { "thread_num" },{ ToString(thread_num) } });
				tb.set_cell_in_row(index++, { { "enable_action_policy" },{ ToString(enable_action_policy) } });
				tb.set_cell_in_row(index++, { { "no_winner_index" },{ ToString(no_winner_index) } });
				tb.set_cell_in_row(index++, { { "simulation_warning_length" },{ ToString(simulation_warning_length) } });
				return tb.output_string();
			}
		};

		/*
		* MonteCarloFuncPackage include necessary function to execute monte-carlo simulation.
		*
		* the only constructor is MonteCarloSetting(params), which include 4 parameters:
		*
		* 1. GetNewStateFunc      = std::function<State(const State&, const Action&)>;
		* 2. MakeActionFunc       = std::function<void(const State&, ActionSet&)>;
		* 3. DetemineWinnerFunc   = std::function<AgentIndex(const State&)>;
		* 4. StateToResultFunc    = std::function<Result(const State&, AgentIndex)>;
		* 5. AllowUpdateValueFunc = std::function<bool(const State&, const Result&)>;
		*
		* more details, see document.
		*/
		template<typename State, typename Action, typename Result, bool _is_debug>
		struct MonteCarloFuncPackage;

		/*
		* MonteCarloNode is the node of Monte Carlo.
		*
		* MonteCarloSetting() would use default setting.
		* MonteCarloSetting(params) would generate custom setting.
		*
		* more details, see document.
		*/
		template<typename State, typename Action, typename Result, bool _is_debug>
		class MonteCarloNode
		{
		private:
			using FuncPackage = MonteCarloFuncPackage<State, Action, Result, _is_debug>;

		private:
			const State		_state;				//state of this node.
			AgentIndex		_winner_index;		//the winner index of the state.
			uint32_t		_visited_time;		//how many times that this node had been visited.
			uint32_t		_win_time;			//win time accmulated by the simulation.

		public:
			const State&        state()               const { return _state; }
			AgentIndex          winner_index()        const { return _winner_index; }
			uint32_t            visited_time()        const { return _visited_time; }
			uint32_t            win_time()            const { return _win_time; }

			//increase visited time.
			inline void incr_visited_time()
			{
				_visited_time++;
			}

			//increase win time.
			inline void incr_win_time()
			{
				volatile uint32_t temp = _win_time + 1;
				_win_time = temp;
			}

			//return true if the state is the terminal-state of the game.
			inline bool is_end_state(const MonteCarloSetting& setting) const
			{
				return _winner_index != setting.no_winner_index;
			}

			//constructor
			MonteCarloNode(const State& state, const FuncPackage& func_package):
				_state(state),
				_winner_index(func_package.DetemineWinner(_state)),
				_visited_time(1),
				_win_time(0)
			{
			}
		};

		/*
		* MonteCarloFuncPackage include necessary function to execute monte-carlo simulation.
		*
		* the only constructor is MonteCarloSetting(params), which include 5 parameters:
		*
		* 1. GetNewStateFunc      = std::function<State(const State&, const Action&)>;
		* 2. MakeActionFunc       = std::function<void(const State&, ActionSet&)>;
		* 3. DetemineWinnerFunc   = std::function<AgentIndex(const State&)>;
		* 4. StateToResultFunc    = std::function<Result(const State&, AgentIndex)>;
		* 5. AllowUpdateValueFunc = std::function<bool(const State&, const Result&)>;
		*
		* more details, see document.
		*/
		template<typename State, typename Action, typename Result, bool _is_debug>
		struct MonteCarloFuncPackage final : public GameAlgorithmFuncPackageBase<State, Action, _is_debug>
		{
		public:
#ifdef __GADT_GNUC
			using ActionList = typename GameAlgorithmFuncPackageBase<State, Action, _is_debug>::ActionList;
			using UpdateStateFunc = typename GameAlgorithmFuncPackageBase<State, Action, _is_debug>::UpdateStateFunc;
			using MakeActionFunc = typename GameAlgorithmFuncPackageBase<State, Action, _is_debug>::MakeActionFunc;
			using DetemineWinnerFunc = typename GameAlgorithmFuncPackageBase<State, Action, _is_debug>::DetemineWinnerFunc;
			using GameAlgorithmFuncPackageBase<State, Action, _is_debug>::is_debug;
#endif
			using Node					= MonteCarloNode<State, Action, Result, _is_debug>;
			using StateToResultFunc		= std::function<Result(const State&, AgentIndex)>;
			using AllowUpdateValueFunc	= std::function<bool(const State&, const Result&)>;
			using ActionPolicyFunc		= std::function<UcbValue(const Node&, const Node&)>;
			using DefaultPolicyFunc		= std::function<const Action&(const ActionList&)>;
			using ValueForRootNodeFunc	= std::function<UcbValue(const Node&, const Node&)>;

		public:
			const StateToResultFunc		StateToResult;		//get a result from state and winner.
			const AllowUpdateValueFunc	AllowUpdateValue;	//update values in the node by the result.
			ActionPolicyFunc			ActionPolicy;		//action policy is used for root node to select next action for simulation.
			DefaultPolicyFunc			DefaultPolicy;		//the default policy to select action in simulation.
			ValueForRootNodeFunc		ValueForRootNode;	//select best action of root node after iterations finished.

		public:
			MonteCarloFuncPackage(
				UpdateStateFunc			_UpdateState,
				MakeActionFunc			_MakeAction,
				DetemineWinnerFunc		_DetemineWinner,
				StateToResultFunc		_StateToResult,
				AllowUpdateValueFunc	_AllowUpdateValue
			) :
				GameAlgorithmFuncPackageBase<State, Action, _is_debug>(_UpdateState, _MakeAction, _DetemineWinner),
				StateToResult(_StateToResult),
				AllowUpdateValue(_AllowUpdateValue),
				ActionPolicy([](const Node& parent, const Node& child)->UcbValue {
					UcbValue avg = static_cast<UcbValue>(child.win_time()) / static_cast<UcbValue>(child.visited_time());
					return policy::UCB1(avg, static_cast<UcbValue>(parent.visited_time()), static_cast<UcbValue>(child.visited_time()));
				}),
				DefaultPolicy([](const ActionList& actions)->const Action&{
					GADT_CHECK_WARNING(_is_debug, actions.size() == 0, "MCTS104: empty action set during default policy.");
					return actions[rand() % actions.size()];
				}),
				ValueForRootNode([](const Node& parent, const Node& child)->UcbValue {
					UcbValue avg = static_cast<UcbValue>(child.win_time()) / static_cast<UcbValue>(child.visited_time());
					return policy::UCB1(avg, static_cast<UcbValue>(parent.visited_time()), static_cast<UcbValue>(child.visited_time()));
				})
			{
			}


		};

		template<typename State, typename Action, typename Result, bool _is_debug>
		class MonteCarloSimulation final :public GameAlgorithmBase<State, Action, Result, _is_debug>
		{
#ifdef __GADT_GNUC
		private:
			using GameAlgorithmBase<State, Action, Result, _is_debug>::_algorithm_name;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::_log_controller;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::logger;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::log_enabled;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::json_output_enabled;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::timeout;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::is_debug;

		public:
			using GameAlgorithmBase<State, Action, Result, _is_debug>::SetName;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::InitLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableJsonOutput;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableJsonOutput;
#endif
		public:
			using Node = MonteCarloNode<State, Action, Result, _is_debug>;
			using FuncPackage = MonteCarloFuncPackage<State, Action, Result, _is_debug>;
			using ActionList = typename FuncPackage::ActionList;
			using NodeList = std::vector<Node>;
			using CountList = std::vector<size_t>;

		private:
			FuncPackage _func_package;
			MonteCarloSetting _setting;

		private:
			
			//simulation.
			Result Simulation(const State& original_state) const
			{
				State state = original_state;	//copy
				ActionList actions;
				for (size_t i = 0;; i++)
				{
					GADT_CHECK_WARNING(is_debug(), i > _setting.simulation_warning_length, "out of default policy process max length.");

					//detemine winner
					AgentIndex winner = _func_package.DetemineWinner(state);

					//return result if exist.
					if (winner != _setting.no_winner_index)
					{
						return _func_package.StateToResult(state, winner);
					}

					//generate new actions.
					actions.clear();
					_func_package.MakeAction(state, actions);
					GADT_CHECK_WARNING(is_debug(), actions.size() == 0, "empty action list.");

					//choose action by default policy.
					Action action = _func_package.DefaultPolicy(actions);

					//state update.
					_func_package.UpdateState(state, action);
				}
				return _func_package.StateToResult(state, _setting.no_winner_index);
			}

			//execute simulation.
			void ExecuteSimulation(Node& parent, Node& child) const
			{
				Result result = child.is_end_state(_setting)
					? _func_package.StateToResult(child.state(),child.winner_index())
					: Simulation(child.state());
				child.incr_visited_time();
				parent.incr_visited_time();
				if (_func_package.AllowUpdateValue(parent.state(), result))
				{
					child.incr_win_time();
					parent.incr_win_time();
				}
			}

			//select best child by action policy.
			void Selection(Node& parent, NodeList& child_nodes) const
			{
				size_t best_index = 0;
				UcbValue best_value = _func_package.ActionPolicy(parent, child_nodes[0]);
				for (size_t i = 1; i < child_nodes.size(); i++)
				{
					UcbValue value = _func_package.ActionPolicy(parent, child_nodes[i]);
					if (value > best_value)
					{
						best_value = value;
						best_index = i;
					}
				}

				//execute simulation in best child
				ExecuteSimulation(parent, child_nodes[best_index]);
			}

			//execute simulatio for all child nodes.
			void ExecuteAllChild(Node& parent, NodeList& child_nodes) const
			{
				for (Node& child : child_nodes)
				{
					ExecuteSimulation(parent, child);
				}
			}

			//execute monte carlo.
			Action ExecuteMonteCarlo(const State& state) const
			{
				//get available actions
				Node root(state, _func_package);
				ActionList action_list;
				NodeList child_nodes;
				timer::TimePoint tp_mc_start;

				//outputt log.
				if (log_enabled())
				{
					logger() << "[ Monte Carlo Simulation ]" << std::endl;
					logger() << _setting.info();
					logger() << std::endl << ">> Executing Monte Carlo Simulation......" << std::endl;
				}

				//make actions
				_func_package.MakeAction(state, action_list);
				GADT_CHECK_WARNING(is_debug(), action_list.empty(), "empty action list for root node");

				//get child nodes
				for (const Action& act : action_list)
				{
					State temp = state;
					_func_package.UpdateState(temp, act);
					child_nodes.push_back(Node(temp, _func_package));
				}
				

				//create threads.
				std::vector<std::thread> threads;
				for (size_t thread_id = 0; thread_id < _setting.thread_num; thread_id++)
				{
					threads.push_back(std::thread([&]()->void {
						if (_setting.enable_action_policy)
						{
							size_t sim_time = 1 + _setting.simulation_times;
							for (size_t i = 0; i < sim_time; i++)
							{
								if (this->timeout(tp_mc_start, _setting)) { return; }
								Selection(root, child_nodes);
							}
						}
						else
						{
							size_t sim_time = 1 + (_setting.simulation_times / child_nodes.size());
							for (size_t i = 0; i < sim_time; i++)
							{
								if (this->timeout(tp_mc_start, _setting)) { return; }
								ExecuteAllChild(root, child_nodes);
							}
						}
					}));
				}
				//join all threads.
				for (size_t i = 0; i < threads.size(); i++)
				{
					threads[i].join();
				}

				//accumulate all the count list
				std::vector<UcbValue> child_value_set(child_nodes.size(), 0);
				for (size_t i = 0; i < child_nodes.size(); i++)
				{
					child_value_set[i] = _func_package.ValueForRootNode(root, child_nodes[i]);
				}
				size_t best_node_index = func::GetMaxElement<UcbValue>(child_value_set);
				
				//output log.
				if (log_enabled())
				{
					//MCTS RESULT
					log::ConsoleTable tb(6, child_nodes.size() + 2);
					tb.enable_title({ "MONTE CARLO SIMULATION RESULT: TIME = [ " + ToString(tp_mc_start.time_since_created()) + "s ]" });
					tb.set_cell_in_row(0, { { "Index" },{ "Action" },{ "Value" },{ "Visit" },{ "Win" },{ "Best" } });
					tb.set_width({ 3,10,4,4,4,2 });
					for (size_t i = 0; i < child_nodes.size(); i++)
					{
						tb.set_cell_in_row(i + 1, {
							{ ToString(i) },
							{ this->_log_controller.action_to_str_func()(action_list[i]) },
							{ ToString(child_value_set[i]) },
							{ ToString(child_nodes[i].visited_time()) },
							{ ToString(child_nodes[i].win_time()) },
							{ i == best_node_index ? "Yes " : "  " }
						});
					}
					tb.set_cell_in_row(child_nodes.size() + 1, {
						{ "Total" },
						{ "" },
						{ "" },
						{ ToString(root.visited_time()) },
						{ ToString(root.win_time()) },
						{ ToString(best_node_index) }
					});
					logger() << tb.output_string(true, false) << std::endl;
				}

				return action_list[best_node_index];
			}

		public:

			MonteCarloSimulation(
				typename FuncPackage::UpdateStateFunc		_UpdateState,
				typename FuncPackage::MakeActionFunc		_MakeAction,
				typename FuncPackage::DetemineWinnerFunc	_DetemineWinner,
				typename FuncPackage::StateToResultFunc		_StateToResult,
				typename FuncPackage::AllowUpdateValueFunc	_AllowUpdateValue
			):
				GameAlgorithmBase<State, Action, Result, _is_debug>("Monte Carlo"),
				_func_package(_UpdateState, _MakeAction, _DetemineWinner, _StateToResult, _AllowUpdateValue),
				_setting()
			{
			}

			MonteCarloSimulation(const FuncPackage& func_package) :
				_func_package(func_package),
				_setting()
			{
			}

			Action DoMonteCarlo(const State state, MonteCarloSetting setting)
			{
				_setting = setting;
				return ExecuteMonteCarlo(state);
			}
		};
	}
}

