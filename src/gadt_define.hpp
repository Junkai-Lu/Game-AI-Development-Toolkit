#include "gadtlib.h"
#include "gadt_stl.hpp"

#pragma once

namespace gadt
{
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

		virtual std::string info() const = 0;
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

		using ActionList			= std::vector<Action>;
		using UpdateStateFunc		= std::function<void(State&, const Action&)>;
		using MakeActionFunc		= std::function<void(const State&, ActionList&)>;
		using DetemineWinnerFunc	= std::function<AgentIndex(const State&)>;

		const UpdateStateFunc		UpdateState;		//get a new state from previous state and action.
		const MakeActionFunc		MakeAction;			//the function which create action set by the state.
		const DetemineWinnerFunc	DetemineWinner;		//return no_winner_index if a state is not terminal state.

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
		inline std::ostream& logger()
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

		//return the value of _is_debug.
		constexpr inline bool is_debug() const
		{
			return _is_debug;
		}

	public:
	
		inline void SetName(std::string name)
		{
			_algorithm_name = name;
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
		inline void EnableJsonOutput(std::string output_folder = _algorithm_name)
		{
			_log_controller.EnableJsonOutput(output_path);
		}

		//disable json output.
		inline void DisableJsonOutput()
		{
			_log_controller.DisableJsonOutput();
		}
	};
	
}