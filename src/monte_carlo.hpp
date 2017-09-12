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
#include "gadt_define.hpp"

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
			size_t		simulation_warning_length;	//if the simulation length out of this value, it would throw a warning if is debug.

			//default setting constructor.
			MonteCarloSetting() :
				GameAlgorithmSettingBase(30,0),
				thread_num(1),
				simulation_warning_length(1000)
			{
			}

			//custom setting constructor.
			MonteCarloSetting(
				double _timeout,
				size_t _thread_num,
				AgentIndex _no_winner_index = 0,
				size_t _simulation_warning_length = 1000
			) :
				GameAlgorithmSettingBase(_timeout, no_winner_index),
				thread_num(_thread_num),
				simulation_warning_length(_simulation_warning_length)
			{
			}

			//output print with str behind each line.
			std::string info() const override
			{
				table::ConsoleTable tb(2, 6);
				tb.set_width({ 12,6 });
				tb.enable_title({ "MONTE CARLO SETTING" });
				tb.set_cell_in_row(0, { { "timeout" },{ console::ToString(timeout) } });
				tb.set_cell_in_row(1, { { "thread_num" },{ console::ToString(thread_num) } });
				tb.set_cell_in_row(4, { { "no_winner_index" },{ console::ToString(no_winner_index) } });
				tb.set_cell_in_row(5, { { "simulation_warning_length" },{ console::ToString(simulation_warning_length) } });
				return tb.output_string();
			}
		};

		/*
		* MonteCarloFuncPackage include necessary function to execute monte-carlo simulation.
		*
		* the only constructor is MonteCarloSetting(params), which include 4 parameters:
		*
		* 1. GetNewStateFunc    = std::function<State(const State&, const Action&)>;
		* 2. MakeActionFunc     = std::function<void(const State&, ActionSet&)>;
		* 3. DetemineWinnerFunc = std::function<AgentIndex(const State&)>;
		* 4. StateToResultFunc  = std::function<Result(const State&, AgentIndex)>;
		*
		* more details, see document.
		*/
		template<typename State, typename Action, bool _is_debug>
		struct MonteCarloFuncPackage final : public GameAlgorithmFuncPackageBase<State, Action, _is_debug>
		{
			MonteCarloFuncPackage(
				UpdateStateFunc			_UpdateState,
				MakeActionFunc			_MakeAction,
				DetemineWinnerFunc		_DetemineWinner
			) :GameAlgorithmFuncPackageBase<State, Action, _is_debug>(_UpdateState, _MakeAction, _DetemineWinner)
			{
			}
		};


		template<typename State, typename Action, bool _is_debug>
		class MonteCarloSimulation final :public GameAlgorithmBase<State,Action,int,_is_debug>
		{
		public:
			using FuncPackage = MonteCarloFuncPackage<State, Action, _is_debug>;

		private:
			FuncPackage _func_package;
			MonteCarloSetting _setting;

		private:
			AgentIndex SimulationProcess() const
			{
				State state = _state;	//copy
				ActionList actions;
				for (size_t i = 0;; i++)
				{
					GADT_CHECK_WARNING(is_debug(), i > _setting.simulation_warning_length, "out of default policy process max length.");

					//detemine winner
					AgentIndex winner = _func_package.DetemineWinner(state);

					//return result if exist.
					if (winner != _setting.no_winner_index)
					{
						return winner;
					}

					//generate new actions.
					actions.clear();
					_func_package.MakeAction(state, actions);

					//choose action by default policy.
					const Action& action = _func_package.DefaultPolicy(actions);

					//state update.
					_func_package.UpdateState(state, action);
				}
				return _setting.no_winner_index;
			}

			/*Action ParallelSimulation() const
			{
				using CountList = std::vector<size_t>;
				std::vector<CountList> count_lists;
				std::vector<std::thread> threads;
			}*/

		public:

		};
	}
}

