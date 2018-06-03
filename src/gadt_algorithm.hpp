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
#include "gadt_memory.hpp"
#include "gadt_log.hpp"

#pragma once

namespace gadt
{
	namespace policy
	{
		inline UcbValue UCB1(UcbValue average_reward, UcbValue overall_time, UcbValue played_time, UcbValue c = 1.41421)
		{
			UcbValue ln = log10(overall_time);
			UcbValue exploration = sqrt(ln / played_time);
			return average_reward + c * exploration;
		}
	}

	struct GameAlgorithmSettingBase
	{
		double		timeout;			//timeout
		AgentIndex	no_winner_index;	//the index of no winner

	protected:
		GameAlgorithmSettingBase() :
			timeout(0),
			no_winner_index(0)
		{
		}

		GameAlgorithmSettingBase(double _timeout, AgentIndex _no_winner_index) :
			timeout(_timeout),
			no_winner_index(_no_winner_index)
		{
		}

		virtual void PrintInfo() const = 0;
	};

	/*
	* GameAlgorithmFuncPackageBase include necessary function for game algorithms.
	* it is the base class of all game algorithms like minimax and monte carlo tree search.
	*
	* the only constructor is GameAlgorithmFuncPackageBase(params), which include 4 parameters:
	*
	* 1. UpdateStateFunc    = std::function<void(State&, const Action&)>;
	* 2. MakeActionFunc     = std::function<void(const State&, ActionSet&)>;
	* 3. DetemineWinnerFunc = std::function<AgentIndex(const State&)>;
	*
	* more details, see document.
	*/
	template<typename State, typename Action, bool _is_debug>
	struct GameAlgorithmFuncPackageBase
	{
	public:
		using ActionList			= std::vector<Action>;
		using UpdateStateFunc		= std::function<void(State&, const Action&)>;
		using MakeActionFunc		= std::function<void(const State&, ActionList&)>;
		using DetemineWinnerFunc	= std::function<AgentIndex(const State&)>;

	public:
		const UpdateStateFunc		UpdateState;		//get a new state from previous state and action.
		const MakeActionFunc		MakeAction;			//the function which create action set by the state.
		const DetemineWinnerFunc	DetemineWinner;		//return no_winner_index if a state is not terminal state.

	public:
		explicit GameAlgorithmFuncPackageBase(
			UpdateStateFunc			_UpdateState,
			MakeActionFunc			_MakeAction,
			DetemineWinnerFunc		_DetemineWinner
		) :
			UpdateState(_UpdateState),
			MakeAction(_MakeAction),
			DetemineWinner(_DetemineWinner)
		{
		}

		//return the value of _is_debug.
		inline bool is_debug() const
		{
			return _is_debug;
		}
	};

	template<typename State, typename Action, typename Result, bool _is_debug>
	class GameAlgorithmBase
	{
	public:
		using ActionList = typename GameAlgorithmFuncPackageBase<State, Action, _is_debug>::ActionList;
		using LogController = log::SearchLogger<State, Action, Result>;

	protected:

		std::string _algorithm_name;
		LogController _log_controller;

	protected:

		GameAlgorithmBase(std::string algorithm_name) :
			_algorithm_name(algorithm_name),
			_log_controller()
		{
		}

		//get reference of log ostream
		inline std::ostream& logger() const
		{
			return _log_controller.log_ostream();
		}

		//return true if log enabled.
		inline bool log_enabled() const
		{
			return _log_controller.log_enabled();
		}

		//return true if json output enabled.
		inline bool json_output_enabled() const
		{
			return _log_controller.json_output_enabled();
		}

		//return true if timeout.
		inline bool timeout(const timer::TimePoint tp, const GameAlgorithmSettingBase& setting) const
		{
			return tp.time_since_created() > setting.timeout && setting.timeout > 0;
		}

		//return the value of _is_debug.
		inline bool is_debug() const
		{
			return _is_debug;
		}

	public:
	
		//set algorithm name.
		inline void SetName(std::string name)
		{
			_algorithm_name = name;
		}

		//enable log output to ostream.
		inline void InitLog(
			typename LogController::StateToStrFunc     _state_to_str,
			typename LogController::ActionToStrFunc    _action_to_str,
			typename LogController::ResultToStrFunc    _result_to_str = [](const Result&)->std::string {return ""; }
		)
		{
			_log_controller.Init(_state_to_str, _action_to_str, _result_to_str);
		}

		//enable log by ostream.
		inline void EnableLog(std::ostream& os = std::cout)
		{
			_log_controller.EnableLog(os);
		}

		//disable log.
		inline void DisableLog()
		{
			_log_controller.DisableLog();
		}

		//enable json output
		inline void EnableJsonOutput(std::string output_folder = "")
		{
			if (output_folder == "") { output_folder = _algorithm_name; }
			_log_controller.EnableJsonOutput(output_folder);
		}

		//disable json output.
		inline void DisableJsonOutput()
		{
			_log_controller.DisableJsonOutput();
		}

	};
	
}