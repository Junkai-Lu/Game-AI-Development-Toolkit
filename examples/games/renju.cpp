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

#include "renju.h"

namespace gadt
{
	namespace renju
	{
		void UpdateState(RenjuState & state, const RenjuAction & action)
		{
			state.TakeAction(action);
		}

		void MakeAction(const RenjuState & state, RenjuActionList & list)
		{
			RenjuActionGenerator generator(state);
			list = generator.GetNearbyActions(state.next_player());
		}

		RenjuPlayer DetemineWinner(const RenjuState & state)
		{
			if (state.is_draw())
				if (state.winner() != 0)
					return (RenjuPlayer)state.winner();
				else
					return DRAW;
			return (RenjuPlayer)state.winner();
		}

		RenjuResult StateToResult(const RenjuState & state, AgentIndex winner)
		{
			return (RenjuPlayer)state.winner();
		}

		bool AllowUpdateValue(const RenjuState & state, RenjuResult winner)
		{
			if (state.next_player() == BLACK && winner == BLACK)
			{
				return true;
			}
			if (state.next_player() == WHITE && winner == WHITE)
			{
				return true;
			}
			return false;
		}

		std::string StateToString(const RenjuState & state)
		{
			return "...";
		}

		std::string ActionToString(const RenjuAction & action)
		{
			return action.to_string();
		}

		std::string ResultToString(const RenjuResult & result)
		{
			return gadt::ToString(int(result));
		}

		void PrintRenjuState(const RenjuState& state)
		{
			console::Table tb(state.width(), state.height());
			tb.set_width(1);
			std::stringstream ss;
			ss << "player:" << (int)state.next_player() << " winner:" << (int)state.winner();
			tb.enable_title({ ss.str() });
			for (auto point : state)
			{
				if (state.piece(point) == BLACK)
				{
					tb.set_cell({ "X" }, point);
				}
				else if (state.piece(point) == WHITE)
				{
					tb.set_cell({ "O" }, point);
				}
				else if (state.piece(point) == FLAG)
				{
					tb.set_cell({ "F" }, point);
				}
				else
				{
					tb.set_cell({ "+" }, point);
				}
			}
			tb.Print(console::TableFrame::Disable, console::TableIndex::BeginFromZero);
		}

		void PrintRenjuActions(const RenjuState& state, RenjuActionList action_list)
		{
			RenjuState temp = state;
			for (auto act : action_list)
			{
				temp.TakeAction({act.point, FLAG});
			}
			std::cout << "Action Num: " << action_list.size();
			PrintRenjuState(temp);
		}

		void DefineRenjuShell(shell::GameShell& shell)
		{
			auto* renju = shell.CreateShellPage<RenjuState>("renju", 1);
			renju->AddFunction("print", "print state", [](RenjuState& state) {
				PrintRenjuState(state);
			});

			renju->AddFunction("take", "take action", [](RenjuState& state, const shell::ParamsList& params)->bool{
				if (params.size() >= 3)
				{
					RenjuAction action = {
						{ (size_t)ToInt(params[0]),(size_t)ToInt(params[1]) },
						RenjuPlayer(ToInt(params[2]))
					};
					if (state.IsLegalAction(action))
					{
						state.TakeAction(action);
						PrintRenjuState(state);
						return true;
					}
				}
				return false;
			});

			renju->AddFunction("winner", "show winner", [](RenjuState& state)->void{
				std::cout << "winner = " << (int)state.winner() << std::endl;
			});

			renju->AddFunction("reset", "reset state", [](RenjuState& state)->void{
				state = RenjuState(1);
			});

			renju->AddFunction("actions", "show actions", [](RenjuState& state, const shell::ParamsList& params)->bool{
				renju::RenjuActionGenerator generator(state);
				if (params.size() > 0)
				{
					if (params[0] == "all")
					{
						PrintRenjuActions(state, generator.GetAllActions(state.next_player()));
						return true;
					}
				}
				PrintRenjuActions(state, generator.GetNearbyActions(state.next_player()));
				return true;
			});

			renju->AddFunction("mcts", "run mcts", [](renju::RenjuState& state)->void{
				RenjuMCTS mcts(
					UpdateState,
					MakeAction,
					DetemineWinner,
					StateToResult,
					AllowUpdateValue);
				mcts.InitLog(StateToString, ActionToString, ResultToString);
				mcts.EnableLog();
				mcts::MctsSetting setting;
				setting.max_node_per_thread = 100000;
				setting.max_iteration_per_thread = 100000;
				setting.no_winner_index = 0;
				setting.simulation_warning_length = 382;
				setting.max_thread = 4;
				setting.timeout = 15;
				auto act = mcts.Run(state, setting);
				if (console::GetUserConfirm("take this action?"))
				{
					state.TakeAction(act);
					PrintRenjuState(state);
				}
			});

		}
	}
}